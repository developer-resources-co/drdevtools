/*************************************************************************
 *  Convert .EQU file from A68K to SYM file for GenMOn                   *
 ************************************************************************/

#include <stdio.h>

void
main(int argc, char ** argv)
{
	char oFname[15];
	char *t,*s;
	char textBuffer[80],outBuffer[32];
	char symbolName[] =
"  Symbol file produced by SPASM/65816"; /* 38 */
	char trash[]      =
" Not really - actually it was from the EQU converter ";/* + 54 = 92 */
	char trash2[]	  =
"zz345678901234567890123456789012345678";

	FILE *inFile;
	FILE *outFile;

	unsigned long address;

	if ( argc < 2 )
	{
		fprintf(stderr,"Equ2Sym V1.0 (c) 1991 Developer Resources\nEqu to Sym file converter\nBy Scott L. Statton\n\nFormat: equ2sym equfile\n");
		exit(1);
	}

	if (!(inFile = fopen(argv[1],"r")))
	{
		fprintf(stderr,"equ2sym: cannot open input file %s.\n",argv[1]);
		exit(1);
	}

	t = oFname;
	s = argv[1];
	for ( ; *s != '.' ; *t++ = *s++ );
	*t = 0;
	strcat (oFname,".sym");

	if (!(outFile = fopen(oFname,"wb")))
	{
		fprintf(stderr,"Equ2sym:  cannot open output file %s.\n",oFname);
		fclose(inFile);
		exit(1);
	}
	fputs(symbolName,outFile);
	fputs(trash,outFile);
	fputs(trash2,outFile);
	do
	{
		s = outBuffer;
		t = textBuffer;
		fgets(textBuffer,80,inFile);
		for ( ; (*t != 9) && *t ; *s++ = *t++ );
		t++;	/* point past that tab */
		*s++ = 0;
		if (strncmp(t,"EQU\11",4))
		{
			fprintf(stderr,"equ2sym: garbage line.\n");
			continue;
		}

		t+=5;	/* point past EQU\11$ tab */

		sscanf(t,"%lx",&address);

#ifdef DEBUG
		printf("label %-28s :: %8lX\n",outBuffer,address);
#endif

		fputs(outBuffer,outFile);
		fputc(0,outFile);
		fputc(address & 0xff,outFile );
		fputc((address & 0xff00) >> 8,outFile );
		fputc((address & 0xff0000) >> 16,outFile );
		fputc((address & 0xff000000) >> 24,outFile );
	} while (!feof(inFile));

	fprintf(stderr,"equ2sym:  File conversion complete.\n");
	fclose(inFile);
	fclose(outFile);

	exit(0);
}














// I hate this editor