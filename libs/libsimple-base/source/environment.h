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
	template <typename CallbackTy>
	void MonitorDirectoryForFileWrites(const std::string& dir, CallbackTy callback)
	{
		//Create a wait status and change handle
		DWORD  dwWaitStatus;
		HANDLE dwChangeHandle;
		
		//Set the change handle to monitor for file changes
		dwChangeHandle = FindFirstChangeNotification
		(
			dir.c_str(),                   //Directory to monitor
			false,                         //Exclude subdirectories
			FILE_NOTIFY_CHANGE_LAST_WRITE  //Monitor for file changes
		);
		
		//Check that the handle was created properly
		if (dwChangeHandle == NULL || dwChangeHandle == INVALID_HANDLE_VALUE)
		{
			//Handle could not be created
			return;
		}
		
		//The main wait loop
		while(1)
		{
			//Wait for a change notification
			dwWaitStatus = WaitForSingleObject(dwChangeHandle, INFINITE);
			
			//This will hold the return value of the callback
			//We exit when the callback returns false
			bool callbackReturn = true;
			
			//Once we have received a notification, check the type
			switch (dwWaitStatus)
			{
				case WAIT_OBJECT_0:
					//A file was changed
					//Execute the callback function
					callbackReturn = callback();
					
					//Move on to waiting for the next notification
					if (!FindNextChangeNotification(dwChangeHandle) || callbackReturn == false)
					{
						//Something went wrong, we could not proceed.
						
						//Close the handle and exit.
						FindCloseChangeNotification(dwChangeHandle);
						return;
					}
					
					break;
				
				case WAIT_TIMEOUT:
					//The wait timed out.
					//This should not happen when INFINITE is used above.
					
					//Close the handle and exit.
					FindCloseChangeNotification(dwChangeHandle);
					return;
			}
		}
	}
#endif

//Linux version of MonitorDirectoryForFileWrites, using inotify
#ifdef __linux__
#include <sys/inotify.h>

template <typename CallbackTy>
void MonitorDirectoryForFileWrites(const char* dir, CallbackTy callback)
{
	//Setup the watch
	int inotifyDescriptor = inotify_init();
	int watchDescriptor = inotify_add_watch(inotifyDescriptor, dir, IN_MODIFY);
	
	//Prepare the buffer
	int bufSize = (sizeof(inotify_event) + 16) * 1024;
	char buffer[bufSize];
	
	//This will hold the return value of the callback
	//We exit when the callback returns false
	bool callbackReturn = true;
	
	//This should be equivalent to a while(1) so long as the watch is not deleted
	int len = 0;
	do
	{
		len = read(inotifyDescriptor, buffer, bufSize);
		int i = 0;
		while (i < len && callbackReturn == true)
		{
			inotify_event* event = (inotify_event*)buffer + i;
			
			//Execute the callback if a file was changed
			if (event->mask & IN_MODIFY) {
				callbackReturn = callback();
			}
			
			i += sizeof(inotify_event) + event->len;
		}
	}
	while (len > 0 && callbackReturn == true);
	
	//Cleanup
	inotify_rm_watch(inotifyDescriptor, watchDescriptor);
	close(inotifyDescriptor);
}

#endif

//Mac OS X (10.5 and higher) version of MonitorDirectoryForFileWrites, using the FSEvents API
//Applications that use this version of MonitorDirectoryForFileWrites will need to link against both the
//CoreServices and CoreFoundation frameworks, using "-framework CoreServices -framework CoreFoundation"
#include "osx-fsevents.h"

#endif
