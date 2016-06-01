/*
//  CSV File Concatenator
//  Copyright (c) 2015-2016, Adam Rehn
//  
//  ---
//  
//  This utility concatenates multiple CSV files by using the header row of the
//  first file, and removing the header rows of all subsequent files.
//  
//  (Note: for CSV files without header rows, just use the standard `cat` command.)
//  
//  Usage Syntax:    cat-csv OUTFILE <FILE/PATTERN> <FILE/PATTERN> ...
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
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <simple-base/base.h>

#define BUFSIZE (4 * 1024 * 1024)

using namespace std;

//Extracts the header from a CSV file, or throws an error if one wasn't found
string extractHeader(istream& csvFile, const string& filename)
{
	string header = "";
	getline(csvFile, header, '\n');
	
	if (csvFile.fail()) {
		throw std::runtime_error("failed to read header row from file \"" + filename + "\"!");
	}
	
	return header;
}

int main (int argc, char* argv[])
{
	try
	{
		if (argc > 2)
		{
			string outfilePath = argv[1];
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
			
			//Verify that at least one file was specified after wildcard expansion
			if (files.empty()) {
				throw std::runtime_error("no input files specified.");
			}
			
			//Attempt to open the output file
			ofstream outfile(outfilePath.c_str(), ios::binary);
			if (!outfile.is_open()) {
				throw std::runtime_error("failed to open output file!");
			}
			
			//Read the header row of the first CSV file and write it to the output file
			ifstream firstFile(files[0].c_str(), ios::binary);
			string headerRow = extractHeader(firstFile, files[0]);
			firstFile.close();
			outfile << headerRow << "\n";
			
			//Iterate over each of the input CSV files
			for (vector<string>::iterator currFilePath = files.begin(); currFilePath != files.end(); ++currFilePath)
			{
				//Verify that the file's header row matches the one we're using
				ifstream currFile(currFilePath->c_str(), ios::binary);
				if (extractHeader(currFile, *currFilePath) != headerRow) {
					throw std::runtime_error("the header for file \"" + *currFilePath + "\" does not match the header row of the first input file!");
				}
				
				//Copy the remaining rows to the output file
				char buffer[BUFSIZE];
				size_t bytesRead = 0;
				bool hasTrailingNewline = false;
				while ( (bytesRead = currFile.read(buffer, sizeof(buffer)).gcount()) != 0 )
				{
					hasTrailingNewline = (buffer[bytesRead - 1] == '\n');
					outfile.write(buffer, bytesRead);
				}
				
				//If the input file didn't end with a trailing newline, append one ourselves
				if (hasTrailingNewline == false) {
					outfile << "\n";
				}
				
				currFile.close();
			}
			
			outfile.close();
		}
		else {
			clog << "Usage syntax:\ncat-csv OUTFILE <FILE/PATTERN> <FILE/PATTERN> ..." << endl;
		}
	}
	catch (std::runtime_error& e) {
		clog << "Error: " << e.what() << endl;
	}
	
	return 0;
}
