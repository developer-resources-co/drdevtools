//==============================================================================
// output.hpp:
//==============================================================================

#ifndef OUTPUT_HPP
#define OUTPUT_HPP

//==============================================================================

#include <iostream.h>

#include <pclib/libtypes.hpp>

//==============================================================================

class label
	{
public:
	label( const char* newLabel )	{ szLabel = newLabel; }
	~label()					 	{}

	const char* operator()()		{ return( szLabel ); }
	const char* operator()( const char* newLabel )
		{ return( szLabel=newLabel ); }
private:
	const char* szLabel;
	};


//==============================================================================

class output												// abstract class
{
public:
	virtual size_t Put(const ubyte &data, size_t count, const label &name) = 0;
	virtual size_t Put(const ubyte &data, size_t count = 1) = 0;
//?	virtual Put(const ubyte data) = 0;

	virtual const label& Label(const label&) = 0;
protected:
	virtual ~output();
	output();					// only for outputNowhere
private:
};

//------------------------------------------------------------------------------

class outputNowhere : public output
{
public:
	outputNowhere() { }
	virtual ~outputNowhere() { }
	virtual size_t Put(const ubyte &data, size_t count, const label &name)  { return(count); }
	virtual size_t Put(const ubyte &data, size_t count = 1)  { return(count); }
};

//------------------------------------------------------------------------------

class outputBinary : public output
{
public:
	outputBinary(ostream& out);
	virtual ~outputBinary();
	virtual size_t Put(const ubyte &data, size_t count, const label &name);
	virtual size_t Put(const ubyte &data, size_t count = 1);
	virtual const label& Label(const label& lab)  { return(lab); }
private:
	ostream &stream;
	outputBinary();
};

//------------------------------------------------------------------------------

class outputSource : public output					// abstract class
{
public:
	outputSource();
	virtual ~outputSource();

	enum DataSize
		{
		BYTE,
		WORD,
		LONG
		};

private:
	char* szDelimiter;
	char* szCommentBegin, * CommentEnd;
	DataSize size;
//	void *fn();
};

//------------------------------------------------------------------------------

class outputSourceC : public outputSource
{
public:
	outputSourceC();
	virtual ~outputSourceC();

};

//------------------------------------------------------------------------------

class outputSourceAssembly : public outputSource
{
public:
	outputSourceAssembly();
	virtual ~outputSourceAssembly();

};

//------------------------------------------------------------------------------

class outputSourceAssembly68000 : public outputSourceAssembly
{
public:
	outputSourceAssembly68000();
	virtual ~outputSourceAssembly68000();

};

//------------------------------------------------------------------------------

class outputSourceAssembly65816 : public outputSourceAssembly
{
public:
    outputSourceAssembly65816();
	virtual ~outputSourceAssembly65816();

};

//==============================================================================

#endif

//==============================================================================
