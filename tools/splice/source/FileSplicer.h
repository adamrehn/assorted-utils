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
#ifndef _SPLICE_FILE_SPLICER
#define _SPLICE_FILE_SPLICER

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
using std::istream;
using std::ostream;
using std::string;
using std::vector;
using std::pair;
using std::endl;

//Represents a file and a set of offset pairs
class FileSliceDetails
{
	public:
		FileSliceDetails(const string& filename, int64_t filesize);
		void addSlice(const pair<int64_t, int64_t>& slice);
		
		string filename;
		int64_t filesize;
		vector< pair<int64_t, int64_t> > slices;
};

ostream& operator<<(ostream& o, const FileSliceDetails& details);

class FileSplicer
{
	public:
		static void splice(const string& outputFilename, const vector<FileSliceDetails>& slices);
		
	private:
		static void copySlice(istream& in, ostream& out, int64_t offsetStart, int64_t offsetEnd);
};

#endif
