/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2009-2013, Adam Rehn
//
//  ---
//
//  String Manipulation Functions
//
//  These string manipulation functions include implementations of native PHP string functions,
//  a few handy functions of my own, and some basic type juggling using STL stringstreams.
//
//  ---
//
//  This file is part of the Simple Base Library for C++ (libsimple-base).
//
//  libsimple-base is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with libsimple-base. If not, see <http://www.gnu.org/licenses/>.
*/
#include "string_manipulation.h"

//PHP String Functions, behave the same as their PHP counterparts

//Adapted from a string search and replace function from <http://snipplr.com/view/1055/find-and-replace-one-string-with-another>
//and the code from the comment by "icstatic" that fixes the infinite loop bugs present in the original.
string str_replace (const string& find, const string& replace, string subject)
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

string basename (const string& path)
{
	//Truncate everything up to, and including, the last ocurrence of a slash
	size_t pos = path.find_last_of("\\/");
	if (pos != string::npos) {
		return path.substr(pos + 1);
	}
	else {
		return path; //Return the path
	}
}

string dirname (const string& path)
{
	//If path is "/", return "/"
	if (path == "/") {
		return path;
	}
	
	//Truncate everything after (and including) the last ocurrence of a slash
	size_t pos = path.find_last_of("\\/");
	if (pos != string::npos) {
		return path.substr(0, pos);
	}
	else {
		return string(".");
	}
}

string implode(const string& glue, const vector<string>& pieces)
{
	//Create a string to hold the result
	string result = "";
	
	//Check that the vector is not empty
	if (pieces.size() > 0)
	{
		//Loop through all but the last of the array elements
		for (size_t i = 0; i < pieces.size()-1; ++i)
		{
			//Concatenate the element, with the delimiter
			result += pieces[i] + glue;
		}
		
		//Concatenate the last element
		result += pieces[pieces.size()-1];
	}
	
	//Return the result
	return result;
}

//Written by me, but based loosely on the str_replace function above
vector<string> explode(const string& delim, const string& s, size_t limit)
{
	//Counters for the current and previous match positions
	size_t prevPos      = 0;
	size_t currPos      = 0;
	
	//Keep count of the number of substrings we have captured
	size_t captureCount = 0;
	
	//Keep track of the length of the delimiter itself so it can be excluded from the results
	size_t uDelimLen    = delim.length();
	
	//A vector of strings to hold the results
	vector<string> result;
	
	//If looking for an empty string, simply return a vector containing the subject
	if (uDelimLen == 0)
	{
		result.push_back(s);
		return result;
	}
	
	//Loop through and find all instances of the delimiter
	while (((currPos = s.find(delim, currPos)) != string::npos) && (!limit || captureCount < limit-1))
	{
		//Add the intervening substring to the vector
		result.push_back(s.substr(prevPos, currPos - prevPos));
		
		//Increment the captured substring count
		captureCount++;
		
		//Skip over the delimiter and copy to prevPos
		currPos += uDelimLen;
		prevPos = currPos;
	}
	
	//Retrieve the remaining characters (if any)
	currPos = s.length();
	if (prevPos <= currPos) {
		result.push_back(s.substr(prevPos, currPos - prevPos));
	}
	
	//Return the modified string
	return result;
}

string urldecode (string uri)
{
	//Traverse the string, looking for %
	size_t lastPos = 0;
	size_t foundPos = string::npos;
	while ((foundPos = uri.find('%', lastPos)) != string::npos)
	{
		//Check that there are two characters after the %
		if (uri.length() > foundPos + 2)
		{
			//Retrieve the two characters after the %
			string entity = uri.substr(foundPos + 1, 2);
			
			//Convert the hex to decimal
			long int decimal = hex_to_dec(entity);
			
			//Truncate to one byte and cast as a character
			char decoded = decimal;
			
			//Replace the encoded form with the decoded character
			uri.replace(foundPos, 3, string(1, decoded));
		}
		
		//Prevent a match ocurring next time if the decoded character was %
		lastPos = foundPos + 1;
	}
	
	//Return the decoded string
	return uri;
}

string strtoupper (const string& s)
{
	string result = "";
	
	for (size_t i = 0; i < s.length(); ++i)
		result += toupper(s[i]);
	
	return result;
}

string strtolower (const string& s)
{
	string result = "";
	
	for (size_t i = 0; i < s.length(); ++i)
		result += tolower(s[i]);
	
	return result;
}


//String Manipulation Functions

