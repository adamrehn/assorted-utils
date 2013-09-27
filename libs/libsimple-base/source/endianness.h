/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2010-2013, Adam Rehn
//
//  ---
//
//  Endianness-related Functionality
//
//  Functionality for detecting system endianness and performing conversion.
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
#ifndef _LIB_SIMPLE_BASE_ENDIANNESS_H
#define _LIB_SIMPLE_BASE_ENDIANNESS_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
	//We need to define these ourselves under windows
	#define LITTLE_ENDIAN 1234
	#define BIG_ENDIAN    4321
#else
	//Under POSIX systems we already have LITTLE_ENDIAN and BIG_ENDIAN defined for us
	#if defined __APPLE__ && defined __MACH__
		#include <machine/endian.h>
	#else
		#include <endian.h>
	#endif
#endif

//Function to detect endianness at runtime, see function definitions file for details
int endianness();

//Function for flipping an array of bytes
void flipBytes(char* bytes, unsigned int n);

//Template function for flipping the endianness of integral datatypes
template <typename T>
T flipEndianness (const T& original)
{
	//Create a new instance of the datatype
	T newVar;
	
	//Create pointers for convenience
	uint8_t* oldBytes = (uint8_t*)&original;
	uint8_t* newBytes = (uint8_t*)&newVar;
	
	//Copy the bytes in reverse order
	size_t length = sizeof(T);
	for (size_t i = 0; i < length; ++i) {
		newBytes[(length-1) - i] = oldBytes[i];
	}
	
	//Return the new instance of the datatype
	return newVar;
}

//Template functions for flipping the endianness of integral datatypes, only when necessary
template <typename T>
T fromLittleEndian(const T& original)
{
	return ( (endianness() == LITTLE_ENDIAN) ? original : flipEndianness(original) );
}

template <typename T>
T fromBigEndian(const T& original)
{
	return ( (endianness() == BIG_ENDIAN) ? original : flipEndianness(original) );
}

//Hopefully the compiler will perform inlining so these functions effectively become aliases
template <typename T>
T toLittleEndian(const T& original) {
	return fromLittleEndian(original);
}

template <typename T>
T toBigEndian(const T& original) {
	return fromBigEndian(original);
}

#endif
