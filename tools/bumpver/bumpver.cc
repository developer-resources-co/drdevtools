//==============================================================================
// BumpVer.cpp: Update version # in source
// By Kevin T. Seghetti
// (c) 1993 Adept Creations
//==============================================================================

// restrictions:
//	version # must be surrounded by delimeter
//
//
//
//
//

//== C++ =======================================================================
#include <iostream.h>
#include <fstream.h>

//== C =========================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//== Turbo C++ (IBM PC C libraries, but not standard C) ========================
//#include <dir.h>

//== Adept Creations (or Developer Resources) "Library" ========================
#include <pclib\libtypes.hpp>

//== Application ===============================================================
#include "version.hp"

//==============================================================================

class version
{
	ulong major,minor,revision;
public:
	version(ulong newMajor = 1, ulong newMinor = 0, ulong newRevision = 0)
		{ major = newMajor; minor = newMinor; revision = newRevision; }
	ulong Major(void) { return(major); }
	ulong Major(ulong newMajor) { major = newMajor; return(major); }
	ulong Minor(void) { return(minor); }
	ulong Minor(ulong newMinor) { minor = newMinor; return(minor); }
	ulong Revision(void) { return(revision); }
	ulong Revision(ulong newRevision) { revision = newRevision; return(revision); }

	void BumpMajor(ulong howFar = 1) { major += howFar; minor = revision = 0; }
	void BumpMinor(ulong howFar = 1) { minor += howFar; revision = 0; }
	void BumpRevision(ulong howFar = 1) { revision += howFar; }
};

//==============================================================================


char delimeter = '\"';
char verDelimeter = '.';

const char szAppName[] = "BumpVer";

typedef enum
	{
	FALSE, TRUE
	} BOOL;

//==============================================================================

streampos
FindVerNumber(istream &input, char* sigString)
{
	ulong pos;
	char ch;
	ulong strPos,len;
	ulong sLen = strlen(sigString);

	streampos start = input.tellg();
	streampos prev;

	cout << "sigString [" << sigString << "]" << endl;

	while( !input.eof() )	// input.rdstate() & ios::eofbit )
	 {
		// search for first char in sigString
    	do
        	{
				input.get(ch);
				printf( "%c", ch );
        	}
		while ( ch != sigString[0] && !input.eof() );

		cout << "Matched first character" << endl;

		if( input.eof() )
			return(-1);
		prev = input.tellg();

		// compare rest of sigString
		strPos = 0;
		len = sLen-1;
    	do
        	{
				input.get(ch);
				printf( "%c", ch );
        	}
		while ( len > 0 && ch == sigString[++strPos] && (!input.eof()) );

		cout << "len: " << len << endl;
		// did it match?
		if ( len == 0 )
		 {
			prev = input.tellg();
			input.seekg(start);
			cout << "returning " << prev << endl;
			return(prev);
		 }

		if( input.eof() )
			return(-1);
		input.seekg(prev);
	 }
	return(-1);
}

//==============================================================================

ulong
ReadNum(istream &input, char delimeter)
{
	char str[100];
	int i = 0;
	char ch;
	input.get(ch);
    while ( ch != delimeter && i < 99)
        {
			str[i++] = ch;
			input.get(ch);
        }

	str[i] = 0;
	return(atol(str));
}

//==============================================================================

