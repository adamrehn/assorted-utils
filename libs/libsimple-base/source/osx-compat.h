/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2012-2013, Adam Rehn
//
//  ---
//
//  OSX Compatibility Include
//
//  This header contains the function prototypes for the components in
//  libadam-osx-compat.so
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
#ifndef _LIB_SIMPLE_BASE_OSX_COMPAT_H
#define _LIB_SIMPLE_BASE_OSX_COMPAT_H

extern "C"
{

//These functions are for OSX only, and are not applicable to iOS
#if defined __APPLE__ && defined __MACH__
	#include "TargetConditionals.h"
	#ifndef TARGET_OS_IPHONE
		
		void MonitorDirectoryForFileWrites(const char* dir, bool(*callback)(void));
		
	#endif
#endif

}

#endif
