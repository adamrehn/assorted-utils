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
#ifndef _LIB_SIMPLE_BASE_STRING_H
#define _LIB_SIMPLE_BASE_STRING_H

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cstddef>
using std::string;
using std::stringstream;
using std::vector;
using std::size_t;

#define APP_DIR    StartupArgsParser(argc, argv).appDir()
#define INVOCATION StartupArgsParser(argc, argv).invocationString()

//PHP String Functions, behave the same as their PHP counterparts
string         str_replace (const string& find, const string& replace, string subject);
string         basename    (const string& path);
string         dirname     (const string& path);
string         implode     (const string& delim, const vector<string>& array);
vector<string> explode     (const string& delim, const string& s, size_t limit = 0);
string         urldecode   (string uri);
string         strtoupper  (const string& s);
string         strtolower  (const string& s);

//String Manipulation Functions
string          strip_extension    (const string& s);                                                      //Truncates the extension from a filename and returns just the filename
string          get_extension      (const string& s);                                                      //Truncates everything but the extension from a filename and returns just the extension
string          strip_chars        (const string& chars, const string& s);                                 //Strips each of the characters in the string chars from the string s
string          strip_first_word   (const string& s);                                                      //Removes everything up and including to the first occurrence of a space
string          get_first_word     (const string& s);                                                      //Removes everything after the first occurrence of a space
bool            split_in_half      (const string& s, string array[2], string delim);                       //Splits a string into two based on the delimiter and puts the halves in the array
string          extract            (const string& s, string start, string end, size_t* startPos = NULL, size_t* endPos = NULL); //Mimics a simple regular expression to extract the first occurrence of a string starting and ending with start and end, respectively
bool            in                 (const string& needle, const string& haystack);                         //Searches the haystack for the needle and returns whether or not it was found
bool            starts_with        (const string& start, const string& s);                                 //Returns whether or not s begins with start
bool            ends_with          (const string& end, const string& s);                                   //Returns whether or not s ends with end
string          replace_extension  (const string& s, string newExtension);                                 //Replaces the file extension of a path with the new extension specified
string          replace_first      (const string& search, const string& replace, const string& haystack);  //Replaces only the first instance of needle if found in haystack
string          strip_quotes       (const string& s, bool recursive = false);                              //If the supplied string is enclosed in qoutes, either the outermost pair will be removed, or all outer pairs will be
string          addquotes          (const string& s);                                                      //Wraps double qoutes around a string
string          determine_quotes   (const string& s, bool checkForExistingQuotes = false);                 //If the supplied string has spaces in it, wrap it in double quotes
string          unix_line_endings  (const string& s);                                                      //Converts all line endings to UNIX style (\n)
int             intFromSuffix      (const string& s);                                                      //Takes a string like "4MB" or "2MiB" and returns the computed value
long int        hex_to_dec         (const string& hex);                                                    //Converts a hex string to a decimal integer
vector<string>  argv_from_string   (string command);                                                       //Breaks a command string into an argv-style structure

//Template Functions for type juggling

//Uses a stringstream to generate a textual representation of a value
template <typename T>
string toString(const T& i, bool useScientificNotationForFloats = true)
{
	//Special case for converting to strings: input and return the stringstream's .str()
	stringstream buf;
	buf << ((useScientificNotationForFloats) ? std::scientific : std::fixed);
	buf << i;
	return buf.str();
}

//Uses stringstream functionality for typecasting
template <typename O, typename I>
O to (const I& input)
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
bool is (const T& i)
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

//Predefined names for unwrapping specific types
template <typename T>
bool isInt (const T& i)
{
	return is<int>(i);
}

template <typename T>
bool isDouble (const T& i)
{
	return is<double>(i);
}

template <typename T>
int toInt (const T& i)
{
	return to<int, T>(i);
}

template <typename T>
double toDouble (const T& i)
{
	return to<double, T>(i);
}

#endif
