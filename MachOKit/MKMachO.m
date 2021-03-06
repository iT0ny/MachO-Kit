//----------------------------------------------------------------------------//
//|
//|             MachOKit - A Lightweight Mach-O Parsing Library
//|             MKMachO.m
//|
//|             D.V.
//|             Copyright (c) 2014-2015 D.V. All rights reserved.
//|
//| Permission is hereby granted, free of charge, to any person obtaining a
//| copy of this software and associated documentation files (the "Software"),
//| to deal in the Software without restriction, including without limitation
//| the rights to use, copy, modify, merge, publish, distribute, sublicense,
//| and/or sell copies of the Software, and to permit persons to whom the
//| Software is furnished to do so, subject to the following conditions:
//|
//| The above copyright notice and this permission notice shall be included
//| in all copies or substantial portions of the Software.
//|
//| THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//| OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//| MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//| IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//| CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//| TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//| SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//----------------------------------------------------------------------------//

#import "MKMachO.h"
#import "NSError+MK.h"
#import "MKMachHeader.h"
#import "MKMachHeader64.h"
#import "MKLoadCommand.h"
#import "MKLCSegment.h"
#include "core_internal.h"

#include <objc/runtime.h>

//----------------------------------------------------------------------------//
@implementation MKMachOImage

//|++++++++++++++++++++++++++++++++++++|//
- (instancetype)initWithName:(const char*)name slide:(intptr_t)slide flags:(MKMachOImageFlags)flags atAddress:(mk_vm_address_t)contextAddress inMapping:(MKMemoryMap*)mapping error:(NSError**)error
{
    NSParameterAssert(mapping);
    NSError *localError = nil;
    
    self = [super initWithParent:nil error:error];
    if (self == nil) return nil;
    
    // <TODO> Remove this eventually
    _context.user_data = (void*)self;
    _context.logger = (mk_logger_c)method_getImplementation(class_getInstanceMethod(self.class, @selector(_logMessageAtLevel:inFile:line:function:message:)));
    // </TODO> Remove this eventually
    
    _mapping = [mapping retain];
    _contextAddress = contextAddress;
    _slide = slide;
    _flags = flags;
    
    // Convert the name to an NSString
    if (name)
        _name = [[NSString alloc] initWithCString:name encoding:NSUTF8StringEncoding];
    
    // Read the Magic
    uint32_t magic = [mapping readDoubleWordAtOffset:0 fromAddress:contextAddress withDataModel:nil error:error];
    if (magic == 0) { [self release]; return nil; }
    
    // Load the appropriate data model for the image
    switch (magic) {
        case MH_CIGAM:
        case MH_MAGIC:
            _dataModel = [[MKILP32DataModel sharedDataModel] retain];
            _header = [[MKMachHeader alloc] initWithOffset:0 fromParent:self error:&localError];
            break;
        case MH_CIGAM_64:
        case MH_MAGIC_64:
            _dataModel = [[MKLP64DataModel sharedDataModel] retain];
            _header = [[MKMachHeader64 alloc] initWithOffset:0 fromParent:self error:&localError];
            break;
        default:
            MK_ERROR_OUT = [NSError mk_errorWithDomain:MKErrorDomain code:MK_EINVAL description:@"Unknown Mach-O magic: 0x%" PRIx32 " in: %s", magic, name];
            [self release]; return nil;
    }
    
    if (_header == nil) {
        MK_ERROR_OUT = [NSError mk_errorWithDomain:MKErrorDomain code:localError.code underlyingError:localError description:@"Failed to load Mach header"];
        [self release]; return nil;
    }
    
    // Only support a subset of the MachO types at this time
    switch (_header.filetype) {
        case MH_EXECUTE:
        case MH_DYLIB:
            break;
        default:
            MK_ERROR_OUT = [NSError mk_errorWithDomain:MKErrorDomain code:MK_EINVAL description:@"Unsupported file type: %" PRIx32 "", _header.filetype];
            [self release]; return nil;
    }
    
    // Parse load commands
    {
        uint32_t loadCommandLength = _header.sizeofcmds;
        uint32_t loadCommandCount = _header.ncmds;
        
        // The kernel will refuse to load a MachO image in which the
        // mach_header_size + header->sizeofcmds is greater than the size of the
        // MachO image.  However, we can not know the size of the MachO image here.
        
        // TODO - Premap the load commands once MKMemoryMap has support for that.
        
        NSMutableArray *loadCommands = [[NSMutableArray alloc] initWithCapacity:loadCommandCount];
        mach_vm_offset_t offset = _header.nodeSize;
        mach_vm_offset_t oldOffset;
        
        while (loadCommandCount--) {
        @autoreleasepool {
                
            NSError *loadCommandError = nil;
                
            // It is safe to pass the mach_vm_offset_t offset as the offset
            // parameter because the offset can not grow beyond the header size,
            // which is capped at UINT32_MAX.  Any uint32_t can be acurately
            // represented by an mk_vm_offset_t.
                
            MKLoadCommand *lc = [MKLoadCommand loadCommandAtOffset:offset fromParent:self error:&loadCommandError];
            if (lc == nil) {
                // If we fail to instantiate an instance of the MKLoadCommand it
                // means we've walked off the end of memory that can be mapped by
                // our MKMemoryMap.
                MK_PUSH_UNDERLYING_WARNING(loadCommands, loadCommandError, @"Failed to instantiate load command at index %" PRIi32 "", _header.ncmds - loadCommandCount);
                break;
            }
                
            oldOffset = offset;
            offset += lc.cmdSize;
                
            [loadCommands addObject:lc];
                
            // The kernel will refuse to load a MachO image if it detects that the
            // kernel's offset into the load commands (when parsing the load
            // commands) has exceeded the total mach header size (mach_header_size
            // + mach_header->sizeofcmds).  However, we don't care as long as there
            // was not an overflow.
            if (oldOffset > offset) {
                MK_PUSH_WARNING(loadCommands, MK_EOVERFLOW, @"Adding size of load command at index %" PRIi32 " to offset into load commands triggered an overflow.", _header.ncmds - loadCommandCount);
                break;
            }
            // We will add a warning however.
            if (offset > _header.nodeSize + (mach_vm_size_t)loadCommandLength)
                MK_PUSH_WARNING(loadCommands, MK_EINVALID_DATA, @"Part of load command at index %" PRIi32 " is beyond sizeofcmds for this image.  This is invalid.", _header.ncmds - loadCommandCount);
        }}
        
        _loadCommands = [loadCommands copy];
        [loadCommands release];
    }
    
    // Determine the file and VM address of this image
    {
        mk_error_t err;
        
        NSArray *segmentLoadCommands = [self loadCommandsOfType:(self.dataModel.pointerSize == 8) ? LC_SEGMENT_64 : LC_SEGMENT];
        for (id<MKLCSegment> segmentLC in segmentLoadCommands) {
            // The VM address of the image is defined as the vmaddr of the
            // *last* segment load command with a 0 fileoff and non-zero
            // filesize.  That's right, if another segment is later found
            // that satisfies this criteria, it will be used instead.
            if (segmentLC.mk_fileoff == 0 && segmentLC.mk_filesize != 0)
                _vmAddress = segmentLC.mk_vmaddr;
        }
        
        // Slide the VM address
        if ((err = mk_vm_address_apply_offset(_vmAddress, slide, &_vmAddress))) {
            MK_ERROR_OUT = MK_MAKE_VM_ARITHMETIC_ERROR(err, _vmAddress, slide);
            [self release]; return nil;
        }
    }
    
    return self;
}

