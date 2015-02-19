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
//  The following backreference-style tokens are supported in the command:
//  
//    $0    The filename
//    $1    The basename of the filename
//    $2    The basename of the filename with the file extension removed
//    $3    The file extension of the filename
//  
//  If $0 is not included in the command, the filename will be appended at the end.
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

void InvokeForFile(const string& command, const string& file)
{
	//Determine if $0 appears in the command
	bool shouldAppendFilename = (!in("$0", command));
	
	//Expand any backreference-style tokens in the command
	string expandedCommand = command;
	expandedCommand = str_replace("$0", file,                            expandedCommand);
	expandedCommand = str_replace("$1", basename(file),                  expandedCommand);
	expandedCommand = str_replace("$2", strip_extension(basename(file)), expandedCommand);
	expandedCommand = str_replace("$3", get_extension(basename(file)),   expandedCommand);
	
	//If $0 did not appear in the command, append the filename to the end
	if (shouldAppendFilename) {
		expandedCommand += " \"" + file + "\"";
	}
	
	//Invoke the expanded command
	system(expandedCommand.c_str());
}

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
			InvokeForFile(command, *currFile);
		}
		
		//Report the total number of invocations
		clog << "Invoked " << files.size() << " time(s)." << endl;
	}
	else
	{
		clog << "Usage syntax:\n" << argv[0] << " <COMMAND> <FILE/PATTERN> <FILE/PATTERN> ..." << endl << endl
		     << "The following backreference-style tokens are supported in the command:" << endl << endl
		     << "  $0    The filename" << endl
		     << "  $1    The basename of the filename" << endl
		     << "  $2    The basename of the filename with the file extension removed" << endl
		     << "  $3    The file extension of the filename" << endl << endl
		     << "If $0 is not included in the command, the filename will be appended at the end." << endl;
	}
	
	return 0;
}
