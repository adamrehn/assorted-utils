/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2011-2013, Adam Rehn
//
//  ---
//
//  Applcation Startup Args Parser Class
//
//  This class parses argc and argv to provide a few useful functions based
//  on the values therein.
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
#include "StartupArgsParser.h"

#include <cstring>
#include <cstddef>

//Constructor, creates copies argc and argv
StartupArgsParser::StartupArgsParser(int argc, char** argv)
{
	//Copy argc
	this->argc = argc;
	
	//Copy argv
	this->argv = NULL;
	try
	{
		this->argv = new char*[this->argc];
		for (int i = 0; i < this->argc; ++i)
		{
			this->argv[i] = new char[strlen(argv[i])];
			strcpy(this->argv[i], argv[i]);
		}
	}
	catch (...)
	{
		throw string("Error copying argv");
	}
}

//Destructor
StartupArgsParser::~StartupArgsParser()
{
	if (this->argv != NULL)
	{
		//Loop through and free each of the character arrays
		for (int i = 0; i < this->argc; ++i)
		{
			if (this->argv[i] != NULL)
			{
				try
				{
					delete[] this->argv[i];
					this->argv[i] = NULL;
				}
				catch(...)
				{
					//Do nothing
				}
			}
		}
		
		//Free the argv arraay itself
		try
		{
			delete[] this->argv;
			this->argv = NULL;
		}
		catch(...)
		{
			//Do nothing
		}
	}
}

//Retrieve the directory the app is in (empty string when invoked by name instead of full path)
string StartupArgsParser::appDir()
{
	//Find the last instance of a slash and truncate everything after it
	string dir = string(this->argv[0]);
	size_t pos = dir.find_last_of("\\/");
	if (pos != string::npos) {
		return dir.substr(0, pos + 1);
	}
	else {
		return string(""); //Return an empty string
	}
}

//Retrieve the application executable name (does not resolve symlinks)
string StartupArgsParser::appName()
{
	//Find the last instance of a slash and truncate everything before it
	string dir = string(this->argv[0]);
	size_t pos = dir.find_last_of("\\/");
	if (pos != string::npos) {
		return dir.substr(pos + 1);
	}
	else {
		return dir; //The first argument is just the command name
	}
}

//Reconstruct the invocation string used to invoke the application
string StartupArgsParser::invocationString()
{
	//Create a temporary string to hold the results and loop through the arguments
	string invocation = "";
	for (int i = 0; i < this->argc; ++i)
	{
		//Create a temporary string to hold the current argument
		string currentArg = this->argv[i];
		
		//If the argument has spaces, wrap it in double quotes
		if (currentArg.find(string(" ")) != string::npos)
		{
			//Wrap the argument in double quotes
			invocation += "\"" + currentArg + "\"";
		}
		else
		{
			//Add the argument verbatim
			invocation += currentArg;
		}
		
		//Add a space between each of the arguments
		if (i != (this->argc - 1)) invocation += " ";
	}
	
	//Return the result
	return invocation;
}
