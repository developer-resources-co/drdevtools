//==============================================================================
//
// output.cpp
//	by William B. Norris IV
//
// (c) 1993 developer resources.  All Rights Reserved.
//
//==============================================================================

#include <string.h>

#include "output.hpp"

//==============================================================================

output::output()
{
}

//------------------------------------------------------------------------------

output::~output()
{
}

//------------------------------------------------------------------------------

outputBinary::outputBinary(ostream &out) : output(), stream(out)
{
}

//------------------------------------------------------------------------------

outputBinary::~outputBinary()
{
}

//------------------------------------------------------------------------------

size_t
outputBinary::Put(const ubyte &data, size_t count, const label &name)
{
	return(Put(data,count));
}

//------------------------------------------------------------------------------

size_t
outputBinary::Put(const ubyte &data, size_t count)
{
#ifdef __BORLANDC__
	stream.write(&data,int(count));
#else
	stream.write(&data,count);
#endif
	return(count);
}

//------------------------------------------------------------------------------

outputSource::outputSource() : output()
{
	cout << "SourceOutput() constructor\n";
}

//------------------------------------------------------------------------------

outputSource::~outputSource()
	{
	cout << "SourceOutput() destructor\n";
	}

//------------------------------------------------------------------------------

outputSourceC::outputSourceC() : outputSource()
	{
	cout << "CSourceOutput() constructor\n";
	}

//------------------------------------------------------------------------------

outputSourceC::~outputSourceC()
	{
	cout << "CSourceOutput() destructor\n";
	}

//------------------------------------------------------------------------------

outputSourceAssembly::outputSourceAssembly() : outputSource()
	{
	cout << "AssemblySourceOutput() constructor\n";
	}

//------------------------------------------------------------------------------

outputSourceAssembly::~outputSourceAssembly()
	{
	cout << "AssemblySourceOutput() destructor\n";
	}

//------------------------------------------------------------------------------

outputSourceAssembly68000::outputSourceAssembly68000() : outputSourceAssembly()
	{
	}

outputSourceAssembly68000::~outputSourceAssembly68000()
	{
	cout << "MC68000AssemblySourceoutput() destructor\n";
	}


outputSourceAssembly65816::outputSourceAssembly65816() : outputSourceAssembly()
	{
	}

outputSourceAssembly65816::~outputSourceAssembly65816()
	{
	cout <<"WDC65816AssemblySourceoutput() destructor\n";
	}

//==============================================================================
