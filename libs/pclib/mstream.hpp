// File: mstream.hpp
// Header file for using the MDA output stream, mout.

#ifndef __MSTREAM_H
#define __MSTREAM_H

#include <iostream.h>
#include <strstrea.h>

class mstreambuf : public streambuf
	{
private:
	static unsigned short* screen;
	static int column;
	static int row;
	void newline();

public:
	mstreambuf( char* s, int n );
	~mstreambuf();
	virtual int sync();
	virtual int overflow( int );
	virtual int underflow()	{ return EOF; }
	};


class mstream : public ostream
	{
private:
	enum { bsize = 128 };
	char msgs[ bsize ];
	mstreambuf* strbuf;

public:
	mstream();
	~mstream();
	};

extern mstream mout;

#endif