//|++++++++++++++++++++++++++++++++++++|//
- (instancetype)initWithParent:(MKBackedNode*)parent error:(NSError**)error
{
    NSParameterAssert(parent);
    
    MKMemoryMap *mapping = parent.memoryMap;
    NSParameterAssert(mapping);
    
    self = [self initWithName:NULL slide:0 flags:0 atAddress:0 inMapping:mapping error:error];
    if (!self) return nil;
    
    objc_storeWeak(&_parent, parent);
    
    return self;
}

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark - Retrieving the Initialization Context
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

@synthesize dataModel = _dataModel;
@synthesize flags = _flags;

//|++++++++++++++++++++++++++++++++++++|//
- (mk_context_t*)context
{ return &_context; }

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark - Getting Image Metadata
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

@synthesize name = _name;
@synthesize slide = _slide;

//|++++++++++++++++++++++++++++++++++++|//
- (BOOL)isFromSharedCache
{
    // 0x80000000 is the private in-shared-cache bit
    return !!(self.header.flags & 0x80000000);
}

//|++++++++++++++++++++++++++++++++++++|//
- (BOOL)isFromMemoryDump
{
    return !!(_flags & MKMachOImageWasProcessedByDYLD);
}

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark - Header and Load Commands
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

@synthesize header = _header;
@synthesize loadCommands = _loadCommands;