int
UpdateVersion(char* inName, char* outName, char whatToBump, long howFar)
{
	unsigned char ch;
	streampos verStringPos;
	version ver;

    ifstream input(inName,ios::in | ios::binary);
    if ( input.rdstate() & ios::badbit )
        {
            cout << "Error opening input file:" << inName << '\n';
            return(1);
        }

	ofstream output(outName,ios::out | ios::binary);
    if ( output.rdstate() & ios::badbit )
        {
            cout << "Error opening output file:" << outName << '\n';
            return (1);
        }

	verStringPos = FindVerNumber(input,"BumpVer signature string, delimiter=");
	if(verStringPos == -1)
     {
        printf ("Error locating signature string");
        return (1);
     }

	 // copy file up to end of signature string
    while ( --verStringPos )
        {
			input.get(ch);
			output.put(ch);
        }

	 input.get(delimeter);					// read delimeter
	 output.put(delimeter);

	// scan forward to version string
	input.get(ch);
    while ( ch != delimeter )
        {
			output.put(ch);
			input.get(ch);
        }
	output.put(ch);

	// ok, time to read in current version #
	ver.Major(ReadNum(input,verDelimeter));
	ver.Minor(ReadNum(input,verDelimeter));
	ver.Revision(ReadNum(input,delimeter));
	// update version #
	switch(whatToBump)
	{
		case 'm':
			ver.BumpMajor(howFar);
			break;
		case 'i':
			ver.BumpMinor(howFar);
			break;
		case 'r':
		default:
			ver.BumpRevision(howFar);
			break;
	}
	// now write out new version #
	output << ver.Major() << verDelimeter << ver.Minor() << verDelimeter << ver.Revision() << delimeter;

	// copy rest of file
	input.get(ch);
    while ( !input.eof() )
        {
			output.put(ch);
			input.get(ch);
        }
	return 0;
}

//==============================================================================

int
ReplaceFile(const char* inName, const char* outName)
{
	char ch;
	char name[_MAX_PATH];

	tmpnam(name);
	int retCode = rename(outName,name);
	if(retCode)
		return(2);						// kts need error array

	retCode = rename(inName,outName);
	if(retCode)
		return(2);						// kts need error array

	retCode = remove(name);
	if(retCode)
		return(2);						// kts need error array


#if 0
    ifstream input(inName,ios::in | ios::binary);
    if ( input.rdstate() & ios::badbit )
        {
            cout << "Error opening input file:" << inName << '\n';
            return(1);
        }

	ofstream output(outName,ios::out | ios::binary);
    if ( output.rdstate() & ios::badbit )
        {
            cout << "Error opening output file:" << outName << '\n';
            return (1);
        }

	// copy file character at a time
	input.get(ch);
    while ( !input.eof() )
        {
			output.put(ch);
			input.get(ch);
        }
#endif
	return(0);
}


void
createNewVersionFile( const char* filename )
	{
	FILE* fp = fopen( filename, "wt" );
	assert( fp );

	fprintf( fp, "// %s\n", filename );
	fprintf( fp,
		"\n"
		"// BumpVer signature string, delimiter=\"\n"
		"char szVersion[] = \"1.0.0\";\n"
		"char szDate[] = __DATE__;\n"
		"char szTime[] = __TIME__;\n" );

	fclose( fp );
	}


//==============================================================================

int
main (int argc,char *argv[])
{
	char tempName[_MAX_PATH];
	char whatToBump = 'r';
	int argCount = 1;
	long howFar = 1;

	while(argCount < argc)
	 {
		if(*argv[argCount] == '-')				// must be command line switch
			switch(argv[argCount][1])
			 {
				case 'b':
					whatToBump = argv[argCount][2];
					if(argv[argCount][3])
						howFar = atoi(&argv[argCount][3]);
					break;
				case 'n':
					createNewVersionFile( "version.cc" );
					return 0;
					break;
			 }
		else
			break;
		argCount++;
	 }

	if(argc-argCount < 1)
		{
		char szProgName[_MAX_FNAME];

		_splitpath( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

		cout << szAppName << " V" << szVersion <<
" Copyright 1993-96 Cave Logic Studios, Ltd.  All Rights Reserved.\n"
"Updates source version number\n"
"By Kevin T. Seghetti\n"
"Usage: " << szProgName << " [<switches>] <versionfile>\n"
"Switches:\n"
"\t-bm{<#>}: Bump major version by #(default = 1)\n"
"\t-bi{<#>}: Bump minor version by #(default = 1)\n"
"\t-br{<#>}: Bump revision(default) by #(default = 1)\n"
"\t-n:       Generate a new \"version.cc\" file\n";
            return (1);
        }

	tmpnam(tempName);
	int errorCode = UpdateVersion(argv[0+argCount],tempName,whatToBump,howFar);
	if(errorCode > 0)
		return(errorCode);

	errorCode = ReplaceFile(tempName,argv[0+argCount]);
//	cout << "\ndone.";
	return(errorCode);
}

//==============================================================================

