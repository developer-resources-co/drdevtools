//=============================================================================
// iffdump.cpp:
//=============================================================================

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

#include <pclib/general.hpp>
#include <pclib/list.hpp>
#include <pclib/iff.hpp>

//==============================================================================

boolean printChunkData = boolean::FALSE;

const int ERR_NOFILE = 5;

//==============================================================================

int
main(int argc,char *argv[])
{
	iffRead *readPtr = NULL;
	int columnCount;
	ulong offset;
	int argCount;

	if(argc < 2)
	 {
		cerr << "\
IffDump V1.3 (c) 1992,94 Adept Creations\n\
Iff Chunk displayer\n\
By Kevin T. Seghetti\n\
Format: IffDump [<switches>] <filename>\n\
	Switches:\n\
		-d: Dump chunk data\n\
";
		return(ERR_NOFILE);
	 }

	argCount = 1;
	while(argCount < argc)
	 {
		if(*argv[argCount] == '-')				// must be command line switch
			switch(argv[argCount][1])
			 {
				case 'd':
				case 'D':
					printChunkData = boolean::TRUE;
					break;
			 }
		else break;
		argCount++;
	 }

	ifstream input(argv[argCount],ios::in | ios::binary);

	cout.fill('0');

	if(input)
	 {
		readPtr = new iffRead(input);
		readPtr->NextForm();
		cout << "Form: ";
		PrintTag(readPtr->GetForm());
		cout << "  Length: " << hex << readPtr->GetFormLen() << '\n';
		while((input.rdstate() == ios::goodbit) && readPtr->GetFormBytesLeft() > 0)
		 {
			readPtr->NextChunk();
			if(readPtr->GetFormBytesLeft() > 0)
			 {
				cout << "    Chunk: ";
				PrintTag(readPtr->GetChunk());
				cout << "  Length: " << hex << readPtr->GetChunkLen();
				if(printChunkData)
				 {
					offset = 0;
					columnCount = 15;
					while(readPtr->GetChunkBytesLeft() && readPtr->GetFormBytesLeft())
					 {
						if(columnCount++ == 15)
						 {
							cout << "\n        " << setw(6) << hex << offset << ": ";
							columnCount = 0;
						 }
						else if(!(columnCount & 3))
							cout << ' ';
						cout << setw(2) << hex << (int)readPtr->GetChunkByte();
						offset += 16;
					 }
				 }
				cout << '\n';
			 }
		 }
	 }
	else
		cout << "Error: file not found\n";
	delete readPtr;
	return(0);
}

//==============================================================================

