#include <process.h>
#include <io.h>
#include <stdio.h>
#include <ctype.h>

/* Utility to convert between Motorola S-records and straight binary. */

typedef unsigned char byte;

int count; /* line counter */
long address; /* address pointer */


byte GetHexByte (char **s)     /* read a byte from the stream */
{
       char *p; 	       /* pointer */
       byte c,d;
       p = *s;		       /* pick up char ptr */
       c = toupper(*p);        /* get character */
       c -= ( c > '9' ? 0x37 : 0x30 ); /* convert to binary */
       c <<= 4; 	       /* shift over */
       p++;
       d = toupper(*p);
       d -= ( d > '9' ? 0x37 : 0x30 ); /* convert to binary */
       c |= d;		       /* mix together */
       p++;
       *s = p;		       /* update string pointer */
       return c;
}

void SetBinAddr(long a, FILE *outfile)	      /* set address of write-file, pad as nec */
{
       int     i;

       if  ( a != address )    /* is it at the current pointer? */
       {
	       printf("s2bin:  writing %ld null bytes.\n",a-address);
	       for ( i = address ; i < a ; i ++ )
		   putc(0,outfile);
	       address = a;
       }
       return;
}

void WriteBinData(byte *p, int l, FILE *outfile)
{
       int i;

       for ( i = 0 ; i < l ; i ++, address++ )
	       putc(*p++,outfile);

}


void
ParseSRec(char *s, FILE *outfile)
/* convert a single line of S-Record to binary text. */
{
       int     i,len;
       byte    hex[50];
       byte    cksum;
       long    addr;

       i = *(++s) - '0';       /* get flavour of S record */

       switch (i) {
       case 0:		       /* bogus flavour of line ... eat it */
	       break;

       case 2:		       /* meaty line ... convert it */
	       s++;	       /* point past flavour */
	       len = GetHexByte (&s); /* get the LENGTH byte */
	       cksum = len; addr = 0L;

	       for ( i = 0 ; i < len ; i ++ )
		   cksum += hex[i] = GetHexByte (&s);
	       cksum++;
	       if ( cksum ) {
		  printf("s2bin:  Bad checksum in line %d.\n",count);
		  break;
		  }
	       addr = (hex[0] << 16) + (hex[1] << 8) + (hex [2]);
#ifdef DEBUG
	       printf("s2bin:  address: %lx at line %d \n",addr,count);
#endif
	       SetBinAddr (addr,outfile);
	       len -=4; /* one byte for checksum, three for address */
	       WriteBinData (&hex[3],len,outfile);

	       break;
       case 8:		       /* EOF marker */
	       break;

       default: 	       /* I don't get it ... */
	       printf("s2bin:  Invalid S-Record flavour, line %d.\n",count);
	       break;
       }       /* end of switch */
}

void
main(int argc, char **argv)
	{
	int     i;
	FILE    *infile,*outfile;
	char    s[100];
	long	tempaddr;

	/* initialize global variables */

	count = 0;
	address = 0L;

	if ( argc < 3 )
		{
		printf( "s2bin  (c)1991,93 Developer Resources.  All Rights Reserved.\n" );
		printf( "Usage: s2bin <infile> <outfile> [start address]\n");
		exit(0);
		}

	infile = fopen (argv[1],"r");

	if (!infile)
		{
		printf ("s2bin:  Cannot open file %s.\n",argv[1]);
		exit(1);
		}

	outfile = fopen (argv[2],"wb");

	if (!outfile)
		{
		printf ("s2bin:  Cannot open file %s.\n",argv[2]);
		exit(1);
		}

	if (argc == 4 )
		if ( sscanf( argv[3], "%lx", &tempaddr ) )
			address = tempaddr;


	while (!feof(infile))
		{
		fgets (s, 100, infile );	   /* read a line of input */
		count++;
		switch ( *s )
			{
			case '$':
				printf( s );			// Echo comments to the screen
				break;
			case 'S':
				ParseSRec(s,outfile);
				break;
			default:
				printf("s2bin:  Mystery line: %d.\n",count);
				break;
			}
		}

	fclose(infile);
	fclose(outfile);

	exit(0);
	}
