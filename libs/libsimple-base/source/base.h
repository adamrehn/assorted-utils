/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2009-2013, Adam Rehn
//
//  ---
//
//  Base Library Main Include
//
//  Aggregate library header file.
//
//  ---
//
//  This file is part of the Simple Base Library for C++ (libsimple-base).
//
//  libsimple-base is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with libsimple-base. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _LIB_SIMPLE_BASE_H
#define _LIB_SIMPLE_BASE_H

//When using Visual Studio, Winsock2.h must be included before windows.h
#ifdef _MSC_VER
	#include <Winsock2.h>
#endif

//Include all of the base functions
#include "array_manipulation.h"
#include "binary_manipulation.h"
#include "bitwise.h"
#include "checksum.h"
#include "crc32.h"
#include "debug.h"
#include "endianness.h"
#include "environment.h"
#include "file_manipulation.h"
#include "maths.h"
#include "multiple_input_files.h"
#include "pointer_manipulation.h"
#include "random.h"
#include "string_manipulation.h"
#include "time.h"

//Include all of the base classes
#include "StartupArgsParser.h"
#include "DynamicLibrary.h"

//SHA-1 implementation Copyright (C) 1998, 2009 Paul E. Jones <paulej@packetizer.com>
//From <http://www.packetizer.com/security/sha1>
//Listed as "Freeware Public License (FPL)", which appears to be LGPL-compatible
#include "sha1.h"

//SimpleGlob (for systems without native glob)
//Copyright (c) 2006-2007, Brodie Thiesfield
//<http://code.jellycan.com/simpleopt/>
#include "SimpleGlob.h"

//A null-byte character that can be accessed from anywhere
extern char* NULL_BYTE;

#endif
