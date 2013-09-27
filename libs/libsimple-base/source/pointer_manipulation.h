/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2009-2013, Adam Rehn
//
//  ---
//
//  Pointer Manipulation Functions
//
//  Memory-management related functionality.
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
#ifndef _LIB_SIMPLE_BASE_POINTER_H
#define _LIB_SIMPLE_BASE_POINTER_H

#include <vector>
using std::vector;

//Cleans up the memory used by a vector of pointers (not good for arrays, only pointers to single objects or native types)
template <typename T>
void cleanup_vector(vector<T*>& v)
{
	for (int i = 0; i < v.size(); ++i)
	{
		try {
			if (v[i] != NULL) delete v[i];
		}
		catch (...) {
			//Do nothing
		}
	}
}

#endif
