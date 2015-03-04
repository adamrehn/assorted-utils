/*
//  CSV File Concatenator
//  Copyright (c) 2015, Adam Rehn
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
#include <stdexcept>
#include <vector>
#include <string>
#include <simple-base/base.h>

using namespace std;

//Retrieves the contents of a CSV file, or throws an error if the file couldn't be read
string getFileContentsOrError(const string& filename)
{
	string contents = file_get_contents(filename);
	if (contents.empty()) {
		throw std::runtime_error("Failed to open file \"" + filename + "\"");
	}
	
	return unix_line_endings(contents);
}

//Extracts the header from a CSV file, or throws an error if one wasn't found
string extractHeader(const string& csvData, const string& filename)
{
	size_t newlinePos = csvData.find("\n");
	if (newlinePos == string::npos) {
		throw std::runtime_error("no header row found in file \"" + filename + "\"!");
	}
	
	return csvData.substr(0, newlinePos);
}

//Removes the header from a CSV file, if one is found
string removeHeader(const string& csvData)
{
	size_t newlinePos = csvData.find("\n");
	if (newlinePos != string::npos) {
		return csvData.substr(newlinePos + 1);
	}
	
	//No header row found
	return csvData;
}

int main (int argc, char* argv[])
{
	try
	{
		if (argc > 2)
		{
			string outfile = argv[1];
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
			
			//Read the contents of the first CSV file, and extract the header row
			string csvData   = getFileContentsOrError(files[0]);
			string headerRow = extractHeader(csvData, files[0]);
			files.erase(files.begin());
			
			//Iterate over the remaining input CSV files
			for (vector<string>::iterator currFile = files.begin(); currFile != files.end(); ++currFile)
			{
				//Verify that the file's header row matches the one we're using
				string fileCsv = getFileContentsOrError(*currFile);
				if (extractHeader(fileCsv, *currFile) != headerRow) {
					throw std::runtime_error("the header for file \"" + *currFile + "\" does not match the header row of the first input file!");
				}
				
				//Remove the header and append the data
				csvData.append(removeHeader(fileCsv));
			}
			
			//Write the concatenated CSV data to the output file
			if (file_put_contents(outfile, csvData) == false) {
				throw std::runtime_error("failed to write output file!");
			}
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
