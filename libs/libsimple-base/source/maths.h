/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2010-2013, Adam Rehn
//
//  ---
//
//  Mathematical Functions
//
//  These functions perform mathematical calculations.
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
#ifndef _LIB_SIMPLE_BASE_MATHS_H
#define _LIB_SIMPLE_BASE_MATHS_H

#include <cmath>
#include <sstream>
#include <iomanip>
#include <vector>
using std::stringstream;
using std::fixed;
using std::setprecision;
using std::vector;

bool        is_even(int num);                         //Determines whether the supplied number is an even number
bool        is_odd(int num);                          //Determines whether the supplied number is an odd number
int         round_number(double num);                 //Rounds a number to the nearest whole number
bool        is_multiple_of(int number, int factor);   //Checks to see whether a number is a multiple of another number
vector<int> simplify(int numerator, int denominator); //Simplifies a fraction and returns the result as a tuple/vector
int         round_to_nearest_multiple_of(int, int);	  //Rounds the first number to the nearest multiple of the second

#endif
