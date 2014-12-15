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
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
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
