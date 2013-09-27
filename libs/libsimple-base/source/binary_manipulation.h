/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2009-2013, Adam Rehn
//
//  ---
//
//  Binary Manipulation Functions
//
//  Functions for manipulating raw binary data.
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
#ifndef _LIB_SIMPLE_BASE_BINARY_H
#define _LIB_SIMPLE_BASE_BINARY_H

#include <string>
#include <sstream>
using std::string;
using std::stringstream;
using std::hex;

string byte_to_hex(char byte, bool prefix = true);    //Converts a single byte to hexadecimal, prefixing with 0x and padding numbers under 10 (0xXX)
string bin_to_hex(const char* bytes, int length);     //Converts a C-String to hexadecimal, prefixing with 0x and padding numbers under 10 (0xXX)
string bin_to_hex(string data);                       //Converts a string of bytes into a string of hex values. The reultant string will be twice the length of the original.
string bin_to_hex(unsigned int numbers[5]);           //Version of bin_to_hex() designed specifically with SHA-1 Message Digests in mind
string hex(const char* bytes, int length);            //Bytes to hex, no prefixes
string hex_with_ascii(const char* bytes, int length); //Converts non-printable characters to hexadecimal, but leaves printable characters intact

#endif
