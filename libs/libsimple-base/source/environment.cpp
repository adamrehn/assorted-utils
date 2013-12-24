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
#include "environment.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include "string_manipulation.h"

//Platform-specific includes
#ifdef _WIN32
	#include <windows.h>
	
	#ifdef _MSC_VER
		#define popen _popen
		#define pclose _pclose
	#endif
#else
	#include <unistd.h>
	#include <glob.h>
	#include <termios.h>    //Part of the POSIX library
	#include <string.h>
	#include <sys/wait.h>
#endif

//Function Definitions
string temp_dir(bool trailing_slash)
{
	string temp = "";
	
	#ifdef _WIN32
		temp = string(getenv("TEMP"));
	#else
		try {
		  temp = string(getenv("TMPDIR"));
		} catch(...) {;}
		if (temp == "") temp = "/tmp";
	#endif
	
	if (trailing_slash)
	{
		#ifdef _WIN32
			return temp + "\\";
		#else
			return temp + "/";
		#endif
	}
	else
		return temp;
}

string capture_output(string command)
{
	return capture_output(command.c_str());
}

//This function derived from the excellent code example by Salem from <http://www.daniweb.com/forums/post654859.html#post654859>
string capture_output(const char* command)
{
	//Stringstream to hold the output and a character array to use as a line buffer
	stringstream output;
	char buff[100];
	
	//Execute the command and read the output, 100 characters at a time
	FILE *fp = popen(command, "r");
	if (fp)
	{
		while ( fgets( buff, sizeof buff, fp ) != NULL ) {
			//Add the contents of the buffer to the overall output
			output << buff;
		}
		pclose(fp);
	}
	
	//Return the complete output
	return output.str();
}

//Based on the function above, reads everything from stdin
string read_entire_stdin()
{
	//Stringstream to hold the input and a character array to use as a line buffer
	stringstream output;
	char buff[100];
	
	//Read stdin, 100 characters at a time
	while ( fgets( buff, sizeof buff, stdin ) != NULL ) {
		//Add the contents of the buffer to the overall output
		output << buff;
	}
	
	//Return the complete output
	return output.str();
}

string generate_unique_filename()
{
	srand(time(NULL));
	return temp_dir(true) + toString(time(0)) + toString(rand()) + toString(rand());
}

//Wraps around getenv to support std::strings
string get_env(const string& var)
{
	char* val = getenv(var.c_str());
	if (val != NULL) {
		return string(val);
	}
	else {
		return string("");
	}
}

