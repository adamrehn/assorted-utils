/*
//  Static Library Merging Tool (mergelib)
//  Copyright (c) 2009-2013, Adam Rehn
//
//  ---
//
//  This utility merges one or more static libraries into a single output
//  static library, utilising "libtool" under Darwin and "ar" under all
//  other platforms.
//
//  Usage Syntax:       mergelib OUTLIB LIB1 [LIB2] [...] [LIBN]
//
//  ---
//
//  This file is part of the "mergelib" Static Library Merging Tool.
//
//  mergelib is free software: you can redistribute it and/or modify it
//  under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with mergelib. If not, see <http://www.gnu.org/licenses/>.
*/
#include <simple-base/base.h>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
using std::clog;
using std::endl;
using std::string;
using std::vector;

#include "StaticLibraryManager.h"

int main (int argc, char* argv[])
{
	if (argc > 2)
	{
		string tmpdir = generate_unique_filename();
		string outlib = argv[1];
		vector<string> libs(argv + 2, argv + argc);
		
		//Under Darwin, libtool can merge static libraries for us
		#if defined __APPLE__ && defined __MACH__
			
			//Build the libtool command
			string command = "libtool -static -o \"" + outlib + "\"";
			for (vector<string>::iterator currLib = libs.begin(); currLib != libs.end(); ++currLib) {
				command += " \"" + *currLib + "\"";
			}
			
			//Execute libtool
			clog << command << endl;
			system(command.c_str());
			
		#else
			
			try
			{
				//Extract the object files from the input libraries using ar
				StaticLibraryManager manager(tmpdir);
				for (vector<string>::iterator currLib = libs.begin(); currLib != libs.end(); ++currLib) {
					manager.AddLibrary(*currLib);
				}
				
				//Create a new archive containing the extracted libraries using ar
				manager.MergeLibraries(outlib);
			}
			catch (std::runtime_error& e)
			{
				clog << "Error: " << e.what() << endl;
				return -1;
			}
			
		#endif
		
	}
	else {
		clog << "Usage Syntax:" << endl << "mergelib OUTLIB LIB1 LIB2 ... LIBN" << endl;
	}
	
	return 0;
}
