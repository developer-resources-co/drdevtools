//==============================================================================
// Mem.h: memory managment classes header file
//==============================================================================

#ifndef PCLIB_mem_H
#define PCLIB_mem_H

//==============================================================================

#include <string.h>

//==============================================================================

class memoryBlock
{
};


class memory
{
	memory() {}
	ubyte *memPtr;
	ulong size;
	ubyte type;
	enum {CONVENTIONAL, EXTENDED, EXPANDED, DISK};
public:
	memory(ulong sz);
	~memory();
	memory(ulong sz, ulong fillValue);
	memory(ubyte *memPtr, ulong size);

	int
	MemNCmp(int start, ubyte *mem,int len) { if(start >= size) return(-1); return(strncmp( (const char*)memPtr+start, (const char*)mem, len));}

	ulong ReadMem(ulong which);
	void WriteMem(ulong which,ulong value);
	void MemCopyTo(ulong *mem,ulong length);
	void MemCopyFrom(ulong *mem,ulong length);
};

//==============================================================================

#endif

//==============================================================================

