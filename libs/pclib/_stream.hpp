// File: _stream.hpp
// Generic header for for creating new "cout"-like classes

#ifndef ___STREAM_H
#define ___STREAM_H

#include <iostream.h>
#include <strstrea.h>

class _streambuf : public streambuf
	{
private:
	void cr();
	void newline();

public:
	_streambuf( char* s, int n );
	~_streambuf();
	virtual int sync();
	virtual int overflow( int );
	virtual int underflow()	{ return EOF; }
	};


class _stream : public ostream
	{
private:
	enum { bsize = 128 };
	char msgs[ bsize ];
	_streambuf* strbuf;

public:
	_stream();
	~_stream();
	};

#if COMMENT
extern _stream _out;		// Create a global instance of the output
#endif


#endif
