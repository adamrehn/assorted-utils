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
#include "FileSplicer.h"

#include <stdexcept>
#include <fstream>
using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;

FileSliceDetails::FileSliceDetails(const string& filename, int64_t filesize)
{
	this->filename = filename;
	this->filesize = filesize;
}

void FileSliceDetails::addSlice(const pair<int64_t, int64_t>& slice) {
	this->slices.push_back(slice);
}

ostream& operator<<(ostream& o, const FileSliceDetails& details)
{
	o << "Input file: \"" << details.filename << "\"" << endl;
	for (auto currPair : details.slices) {
		o << "\t" << currPair.first << " to " << currPair.second << endl;
	}
	
	return o;
}

void FileSplicer::copySlice(istream& in, ostream& out, int64_t offsetStart, int64_t offsetEnd)
{
	//Seek to the starting offset
	in.seekg(offsetStart);
	
	//Read the slice in multiple chunks, unless the slice size is smaller than our buffer size
	char buffer[1024*1024];
	uint64_t sliceSize = offsetEnd - offsetStart;
	uint64_t chunkSize = std::min(sliceSize, (uint64_t)(sizeof(buffer)));
	
	//Copy the slice
	size_t bytesCopied = 0;
	while (bytesCopied < sliceSize)
	{
		//Attempt to read the data from the input file
		size_t bytesRead = in.read(buffer, chunkSize).gcount();
		if (bytesRead == 0 || in.fail()) {
			throw std::runtime_error("I/O error reading input file");
		}
		
		//Write the data to the output file
		out.write(buffer, bytesRead);
		bytesCopied += bytesRead;
	}
}

void FileSplicer::splice(const string& outputFilename, const vector<FileSliceDetails>& slices)
{
	//Attempt to open the output file
	ofstream outfile(outputFilename.c_str(), std::ios::binary);
	if (outfile.is_open())
	{
		//Iterate over each of the input files
		for (auto currDetails : slices)
		{
			//Attempt to open the input file
			ifstream infile(currDetails.filename.c_str(), std::ios::binary);
			if (infile.is_open())
			{
				//Determine if offsets have been specified for the current input file
				if (currDetails.slices.empty())
				{
					//No slices specified, copy entire file
					copySlice(infile, outfile, 0, currDetails.filesize);
				}
				else
				{
					//Extract each of the slices in turn
					for (auto currPair : currDetails.slices) {
						copySlice(infile, outfile, currPair.first, currPair.second);
					}
				}
				
				infile.close();
			}
			else {
				throw std::runtime_error("could not open input file \"" + currDetails.filename + "\"");
			}
		}
		
		outfile.close();
	}
	else {
		throw std::runtime_error("could not open output file \"" + outputFilename + "\"");
	}
}
