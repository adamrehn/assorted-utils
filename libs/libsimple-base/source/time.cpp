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
#include "time.h"
#include "string_manipulation.h"
#include "environment.h"
#include <stdexcept>
#include <ctime>

#ifdef _MSC_VER


//This implementation from <http://suacommunity.com/dictionary/gettimeofday-entry.php>
#define DELTA_EPOCH_IN_MICROSECS  116444736000000000Ui64 // CORRECT

struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};
 
// Definition of a gettimeofday function
 
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	// Define a structure to receive the current Windows filetime
	FILETIME ft;
	
	// Initialize the present time to 0 and the timezone to UTC
	unsigned __int64 tmpres = 0;
	static int tzflag = 0;
	
	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);
		
		// The GetSystemTimeAsFileTime returns the number of 100 nanosecond 
		// intervals since Jan 1, 1601 in a structure. Copy the high bits to 
		// the 64 bit tmpres, shift it left by 32 then or in the low 32 bits.
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;
		
		// Convert to microseconds by dividing by 10
		tmpres /= 10;
		
		// The Unix epoch starts on Jan 1 1970.  Need to subtract the difference 
		// in seconds from Jan 1 1601.
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		
		// Finally change microseconds to seconds and place in the seconds value. 
		// The modulus picks up the microseconds.
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}
	
	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		
		// Adjust for the timezone west of Greenwich
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}
	
	return 0;
}

#endif

timeval microtime()
{
	timeval theTime;
	if (gettimeofday(&theTime, NULL) != 0) {
		throw std::runtime_error("Couldn't get time!");
	}
	return theTime;
}

double timeDiff(const timeval& firstTime, const timeval& secondTime)
{
	//Calculate the difference in seconds
	int diffSeconds = secondTime.tv_sec - firstTime.tv_sec;
	
	//Calculate the difference in microseconds
	int diffMicroseconds = secondTime.tv_usec - firstTime.tv_usec;
	
	//Convert to a double
	double diff = static_cast<double>(diffSeconds);
	
	//Add the microseconds
	diff += (static_cast<double>(diffMicroseconds) / 1000000.0);
	
	//Return the result
	return diff;
}

int timeDiffMs(const timeval& firstTime, const timeval& secondTime)
{
	//Calculate the difference in seconds
	int diffSeconds = secondTime.tv_sec - firstTime.tv_sec;
	
	//Calculate the difference in microseconds
	int diffMicroseconds = secondTime.tv_usec - firstTime.tv_usec;
	
	//Scale the two components to milliseconds
	int diff = (diffSeconds * 1000) + (diffMicroseconds / 1000);
	
	//Return the result
	return diff;
}

//Sets the timezone environment variable
void set_timezone(string timezone)
{
	putenv(const_cast<char*>(string("TZ=" + timezone).c_str()));
}

//Creates the UTC timestamp for the supplied date details
time_t createTime(int year, int month, int day, int hour, int minute, int second)
{
	//Set the timezone to UTC so the generated timestamp will be UTC
	set_timezone("UTC");
	
	//Create a time_t variable of the current time
	time_t t = time(NULL);
	
	//Create a tm struct to fill with the user-supplied values
	tm* timeDetails = gmtime(&t);
	
	//Fill in the user-supplied values
	timeDetails->tm_year = year - 1900; //The value of tm_year is years since 1900
	timeDetails->tm_mon  = month - 1;   //Zero-based index
	timeDetails->tm_mday = day;
	timeDetails->tm_hour = hour;
	timeDetails->tm_min  = minute;
	timeDetails->tm_sec  = second;
	
	//Generate the unix timestamp
	return mktime(timeDetails);
}

//Formats a UTC timestamp as human-readable text
string date(string format, time_t timestamp, bool insertSymbols)
{
	//Create a tm struct from the timestamp
	tm* timeDetails = gmtime(&timestamp);
	
	//Create a buffer to hold the formatted string
	char buf[255];
	
	//Insert % symbols in the format string
	if (insertSymbols)
	{
		string specifiers = "%aAbBcdHIjmMpSUwWxXyYZ";
		for (size_t i = 0; i < specifiers.length(); ++i)
		{
			string specifier = string(1, specifiers[i]);
			format = str_replace(specifier, "%" + specifier, format);
		}
	}
	
	//Use monthSuffix to insert the day of month suffix
	format = str_replace("%suf", monthSuffix(timeDetails->tm_mday), format);
	
	//Generate the formatted string
	strftime(buf, sizeof(buf), format.c_str(), timeDetails);
	string formattedString = string(buf);
	
	//Correct any days of the month under 10
	formattedString = str_replace(" 01st ", " 1st ", formattedString);
	formattedString = str_replace(" 02nd ", " 2nd ", formattedString);
	formattedString = str_replace(" 03rd ", " 3rd ", formattedString);
	formattedString = str_replace(" 04th ", " 4th ", formattedString);
	formattedString = str_replace(" 05th ", " 5th ", formattedString);
	formattedString = str_replace(" 06th ", " 6th ", formattedString);
	formattedString = str_replace(" 07th ", " 7th ", formattedString);
	formattedString = str_replace(" 08th ", " 8th ", formattedString);
	formattedString = str_replace(" 09th ", " 9th ", formattedString);
	
	//Return the formatted string
	return formattedString;
}

string date(string format)
{
	//Use the current time as the timestamp
	time_t now = time(NULL);
	return date(format, now);
}

//Returns the English ordinal suffix for the day of the month (st, nd, rd, th)
string monthSuffix(int dayOfMonth)
{
	//Convert the day of month to a string and isolate the last two digits
	string dayStr        = toString(dayOfMonth);
	char lastDigit       = toInt(dayStr.substr(dayStr.length() - 1, 1));
	char secondLastDigit = (dayStr.length() > 1) ? toInt(dayStr.substr(dayStr.length() - 2, 1)) : 0;
	
	//1st
	if (lastDigit == 1 && secondLastDigit != 1) {
		return "st";
	}
	
	//2nd
	if (lastDigit == 2 && secondLastDigit != 1) {
		return "nd";
	}
	
	//3rd
	if (lastDigit == 3 && secondLastDigit != 1) {
		return "rd";
	}
	
	//4th, 5th, etc...
	return "th";
}

//Returns the offset (in seconds) from GMT for the current timezone
int get_current_timezone_offset()
{
	#ifdef _WIN32
		
		//Retrieve the current timezone information (Win32 only)
		TIME_ZONE_INFORMATION TimeZoneInfo;
		if (GetTimeZoneInformation(&TimeZoneInfo) == TIME_ZONE_ID_INVALID) {
			throw std::runtime_error("Failed to get timezone information! (Error Code " + toString(GetLastError()) + ")");
		}
		
		//Transform the bias into an offset in seconds
		int timezoneOffset = (TimeZoneInfo.Bias * -1) * 60;
		return timezoneOffset;
		
	#else
		
		//On other systems, we only support those with a date command supporting the non-standard %z literal
		std::string output = capture_output("date +%z 2>&1");
		if (is<double>(output))
		{
			double offset = to<double>(output);
			offset = offset / 100;
			return static_cast<int>(offset * (60 * 60));
		}
		else {
			throw std::runtime_error("Timezone offset reporting unsupported by this operating system!");
		}
		
	#endif
}
