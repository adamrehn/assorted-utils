/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2011-2013, Adam Rehn
//
//  ---
//
//  Time Related Functions
//
//  Functionality related to time.
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
#ifndef _LIB_SIMPLE_BASE_TIME_H
#define _LIB_SIMPLE_BASE_TIME_H

//Includes
#ifdef _MSC_VER
	#include <windows.h>
#else
	#include <sys/time.h>
#endif

#include <string>
using std::string;

//Returns a structure containing the current time in seconds and microseconds
timeval microtime();

//Calculates the difference, in seconds, between two timestamps
double timeDiff(const timeval& firstTime, const timeval& secondTime);

//Calculates the difference, in milliseconds, between two timestamps
int timeDiffMs(const timeval& firstTime, const timeval& secondTime);

//Sets the timezone environment variable
void set_timezone(string timezone);

//Creates the UTC timestamp for the supplied date details
time_t createTime(int year = 1970, int month = 1, int day = 1, int hour = 0, int minute = 0, int second = 0);

//Formats a UTC timestamp as human-readable text
string date(string format, time_t timestamp, bool insertSymbols = false);
string date(string format);

//Returns the English ordinal suffix for the day of the month (st, nd, rd, th)
string monthSuffix(int dayOfMonth);

//Returns the offset (in seconds) from GMT for the current timezone
int get_current_timezone_offset();

#endif
