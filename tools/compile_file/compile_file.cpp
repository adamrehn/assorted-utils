/*
//  Binary File to C-code byte array Converter (compile_file)
//  Copyright (c) 2009-2013, Adam Rehn
//
//  This handy little utility creates C code files so that programs can easily
//  embed binary files and then unpack or otherwise utilise them at runtime.
//
//  Usage Syntax:       compile_file  INFILE [INFILE] [INFILE] [...]
//
//  ---
//
//  compile_file is free software: you can redistribute it and/or modify it
//  under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with compile_file. If not, see <http://www.gnu.org/licenses/>.
*/
#define BUFSIZE 512*1024

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <simple-base/base.h>

using namespace std;

int main (int argc, char *argv[])
{
	//We require at least one argument - an input file
	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			//Open the file at the end, so we can ascertain its length
			ifstream infile(argv[i], ios::binary | ios::ate);
			if (infile.is_open())
			{
				//Get the length and then seek back to the beginning
				size_t length = infile.tellg();
				infile.seekg(0, ios::beg);
				
				//This will transform the filename into a constant variable style, e.g: "file.dat" becomes "FILE_DAT"
				string filename = strtoupper(strip_chars(" '~!@#$%^&()+[]{}", str_replace(".", "_", basename(string(argv[i])))));
				string outpath = string(dirname(string(argv[i])) + strtolower(filename) + ".c");
				
				//Open the output file, which will be the transformed filename with a .c extension (following the earlier example, "file.dat" becomes "file_dat.c")
				ofstream outfile(outpath.c_str(), ios::binary);
				if (outfile.is_open())
				{
					//Each file becomes a constant integer storing its length, and a character array holding all of the individual bytes
					outfile << "const int SIZEOF_" << filename << " = " << length << ";" << endl;
					outfile << "const char " << filename << "[" << length << "] = {\n";
					
					//Read the file one block at a time
					char buffer[BUFSIZE];
					size_t bytesRead = 0;
					while ( (bytesRead = infile.read(buffer, sizeof(buffer)).gcount()) != 0 )
					{
						for (size_t i = 0; i < bytesRead; ++i)
						{
							//Output the hex value of each byte and seperate them with commas
							outfile << byte_to_hex(buffer[i]) << ",";
						}
					}
					
					//Rewind to remove the last comma
					unsigned int currPos = (int)outfile.tellp();
					unsigned int newPos = currPos - 1;
					outfile.seekp(newPos);
					
					//Finish the array (making sure we add a newline to keep certain versions of GCC happy), and close the output file
					outfile << "\n};" << endl;
					outfile.close();
				}
			}
		}
	}
	else {
		clog << "Usage Syntax:" << endl << "compile_file  INFILE [INFILE] [INFILE] [...]" << endl;
	}
	
	return 0;
}
