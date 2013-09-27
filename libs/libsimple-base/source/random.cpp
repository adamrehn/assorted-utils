/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2012-2013, Adam Rehn
//
//  ---
//
//  Random Number Generation Functions
//
//  These functions abstract platform-specific facilities for pseudorandom
//  number generation.
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
#include "random.h"

#ifdef _WIN32

#include <windows.h>
#include <wincrypt.h>

//Windows doesn't have /dev/random or /dev/urandom, so we need to use the Win32 Cryptographic API

//Based on the code example from CERT Secure Coding Standard MSC32-C
//<https://www.securecoding.cert.org/confluence/display/seccode/MSC32-C.+Ensure+your+random+number+generator+is+properly+seeded>

bool GenerateRandomBytes(char* outputBuffer, size_t numBytes, bool useBestEntropy)
{
	//Acquire a cryptographic context
	HCRYPTPROV hCryptProv;
	if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0))
	{
		//Failed to acquire cryptographic context
		return false;
	}
	
	//Generate the random bytes
	if (!CryptGenRandom(hCryptProv, numBytes, (BYTE*) outputBuffer))
	{
		//Failed to generate random bytes
		return false;
	}
	
	//Success!
	return true;
}

#else

//On all other systems, just use /dev/random or /dev/urandom

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

bool GenerateRandomBytes(char* outputBuffer, size_t numBytes, bool useBestEntropy)
{
	//Determine whether we are using /dev/random or /dev/urandom
	const char* fileToUse = (useBestEntropy) ? "/dev/random" : "/dev/urandom";
	
	//Attempt to open the file
	int fileDescriptor = open(fileToUse, O_RDONLY);
	if (fileDescriptor == -1)
	{
		//Failed to open the file
		return false;
	}
	
	//Attempt to read the specified number of bytes
	size_t bytesToRead = numBytes;
	while (bytesToRead)
	{
		int bytesRead = read(fileDescriptor, outputBuffer, bytesToRead);
		
		//If an error other than EINTR occurred, reading failed
		if (bytesRead == -1 && errno == EINTR) {
			continue;
		}
		else if (bytesRead == -1 || bytesRead == 0)
		{
			//Close the file descriptor
			close(fileDescriptor);
			return false;
		}
		
		//Increment the output pointer and decrement the number of remaining bytes to read
		outputBuffer += bytesRead;
		bytesToRead  -= bytesRead;
	}
	
	//Close the file descriptor
	close(fileDescriptor);
	return true;
}

#endif
