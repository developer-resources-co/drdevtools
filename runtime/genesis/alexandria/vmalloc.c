/*===========================================================================*/
/* vmalloc.c: vdp memory allocation handler     							 */
/* By Scott L. Statton (c) 1993 Alexandria, Inc.                             */
/*===========================================================================*/
/* KTS: 03-26-93 00:33am added invalid input checking to vfree */
/* TJ: 05-02-93  changed array to malloc'ed memory */
/*===========================================================================*/

#include <stdlib.h>
#include "debug.h"
#include "vmalloc.h"
#include "assert.h"

#define VDPMEMSIZE 2048


/*===========================================================================*/

/*
 * vmalloc -- allocated video memory.
 */

#if 1

PUBLIC unsigned short wBlock[VDPMEMSIZE];

#if 0
PUBLIC unsigned short wObjID[VDPMEMSIZE];
#endif	/* NDEBUG */


PUBLIC void
InitVM(void)
{
	int	i;

	for ( i = 0 ; i < VDPMEMSIZE ; i++ )
		{
#if 0 
		wObjID[i] =
#endif	/*NDEBUG*/
		wBlock[i] = 0;
		}
}
#else	/*1*/

PUBLIC unsigned short *wBlock;
PUBLIC unsigned short wBlockBase;
PUBLIC unsigned short *wBlockEnd;

PUBLIC void
InitVM(unsigned short base)
{
	int i;

	wBlockBase = base;
	if (wBlock)
		free(wBlock);
	wBlock = (unsigned short *) malloc(sizeof(short)*(VDPMEMSIZE-base));
	assertString(wBlock,"InitVM: could not allocate VDP memory");
	for (i = 0; i < VDPMEMSIZE-base; i++)
		wBlock[i] = 0;
	wBlockEnd = wBlock + (VDPMEMSIZE-wBlockBase)*2;
}
#endif /* 1 */

/*===========================================================================*/

PUBLIC void
VMConsume(unsigned short nCharBase, unsigned short nNum)
{
/*====
	while (nNum--)
	{
		wBlock[nCharBase-wBlockBase] = 0xffff;
		nCharBase++;
	}
====*/
    
    wBlock[ nCharBase++ ] = nNum--;
	for ( ; nNum--; ++nCharBase )
		{
#if 0
		wObjID[ nCharBase ] =
#endif
		wBlock[ nCharBase ] = 0xFFFF;
		}
}

/*===========================================================================*/
/* see vmalloc.asm for assembly version */

#if 0
PUBLIC unsigned short
vmalloc(unsigned short nChars)
{
	register 	         short	size = 0;
	register	unsigned short	base = 0;
				unsigned short	j;

	while ( base < VDPMEMSIZE )
	{
		if (!wBlock[base])
			size++;
		else
		{
			base += size+1;
			size = 0;
		}

		if ( size == nChars )
		{
			j = base;
			wBlock[base] = nChars;
			size--;
			while (size--)
				wBlock[++j] = 0xffff;
			return base;
		}
	}
	return 0;
}
#endif

/*===========================================================================*/

PUBLIC unsigned short
vfree(unsigned short nChar)
{
	short	i,j;

	/* nChar -= wBlockBase; */
	if ( nChar != 0xffff && wBlock[nChar] && (wBlock[nChar] != 0xffff) )
	{
		for ( i=nChar, j=wBlock[nChar]; j; ++i, --j )
			{
#if 0
			wObjID[ i ] = ~0;
#endif
			wBlock[ i ] = 0;
			}
		return(0);
	}
	assertString(wBlock[nChar],"vfree: attempt to free memory already freed");
	assertString(wBlock[nChar] == 0xffff,"vfree: misaligned block freed");
/*	for ( ; ; )
		ColorCycle(0); */

	return 0xffff;
}

/*===========================================================================*/

PUBLIC unsigned short
vtotalfree(void)
{
	int 	i,j = 0;

	/* for ( i = 0 ; i < VDPMEMSIZE-wBlockBase ; i++ ) */
	for ( i = 0 ; i < VDPMEMSIZE ; i++ )
		if (!wBlock[i]) j++;
	return j;
}

PUBLIC unsigned short
vlargestblock(void)
{
	int		i,current,largest;
	current = largest = 0;

	/* for ( i = 0 ; i < VDPMEMSIZE-wBlockBase ; i++ ) */
	for ( i = 0 ; i < VDPMEMSIZE ; i++ )
		if (!wBlock[i])
			current++;
		else
		{
			largest = ( ( current > largest ) ? current : largest);
		 	current = 0;
		}
	return largest;
}

/*===========================================================================*/
