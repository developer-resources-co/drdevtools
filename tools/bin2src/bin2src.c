#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINELENGTH   20

FILE *infp;
FILE *outfp;

enum
{
	FALSE,TRUE
};

enum
{
	OTC,OT68000,OT6502
};

unsigned char *
FindChar(char *sourceString,unsigned char c)
{
	while(*sourceString)
	 {
		if(*sourceString == c)
			break;
		sourceString++;
	 }
	return(sourceString);
}



int
main (int argc,char *argv[])
	{
    long a;
    int linesize=0;
	unsigned int charCount = 0;
	int argCount,outputType = OT6502,firstLine;
    unsigned char buffer;
	unsigned char name[20];

	argCount = 1;
	while(argCount < argc)
	 {
		if(*argv[argCount] == '-')				// must be command line switch
			switch(argv[argCount][1])
			 {
				case 'c':
				case 'C':
					outputType = OTC;
					break;
				case '8':
					outputType = OT68000;
					break;
				case '6':
					outputType = OT6502;
					break;
			 }
		else break;
		argCount++;
	 }

	if(argc-argCount < 2)
        {
            printf ("\
Bin2Src V2.0 (c) Developer Resources 1991,92\n\
Binary file to source data converter\n"
__DATE__ " " __TIME__ "\n\
By Lars Norpchen\n\
Format: Bin2Src [<switches>] <binaryinfile> <sourceoutfile>\n\
Switches:\n\
		-c: Write output in C format\n\
		-6: Write output in 6502 format\n\
		-8: Write output in 68000 format\n");
            exit (1);
        }

    infp = fopen (argv[0+argCount],"rb");
    if (infp==NULL)
        {
            printf ("Error opening input file:%s",argv[0+argCount]);
            exit (1);
        }
	strcpy(name,argv[0+argCount]);
	*FindChar(name,'.') = 0;
    outfp = fopen (argv[1+argCount],"w");
    if (outfp==NULL)
        {
            printf ("Error opening output file:%s",argv[1+argCount]);
            exit (1);
        }

	fprintf( outfp, "; %s\n", argv[1+argCount] );
	firstLine = TRUE;
    fread (&buffer,1,1,infp);
    while (!feof (infp))
        {
            if (linesize)
				{
				switch ( outputType )
					{
					case OTC:
						fprintf( outfp, ",0x%02x", buffer );
						break;
					case OT68000:
					case OT6502:
						fprintf( outfp, ",$%02X", buffer );
						break;
					}
				}
            else
			 {
				switch(outputType)
				 {
					case OTC:
						if(firstLine)
							fprintf( outfp,"unsigned char %s[] =\n{\n\t0x%02x",name,buffer );
						else
							fprintf( outfp,",\n\t0x%02x",buffer );
						break;
					case OT68000:
               			fprintf (outfp,"\n\tdc.b\t$%02X",buffer);
						break;
					case OT6502:
               			fprintf (outfp,"\n\tdb\t$%02X",buffer);
						break;
				 }
				firstLine = FALSE;
			 }
            linesize++;
            linesize %=MAXLINELENGTH;
            fread (&buffer,1,1,infp);
			charCount++;
        }
	switch(outputType)
	 {
		case OTC:
				fprintf (outfp,"\n};\n#define %sSize %u\n",name,charCount);
			break;
		case OT68000:
		case OT6502:
            fprintf (outfp,"\n\tend\n");
			break;
	 }

    fclose (infp);
    fclose (outfp);

	return 0;
	}   	
