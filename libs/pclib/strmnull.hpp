// File: strmnull.hpp
// Stream which eats input (bit-bucket output stream)

#ifndef PCLIB_NULLSTREAM_H
#define PCLIB_NULLSTREAM_H

#include <iostream.h>
#include <strstrea.h>

class nullstreambuf : public streambuf
	{
public:
	nullstreambuf( char* s, int n );
	~nullstreambuf();
	virtual int sync();
	virtual int overflow( int );
	virtual int underflow()	{ return EOF; }
	};


class nullstream : public ostream
	{
private:
	enum { bsize = 128 };
	char msgs[ bsize ];
	nullstreambuf* strbuf;

public:
	nullstream();
	~nullstream();
	};

extern nullstream cnull; 	// Create a global instance of the output

#endif
