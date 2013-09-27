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
#include "binary_manipulation.h"

//The null-byte character
static char nullbyte[1] = {0};
char* NULL_BYTE = (char*)nullbyte;

//Function Definitions
string byte_to_hex(char byte, bool prefix)
{
	//Temporary stringstream to hold the hex value
	stringstream temp;
	
	//Insert the byte
	temp << hex << (unsigned int)(unsigned char)byte;
	
	//Add any padding, if neccesary, and return the string
	if (temp.str().length() < 2)
	{
		if (prefix) {
			return "0x0" + temp.str();
		}
		else {
			return "0" + temp.str();
		}
	}
	else
	{
		if (prefix) {
			return "0x" + temp.str();
		}
		else {
			return temp.str();
		}
	}
}

string bin_to_hex(const char* bytes, int length)
{
	//Temporary stringstream to hold the hex values
	stringstream temp;
	
	//Loop through each of the bytes
	for (int i = 0; i < length; ++i)
		temp << byte_to_hex(bytes[i]) << " ";
	
	//Return the contents of the stringstream
	return temp.str();
}

string hex(const char* bytes, int length)
{
	//Temporary stringstream to hold the hex values
	stringstream temp;
	
	//Loop through each of the bytes
	for (int i = 0; i < length; ++i) {
		temp << byte_to_hex(bytes[i], false);
	}
	
	//Return the contents of the stringstream
	return temp.str();
}

string bin_to_hex(string data)
{
	//Temporary stringstream to hold the hex values
	stringstream temp;
	temp << hex;
	
	//Calculate the hex of each of the bytes
	//(we need to typecast to unsigned chars because negative values make no sense in this context)
	for (size_t i = 0; i < data.length(); ++i)
		temp << (int)(unsigned char)data[i];
	
	//Return the contents of the stringstream
	return temp.str();
}

string bin_to_hex(unsigned int numbers[5])
{
	//Temporary stringstream to hold the hex values
	stringstream temp;
	
	//Loop through the integers and convert each of their bytes to hex
	for (int i = 0; i < 5; ++i)
		temp << hex((char*)&numbers[i], sizeof(unsigned int));
	
	//Return the contents of the stringstream
	return temp.str();
}

string hex_with_ascii(const char* bytes, int length)
{
	//Temporary stringstream to hold the hex values
	stringstream temp;
	
	//Loop through the characters and convert them where neccessary
	for (int i = 0; i < length; ++i)
	{
		if (bytes[i] <= 31 || bytes[i] >= 127) {
			temp << byte_to_hex(bytes[i]);
		}
		else {
			temp << bytes[i];
		}
	}
	
	//Return the contents of the stringstream
	return temp.str();
}