//Creating child processes is a very different affair under Windows when compared to POSIX-compliant operating systems
#ifdef _WIN32
	
	int executeProcessWithPipes(const std::string& command, const string& writeThisToStdIn, string& thisReceivesStdOut, string& thisReceivesStdErr, bool combineStdErrWithStdOut)
	{
		//---- Stage 1 - Pipe Creation ----
		
		//We need to create a SECURITY_ATTRIBUTES instance with bInheritHandle to true for creating inheritable pipe handles 
		SECURITY_ATTRIBUTES saAttr; 
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
		saAttr.bInheritHandle = TRUE; 
		saAttr.lpSecurityDescriptor = NULL; 
		
		//Create the pipes (if any CreatePipe call fails, close all pipes that were opened)
		HANDLE stdInput[2];
		HANDLE stdOutput[2];
		HANDLE stdError[2];
		if (CreatePipe(&stdInput[0],  &stdInput[1],  &saAttr, 0) == 0) return -1;
		if (CreatePipe(&stdOutput[0], &stdOutput[1], &saAttr, 0) == 0)
		{
			CloseHandle(stdInput[0]);
			CloseHandle(stdInput[1]);
			return -1;
		}
		if (CreatePipe(&stdError[0],  &stdError[1],  &saAttr, 0) == 0)
		{
			CloseHandle(stdInput[0]);
			CloseHandle(stdInput[1]);
			CloseHandle(stdOutput[0]);
			CloseHandle(stdOutput[1]);
			return -1;
		}
		
		//Ensure the read handle to the pipe for STDOUT is not inherited.
		SetHandleInformation(stdOutput[0], HANDLE_FLAG_INHERIT, 0);
		
		//Ensure the read handle to the pipe for STDERR is not inherited.
		SetHandleInformation(stdError[0], HANDLE_FLAG_INHERIT, 0);
		
		//Ensure the write handle to the pipe for STDIN is not inherited.
		SetHandleInformation(stdInput[1], HANDLE_FLAG_INHERIT, 0);
		
		
		//---- Stage 2 - Process Creation ----
		
		//Create the required structures
		STARTUPINFO sInfo;
		PROCESS_INFORMATION pInfo;
		
		//Zero their memory
		memset(&sInfo, 0, sizeof(STARTUPINFO));
		memset(&pInfo, 0, sizeof(PROCESS_INFORMATION));
		
		//Set the required fields for using the pipes as handles for standard streams
		sInfo.cb         = sizeof(STARTUPINFO);
		sInfo.dwFlags    = STARTF_USESTDHANDLES;
		sInfo.hStdInput  = stdInput[0];
		sInfo.hStdOutput = stdOutput[1];
		sInfo.hStdError  = (combineStdErrWithStdOut) ? stdOutput[1] : stdError[1]; //Check if redirecting stderr to stdout
		
		//Create the process
		if (CreateProcess(NULL, const_cast<char*>(command.c_str()), NULL, NULL, true, 0, NULL, NULL, &sInfo, &pInfo))
		{
			//Write the data to the child's stdin
			if (writeThisToStdIn.length() > 0)
			{
				DWORD bytesWritten = 0;
				WriteFile(stdInput[1], writeThisToStdIn.data(), (DWORD)(writeThisToStdIn.length()), &bytesWritten, NULL);
			}
			
			//Close the write handle for the child's stdin
			CloseHandle(stdInput[1]);
			
			//Close the read handle for the child's stdin
			CloseHandle(stdInput[0]);
			
			//Close the write ends of the child's output pipes (otherwise ReadFile will block indefinitely)
			CloseHandle(stdOutput[1]);
			CloseHandle(stdError[1]);
			
			//Read data from the child's stdout
			char buffStdOut[100];
			DWORD bytesReadStdOut = 0;
			while (ReadFile(stdOutput[0], buffStdOut, sizeof buffStdOut, &bytesReadStdOut, NULL) && bytesReadStdOut) {
				thisReceivesStdOut.append(buffStdOut, (size_t)bytesReadStdOut);
			}
			
			//Read data from the child's stderr
			char buffStdErr[100];
			DWORD bytesReadStdErr = 0;
			while (ReadFile(stdError[0], buffStdErr, sizeof buffStdErr, &bytesReadStdErr, NULL) && bytesReadStdErr) {
				thisReceivesStdErr.append(buffStdErr, (size_t)bytesReadStdErr);
			}
			
			//Close the read handles for the child's output pipes
			CloseHandle(stdOutput[0]);
			CloseHandle(stdError[0]);
			
			//Wait for the child to complete
			WaitForSingleObject(pInfo.hProcess, INFINITE);
			
			//Retrieve the exit code of the child process
			DWORD returnVal = -1;
			GetExitCodeProcess(pInfo.hProcess, &returnVal);
			
			//Close the handles to the child process itself
			CloseHandle(pInfo.hProcess);
			CloseHandle(pInfo.hThread);
			
			//Return the return code
			return (int)returnVal;
		}
		
		return -1;
	}
	
