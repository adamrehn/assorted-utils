/*
//  Static Library Merging Tool (mergelib)
//  Copyright (c) 2009-2013, Adam Rehn
//
//  ---
//
//  This class manages the extration and merging of the object files
//  from a set of static libraries.
//
//  ---
//
//  This file is part of the "mergelib" Static Library Merging Tool.
//
//  mergelib is free software: you can redistribute it and/or modify it
//  under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with mergelib. If not, see <http://www.gnu.org/licenses/>.
*/
#include "StaticLibraryManager.h"

#include <simple-base/base.h>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <iostream>
using std::clog;
using std::endl;

namespace
{
	string ExecuteCommandAndRetrieveStdOut(const string& command)
	{
		string thisReceivesStdOut;
		string thisReceivesStdErr;
		int exitCode = executeProcessWithPipes(command, "", thisReceivesStdOut, thisReceivesStdErr, false);
		
		if (exitCode != 0) {
			throw std::runtime_error("Process failed with Exit Code " + toString(exitCode) + ": \"" + command + "\"");
		}
		
		return thisReceivesStdOut;
	}
}

StaticLibraryManager::StaticLibraryManager(const string& tempDir)
{
	this->tempDir = tempDir;
	make_dir(this->tempDir);
}

StaticLibraryManager::~StaticLibraryManager() {
	this->Cleanup();
}

void StaticLibraryManager::AddLibrary(const string& lib)
{
	//Check to ensure we are not attempting to process a duplicate library
	string libChecksum = sha1_file(lib);
	if (std::find(seenLibs.begin(), seenLibs.end(), libChecksum) != seenLibs.end())
	{
		clog << "Warning: ignoring duplicate library \"" << lib << "\"" << endl;
		return;
	}
	
	//Extract the library to the temp directory
	if (this->ExtractLibToTempDirectory(lib) == false) {
		throw std::runtime_error("Failed to extract object files from library \"" + lib + "\"");
	}
	
	//Mark the library as having been processed
	seenLibs.push_back(libChecksum);
}

void StaticLibraryManager::MergeLibraries(const string& outlib)
{
	//Build the ar command
	string mergeCommand = "ar rcs \"" + outlib + "\"";
	for (vector<string>::iterator currFile = seenObjectFiles.begin(); currFile != seenObjectFiles.end(); ++currFile) {
		mergeCommand += " \"" + this->tempDir + "/" + *currFile + ".o" "\"";
	}
	
	//Execute ar
	string thisReceivesStdOut;
	string thisReceivesStdErr;
	int exitCode = executeProcessWithPipes(mergeCommand, "", thisReceivesStdOut, thisReceivesStdErr, false);
	
	//Determine if the merge was performed successfully
	if (exitCode != 0) {
		throw std::runtime_error("Merge failed with Exit Code " + toString(exitCode) + " and stderr: \"" + thisReceivesStdErr + "\"");
	}
}

bool StaticLibraryManager::ExtractLibToTempDirectory(const string& lib)
{
	//Verbose output
	clog << "Processing static library \"" + lib + "\"..." << endl;
	
	//Iterate over list of files in the archive
	vector<string> files = explode("\n", str_replace("\r\n", "\n", ExecuteCommandAndRetrieveStdOut("ar -t \"" + lib + "\"")));
	for (vector<string>::iterator currFile = files.begin(); currFile != files.end(); ++currFile)
	{
		//Verbose output
		std::clog << "\t" << *currFile << std::endl;
		
		//Extract the file contents
		string fileContents = ExecuteCommandAndRetrieveStdOut("ar -p \"" + lib + "\" \"" + *currFile + "\"");
		
		#ifdef _WIN32
			//Under Windows, ar will write in text mode instead of binary mode, converting \n sequences to \r\n
			fileContents = str_replace("\r\n", "\n", fileContents);
		#endif
		
		//Name temporary object files using the SHA-1 sum of their contents, thereby consolidating any duplicate files
		string fileChecksum = sha1(fileContents);
		if (file_put_contents(this->tempDir + "/" + fileChecksum + ".o", fileContents) == false) {
			return false;
		}
		
		//Keep track of the object files we have processed
		if (std::find(seenObjectFiles.begin(), seenObjectFiles.end(), fileChecksum) != seenObjectFiles.end()) {
			clog << "Note: consolidating duplicate object files with SHA-1 checksum " << fileChecksum << endl;
		}
		else {
			seenObjectFiles.push_back(fileChecksum);
		}
	}
	
	return true;
}

void StaticLibraryManager::Cleanup()
{
	for (vector<string>::iterator currFile = seenObjectFiles.begin(); currFile != seenObjectFiles.end(); ++currFile) {
		unlink(string(this->tempDir + "/" + *currFile + ".o").c_str());
	}
	
	unlink(this->tempDir.c_str());
}
