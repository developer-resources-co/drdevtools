
#include <iostream.h>
#include <fstream.h>
#include <conio.h>

void
PutCommand( int command )
	{
	while ( inportb( 0x379 ) != 0xFF )
		;
	outportb( 0x378, command );
	outportb( 0x37A, 0xFD );
	while ( inportb( 0x379 ) != 0xBF )
		;
	outportb( 0x37A, 0xFF );

//	cout << "Command " << command << " sent." << endl;
	}

void
PutByte( unsigned b )
	{
	while ( inportb( 0x379 ) != 0xFF )
		;
	outportb( 0x378, b );
	outportb( 0x37A, 0xFE );
	while ( inportb( 0x379 ) != 0xBF )
		;
	outportb( 0x37A, 0xFF );

//	cout << "Byte " << b << " sent." << endl;
	}


void
PutWord( unsigned short i )
	{
	PutByte( i & 0xFF );
	PutByte( i >> 8 );
	}


void
PutLong( unsigned long l )
	{
	PutByte( l & 0xFF );
	PutByte( ( l >> 8 ) & 0xFF );
	PutByte( ( l >> 16 ) & 0xFF );
	}


void
SendFile( ifstream& input )
	{
	input.seekg( 0, ios::end );
	int size = input.tellg();
	input.seekg( 0, ios::beg );

	PutWord( size );

	for ( int i=0; i<size; ++i )
		{
		unsigned char byte;
		input.get( byte );
		PutByte( byte );
		}
	}
