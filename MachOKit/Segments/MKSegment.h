//----------------------------------------------------------------------------//
//|
//|             MachOKit - A Lightweight Mach-O Parsing Library
//! @file       MKSegment.h
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

#import <MachOKit/MKBackedNode.h>
#import <MachOKit/MKLCSegment.h>
#import <MachOKit/MKSection.h>

@class MKMachOImage;

//----------------------------------------------------------------------------//
//! @name       Segment Flags
//! @relates    MKSegment
//!
//
typedef NS_OPTIONS(uint32_t, MKSegmentFlags) {
    //! the file contents for this segment is for the high part of the VM
    //! space, the low part is zero filled (for stacks in core files)
    MKSegmentHighVM                             = SG_HIGHVM,
    //! This segment is the VM that is allocated by a fixed VM library, for
    //! overlap checking in the link editor
    MKSegmentFixedVM                            = SG_FVMLIB,
    //! this segment has nothing that was relocated in it and nothing
    //! relocated to it, that is it maybe safely replaced without relocation.
    MKSegmentNoRelocations                      = SG_NORELOC,
    //! This segment is protected.  If the segment starts at file offset 0,
    //! the first page of the segment is not protected.  All other pages of
    //! the segment are protected.
    MKSegmentProtectedV1                        = SG_PROTECTED_VERSION_1,
};



//----------------------------------------------------------------------------//
//! An instance of \c MKSegment represents a contiguous range of memory mapped
//! from a Mach-O binary into memory when the image is loaded.  Each segment
//! is identified by an instance of \ref MKLCSegment or \ref MKLCSegment64 in
//! the list of load commands.
//
@interface MKSegment : MKBackedNode {
@package
    mk_vm_address_t _nodeContextAddress;
    mk_vm_size_t _nodeContextSize;
    //
    NSString *_name;
    id<MKLCSegment> _loadCommand;
    NSSet *_sections;
    //
    mk_vm_address_t _vmAddress;
    mk_vm_size_t _vmSize;
    mk_vm_address_t _fileOffset;
    mk_vm_size_t _fileSize;
    vm_prot_t _maximumProtection;
    vm_prot_t _initialProtection;
    MKSegmentFlags _flags;
}

+ (uint32_t)canInstantiateWithSegmentLoadCommand:(id<MKLCSegment>)segmentLoadCommand;

+ (Class)classForSegmentLoadCommand:(id<MKLCSegment>)segmentLoadCommand;

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark -  Creating a Segment
//! @name       Creating a Segment
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

+ (instancetype)segmentWithLoadCommand:(id<MKLCSegment>)segmentLoadCommand error:(NSError**)error;

- (instancetype)initWithLoadCommand:(id<MKLCSegment>)segmentLoadCommand error:(NSError**)error  NS_DESIGNATED_INITIALIZER;

//! The name of this segment, as specified in the load command.
@property (nonatomic, readonly) NSString *name;
//! The load command identifying this segment.
@property (nonatomic, readonly) id<MKLCSegment> loadCommand;

@property (nonatomic, readonly) mk_vm_address_t vmAddress;
@property (nonatomic, readonly) mk_vm_size_t vmSize;
@property (nonatomic, readonly) mk_vm_address_t fileOffset;
@property (nonatomic, readonly) mk_vm_size_t fileSize;

@property (nonatomic, readonly) vm_prot_t maximumProtection;
@property (nonatomic, readonly) vm_prot_t initialProtection;

@property (nonatomic, readonly) MKSegmentFlags flags;

//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//
#pragma mark -  Sections
//! @name       Sections
//◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦◦//

//! A set of \ref MKSection instances, each representing a section within
//! this segment.
@property (nonatomic, readonly) NSSet /*MKSection*/ *sections;

//! Returns the \ref MKSection from the \ref sections array that is identified
//! by the provided load command.
- (MKSection*)sectionForLoadCommand:(id<MKLCSection>)sectionLoadCommand;

@end
