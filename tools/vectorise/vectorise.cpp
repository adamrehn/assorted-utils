/*
//  Command Vectoriser
//  Copyright (c) 2015, Adam Rehn
//  
//  ---
//  
//  This utility invokes the specified command once for each specified input file.
//  This is useful when invoking a program that does not natively support processing
//  multiple input files.
//  
//  Usage Syntax:    vectorise <COMMAND> <FILE/PATTERN> <FILE/PATTERN> ...
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
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <simple-base/base.h>

using namespace std;

int main (int argc, char* argv[])
{
	if (argc > 2)
	{
		string command = argv[1];
		vector<string> files;
		
		//Expand any wildcards in the input file list
		for (int i = 2; i < argc; ++i)
		{
			string currFile = argv[i];
			if (in("*", currFile))
			{
				vector<string> expanded = glob(currFile);
				files.insert(files.end(), expanded.begin(), expanded.end());
			}
			else {
				files.push_back(currFile);
			}
		}
		
		//Invoke the command once for each input file
		for (vector<string>::iterator currFile = files.begin(); currFile != files.end(); ++currFile)
		{
			clog << "Invoking for " << *currFile << "..." << endl;
			system(string(command + " \"" + *currFile + "\"").c_str());
		}
		
		//Report the total number of invocations
		clog << "Invoked " << files.size() << " time(s)." << endl;
	}
	else {
		clog << "Usage syntax:\n" << argv[0] << " <COMMAND> <FILE/PATTERN> <FILE/PATTERN> ..." << endl;
	}
	
	return 0;
}
