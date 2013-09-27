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
#include "endianness.h"

//For memcpy()
#include <cstring>

//Function to detect endianness at runtime, adapted from
//<http://www.ibm.com/developerworks/aix/library/au-endianc/index.html?ca=drs->
int endianness()
{
	uint16_t i = 1;
	uint8_t *p = (uint8_t*)&i;
	
	return ( (*p == 1) ? LITTLE_ENDIAN : BIG_ENDIAN );
}

//Function for flipping an array of bytes
void flipBytes(char* bytes, unsigned int n)
{
	//Create a copy of the byte array
	char* bytesCopy = new char[n];
	memcpy(bytesCopy, bytes, n);
	
	//Copy the bytes back to the original array in reverse order
	for (unsigned int i = 0; i < n; ++i) {
		bytes[i] = bytesCopy[(n-1) - i];
	}
	
	//Free the copy array
	delete[] bytesCopy;
}
