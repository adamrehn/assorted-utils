/*
//  Binary File Splicer
//  Copyright (c) 2010-2014, Adam Rehn
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
#include "ArgumentsParser.h"

#include "OffsetParser.h"
#include "StringUtil.h"

#ifdef _WIN32
	#include <direct.h>
#endif

#include <sys/stat.h>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <string>
using std::string;

namespace
{
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
}

void ArgumentsParser::parseArguments(int argc, char* argv[], string& outputFile, vector<FileSliceDetails>& filesAndSlices)
{
	OffsetParser<int64_t> parser;
	outputFile.clear();
	filesAndSlices.clear();	
	
	//Process each argument in turn
	int currArgIndex = 1;
	while (currArgIndex < argc)
	{
		//Determine if the current argument specifies an input file, an offset, or the output file
		string currArg = string(argv[currArgIndex]);
		if (currArg == "-i")
		{
			//Check that a filename has actually been provided
			if (argc > (currArgIndex + 1))
			{
				//Determine the size of the file
				string filename = string(argv[currArgIndex + 1]);
				int64_t filesize = ::filesize(filename);
				if (filesize == -1) {
					throw std::runtime_error("could not open input file \"" + filename + "\"");
				}
				
				//Add the new file to the list and consume the next argument
				filesAndSlices.push_back( FileSliceDetails(filename, filesize) );
				currArgIndex++;
			}
			else {
				throw std::runtime_error("expected filename");
			}
		}
		else if (currArgIndex == (argc - 1))
		{
			//The current argument specifies the output filename
			outputFile = currArg;
		}
		else
		{
			//If we are encountering an offset prior to encountering an input file, the pair is invalid
			if (filesAndSlices.empty()) {
				throw std::runtime_error("encountered an offset before an input file was specified");
			}
			
			//Check that the offset is part of a pair
			if (argc > (currArgIndex + 1))
			{
				//Parse and store the offset pair
				std::pair<int64_t, int64_t> parsedPair = parser.parseOffsetPair(currArg, string(argv[currArgIndex + 1]), filesAndSlices.back().filesize);
				filesAndSlices.back().addSlice(parsedPair);
				
				//Consume the next argument
				currArgIndex++;
			}
			else {
				throw std::runtime_error("expected second offset in pair");
			}
		}
		
		//Proceed to the next argument
		currArgIndex++;
	}
	
	//If no input or output files were specified, the arguments are invalid
	if (outputFile.empty() || filesAndSlices.empty()) {
		throw std::runtime_error("must specify an output file and at least one input file");
	}
}
