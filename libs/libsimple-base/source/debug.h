/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2010-2013, Adam Rehn
//
//  ---
//
//  Debug-Related Functionality
//
//  Functionality to facilitate debug messages that can easily be switched off
//  for compiling the release version of an application.
//  
//  In order to enable debugging, either use the preprocessor directive
//  
//  #define DEBUG
//  
//  prior to the inclusion of this header file, or add "-D DEBUG" to the
//  compiler options during compilation.
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
#ifndef _LIB_SIMPLE_BASE_DEBUG_H
#define _LIB_SIMPLE_BASE_DEBUG_H

//Debug output goes to clog, so we need iostream
#include <iostream>

//The blank macro removes the debug message calls in release versions
#ifndef DEBUG
	#define debug(x)
#else
	#define debug(x) std::clog << x
#endif

#endif