//|++++++++++++++++++++++++++++++++++++|//
- (NSArray*)loadCommandsOfType:(uint32_t)type
{ return [self.loadCommands filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"class.ID == %@", @(type)]]; }

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark - MKNode
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

//|++++++++++++++++++++++++++++++++++++|//
- (MKMemoryMap*)memoryMap
{ return _mapping; }

//|++++++++++++++++++++++++++++++++++++|//
- (mk_vm_size_t)nodeSize
{ return 0; }

//|++++++++++++++++++++++++++++++++++++|//
- (mk_vm_address_t)nodeAddress:(MKNodeAddressType)type
{
    switch (type) {
        case MKNodeContextAddress:
            return _contextAddress;
        case MKNodeVMAddress:
            return _vmAddress;
        default:
            @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"Unsupported node address type." userInfo:nil];
    }
}

//|++++++++++++++++++++++++++++++++++++|//
- (MKNodeDescription*)layout
{
    return [MKNodeDescription nodeDescriptionWithParentDescription:super.layout fields:@[
        [MKNodeField nodeFieldWithProperty:MK_PROPERTY(name) description:@"Image Path"],
        [MKNodeField nodeFieldWithProperty:MK_PROPERTY(slide) description:@"Slide"],
        [MKNodeField nodeFieldWithProperty:MK_PROPERTY(header) description:@"Mach-O Header"],
        [MKNodeField nodeFieldWithProperty:MK_PROPERTY(loadCommands) description:@"Load Commands"],
        //[MKNodeField nodeFieldWithProperty:MK_PROPERTY(stringTable) description:@"String Table"],
        //[MKNodeField nodeFieldWithProperty:MK_PROPERTY(symbolTable) description:@"Symbol Table"],
    ]];
}

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark - NSObject
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

//|++++++++++++++++++++++++++++++++++++|//
- (NSString*)description
{ return [NSString stringWithFormat:@"<%@ %p; address = 0x%" MK_VM_PRIxADDR ">", NSStringFromClass(self.class), self, self.nodeContextAddress]; }

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark - mk_context_t
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

//|++++++++++++++++++++++++++++++++++++|//
- (void)_logMessageAtLevel:(mk_logging_level_t)level inFile:(const char*)file line:(int)line function:(const char*)function message:(const char*)message, ...
{
    va_list ap;
    va_start(ap, message);
    CFStringRef str = CFStringCreateWithCString(NULL, message, kCFStringEncodingUTF8);
    CFStringRef messageString = CFStringCreateWithFormatAndArguments(NULL, NULL, str, ap);
    va_end(ap);
    
    id<MKNodeDelegate> delegate = self.delegate;
    if (delegate && [delegate respondsToSelector:@selector(logMessageFromNode:atLevel:inFile:line:function:message:)])
        [delegate logMessageFromNode:self atLevel:level inFile:file line:line function:function message:(NSString*)messageString];
    else
        NSLog(@"MachOKit - [%s][%s:%d]: %@", mk_string_for_logging_level(level), file, line, messageString);
    
    CFRelease(messageString);
    CFRelease(str);
}

@end
