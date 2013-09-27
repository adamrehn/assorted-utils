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
#include "file_manipulation.h"
#include "string_manipulation.h"

#ifdef _WIN32
	#include <direct.h>
#endif

//Implementations of PHP Functions (std::string is utilised in a binary-safe manner when dealing with data)

string file_get_contents(const string& path)
{
	//Open the specified input file in binary mode
	ifstream infile(path.c_str(), ios::binary);
	
	//Create a string to hold the file contents
	string contents;
	
	//Check to ensure that the file opened correctly
	if (infile.is_open())
	{
		//Read the contents one chunk at a time
		char buffer[1024*1024];
		size_t bytesRead = 0;
		while ( (bytesRead = infile.read(buffer, sizeof(buffer)).gcount()) != 0 )
		{
			//Append the data to the string
			contents.append(buffer, bytesRead);
		}
		
		//Close the input file
		infile.close();
	}
	
	//Return the data
	return contents;
}

//Determines if a file/folder exists
bool file_exists(const string& path)
{
	//Attempt to get the status of the file
	struct stat fileInfo;
	if (stat(path.c_str(), &fileInfo) == 0) {
		return true;
	}
	
	//The specified file/folder does not exist
	return false;
}

//Determines if a specified filesystem entry is a directory
bool is_dir(const string& path)
{
	//Get the status of the file
	struct stat fileInfo;
	if (stat(path.c_str(), &fileInfo) == 0)
	{
		//File opened, check if it is a directory
		#if defined(_WIN32) && defined(_MSC_VER) && !defined(__MINGW32__)
		if (fileInfo.st_mode & _S_IFDIR) {
		#else
		if (S_ISDIR(fileInfo.st_mode)) {
		#endif
			return true;
		}
	}
	
	//Not a directory
	return false;
}

//Determines the size in bytes of a file
off_t filesize (const string& path)
{
	//Get the status of the file
	struct stat fileInfo;
	if (stat(path.c_str(), &fileInfo) == 0)
	{
		//File opened, check if it is a regular file
		#if defined(_WIN32) && defined(_MSC_VER)
		if (fileInfo.st_mode & _S_IFREG)
		#else
		if (S_ISREG(fileInfo.st_mode))
		#endif
		{
			//Return the filesize in bytes
			return fileInfo.st_size;
		}
	}
	
	//Could not determine the filesize
	return -1;
}

bool file_put_contents(const string& path, const string& data)
{
	//Opens the specified output file in binary mode, overwriting the file if it already exists
	ofstream outfile(path.c_str(), ios::binary);
	
	//Check to ensure that the file opened correctly
	if (outfile.is_open())
	{
		//Write the data to the file
		outfile.write(data.data(), data.length());
		
		//Close the file
		outfile.close();
		return true;
	}
	else
	{
		//The file couldn't be opened
		return false;
	}
}


//My Functions

time_t file_last_modified(const string& path)
{
	//Get the status of the file
	struct stat fileInfo;
	if (stat(path.c_str(), &fileInfo) == 0)
	{
		//File opened, retrieve the last modified timestamp
		return fileInfo.st_mtime;
	}
	
	//Could not determine the last modified timestamp
	return 0;
}

void write_random_bytes(ofstream& outfile, int number)
{
	if (outfile.is_open())
	{
		//Seed the random number generator
		srand(time(NULL));
		for (int i = 0; i < number; ++i)
		{
			//Write a random byte
			char r = rand() % 256;
			outfile.write(&r, 1);
		}
	}
}

bool at_end(ifstream& stream)
{
	//Return whether or not the current position is the same as the end position
	if (stream.is_open()) {
		return (size_t)stream.tellg() == end_pos(stream);
	}
	else {
		return false;
	}
}

size_t end_pos(ifstream& stream)
{
	if (stream.is_open())
	{
		//Keep track of the current position so that we can return to it
		size_t current_pos = stream.tellg();
		
		//Seek to the end and retrieve the end position
		stream.seekg(0, ios::end);
		size_t end_pos = stream.tellg();
		
		//Seek back to the original position
		stream.seekg(current_pos, ios::beg);
		
		//Return the end position
		return end_pos;
	}
	else {
		return 0;
	}
}

bool make_dir(const string& path)
{
	//Break the path into directory components
	vector<string> components = explode("/", str_replace("\\", "/", path));
	
	//Iterate through the components (exclude the root directory/drive letter)
	for (unsigned int i = 1; i < components.size(); ++i)
	{
		//Attempt to create each directory in the path if it does not exist
		//Since the second argument to vector::vector is the end iterator, we add 1 to i, making the vector 0 to i inclusive
		string currDir = implode("/", vector<string>(components.begin(), components.begin() + (i+1)));
		if (!file_exists(currDir))
		{
			#ifdef _WIN32
			if (_mkdir(currDir.c_str()) == -1)
			#else
			if (mkdir(currDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == -1)
			#endif
			{
				return false;
			}
		}
	}
	
	//All directories successfully created
	return true;
}

