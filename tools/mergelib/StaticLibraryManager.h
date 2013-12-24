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
#ifndef _MERGELIB_STATIC_LIBRARY_MANAGER
#define _MERGELIB_STATIC_LIBRARY_MANAGER

#include <string>
#include <vector>
using std::string;
using std::vector;

class StaticLibraryManager
{
	public:
		StaticLibraryManager(const string& tempDir);
		~StaticLibraryManager();
		
		void AddLibrary(const string& lib);
		void MergeLibraries(const string& outlib);
		
	private:
		bool ExtractLibToTempDirectory(const string& lib);
		void Cleanup();
		
		string tempDir;
		vector<string> seenLibs;
		vector<string> seenObjectFiles;
};

#endif