#else

	#define MAX_ARGV_ARGS 255
	
	int executeProcessWithPipes(const std::string& command, const string& writeThisToStdIn, string& thisReceivesStdOut, string& thisReceivesStdErr, bool combineStdErrWithStdOut)
	{
		//Create the pipes
		int pStdIn[2];
		int pStdOut[2];
		int pStdErr[2];
		
		//If we are unable to create a pipe, make sure we close any that are already open
		if (pipe(pStdIn) == -1) return -1;
		if (pipe(pStdOut) == -1)
		{
			close(pStdIn[0]);
			close(pStdIn[1]);
			return -1;
		}
		if (pipe(pStdErr) == -1)
		{
			close(pStdIn[0]);
			close(pStdIn[1]);
			close(pStdOut[0]);
			close(pStdOut[1]);
			return -1;
		}
		
		//Fork
		pid_t childPid = fork();
		if (childPid == -1)
		{
			//Failed to fork, close the pipes
			close(pStdIn[0]);
			close(pStdIn[1]);
			close(pStdOut[0]);
			close(pStdOut[1]);
			close(pStdErr[0]);
			close(pStdErr[1]);
			return -1;
		}
		else if (childPid == 0)
		{
			//Child process
			
			//Close the ends of the pipes that we won't be using
			close(pStdIn[1]);
			close(pStdOut[0]);
			close(pStdErr[0]);
			
			//Redirect our standard streams to the pipes
			dup2(pStdIn[0],  fileno(stdin));
			dup2(pStdOut[1], fileno(stdout));
			
			//Check if we are redirecting stderr to stdout
			if (combineStdErrWithStdOut) {
				dup2(pStdOut[1], fileno(stderr));
			}
			else {
				dup2(pStdErr[1], fileno(stderr));
			}
			
			//We can now close the other ends of the pipes - the standard streams are aliases to them now
			close(pStdIn[0]);
			close(pStdOut[1]);
			close(pStdErr[1]);
			
			//Break the command into argv components
			vector<string> argvStructure = argv_from_string(command);
			char* argv[__MAX_ARGV_ARGS];
			unsigned int argvIndex = 0;
			for (; argvIndex < (__MAX_ARGV_ARGS - 1) && argvIndex < argvStructure.size(); ++argvIndex) {
				argv[argvIndex] = const_cast<char*>(argvStructure[argvIndex].c_str());
			}
			argv[argvIndex] = NULL;
			
			//Execute the command
			execvp(argv[0], argv);
			
			//If we reach this point, we failed to execute the command
			exit(-1);
		}
		else
		{
			//Parent process
			
			//Close the ends of the pipes that we won't be using
			close(pStdIn[0]);
			close(pStdOut[1]);
			close(pStdErr[1]);
			
			//Write any data to the child's stdin
			if (writeThisToStdIn.length() > 0) {
				write(pStdIn[1], writeThisToStdIn.data(), writeThisToStdIn.length());
			}
			close(pStdIn[1]);
			
			//Read data from the child's stdout
			char buffStdOut[100];
			size_t bytesReadStdOut = 0;
			while ((bytesReadStdOut = read(pStdOut[0], buffStdOut, sizeof(buffStdOut))) > 0) {
				thisReceivesStdOut.append(buffStdOut, bytesReadStdOut);
			}
			
			//Read data from the child's stderr
			char buffStdErr[100];
			size_t bytesReadStdErr = 0;
			while ((bytesReadStdErr = read(pStdErr[0], buffStdErr, sizeof(buffStdErr))) > 0) {
				thisReceivesStdErr.append(buffStdErr, bytesReadStdErr);
			}
			
			//Wait for the child process to complete and retrieve the return code
			int returnCode = -1;
			int status = 0;
			waitpid(childPid, &status, 0);
			if (WIFEXITED(status)) {
				returnCode = WEXITSTATUS(status);
			}
			
			//Close the remaining ends of the pipes
			close(pStdOut[0]);
			close(pStdErr[0]);
			
			//Return the return code
			return returnCode;
		}
		
		return -1;
	}
	
	#undef MAX_ARGV_ARGS
	
#endif

//Uses platform-specific CLI functionality to get a password without displaying the characters
string get_cli_password_hidden(string prompt)
{
	//Create a string to hold the password
	string password = "";
	
	//Output the prompt
	if (prompt.length() > 0)
		std::cout << prompt;
	
	//Everything after this point is platform-specific
	#ifdef _WIN32
		//This excellent code for Windows came from here:
		//<http://www.codeguru.com/forum/showpost.php?p=1786376&postcount=3>
		
		//Get a handle on the console and retrieve the current mode
		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
		DWORD mode = 0;
		GetConsoleMode(hStdin, &mode);
		
		//Set the mode to disable echo
		SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
		
		//Read the user's password
		getline(std::cin, password, '\n');
		
		//Output a newline, since this doesn't happen automatically
		std::cout << std::endl;
		
		//Set the mode to re-enable echo
		SetConsoleMode(hStdin, mode);
	#else
		//This excellent code for POSIX platforms came from here:
		//<http://stackoverflow.com/questions/1196418/getting-a-password-in-c-without-using-getpass-3>
		
		//This will limit our passwords to 1024 characters, unfortunately
		char pwd_cstr[1024];
		memset(pwd_cstr, 0, sizeof(pwd_cstr));
		
		//These will hold the current terminal flags, and the new flags, respectively
		struct termios oflags, nflags;
		
		//Set retrieve the current flags and create the new ones to disable echo
		tcgetattr(fileno(stdin), &oflags);
		nflags = oflags;
		nflags.c_lflag &= ~ECHO;
		nflags.c_lflag |= ECHONL;
		
		//Set the terminal attributes to the new flags
		if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
			//Failed to set the attributes, not much we can do.
			//The user's pasword will be visible onscreen.
		}
		
		//Read the user's password
		getline(std::cin, password, '\n');
		
		//Restore the terminal attributes to the original flags
		if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
			//Failed to set the attributes, not much we can do.
			//A little worrying though, because if this code is reached,
			//then stdin will have echo disabled for the rest of execution.
		}
	#endif
	
	//Return the password
	return password;
}

