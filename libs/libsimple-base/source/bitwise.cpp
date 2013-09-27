/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2010-2013, Adam Rehn
//
//  ---
//
//  Bitwise Functionality
//
//  These functions deal with bitwise operations, and mainly focus on
//  making up for the inability to natively deal with individual bits.
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
#include "bitwise.h"

//Isolates bit (n) from a byte, from 1 to 8 inclusive
unsigned char isolate_bit(unsigned char c, unsigned int n)
{
	//Shift left to obliterate the leftmost bits
	c = c << (BITS_IN_OCTET - n);
	
	//Shift right to obliterate the rightmost bits
	c = c >> (BITS_IN_OCTET - 1);
	
	//Return the isolated bit
	return c;
}
