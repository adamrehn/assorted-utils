/*
//  Simple Base Library for C++ (libsimple-base)
//  Copyright (c) 2012-2013, Adam Rehn
//
//  ---
//
//  Mac OS X FSEvents API Facade
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
#ifndef _LIB_SIMPLE_BASE_OSX_FSEVENTS_H
#define _LIB_SIMPLE_BASE_OSX_FSEVENTS_H

//These functions are for OS X only, and are not applicable to iOS
#if defined __APPLE__ && defined __MACH__
#include "TargetConditionals.h"
#if (!TARGET_OS_IPHONE)

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/event.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

namespace _fsevents_imp {

//This function is from the FSEvents API example code (Watcher.c) at
//<https://developer.apple.com/library/mac/#samplecode/Watcher/Listings/Watcher_c.html>
//The sample code's license states that it can be used if the copyright notice is included:
/*

Disclaimer: IMPORTANT:  This Apple software is supplied to you by 
Apple Inc. ("Apple") in consideration of your agreement to the
following terms, and your use, installation, modification or
redistribution of this Apple software constitutes acceptance of these
terms.  If you do not agree with these terms, please do not use,
install, modify or redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and
subject to these terms, Apple grants you a personal, non-exclusive
license, under Apple's copyrights in this original Apple software (the
"Apple Software"), to use, reproduce, modify and redistribute the Apple
Software, with or without modifications, in source and/or binary forms;
provided that if you redistribute the Apple Software in its entirety and
without modifications, you must retain this notice and the following
text and disclaimers in all such redistributions of the Apple Software. 
Neither the name, trademarks, service marks or logos of Apple Inc. 
may be used to endorse or promote products derived from the Apple
Software without specific prior written permission from Apple.  Except
as expressly stated in this notice, no other rights or licenses, express
or implied, are granted by Apple herein, including but not limited to
any patent rights that may be infringed by your derivative works or by
other works in which the Apple Software may be incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

Copyright (C) 2007 Apple Inc. All Rights Reserved.

*/
//
//--------------------------------------------------------------------------------
//  Simple wrapper to create a CFArray that contains a single
//  CFString in it (in this program it's the path we want to
//  watch).
//
//
template <typename T>
CFMutableArrayRef create_cfarray_from_path(T path)
{
    CFMutableArrayRef cfArray;

    cfArray = CFArrayCreateMutable(kCFAllocatorDefault, 1, &kCFTypeArrayCallBacks);
    if (cfArray == NULL) {
	return NULL;
    }

    CFStringRef cfStr = CFStringCreateWithCString(kCFAllocatorDefault, path, kCFStringEncodingUTF8);
    if (cfStr == NULL) {
	CFRelease(cfArray);
	return NULL;
    }

    CFArraySetValueAtIndex(cfArray, 0, cfStr);
    CFRelease(cfStr);
 	
    return cfArray;
}
//--------------------------------------------------------------------------------

//Since void* cannot be cast to a function pointer, we need to wrap the callback pointer
//in a struct, which allows us to use either function pointers or function objects
template <typename CallbackTy>
struct callbackContainer
{
	CallbackTy callback;
};

//This callback function is called by the library's Run Loop when an event is received
template <typename CallbackTy>
void eventCallback(FSEventStreamRef streamRef, void *clientCallBackInfo, int numEvents, const char *const eventPaths[], const FSEventStreamEventFlags *eventFlags, const uint64_t *eventIDs)
{
	//Retrieve the callback function and call it
	callbackContainer<CallbackTy>* cContainer = (callbackContainer<CallbackTy>*)clientCallBackInfo;
	bool keepRunning = cContainer->callback();
	
	//If the callback returned false, stop monitoring the directory
	if (keepRunning == false) {
		CFRunLoopStop( CFRunLoopGetCurrent() );
	}
}

} //End namespace _fsevents_imp

//The actual OSX implementation of MonitorDirectoryForFileWrites
template <typename CallbackTy>
void MonitorDirectoryForFileWrites(const string& dir, CallbackTy callback)
{
	FSEventStreamContext context = {0, NULL, NULL, NULL, NULL};
	FSEventStreamRef stream_ref = NULL;
	CFMutableArrayRef cfarray_of_paths;
	
	//Store the actual callback pointer in the context's info field,
	//so it will be passed to our internal callback as clientCallBackInfo
	_fsevents_imp::callbackContainer<CallbackTy>* cContainer = new _fsevents_imp::callbackContainer<CallbackTy>;
	cContainer->callback = callback;
	context.info = (void*)cContainer;
	
	//Create the array holding the path string
	cfarray_of_paths = _fsevents_imp::create_cfarray_from_path(dir.c_str());
    if (cfarray_of_paths == NULL)
	{
		//Failed to create array of paths
		delete cContainer;
		return;
	}
	
	//Create the event stream
	stream_ref = FSEventStreamCreate(kCFAllocatorDefault, (FSEventStreamCallback)&_fsevents_imp::eventCallback<CallbackTy>, &context, cfarray_of_paths, kFSEventStreamEventIdSinceNow, 0.5, kFSEventStreamCreateFlagNone);
	CFRelease(cfarray_of_paths);
	if (stream_ref != NULL)
	{
		//Start the event stream
		FSEventStreamScheduleWithRunLoop(stream_ref, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		if (!FSEventStreamStart(stream_ref))
		{
			//Failed to start the FSEventStream
			delete cContainer;
			return;
		}
		
		//Perform the Run Loop
		CFRunLoopRun();
		
		//When we get to this point, the Run Loop has stopped
		
		//Stop and release the stream
		FSEventStreamStop(stream_ref);
		FSEventStreamInvalidate(stream_ref);
		FSEventStreamRelease(stream_ref);
	}
	else
	{
		//Failed to create the event stream
	}
	
	//Free the callback pointer container
	delete cContainer;
}

#endif
#endif
#endif