string strip_extension (const string& s)
{
	if (s.find_last_of(".") != string::npos)
		return s.substr(0, s.find_last_of("."));
	else
		return s;
}

string get_extension (const string& s)
{
	if (s.find_last_of(".") != string::npos)
		return s.substr(s.find_last_of(".") + 1);
	else
		return s;
}

string strip_chars (const string& chars, const string& s)
{
	string result = s;
	for (size_t i = 0; i < chars.length(); ++i) {
		result = str_replace(chars.substr(i, 1), string(""), result);
	}
	
	return result;
}

string strip_first_word (const string& s)
{
	if (s.find(" ") != string::npos) {
		return s.substr(s.find(" ") + 1);
	}
	else {
		return s;
	}
}

string get_first_word (const string& s)
{
	if (s.find(" ") != string::npos) {
		return s.substr(0, s.find(" "));
	}
	else {
		return s;
	}
}

bool split_in_half (const string& s, string array[2], string delim)
{
	if (s.find(delim) != string::npos)
	{
		array[0] = s.substr(0, s.find(delim));
		array[1] = s.substr(s.find(delim) + delim.length());
		return true;
	}
	else {
		return false;
	}
}

string extract (const string& s, string start, string end, size_t* startPos, size_t* endPos)
{
	//A temporary variable to hold the result
	string result = "";
	
	//Search for the starting pattern
	size_t start_pos = s.find(start);
	
	//If the starting pattern was found, find the first occurrence of the ending pattern after it
	if (start_pos != string::npos)
	{
		//Perform the search for the ending pattern, from the character after the position of the starting pattern match (in case the two patterns are the same)
		size_t end_pos = s.find(end, start_pos + 1);
		
		//If the ending pattern was found after the starting pattern, slice out the substring we're after
		if (end_pos != string::npos)
		{
			//Current behaviour is to exclude the starting and ending patterns and just return the content in between them
			start_pos += start.length();
			result = s.substr(start_pos , end_pos - start_pos);
			if (startPos != NULL) *startPos = start_pos;
			if (endPos != NULL) *endPos = end_pos;
		}
	}
	
	//Return the result (which could be blank)
	return result;
}

bool in (const string& needle, const string& haystack)
{
	return haystack.find(needle) != string::npos;
}

bool starts_with (const string& start, const string& s)
{
	if (s.length() == 0 || start.length() > s.length()) return false;
	return s.substr(0, start.length()) == start;
}

bool ends_with (const string& end, const string& s)
{
	if (s.length() == 0 || end.length() > s.length()) return false;
	return s.substr(s.length() - end.length()) == end;
}

string replace_extension (const string& s, string newExtension)
{
	//See where the last occurence of a period, and also a slash, are
	size_t dotPos   = s.find_last_of(".");
	size_t slashPos = s.find_last_of("\\/");
	
	//Check to see that a period was actually found, after all slashes
	if (dotPos != string::npos && (slashPos == string::npos || dotPos > slashPos))
	{
		//Create a copy of the original string to work on
		string newpath = s;
		
		//Replace all of the characters after the last period with the new extension
		newpath.replace(dotPos + 1, newpath.length() - (dotPos + 1), newExtension);
		
		//Return the new file path
		return newpath;
	}
	else
	{
		//No period was found, so simply append the new file extension
		return s + "." + newExtension;
	}
}

string replace_first (const string& search, const string& replace, const string& haystack)
{
	//Make a copy of the input string
	string result = haystack;
	
	//Check to see if the needle was found in the haystack
	size_t pos = result.find(search);
	if (pos != string::npos)
	{
		//Replace the first ocurrence
		result = result.replace(pos, search.length(), replace);
	}
	
	//Return the resulting string, regardless of whether a replacement ocurred or not
	return result;
}

string strip_quotes(const string& s, bool recursive)
{
	//Check to see if we are in recursive mode
	if (recursive == true)
	{
		//We need to loop, calling the function until it has no effect
		string result = s;
		while (result != strip_quotes(result))
		{
			result = strip_quotes(result);
		}
		
		//Return the stripped string
		return result;
	}
	else
	{
		//Not recursive, working on the outermost pair of quotes only
		//Check to see if the string is enclosed in a pair of qoutes
		const char *cstring = s.c_str();
		if ((cstring[0] == '\'' || cstring[0] == '"') && cstring[s.length() - 1] == cstring[0])
		{
			//The string is enclosed by qoutes, so we need to remove them
			string result = s;
			result = result.substr(1);
			result = result.substr(0, result.length() - 1);
			
			//Return the stripped string
			return result;
		}
		else
		{
			//No qoutes were found
			return s;
		}
	}
}

