//==============================================================================
// Mem.cpp: memory managment classes
//==============================================================================

#include "base.hpp"
#include "mem.hpp"

//==============================================================================
//	Memory class handling
//==============================================================================
// creation/deletion
//------------------------------------------------------------------------------

memory::memory(ulong sz)
{
	memPtr = new ubyte[sz]; size  = sz;
}


memory::memory(ulong sz, ulong fillValue)
{
	memPtr = new ubyte[sz];
	size = sz;

	ulong i;
	for(i=0;i<size;i++)
		memPtr[i] = fillValue;
}

//==============================================================================

memory::memory(ubyte *bPtr, ulong sz)
{
	memPtr = new ubyte[sz];
	size = sz;

	ulong i;
	for(i=0;i<size;i++)
		memPtr[i] = *bPtr++;
}

//==============================================================================

memory::~memory()
{
	delete(memPtr);
}

//==============================================================================
// memory I/O routines
//------------------------------------------------------------------------------

ulong memory::ReadMem(ulong which)
{
	if(which < size)
		return *(memPtr+which);
	else
		return(0);				// waiting for throw/catch
}

//==============================================================================

void memory::WriteMem(ulong which,ulong value)
{
	if(which < size)
		*(memPtr+which) = value;
//	else
//		wish I had throw/catch
}

//==============================================================================

void memory::MemCopyTo(ulong *mem,ulong length)
{
	ulong i = 0;
	while(length)
		memPtr[i++] = *mem++;
}

//==============================================================================

void memory::MemCopyFrom(ulong *mem,ulong length)
{
	ulong i = 0;
	while(length)
		*mem++ = memPtr[i++];
}

//==============================================================================
// block allocation routines
//==============================================================================



//==============================================================================
