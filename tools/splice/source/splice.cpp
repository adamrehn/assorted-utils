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

#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

using namespace std;

int main (int argc, char* argv[])
{
	//If no arguments were supplied, display the usage syntax and exit
	if (argc == 1)
	{
		clog << "Usage syntax:" << endl
			<< "splice -i INFILE [START END START END -i INFILE2 START END...] OUTFILE" << endl << endl
			<< "Within pairs, the second value can be absolute or +/- relative to the first." << endl
			<< "Special values BEGIN and END are supported for the start and end of the file." << endl
			<< "Suffixes KB, MB, GB, KiB, MiB, and GiB are supported for all integer values." << endl << endl
			<< "Example:" << endl
			<< "splice -i <INFILE1>  1KB +4  -16KiB END  -i <INFILE2>  1024 2MiB  <OUTFILE>" << endl << endl
			<< "This will extract bytes 1000 to 1004, and the last 16KiB, from the first file," << endl
			<< "and bytes 1024 to 2097152 from the second file.";
		
		return 0;
	}
	
	try
	{
		//Attempt to parse the command-line arguments
		string outputFile;
		vector<FileSliceDetails> filesAndSlices;
		ArgumentsParser::parseArguments(argc, argv, outputFile, filesAndSlices);
		
		//Display the computed list of slices that will be used to perform splicing
		clog << "Output file: " << outputFile << endl;
		clog << "Slices:" << endl << endl;
		
		for (auto currSlice : filesAndSlices) {
			clog << currSlice;
		}
		
		//Perform splicing
		FileSplicer::splice(outputFile, filesAndSlices);
	}
	catch (std::runtime_error& e) {
		clog << "Error: " << e.what() << "." << endl;
	}
	
	return 0;
}
