/*
//  MIDI+DLS to RMID Combiner (midi2rmid)
//  Copyright (c) 2009-2014, Adam Rehn
//  
//  This utility combines a MIDI file and a matching DLS file into an RMID file.
//  
//  Usage Syntax:
//  midi2rmid INPUT.MID [INPUT.DLS] [OUTPUT.RMI]
//  
//  ---
//  
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
*/
#include <simple-base/base.h>
#include <cstring>
#include <stdint.h>
#include <stdexcept>
#include <iostream>
#include <string>

//The size of the header for the RMID chunk itself
#define RMID_CHUNK_HEADER_SIZE 12

using namespace std;

void copyFileData(ifstream& infile, ofstream& outfile)
{
	//Read the input file in chunks
	char buffer[1024*1024];
	size_t bytesRead = 0;
	while ( (bytesRead = infile.read(buffer, sizeof(buffer)).gcount()) != 0 )
	{
		//Write the data to the output file
		outfile.write(buffer, bytesRead);
	}
}

int main (int argc, char* argv[])
{
	//Check that an input file was specified
	if (argc > 1)
	{
		try
		{
			//Parse the supplied arguments
			string midiFile = argv[1];
			string dlsFile  = ((argc > 2) ? argv[2] : replace_extension(midiFile, "dls"));
			string rmidFile = ((argc > 3) ? argv[3] : replace_extension(midiFile, "rmi"));
			
			//Check that both of the input files exist
			uint32_t midiSize = filesize(midiFile);
			uint32_t dlsSize  = filesize(dlsFile);
			if (midiSize == -1 || dlsSize == -1) {
				throw std::runtime_error("the input files \"" + midiFile + "\" and \"" + dlsFile + "\" could not be opened.");
			}
			
			//The 20-byte RMID file header           Placeholder for RMID size                                   Placeholder for MIDI size
			char rmidHeader[] = {'R', 'I', 'F', 'F', 0x0, 0x0, 0x0, 0x0, 'R', 'M', 'I', 'D', 'd', 'a', 't', 'a', 0x0, 0x0, 0x0, 0x0};
			
			//Fill the placeholder header fields with the length values
			uint32_t rmidSize = RMID_CHUNK_HEADER_SIZE + midiSize + dlsSize;
			rmidSize = toLittleEndian(rmidSize);
			midiSize = toLittleEndian(midiSize);
			memcpy(rmidHeader + 4,  &rmidSize, sizeof(uint32_t));
			memcpy(rmidHeader + 16, &midiSize, sizeof(uint32_t));
			
			//Open the input and output files
			ifstream midiFileStream(midiFile.c_str(), ios::binary);
			ifstream dlsFileStream (dlsFile.c_str(),  ios::binary);
			ofstream rmidFileStream(rmidFile.c_str(), ios::binary);
			
			//If any of the input or output files could not be opened, exit immediately
			if (!midiFileStream.is_open()) { throw std::runtime_error("failed to open input file \"" + midiFile + "\""); }
			if (!dlsFileStream.is_open())  { throw std::runtime_error("failed to open input file \"" + dlsFile  + "\""); }
			if (!rmidFileStream.is_open()) { throw std::runtime_error("failed to open input file \"" + rmidFile + "\""); }
			
			//Write the header, MIDI data, and DLS data
			rmidFileStream.write(rmidHeader, sizeof(rmidHeader));
			copyFileData(midiFileStream, rmidFileStream);
			copyFileData(dlsFileStream,  rmidFileStream);
			
			//Close the input and output files
			midiFileStream.close();
			dlsFileStream.close();
			rmidFileStream.close();
		}
		catch (std::runtime_error& e) {
			clog << "Error: " << e.what() << endl;
		}
	}
	else {
		clog << "Usage syntax:" << endl << "midi2rmid INPUT.MID [INPUT.DLS] [OUTPUT.RMI]" << endl;
	}
	
	return 0;
}
