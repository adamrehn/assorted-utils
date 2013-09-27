/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2011-2013, Adam Rehn
//
//  ---
//
//  Array Manipulation Functions
//
//  These functions work with arrays and vectors.
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
#ifndef _LIB_SIMPLE_BASE_ARRAY_H
#define _LIB_SIMPLE_BASE_ARRAY_H

#include <vector>
using std::vector;

//Function to add the contents of one vector to another
template <typename T>
void mergeVectors(vector<T>& first, const vector<T>& second)
{
	first.insert( first.end(), second.begin(), second.end() );
}

#endif
