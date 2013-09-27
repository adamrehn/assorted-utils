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
#include "maths.h"

bool is_even(int num)
{
	return !is_odd(num);
}

bool is_odd(int num)
{
	return num % 2;
}

int	round_number(double num)
{
	stringstream buf;
	buf << fixed << setprecision(0) << num;
	int result = 0;
	buf >> result;
	return result;
}

bool is_multiple_of(int number, int factor)
{
	return !(number % factor);
}

vector<int> simplify(int numerator, int denominator)
{
	//Keep track of whether or not any common factors were found
	bool commonFactors = true;
	
	//This variable is used to loop through numbers when searching for common factors
	int i = 2;
	
	//Keep looping until all common factors have been removed
	while (commonFactors)
	{
		//Loop until a common factor is found (and make sure to stop if none are found)
		while ((i < numerator && i < denominator) && !(is_multiple_of(numerator, i) && is_multiple_of(denominator, i)))
		{
			i++;
		}
		
		//If a common factor was found, divide the numerator and denominator and reset the stepping variable to 2
		if (is_multiple_of(numerator, i) && is_multiple_of(denominator, i))
		{
			//Divide both the numerator and denominator by the common factor
			numerator /= i;
			denominator /= i;
			
			//Reset the stepping variable to 2
			i = 2;
		}
		else
			commonFactors = false;
	}
	
	//Put the simplified numerator and denominator in a vector (since there is no tuple datatype in STL) and return it
	vector<int> result;
	result.push_back(numerator);
	result.push_back(denominator);
	return result;
}

int round_to_nearest_multiple_of(int num, int base)
{
	int upper = num, lower = num;
	int distance_upper = 0, distance_lower = 0;
	
	while (!is_multiple_of(upper, base)) {
		upper++;
	}
	
	while (!is_multiple_of(lower, base)) {
		lower--;
	}
	
	distance_upper = upper - num;
	distance_lower = num - lower;
	
	if (distance_upper < distance_lower) {
		return upper;
	}
	else {
		return lower;
	}
}
