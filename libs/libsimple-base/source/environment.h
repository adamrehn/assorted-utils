/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2009-2013, Adam Rehn
//
//  ---
//
//  Environment Interaction Functions
//
//  Functionality for interacting with the application's environment.
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
#ifndef _LIB_SIMPLE_BASE_ENVIRONMENT_H
#define _LIB_SIMPLE_BASE_ENVIRONMENT_H

#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
using std::string;
using std::stringstream;
using std::vector;

string temp_dir(bool trailing_slash = false); //Retrieves the temporary directory for the currently logged-in user. A trailing slash can be appended if desired.
string capture_output(string command);        //Wrapper for the C-String version of this function
string capture_output(const char* command);   //Executes the specified command and retrieves the command-line output
string read_entire_stdin();                   //Reads all input from standard input and returns it as a string
string generate_unique_filename();            //Generates a unique file name in the temporary files directory

//Wraps around getenv to support std::strings
string get_env(const string& var);

//Executes a command, writing to its stdin, retrieving the stdout and stderr, and returns the return code
int executeProcessWithPipes(const std::string& command, const string& writeThisToStdIn, string& thisReceivesStdOut, string& thisReceivesStdErr, bool combineStdErrWithStdOut = false);

//Uses platform-specific CLI functionality to get a password without displaying the characters
string get_cli_password_hidden(string prompt);

//Uses platform-specific CLI functionality to set the cursor position of stderr
void set_terminal_cursor_position(int x, int y);

//SimpleGlob, our glob() implementation under Windows, breaks under C++11 mode
#if !defined(_WIN32) || __cplusplus < 201103L

//Regardless of platform, we wrap around the native glob() implementation
vector<string> glob(const string& pattern);

#endif

//On Windows, ShellExecuteEx is a pain to use, so we make things much simpler
#ifdef _WIN32
	#include <windows.h>
	bool EasyShellExecuteEx(LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd);
	
	//Monitors a directory for file changes and executes a callback when they occur.
	//Exits when the callback function returns false.
	void MonitorDirectoryForFileWrites(const char* dir, bool(*callback)(void));
#endif

//Linux version of MonitorDirectoryForFileWrites, using inotify
#ifdef __linux__
void MonitorDirectoryForFileWrites(const char* dir, bool(*callback)(void));
#endif

//Mac OSX (10.5 and higher) version of MonitorDirectoryForFileWrites, using the FSEvents API
//Part of libsimple-osx-compat.so, OSX builds of apps using this function need to link with that as well
#include "osx-compat.h"

#endif
