//==============================================================================
// mem.h:
//==============================================================================
/*

Documentation:

	Abstract:
		low level memory interface
	History:
			Created	?
			First documented 10-19-92 02:15pm

	Class Hierarchy:
		none
	Dependancies:
		none
	Restrictions:
		none
	Example:


*/
//==============================================================================
// use only once insurance


#if !defined(PCLIB_mem_H)
#define PCLIB_mem_H

//==============================================================================

char *SafeMalloc (unsigned size);
char far *SafeFarMalloc (unsigned long size);
void MemError (unsigned long size,char type,unsigned long left);

//==============================================================================

#endif

//==============================================================================
