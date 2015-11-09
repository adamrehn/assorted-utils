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
#ifndef _SPLICE_OFFSET_PARSER
#define _SPLICE_OFFSET_PARSER

#include "StringUtil.h"

#include <stdint.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>
using std::string;
using std::vector;
using std::pair;

template <typename ValueTy>
class OffsetParser
{
	public:
		OffsetParser()
		{
			this->suffixes.push_back(std::make_pair("gib", 1024 * 1024 * 1024));
			this->suffixes.push_back(std::make_pair("gb",  1000 * 1000 * 1000));
			this->suffixes.push_back(std::make_pair("mib", 1024 * 1024));
			this->suffixes.push_back(std::make_pair("mb",  1000 * 1000));
			this->suffixes.push_back(std::make_pair("kib", 1024));
			this->suffixes.push_back(std::make_pair("kb",  1000));
			this->suffixes.push_back(std::make_pair("b",   1));
		}
		
		std::pair<ValueTy, ValueTy> parseOffsetPair(const string& firstOffset, const string& secondOffset, ValueTy totalSize) const
		{
			//Parse the pair of values
			ValueTy first  = this->parseOffset(StringUtil::toLower(firstOffset),  totalSize);
			ValueTy second = this->parseOffset(StringUtil::toLower(secondOffset), totalSize);
			
			//If both offsets are negative, the pair is invalid
			if (first < 0 && second < 0) {
				throw std::runtime_error("both offsets in a pair cannot be negative");
			}
			
			//If the either offset is negative, subtract its absolute value from the other
			if      (first < 0)  { first  = second - std::abs(first);  }
			else if (second < 0) { second = first  - std::abs(second); }
			
			//Clip negative computed values to zero
			first  = std::max(first,  (ValueTy)0);
			second = std::max(second, (ValueTy)0);
			
			//Clip values beyond the end of the file to the end itself
			first  = std::min(first,  totalSize);
			second = std::min(second, totalSize);
			
			//Order the offsets
			return std::make_pair( std::min(first, second), std::max(first, second) );
		}
		
	private:
		
		ValueTy parseOffset(const string& offsetStr, ValueTy totalSize) const
		{
			//Detect begin of file
			if (offsetStr == "begin") {
				return 0;
			}
			
			//Detect end of file
			if (offsetStr == "end") {
				return totalSize;
			}
			
			//Determine if the value has a suffix 
			for (auto currSuffix : this->suffixes)
			{
				//Check for the presence of each suffix in turn
				if (StringUtil::endsWith(currSuffix.first, offsetStr))
				{
					//Apply the value of the suffix
					string valWithoutSuffix = StringUtil::replace(currSuffix.first, "", offsetStr);
					return this->parseInteger(valWithoutSuffix) * currSuffix.second;
				}
			}
			
			//No suffix detected, treat the value as a raw number
			return this->parseInteger(offsetStr);
		}
		
		ValueTy parseInteger(const string& s) const
		{
			//Validate that the string represents a valid number
			if (!StringUtil::is<ValueTy>(s)) {
				throw std::runtime_error("invalid number: \"" + s + "\"");
			}
			
			return StringUtil::to<ValueTy>(s);
		}
		
		vector< pair<string, ValueTy> > suffixes;
};

#endif
