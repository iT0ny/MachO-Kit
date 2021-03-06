//----------------------------------------------------------------------------//
//|
//|             MachOKit - A Lightweight Mach-O Parsing Library
//! @file       MKMachO.h
//!
//! @author     D.V.
//! @copyright  Copyright (c) 2014-2015 D.V. All rights reserved.
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

#include <MachOKit/macho.h>
@import Foundation;

#import <MachOKit/MKNode+MachO.h>
#import <MachOKit/MKBackedNode.h>
#import <MachOKit/MKmemoryMap.h>

@protocol MKLCSegment;
@class MKMachOImage;
@class MKMachHeader;
@class MKStringTable;
@class MKSymbolTable;
@class MKIndirectSymbolTable;

//----------------------------------------------------------------------------//
//! @name       Mach-O Image Options
//! @relates    MKMachOImage
//
typedef NS_OPTIONS(NSUInteger, MKMachOImageFlags) {
    //! The Mach-O image has been processed by the dynamic linker.
    MKMachOImageWasProcessedByDYLD      = 0x1
};



//----------------------------------------------------------------------------//
//! An instance of \c MKMachOImage parses a single Mach-O image.
//
@interface MKMachOImage : MKBackedNode {
@package
    mk_context_t _context;
    MKMemoryMap *_mapping;
    id<MKDataModel> _dataModel;
    MKMachOImageFlags _flags;
    NSString *_name;
    // Address //
    mk_vm_address_t _contextAddress;
    mk_vm_address_t _fileAddress;
    mk_vm_address_t _vmAddress;
    intptr_t _slide;
    // Header //
    MKMachHeader *_header;
    NSArray *_loadCommands;
    // Segments //
    NSDictionary *_segments;
    // Symbols //
    MKStringTable *_stringTable;
    MKSymbolTable *_symbolTable;
    MKIndirectSymbolTable *_indirectSymbolTable;
}

- (instancetype)initWithName:(const char*)name slide:(intptr_t)slide flags:(MKMachOImageFlags)flags atAddress:(mk_vm_address_t)contextAddress inMapping:(MKMemoryMap*)mapping error:(NSError**)error NS_DESIGNATED_INITIALIZER;

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark -  Retrieving the Initialization Context
//! @name       Retrieving the Initialization Context
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

//! The \ref <MKDataModel> that this Mach-O image was initialized with.
@property (nonatomic, readonly) id<MKDataModel> dataModel;
//! The context that this Mach-O image was initialized with.
@property (nonatomic, readonly) mk_context_t *context;
//! The flags that this Mach-O image was initialized with.
@property (nonatomic, readonly) MKMachOImageFlags flags;

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark -  Getting Image Metadata
//! @name       Getting Image Metadata
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

//! The name that this Mach-O image was initialized with.
@property (nonatomic, readonly) NSString *name;
//! The slide value that this Mach-O image was initialized with.
@property (nonatomic, readonly) intptr_t slide;

//! Indicates whether this Mach-O image is from dyld's shared cache.
@property (nonatomic, readonly) BOOL isFromSharedCache;

//! Indicates whether this Mach-O image is from a memory dump (or live memory).
@property (nonatomic, readonly) BOOL isFromMemoryDump;

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark -  Header and Load Commands
//! @name       Header and Load Commands
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

@property (nonatomic, readonly) MKMachHeader *header;

//! An array containing instances of \ref MKLoadCommand, each representing a
//! load commands from this Mach-O image.  Load commands are ordered as they
//! appear in the Mach-O header.  The count of the returned array may be less
//! than the value of ncmds in the \ref header, if the Mach-O is malformed
//! and trailing load commands could not be accessed.
@property (nonatomic, readonly) NSArray /*MKLoadCommand*/ *loadCommands;

//! Filters the \ref loadCommands array to those of the specified \a type
//! and returns the result.  The relative ordering of the returned load
//! commands is preserved.
- (NSArray*)loadCommandsOfType:(uint32_t)type;

@end

