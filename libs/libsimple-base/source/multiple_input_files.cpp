/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2010-2013, Adam Rehn
//
//  ---
//
//  Handing Multiple Input Files
//
//  The functionality offered here is for handling multiple input files,
//  supporting wildcard expansion and custom delimiters.
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
#include "multiple_input_files.h"

//We need these for globbing and vector merging
#include "array_manipulation.h"
#include "environment.h"

vector<string> expandWildcards(const string& pattern)
{
	//Check to see if the pattern string contains any wildcards
	if (pattern.find("*") != string::npos)
	{
		//Return the result of globbing the pattern
		return glob(pattern);
	}
	else
	{
		//Return a vector containing the pattern
		vector<string> result;
		result.push_back(pattern);
		return result;
	}
}

vector<string> gatherInputFiles(int argc, char* argv[], const string& infileDelim)
{
	//Create a vector to hold the list of input files
	vector<string> inputFiles;
	
	//Loop through the arguments (ignoring argv[0], which is the application itself)
	for (int i = 1; i < argc; ++i)
	{
		//If a delimiter was specified, we need to check for it
		if (infileDelim != "")
		{
			//Check for the delimiter
			if (string(argv[i]) == infileDelim && i < argc-1)
			{
				//The next argument is treated as an input file
				mergeVectors(inputFiles, expandWildcards(argv[++i]));
			}
		}
		else
		{
			//No delimiter was supplied, so every argument is treated as an input file
			mergeVectors(inputFiles, expandWildcards(argv[i]));
		}
	}
	
	//Return the list of input files
	return inputFiles;
}
