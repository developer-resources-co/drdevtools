//=============================================================================
//=============================================================================

#include <stdio.h>
#include <alloc.h>
#include <dos.h>

#define DELAYTIME 20

//=============================================================================

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

//=============================================================================

typedef unsigned int word;

Cleanup(int foo)
{
	exit(foo);
}

void
main(int argc,char *argv[])
{
	printf("NMI (c) 1992 Developer Resources\nGenesis Restarter\nBy SLS\n");
	outportb(0x318,0xa0);
	outportb(0x318,0xe0);
	exit(0);
}
