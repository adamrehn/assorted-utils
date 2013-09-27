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
#ifndef _LIB_SIMPLE_BASE_MULTI_INPUT_H
#define _LIB_SIMPLE_BASE_MULTI_INPUT_H

//We are utilising vectors of strings
#include <string>
#include <vector>
using std::string;
using std::vector;

//Utility function to expand wildcards using glob, used by the function below
vector<string> expandWildcards(const string& pattern);

/*
	There are several different ways to use this function.
	
	For "app -i INFILE" style with a delim, use
	gatherInputFiles(argc, argv, "-i")
	
	For "app FIXEDARG FIXEDARG INFILES..." style with N fixed args, use
	gatherInputFiles(argc-N, argv+N, "")
*/
vector<string> gatherInputFiles(int argc, char* argv[], const string& infileDelim);

#endif
