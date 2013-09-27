/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2013, Adam Rehn
//
//  ---
//
//  Dynamic Library Class
//
//  This class wraps around the platform-specific implementations for
//  loading dynamic libraries and retrieving function pointers to the
//  functions within them.
//
//  Objects of this wrapper class can be copied because both dlopen()
//  under POSIX systems and LoadLibrary() under Windows systems will
//  maintain a reference count for loaded libraries, and only free them
//  once their reference count reaches zero due to an equal number of
//  calls to dlclose() or FreeLibrary(), respectively. For details, see:
//  <http://linux.die.net/man/3/dlopen>
//  <http://msdn.microsoft.com/en-us/library/windows/desktop/ms684175%28v=vs.85%29.aspx>
//
//  *******
//  Note that under POSIX platforms, applications using this class will
//  need to link against libdl (-ldl), which contains dlopen and friends.
//  *******
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
#ifndef __DYNAMIC_LIBRARY_H
#define __DYNAMIC_LIBRARY_H

#ifdef _WIN32
	#include <windows.h>
	
	typedef HMODULE dynlib_t;
	typedef FARPROC dynfunc_t;
#else
	#include <dlfcn.h>
	
	typedef void* dynlib_t;
	typedef void* dynfunc_t;
#endif

#include <string>
using std::string;

class DynamicLibrary
{
	public:
		DynamicLibrary()
		{
			this->library = NULL;
			this->libraryPath = "";
		}
		
		DynamicLibrary(const string& libraryPath)
		{
			this->library = NULL;
			this->libraryPath = "";
			this->LoadLibrary(libraryPath);
		}
		
		DynamicLibrary(const DynamicLibrary& other)
		{
			this->library = NULL;
			this->libraryPath = "";
			this->CopyFrom(other);
		}
		
		~DynamicLibrary()
		{
			this->CloseLibrary();
		}
		
		DynamicLibrary& operator=(const DynamicLibrary& other)
		{
			if (&other == this) {
				return *this;
			}
			
			this->CopyFrom(other);
			return *this;
		}
		
		bool LoadLibrary(const string& libraryPath)
		{
			//If a library is currently loaded, close it
			if (this->LibraryLoaded() == true) {
				this->CloseLibrary();
			}
			
			//Attempt to load the specified library
			#ifdef _WIN32
				this->library = ::LoadLibrary(libraryPath.c_str());
			#else
				this->library = dlopen(libraryPath.c_str(), RTLD_LAZY);
			#endif
			
			//Store the library path
			this->libraryPath = libraryPath;
			
			//Indicate success or failure
			return (this->LibraryLoaded());
		}
		
		bool LibraryLoaded() const {
			return (library != NULL);
		}
		
		template <typename T>
		T LoadFunction(const string& functionName)
		{
			//Do not attempt to load a function if the library is not loaded
			if (this->LibraryLoaded() == false) {
				return NULL;
			}
			
			//Attempt to load the specified function
			#ifdef _WIN32
				dynfunc_t func = ::GetProcAddress(this->library, functionName.c_str());
			#else
				dynfunc_t func = dlsym(this->library, functionName.c_str());
			#endif
			
			//Attempt to return the function pointer with the requested type
			return (T)func;
		}
		
	private:
		void CloseLibrary()
		{
			if (this->library != NULL)
			{
				#ifdef _WIN32
					::FreeLibrary(this->library);
				#else
					dlclose(this->library);
				#endif
				
				this->library = NULL;
				this->libraryPath = "";
			}
		}
		
		void CopyFrom(const DynamicLibrary& other)
		{
			//If a library is currently loaded, close it
			if (this->LibraryLoaded() == true) {
				this->CloseLibrary();
			}
			
			//Open the library, duplicating the handle and increasing its reference count
			this->LoadLibrary(other.libraryPath);
		}
		
		//The handle to the currently loaded library
		dynlib_t library;
		
		//Keep the path to the library so we can duplicate the handle
		string libraryPath;
};

#endif
