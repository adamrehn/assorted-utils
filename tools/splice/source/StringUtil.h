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
#ifndef _SPLICE_STRING_UTIL
#define _SPLICE_STRING_UTIL

#include <string>
#include <sstream>
using std::string;
using std::stringstream;

class StringUtil
{
	public:
		
		//Uses stringstream functionality for typecasting
		template <typename O, typename I>
		static O to (const I& input)
		{
			//Create a stringstream
			stringstream buf;
			O output;
			
			//Input the argument and extract it as the specified type
			buf << input;
			buf >> output;
			
			//Return the extracted data
			return output;
		}
		
		//Uses stringstream functionality for type validation
		template <typename TestFor, typename T>
		static bool is (const T& i)
		{
			//Create a stringstream
			stringstream buf;
			TestFor test;
			
			//Input the argument and try to extract as the specified type
			buf << i;
			buf >> test;
			
			//Return whether or not the extraction succeeded
			return (!buf.fail());
		}
		
		static string replace(const string& find, const string& replace, string subject);
		static bool endsWith(const string& end, const string& s);
		static string toLower(const string& s);
};

#endif
