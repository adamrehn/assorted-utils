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
#include "StringUtil.h"

string StringUtil::replace(const string& find, const string& replace, string subject)
{
	size_t uPos        = 0;
	size_t uFindLen    = find.length();
	size_t uReplaceLen = replace.length();
	
	//If looking for an empty string, simply return the subject
	if (uFindLen == 0) {
		return subject;
	}
	
	//Loop through and find all instances
	while ((uPos = subject.find(find, uPos)) != string::npos)
	{
		subject.replace(uPos, uFindLen, replace);
		uPos += uReplaceLen;
	}
	
	//Return the modified string
	return subject;
}

bool StringUtil::endsWith(const string& end, const string& s)
{
	if (s.length() == 0 || end.length() > s.length()) return false;
	return s.substr(s.length() - end.length()) == end;
}

string StringUtil::toLower(const string& s)
{
	string result = "";
	
	for (size_t i = 0; i < s.length(); ++i) {
		result += tolower(s[i]);
	}
	
	return result;
}
