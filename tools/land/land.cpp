//////////////////////////////////////////////////////////////////////////////
//
// land.cpp -- Landscape printer for HP LaserJet (and compatible) printers
//
// by William B. Norris IV
// (c) 1992,93,94 Cave Logic Studios.  All Rights Reserved.
//
// Bugs
//  Check multiple opening/closing for "print" of stdprn
//  Copyright message (when specified on command line) is lowercased
//
//////////////////////////////////////////////////////////////////////////////
#include <iostream.h>
#include <iomanip.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <dir.h>

#include "ljmain.h"
void draw_frame( int, int, int, int, int );

typedef unsigned int boolean;

FILE* input;
FILE* print;
int tabsize= 0 ;
char *szCopyrightMessage = "Copyright 1992,93,94 Cave Logic Studios.  All Rights Reserved.";
boolean bOneColumn = 0;


#ifndef __MSDOS__
FILE *stdprn;
#	ifdef AMIGA
#	define PRINTER_NAME "PAR:"
#	endif

#endif
FILE *fopen_laserjet(char *);

int tgets(char * buf,int max, FILE * strm);

int land( char *filename );

inline void position_cursor( int x, int y )	{ fprintf( print, "%c*p%dx%dY", ESC, x, y ); }
inline void rect_width( int width )	{ fprintf( print, "%c*c%dA", ESC, width ); }
inline void rect_height( int height )	{ fprintf( print, "%c*c%dB", ESC, height ); }
inline void pattern( int selection )	{ fprintf( print, "%c*c%dG", ESC, selection ); }
#define shading(x) pattern(x)
inline void print_bold() { fprintf(stdprn,"\x1b\x28\x73\x33\x42"); }
inline void print_norm() { fprintf(stdprn,"\x1b\x28\x73\x30\x42"); }
inline void fill_type( int type ) { fprintf( print, "%c*c%dP", ESC, type ); }



int
	main( int argc, char *argv[] )
	{
#ifndef __MSDOS__
	stdprn = fopen_laserjet("PRN");
#endif
	print = stdprn;

	if ( argc == 1 )
		{
		char szProgName[ 8+1 ];
		_fnsplit( argv[0], NULL, NULL, szProgName, NULL );
		cout << "LANDscape PRINT Version 2.0.4  " __DATE__ " " __TIME__ << endl <<
		     "Copyright 1992,93,94 Cave Logic Studios.  All Rights Reserved." << endl;
		cout << "Usage: " << strlwr(szProgName) << " [-t<tabSize>] [-o<outputFilename>] [-1] <filename|->...\n";
		exit(10);
		}

	// Set COPYRIGHT message based on environment variable
	char* szEnvCopyright = getenv( "COPYRIGHT" );
	if ( szEnvCopyright )
		szCopyrightMessage = szEnvCopyright;

	// Parse command line
	for ( ++argv; **argv && **argv == '-'; ++argv )
		{
		switch ( *(*argv+1) )
			{
			case '\0':
				// "-" means standard input (stdin)
				*argv = "stdin";
				break;
			case 'o':
			case 'O':
				// Close existing output file (if any), open new one
				if ( print )
					fclose( print );
				if ( !(print = fopen( *argv+2, "wb" ) ) )
					{
					cout << "Error opening output \"" << *argv+2 << "\"" << endl;
					exit( 10 );
					}
				continue;
			case 'c':
			case 'C':
				szCopyrightMessage = *argv+2;
				continue;
			case 't':
			case 'T':
				tabsize = atoi( *argv+2 );
				continue;
			case '1':
				bOneColumn = 1;
				continue;
			}
		}
	int nPages = land( strlwr( *argv ) );
	cout << *argv << ": " << nPages << " page" << (nPages==1 ? "" : "s") << endl;
	return 0;
	}


