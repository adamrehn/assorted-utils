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
				
				//Transform the filename into a constant variable style, e.g: "file.dat" becomes "FILE_DAT"
				string filename = basename(string(argv[i]));
				filename =  str_replace(".", "_", filename);
				filename =  str_replace("-", "_", filename);
				filename = strip_chars(" '~!@#$%^&()+[]{}", filename);
				filename = strtoupper(filename);
				string outpath    = string(dirname(string(argv[i])) + strtolower(filename) + ".c");
				string headerpath = string(dirname(string(argv[i])) + strtolower(filename) + ".h");
				
				//Open the output file, which will be the transformed filename with a .c extension (following the earlier example, "file.dat" becomes "file_dat.c")
				ofstream outfile(outpath.c_str(), ios::binary);
				if (outfile.is_open())
				{
					//Each file becomes a constant integer storing its length, and a character array holding all of the individual bytes
					outfile << "const int SIZEOF_" << filename << " = " << length << ";" << endl;
					outfile << "const unsigned char " << filename << "[" << length << "] = {\n";
					
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
				else {
					clog << "Error: failed to open output file!" << endl;
				}
				
				//Generate a header file to accompany the generated C file
				ofstream headerfile(headerpath.c_str(), ios::binary);
				if (headerfile.is_open())
				{
					headerfile << "#ifndef _" << filename << "_H" << endl;
					headerfile << "#define _" << filename << "_H" << endl << endl;
					headerfile << "extern \"C\" const int SIZEOF_" << filename << ";" << endl;
					headerfile << "extern \"C\" const unsigned char " << filename << "[];" << endl << endl;
					headerfile << "#endif" << endl;
					headerfile.close();
				}
				else {
					clog << "Error: failed to open header file!" << endl;
				}
			}
		}
	}
	else {
		clog << "Usage Syntax:" << endl << "compile_file  INFILE [INFILE] [INFILE] [...]" << endl;
	}
	
	return 0;
}
