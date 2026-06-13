//=============================================================================
// summer.c -- checksum a sega Genesis binary
// By Scott Statton Kevin Seghetti -- (c) 1991,92 Developer Resources.
// Jeff is a weenie
//=============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <dos.h>

//=============================================================================

#define TRUE 1
#define FALSE 0

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

//=============================================================================

typedef unsigned int word;

void
Cleanup(int foo)
{
	exit(foo);
}

//=============================================================================

void
main(int argc,char *argv[])
{
	FILE *inFile,*oFile;
	char outFile[12];
	char *s;
	char *t;
	unsigned char c,temp;
	unsigned int sum,count;
	int argCount;
	int addCheck = TRUE;

	unsigned long inLen;
	unsigned long saddr;
	unsigned long len,size = 0;
	unsigned char header[0x200];


	argCount = 1;
	while(argCount < argc)
	 {
		if(*argv[argCount] == '-')				// must be command line switch
			switch(argv[argCount][1])
			 {
				case 's':
				case 'S':
					sscanf(&argv[argCount][2],"%ld",&size);
					break;
				case 'd':
				case 'D':
					addCheck = FALSE;
					break;

			 }
		else break;
		argCount++;
	 }

	if(argc-argCount < 1)
	 {
		printf("\
Summer V1.1 (c) 1991,92 Developer Resources\n\
Genesis Game Checksummer\n\
By Kevin T. Seghetti & Scott Statton\n\
Format: Summer [<switches>] <inputfile> [<outputfile>]\n\
	Switches:\n\
		-s#:	set file size in bytes(ignore header)\n\
			(note: size will not cause file to shrink)\n\
			(see last column in chart for size)\n\
		-d:	Don't insert checksum into file\n\
\n\
	Game Sizes Chart:\n\
		 2 megabits =  256K = 0.25 megabyte  =  262144 bytes\n\
		 4 megabits =  512K = 0.50 megabyte  =  524288 bytes\n\
		 6 megabits =  768K = 0.75 megabyte  =  786432 bytes\n\
		 8 megabits = 1024K = 1.00 megabyte  = 1048576 bytes\n\
		12 megabits = 1536K = 1.50 megabytes = 1572864 bytes\n\
		16 megabits = 2048K = 2.00 megabytes = 2097152 bytes\n\
");

		Cleanup(ERR_NOINPUT);
	 }

	inFile = fopen(argv[argCount+0],"rb");
	if(inFile == 0)
	{
		printf("File not found\n");
		Cleanup(ERR_FILENOTFOUND);
	}

	if(argc < argCount+2)
	 {
		t = outFile;
		s = argv[argCount+0];
		for ( ; *s != '.' && *s; *t++ = *s++ );
		*t = 0;
		strcat (outFile,".sum");
	 }
	else
		strcpy(outFile,argv[argCount+1]);

	oFile = fopen(outFile, "wb");

	if(!oFile)
	 {
			printf("\nCouldn't open output file %s\n",outFile);
			exit(1);
	 }

	count = 0x200;
	len = 0;
	sum = 0;

	c = fgetc(inFile);
	while(count--)						// skip first 200 bytes in file
	 {
		fputc(c,oFile);
		header[len] = c;
		len++;
		c = fgetc(inFile);
	 }

	if(strncmp((char *)&header[0x100],"SEGA GENESIS",11) && !size)
	 {
		printf("\nFile does not contain valid genesis header\n");
		exit(1);
	 }

	if(size == 0)
	 {
		size = (((unsigned long)header[0x1a4])<<24) +  (((unsigned long)header[0x1a5])<<16) +  (((unsigned long)header[0x1a6])<<8) + header[0x1a7];
		size++; 		// since the header has it 1 to small
		printf("\nFile header block indicates size of %ld\n",size);
	 }
	else
		printf("\nUser chosen file size of %ld\n",size);
	if(size > 0xffffff)
	 {
		printf("\nSize is to big\n");
		exit(1);
	 }

//	c = fgetc(inFile);
	while(!feof(inFile))
	 {
		fputc(c,oFile);
		if(len & 1)
			sum += c + (((unsigned int)temp)<<8);
		else
			temp = c;
		len++;
		c = fgetc(inFile);
	 }

	while(len < size)
	 {
		fputc(0xff,oFile);
		if(len & 1)
			sum += 0xffff;
		len++;
	 }

	if(addCheck)
	 {
		printf("Writing CheckSum\n");
		fseek(oFile,0x18e,0);				// actually write out checksum
		fputc(sum >> 8,oFile);
		fputc(sum & 0xff,oFile);
	 }

	printf("sum = %x\n",sum);
	printf("\nFile sum complete.\n");
	exit(0);
}

//=============================================================================