int
	land( char *filename )
	{
	int lpp;
	int curline;
	time_t today;
	int nPage = 1;
	FILE* input;

	if ( strcmp( filename, "stdin" ) == 0 )
		input = stdin;
	else if ( !(input = fopen( filename, "rt" ) ) )
		{
		cout << "Error opening file \"" << filename << "\"\n";
		exit(10);
		}

#ifdef TEST
	if ( strcmp( filename, "stdin" ) != 0 )
		fclose( input );

		return rand() % 10;
#endif

//?	lpp = 58;
	lpp = 56;

	time(&today);

	for ( ; !feof( input ); fprintf( print, "\xc" ), ++nPage )
		{
		char txbuf[512];
		char *todaysDate = ctime( &today );
		*strrchr( todaysDate, '\n' ) = '\0';

		// Landscape mode initialization
		fprintf(print,"\x1b&l1O");						// Landscape orientation
		fprintf(print,"\x1b(8U");						// Primary Symbol Set: Roman-8
		fprintf(print,"\x1b(sp16.67h8.5vs-1b0T");
		fprintf(print,"\x1b&l8D") ;					// 8 lines/inch
		fprintf(print,"\x1b&l10D") ;					// 10 lines/inch

		// Box around entire page
		draw_frame( 0, 38*2, 525*3*2, 750*3-(38*3), 2 );

		if ( !bOneColumn )
			{ // Vertical bar down middle of page
			draw_frame( 525*3, 38*3, 2, 750*3-(38*4), 2 );
			}

		// Shade top line
		draw_frame( 0, 38*2, 525*3*2, 38, 2 );
		position_cursor( 0,38*2 );
		rect_width( 525*3*2 );
		rect_height( 38 );
		shading( 10 );
		fill_type( 2 );

		// Print header
		fprintf(print,"\x1b&a%dR", 2 );
		fprintf(print,"\x1b&a%dC", 0 );
		//- Filename, etc.
	   print_bold();
	   fprintf(print,"[%s]  P:%d  %s", filename, nPage, todaysDate );
		//- Copyright message
		fprintf( print, "\x1b&a%dC", 88 );
		fprintf( print, "%s\r", szCopyrightMessage );
		print_norm();

		fprintf(print,"\x1b&a%dR", 2+1 );
		int tgets_ff;
		for (curline = 0; (!feof(input) && (curline < lpp)); curline++)
			{
			tgets_ff = tgets( txbuf, bOneColumn ? 174 : 86, input );
			fprintf(print,"%s\r",txbuf);
			if ( tgets_ff == -1 )
				break;
			}

		if ( !bOneColumn )
			{
		   	fprintf(print,"\x1b&a%dR",2+1 );
			for (curline = 0; (!feof(input) && (curline < lpp)); curline++)
				{
				tgets_ff = tgets(txbuf,86,input);
		   		fprintf(print,"\x1b&a%dC%s\r",88,txbuf);	// column #88
		   		if ( tgets_ff == -1 )
					break;
		   		}
			}
		}

	if ( strcmp( filename, "stdin" ) != 0 )
		fclose( input );

	return( nPage-1 );
	}


int	tabstop[] = { 4, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64,
					68,72,76,80,84,88,92,96,100 };

int	txttabstop[] =
		{ 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104 } ;

int
	tgets(char * buf,int max, FILE * strm)
	{
	int place = 0;
	int nch;

	if ( feof( strm ) )
		return 0;

	while (place < max && !feof(strm))
		{
		nch = fgetc(strm);
		switch (nch)
			{
			case '\f':
				buf[place++] = '\0';
				return -1;
			case '\n':
				buf[place++] = nch; buf[place++] = '\0';
				return 0;
			case '\t':
				{
					int f = 0;

					if (place>100) {
						buf[place++] = ' ' ;
						break ;
						}

					if (tabsize==0) {
						while (tabstop[f] <= place) {
							f++;
							}

						while (place < tabstop[f]) {
							buf[place++] = ' ';
							}
						}
					 else
						do {
							buf[place ++]=' ';
							} while (place % tabsize) ;

					}
				break;
			default:
				if (isspace(nch) || isprint(nch)) buf[place++] = nch;
			}
		}
	buf[place++] = '\n';
	buf[place++] = '\0';
	while(!feof(strm) && !('\n' == fgetc(strm) ) );
	return 0;
	}


void draw_frame( int x, int y, int width, int height, int border )
	{
	position_cursor(x,y);
	rect_width(border);
	rect_height(height);
	fill_type(0);

	position_cursor(x+width-border,y);
	rect_width(border);
	rect_height(height);
	fill_type(0);

	position_cursor(x,y);
	rect_width(width);
	rect_height(border);
	fill_type(0);

	position_cursor(x,y+height-border);
	rect_width(width);
	rect_height(border);
	fill_type(0);
	}
