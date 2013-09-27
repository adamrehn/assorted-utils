/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2009-2013, Adam Rehn
//
//  ---
//
//  File Manipulation Functions
//
//  Functionality for working with files. Several functions mimic PHP
//  functionality.
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
#ifndef _LIB_SIMPLE_BASE_FILE_H
#define _LIB_SIMPLE_BASE_FILE_H

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::stringstream;

//Implementations of PHP Functions (std::string is utilised in a binary-safe manner when dealing with data)
string file_get_contents  (const string& path);
bool   file_exists        (const string& path);
bool   is_dir             (const string& path);
off_t  filesize           (const string& path);
bool   file_put_contents  (const string& path, const string& data);

time_t    file_last_modified (const string& path);            //Returns the last-modified timestamp of the specified file
void      write_random_bytes (ofstream& outfile, int number); //Writes the specified number of random bytes to the supplied ofstream.
bool      at_end             (ifstream& stream);              //Checks if the read pointer for the supplied stream is at the end
size_t    end_pos            (ifstream& stream);              //Retrieves the end position of the supplied stream
bool      make_dir           (const string& path);            //Creates a directory, creating parent directories as needed

#endif
