#include <stdio.h>


void
main(int argc, char **argv)
{
	unsigned char	i;
	unsigned char far * board;

	board =(char far *) 0xd0000000;

	outportb(0x318,0xf3);
	outportb(0x319,0);

		for ( i = 0 ; i < 255 ; i++ )
		{
			*board = i;
			if (*board != i )
				printf("byte %x came back %x\n",i,*board);
		}

	exit(0);
}
