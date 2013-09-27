/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2011-2013, Adam Rehn
//
//  ---
//
//  Applcation Startup Args Parser Class
//
//  This class parses argc and argv to provide a few useful functions based
//  on the values therein.
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
#ifndef _LIB_SIMPLE_BASE_ARGSPARSER_H
#define _LIB_SIMPLE_BASE_ARGSPARSER_H

#include <string>
using std::string;

class StartupArgsParser
{
	public:
		//Constructor, creates copies argc and argv
		StartupArgsParser(int argc, char** argv);
		
		//Destructor
		~StartupArgsParser();
		
		//Retrieve the directory the app is in (empty string when invoked by name instead of full path)
		string appDir();
		
		//Retrieve the application executable name (does not resolve symlinks)
		string appName();
		
		//Reconstruct the invocation string used to invoke the application
		string invocationString();
		
	private:
		//Our copies of argc and argv
		int argc;
		char** argv;
};

#endif
