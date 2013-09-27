/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2012-2013, Adam Rehn
//
//  ---
//
//  Random Number Generation Functions
//
//  These functions abstract platform-specific facilities for pseudorandom
//  number generation.
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
#ifndef _LIB_SIMPLE_BASE_RANDOM_H
#define _LIB_SIMPLE_BASE_RANDOM_H

//For size_t declaration
#include <cstring>

//Generates a sequence of random bytes.
//Under Unix-based systems, the useBestEntropy argument switches between /dev/random (true) and /dev/urandom (false)
//Under Windows, the useBestEntropy argument is ignored.
bool GenerateRandomBytes(char* outputBuffer, size_t numBytes, bool useBestEntropy = false);

#endif
