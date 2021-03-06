//----------------------------------------------------------------------------//
//|
//|             MachOKit - A Lightweight Mach-O Parsing Library
//! @file       MachOKit.h
//!
//! @author     D.V.
//! @copyright  Copyright (c) 2014-2015 D.V. All rights reserved.
//!
//! @brief
//! The root include for MachOKit.
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

#ifndef _MachOKit_H
#define _MachOKit_H

#include <MachOKit/macho.h>

#import <MachOKit/NSError+MK.h>

#import <MachOKit/MKMemoryMap.h>
#import <MachOKit/MKDataModel.h>
#import <MachOKit/MKNodeDescription.h>
#import <MachOKit/MKNode.h>
#import <MachOKit/MKBackedNode.h>
#import <MachOKit/MKOffsetNode.h>

#import <MachOKit/MKFatBinary.h>
#import <MachOKit/MKFatArch.h>

#import <MachOKit/MKMachO.h>
#import <MachOKit/MKMachO+Segments.h>
#import <MachOKit/MKMachO+Symbols.h>
#import <MachOKit/MKMachHeader.h>
#import <MachOKit/MKMachHeader64.h>
#import <MachOKit/MKLoadCommand.h>
    #import <MachOKit/MKDylibLoadCommand.h>
    #import <MachOKit/MKDylinkerLoadCommand.h>
    #import <MachOKit/MKLinkEditDataLoadCommand.h>
    #import <MachOKit/MKMinVersionLoadCommand.h>
    #import <MachOKit/MKLCSegment.h>
    #import <MachOKit/MKLCSymtab.h>
    #import <MachOKit/MKLCDysymtab.h>
    #import <MachOKit/MKLCLoadDylib.h>
    #import <MachOKit/MKLCIDDylib.h>
    #import <MachOKit/MKLCLoadDylinker.h>
    #import <MachOKit/MKLCIDDylinker.h>
    #import <MachOKit/MKLCRoutines.h>
    #import <MachOKit/MKLCSubFramework.h>
    #import <MachOKit/MKLCSubClient.h>
    #import <MachOKit/MKLCSubLibrary.h>
    #import <MachOKit/MKLCTwoLevelHints.h>
    #import <MachOKit/MKLCPrebindChecksum.h>
    #import <MachOKit/MKLCLoadWeakDylib.h>
    #import <MachOKit/MKLCSegment64.h>
    #import <MachOKit/MKLCRoutines64.h>
    #import <MachOKit/MKLCUUID.h>
    #import <MachOKit/MKLCRPath.h>
    #import <MachOKit/MKLCCodeSignature.h>
    #import <MachOKit/MKLCSegmentSplitInfo.h>
    #import <MachOKit/MKLCReExportDylib.h>
    #import <MachOKit/MKLCEncryptionInfo.h>
    #import <MachOKit/MKLCDyldInfo.h>
    #import <MachOKit/MKLCDyldInfoOnly.h>
    #import <MachOKit/MKLCLoadUpwardDylib.h>
    #import <MachOKit/MKLCVersionMinMacOSX.h>
    #import <MachOKit/MKLCVersionMiniPhoneOS.h>
    #import <MachOKit/MKLCFunctionStarts.h>
    #import <MachOKit/MKLCDyldEnvironment.h>
    #import <MachOKit/MKLCMain.h>
    #import <MachOKit/MKLCDataInCode.h>
    #import <MachOKit/MKLCSourceVersion.h>
    #import <MachOKit/MKLCDylibCodeSignDrs.h>
    #import <MachOKit/MKLCEncryptionInfo64.h>

#import <MachOKit/MKSegment.h>
#import <MachOKit/MKSection.h>
    #import <MachOKit/MKStubsSection.h>
    #import <MachOKit/MKCStringSection.h>
    #import <MachOKit/MKIndirectPointersSection.h>

#import <MachOKit/MKStringTable.h>
#import <MachOKit/MKSymbolTable.h>
    #import <MachOKit/MKSymbol.h>
#import <MachOKit/MKIndirectSymbolTable.h>
    #import <MachOKit/MKIndirectSymbol.h>

#endif /* _MachOKit_H */