//Uses platform-specific CLI functionality to set the cursor position of stderr
void set_terminal_cursor_position(int x, int y)
{
	#ifdef _WIN32
		
		//As usual, Windows just has to be different to everyone else
		HANDLE hStdout = GetStdHandle(STD_ERROR_HANDLE); 
		COORD pos;
		pos.X = x;
		pos.Y = y;
		SetConsoleCursorPosition(hStdout, pos);
		
	#else
		
		fprintf(stderr, "\x1B[%u;%uH", x, y);
		
	#endif
}

#ifdef _WIN32
	#include <windows.h>
	#include "SimpleGlob.h"

	vector<string> glob(const string& pattern)
	{
		//On Windows we utilise the excellent SimpleGlob (MIT License)
		
		//Create a vector to hold the results
		vector<string> results;
		
		//Glob the pattern and loop through the results
		CSimpleGlob glob(SG_GLOB_NODOT|SG_GLOB_FULLSORT|SG_GLOB_MARK);
		if (SG_SUCCESS == glob.Add(pattern.c_str()))
		{
			for (int n = 0; n < glob.FileCount(); ++n) {
				results.push_back(glob.File(n));
			}
		}
		
		//Return the results
		return results;
	}
#else
	//On platforms with native glob(), we use it
	vector<string> glob(const string& pattern)
	{
		//Create a glob structure and perform the match
		glob_t globbuf;
		glob(pattern.c_str(), GLOB_NOESCAPE|GLOB_MARK, NULL, &globbuf);
		
		//Create a vector to hold the results
		vector<string> results;
		
		//Loop through the results
		for (size_t i = 0; i < globbuf.gl_pathc; ++i) {
			results.push_back(string(globbuf.gl_pathv[i]));
		}
		
		//Free the memory allocated in the struct
		globfree(&globbuf);
		
		//Return the results
		return results;
	}
#endif

#ifdef _WIN32

bool EasyShellExecuteEx(LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd)
{
	//Create a SHELLEXECUTEINFO struct to hold the arguments
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize   = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask    = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd     = NULL;
	ShExecInfo.nShow    = nShowCmd;
	ShExecInfo.hInstApp = NULL;
	
	//Gather the arguments
	ShExecInfo.lpVerb       = lpOperation;
	ShExecInfo.lpFile       = lpFile;
	ShExecInfo.lpParameters = lpParameters;
	ShExecInfo.lpDirectory  = lpDirectory;
	
	//Perform the ShellExecuteEx and wait for the spawned process to complete
	if (ShellExecuteEx(&ShExecInfo)) {
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	}
	else {
		return false;
	}
	
	//Success!
	return true;
}

//Monitors a directory for file changes and executes a callback when they occur.
//Exits when the callback function returns false.
void MonitorDirectoryForFileWrites(const char* dir, bool(*callback)(void))
{
	//Create a wait status and change handle
	DWORD  dwWaitStatus;
	HANDLE dwChangeHandle;
	
	//Set the change handle to monitor for file changes
	dwChangeHandle = FindFirstChangeNotification
	(
		dir,                           //Directory to monitor
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

void MonitorDirectoryForFileWrites(const char* dir, bool(*callback)(void))
{
	//Setup the watch
	int inotifyDescriptor = inotify_init();
	int watchDescriptor = inotify_add_watch(inotifyDescriptor, dir, IN_MODIFY);
	
	//Prepare the buffer
	int bufSize = (sizeof(inotify_event) + 16) * 1024;
	char* buffer = new char[bufSize];
	
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
	delete[] buffer;
	inotify_rm_watch(inotifyDescriptor, watchDescriptor);
	close(inotifyDescriptor);
}

#endif