string addquotes(const string& s)
{
	return "\"" + s + "\"";
}

//If the supplied string has spaces in it, wrap it in double quotes
string determine_quotes(const string& s, bool checkForExistingQuotes)
{
	if (in(" ", s))
	{
		if
		(
			!checkForExistingQuotes ||
			(s.substr(0, 1) != "\"" && s.substr(s.length()-1, 1) != "\"")
		)
		{
			return addquotes(s);
		}
	}
	
	return s;
}

string unix_line_endings(const string& s)
{
	//Convert \r\n (Windows line-ending format) to just the line feed
	string result = str_replace("\r\n", "\n", s);
	
	//Convert any single carriage returns (Mac default line-ending format) to line feeds
	result = str_replace("\r", "\n", result);
	
	//Return the result
	return result;
}

long int hex_to_dec(const string& hex)
{
	return strtol(hex.c_str(), NULL, 16);
}

//Filesize suffixes
#define KB   * (1000)
#define MB   * (1000 * 1000)
#define GB   * (1000 * 1000 * 1000)

#define KiB  * (1024)
#define MiB  * (1024 * 1024)
#define GiB  * (1024 * 1024 * 1024)

int intFromSuffix(const string& s)
{
	//Create an int to hold the result
	int result = toInt(s);
	
	//Check for suffixes
	if (s.length() > 2)
	{
		//This will hold the number itself where applicable
		int num = 0;
		
		//If the length is long enough, check for 3-letter suffixes (KiB, MiB, GiB)
		if (s.length() > 3)
		{
			//Extract the last three characters
			string lastThree = strtolower(s.substr(s.length() - 3));
			
			//Extract the number itself
			num = toInt(s.substr(0, s.length() - 3));
			
			//Check for the supported suffixes
			if      (lastThree == "kib")
				result = num KiB;
			else if (lastThree == "mib")
				result = num MiB;
			else if (lastThree == "gib")
				result = num GiB;
		}
		
		//Check for the 2-letter suffixes (KB, MB, GB)
		
		//Extract the last two characters
		string lastTwo = strtolower(s.substr(s.length() - 2));
		
		//Extract the number itself
		num = toInt(s.substr(0, s.length() - 2));
		
		//Check for the supported suffixes
		if      (lastTwo == "kb")
			result = num KB;
		else if (lastTwo == "mb")
			result = num MB;
		else if (lastTwo == "gb")
			result = num GB;
	}
	else
		result = toInt(s);
	
	//Return the result
	return result;
}

//Breaks a command string into an argv-style structure
vector<string> argv_from_string(string command)
{
	//Create a vector to hold the result
	vector<string> result;
	
	//Keep track of these positions
	size_t posStartQuote = string::npos;
	size_t posLastSplit  = 0;
	
	//Iterate over all of the characters in the string
	for (size_t i = 0; i < command.length(); ++i)
	{
		//Check for the presence of a quote character
		if (command[i] == '"')
		{
			//Determine if this is the start quote
			if (posStartQuote == string::npos)
			{
				//This is the start quote, unless it was escaped
				if (i == 0 || command[ i - 1 ] != '\\')
				{
					posStartQuote = i;
				}
				else
				{
					//Quote was escaped, so we need to remove the backslash
					command.erase(i - 1, 1);
					--i;
				}
			}
			else
			{
				//This is the end quote, unless it was escaped
				if (i > 0 && command[ i - 1 ] != '\\')
				{
					//Quote was not escaped, split here
					int numChars = i - (posStartQuote+1);
					
					if (numChars > 0) {
						result.push_back( command.substr(posStartQuote + 1, numChars) );
					}
					
					//Update the position values
					posStartQuote = string::npos;
					posLastSplit  = i + 1;
				}
				else
				{
					//Quote was escaped, so we need to remove the backslash
					command.erase(i - 1, 1);
					--i;
				}
			}
		}
		else if (command[i] == ' ')
		{
			//Determine if we are inside a quote block
			if (posStartQuote == string::npos)
			{
				//Not inside a quote block, split here
				int numChars = i - posLastSplit;
				
				if (numChars > 0) {
					result.push_back( command.substr(posLastSplit, numChars) );
				}
				
				//Update the position value
				posLastSplit = i + 1;
			}
		}
	}
	
	//Consume any remaining characters
	int numChars = command.length() - posLastSplit;
	if (numChars > 0) {
		result.push_back( command.substr(posLastSplit, numChars) );
	}
	
	return result;
}
