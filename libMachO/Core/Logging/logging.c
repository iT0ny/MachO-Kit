//----------------------------------------------------------------------------//
//|
//|             MachOKit - A Lightweight Mach-O Parsing Library
//|             logging.c
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

#include "core_internal.h"

mk_logging_level_t mk_logging_level = MK_LOGGING_LEVEL;

//|++++++++++++++++++++++++++++++++++++|//
const char* mk_string_for_logging_level(mk_logging_level_t level)
{
    const char *levelString;
    switch (level) {
        case MK_LOGGING_LEVEL_TRACE:
            levelString = "TRACE";
            break;
        case MK_LOGGING_LEVEL_DEBUG:
            levelString = "TRACE";
            break;
        case MK_LOGGING_LEVEL_INFO:
            levelString = "INFO";
            break;
        case MK_LOGGING_LEVEL_WARNING:
            levelString = "WARNING";
            break;
        case MK_LOGGING_LEVEL_ERROR:
            levelString = "ERROR";
            break;
        case MK_LOGGING_LEVEL_CRITICAL:
            levelString = "CRITICAL";
            break;
        case MK_LOGGING_LEVEL_FATAL:
            levelString = "FATAL";
            break;
        default:
            levelString = "UNKNOWN";
            break;
    }
    return levelString;
}