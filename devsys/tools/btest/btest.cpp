//==============================================================================
// BTest.c: Hardware Tester Version 2.0 - Developer Resources
// Kevin T. Seghetti & Scott Statton (c) 1991,92  All Rights Reserved.
//==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <dos.h>

#include <conio.h>

#define DELAYTIME 20

#define GENESIS
#define DOPPLEBOARD
//#define SNES

//=============================================================================

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5

#define TRUE 1
#define FALSE 0
#define FLAG unsigned int

unsigned char testString[] = "Monitor Running";

//=============================================================================

#ifdef GENESIS
#define PLATFORMNAME " Genesis"
#endif

#ifdef SNES
#define PLATFORMNAME " SNES"
#endif

#define SLAVEBASE 0x318

#define BOARDNAME "Dopple"
#define BANKCOUNT 0x21

//=============================================================================
// board equates


#ifdef DOPPLEBOARD
// ctrl equates
#define	DB_CTRL_ENAB		1
// 0 = mem off bus, 1 = mem on bus
#define	DB_CTRL_WHO			2
// 0 = target, 1 = host
#define	DB_CTRL_INHIB		4
// 0 = memory enables, 1 = memory disabled
#define	DB_CTRL_EIGHTBIT	8
// 0 = ?, 1 = ?
#define	DB_CTRL_WP			0x10
// 0 = target writes enabled, target writes inhibited
#define	DB_CTRL_CLEAR		0x20
// 0 = clear wprot viol, 1 = normal
#define	DB_CTRL_NMI			0x40
// 0 = NMI asserted, 1 = normal
#define	DB_CTRL_RESET		0x80
// 0 = reset asserted, 1 = normal

#define	DB_CTRL_NORM DB_CTRL_CLEAR|DB_CTRL_NMI|DB_CTRL_RESET

#define DB_CTRL_HOSTBUS DB_CTRL_NORM|DB_CTRL_ENAB|DB_CTRL_WHO
#define DB_CTRL_TARGETBUS DB_CTRL_NORM

#endif

//=============================================================================

unsigned int slaveBase = SLAVEBASE;
unsigned char far *slaveBuffer = (unsigned char far *)0xD0000000;

unsigned int slaveCtrl,slaveBank,slaveWorm;
unsigned char far *slaveBufferCmd;
unsigned char far *slaveBufferData;
FLAG extensive = FALSE;
FLAG testRam = TRUE;
#ifdef DOPPLEBOARD
FLAG testComRam = TRUE;
#else
FLAG testComRam = FALSE;
#endif
FLAG testSegCon = FALSE;
FLAG testNMI = TRUE;
FLAG attemptBoot = TRUE;

//=============================================================================

typedef unsigned int word;
void SetBank(int);

#ifdef GENESIS
#include	"testrom.src"
#endif

//=============================================================================

void
LoadEnvVariables(void)
{
	char *strPtr;
	int temp;
	strPtr = getenv("DR"PLATFORMNAME"PORT");
	if(*strPtr)
	 {
		if(sscanf(strPtr,"%x",&temp))
			slaveBase = temp;
	 }

	strPtr = getenv("DR"PLATFORMNAME"MEMBUFFER");
	if(*strPtr)
	 {
		if(sscanf(strPtr,"%x",&temp))
			slaveBuffer = (unsigned char far *)((long)temp << 16);
	 }
}

//==============================================================================

void
InitSlaveIO(int base,unsigned char far *slaveBuffer)
{
	slaveCtrl = base++;
	slaveBank = base++;
	slaveWorm = base;
	slaveBufferCmd = slaveBuffer;
	slaveBufferData = slaveBufferCmd+2;
}

//=============================================================================

void
SetBank ( int b)
{
	if(b == 0x20)
		outportb ( slaveBank, 0x80);
	else
	 {
		b = ((( b&1) <<3 ) | ((b&0xe)>>1)) | (b&0x10);
		outportb ( slaveBank, b);
	 }
	return;
}


void
Cleanup(int err)
{
	exit (err);
}

//==============================================================================

FLAG
CheckForOtherMem(void)
{
	char temp,temp2;
	outportb(slaveCtrl,DB_CTRL_TARGETBUS);		// make sure we don't have the bus

	temp = *slaveBuffer;
	temp2 = *(slaveBuffer+1);
	*slaveBuffer = 0xaa;
	*(slaveBuffer+1) = 0x55;
	if(*slaveBuffer == 0xaa)
	 {
		*slaveBuffer = 0x55;
		*(slaveBuffer+1) = 0xaa;
		if(*slaveBuffer == 0x55)
		 {
			*slaveBuffer = temp;
			*(slaveBuffer+1) = temp2;
			return(TRUE);
		 }
	 }
	return(FALSE);
}

//==============================================================================

int
BoardSize( void )
	{ // Board found -- determine if 8 or 16 megabit board

	// This test is only valid for the boards with the 16MB PAL.
	// Is there some way to determine which page is set (or something?)

	SetBank( 0 );
	*slaveBuffer = 8;
	SetBank( 10 );
	*slaveBuffer = 16;
	SetBank( 0 );

	return *slaveBuffer==16 ? 8 : 16;
	}

int
CheckForBoard(void)
{
	char temp;

	// first just check if changeing enables changes results
	// I do this test in case the ram is bad, I still want to find the board
	outportb(slaveCtrl,DB_CTRL_TARGETBUS);		// make sure we don't have the bus
	outportb(slaveBank,0);
	temp = *slaveBuffer;
	outportb(slaveCtrl,DB_CTRL_HOSTBUS);		// get bus
	if(*slaveBuffer != temp)
		return BoardSize();

	// at this point, the board could just happen to contain
	// whatever the bus floated to(pretty likely if there is no ram), so lets make sure

	*slaveBuffer = 0xAA;
	outportb(slaveCtrl,DB_CTRL_TARGETBUS);		// make sure we don't have the bus
	*slaveBuffer = 0x55;
	outportb(slaveCtrl,DB_CTRL_HOSTBUS);		// get bus
	if(*slaveBuffer == 0xAA)
		return BoardSize();

	return 0;
}

//==============================================================================

unsigned char
testPatterns[] =
{
	0x55,0xaa,
	0x0,0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80,
	0x3,0x7,0xf,0x1f,0x3f,0x7f,0xff
};


tPat2[7] =
{
	0xaa,0x55,0xf0,0x0f,00,0xff,'K'
};

/*
// notes:
	o0-1	= odd/even
	r0-r3	= which ram chip

		o   e
		d   v
		d   n

	r0 - u4/u8			; segment 0/1/2/3
	r1 - u3/u7			; segment 8/9/a/b
	r2 - u2/u6			; segment 4/5/6/7
	r3 - u1/u5			; segment c/d/e/f
*/

// if segment == 0x20, that means com ram

// table of which chip goes with which segment/lowest addr pin
unsigned int
chipTbl[20] =
{
	8,4,
	7,3,
	6,2,
	5,1,
	0,0,			// expansion board chips
	0,0,
	0,0,
	0,0,
	28,27			// com ram
};

//------------------------------------------------------------------------------

FLAG
TestSeg(unsigned int seg,char *testText)
{
	int pass;
	int passes = 2;
	unsigned char register pat;
	unsigned long i;
	FLAG err = FALSE;
	unsigned char register far *mPtr;
	unsigned char far *mPtr2;

	if(extensive)
		passes = 18;

	for(pass = 0;pass<passes;pass++)
	 {
		pat = testPatterns[pass];
		printf(testText);
		printf(" Pattern: %02x",(unsigned int)pat);
		putchar(13);
		mPtr = slaveBuffer;
		mPtr2 = mPtr+1;
		for(i=0;i<0xffff; i++)
	 	{
			*mPtr = pat;
			if(i != 0xffff)
				*mPtr2++ = 0xff;				// write elsewhere to prevent floating to correct values
			if(*mPtr != pat)
		 	 {
				if(!err)
					fputc('\n',stderr);
				if(err > 20)
				 {
					fprintf(stderr,"\nAborting Segment %x\n",(unsigned int)seg);
					return(TRUE);
				 }
				fprintf(stderr,
					"Mem Err: Addr = %x Expected: %x Actual: %x Chip = U%01d\n",
					(unsigned int)i, (unsigned int)pat, (unsigned int)*mPtr,
					chipTbl[((seg>>1)&0xfe)|(i&1)]
					);
				err++;
		 	 }
			mPtr++;
	 	}
	 }


	if(extensive)
	 {
		// check for shorted address lines
		printf(testText);
		printf(" Pattern: Complex");
		pat = 0;
		mPtr = slaveBuffer;
		for(i=0;i<0xffff;i++)
		 {
			*mPtr++ = tPat2[pat];
			if(++pat >= 7)
				pat = 0;
		 }

		pat = 0;
		mPtr = slaveBuffer;

		for(i=0;i<0xffff;i++)
		 {
			if(*mPtr != tPat2[pat])
		 	 {
				if(!err)
					putchar('\n');
				if(err > 20)
				 {
					fprintf(stderr,"\nAborting Segment %x\n",(unsigned int)seg);
					return(TRUE);
				 }
				fprintf(stderr,"Mem Err: Addr = %x Expected: %x Actual: %x Chip = U%01d\n",(unsigned int)i,(unsigned int)tPat2[pat],(unsigned int)*mPtr,chipTbl[((seg>>1)&0xfe)|(i&1)]);
				err++;
		 	 }
			mPtr++;

			if(++pat >= 7)
				pat = 0;
		 }
	 }
	return(err);
}

//==============================================================================
// verify each segment is unique (ram test)

FLAG
TestSegHits(void)
{
	unsigned char register far *mPtr;
	unsigned long i,s;
	int seg;
	FLAG err = FALSE;
	// check for shorted address lines
	printf("Testing Segment Conflicts\n");
	mPtr = slaveBuffer;
	for(s=0;s<BANKCOUNT;s++)
	 {
		seg = s;

#if 0
		if(s == 0x10)
			seg = 0x20;
#endif

		mPtr = slaveBuffer;
		SetBank(seg);
		printf("Filling segment %x",(unsigned int)seg);
		putchar(13);
		for(i=0;i<0xffff;i++)
			*mPtr++ = seg;
	 }

	putchar('\n');
	for(s=0;s<BANKCOUNT;s++)
	 {
		seg = s;
#if 0
		if(s == 0x10)
			seg = 0x20;
#endif
		mPtr = slaveBuffer;
		SetBank(seg);
		printf("Verifying segment %x",(unsigned int)seg);
		putchar(13);
		for(i=0;i<0xffff;i++)
			if(*mPtr != seg)
		 	 {
				if(!err)
					putchar('\n');
				if(err > 20)
				 {
					fprintf(stderr,"\nAborting Segment %x\n",(unsigned int)seg);
					goto fuck;
				 }
				fprintf(stderr,"Segment conflict between segment %x & segment %x\n",(unsigned int)seg,(unsigned int)*mPtr);
				err++;
		 	 }
fuck:;
	 }
	return(err);
}

//==============================================================================

FLAG
TestBoardMemory(void)
{
	unsigned int seg;
	char testText[80];
	FLAG err = FALSE;
	if(testRam)
		for(seg = 0;seg < 0x20;seg++)
	 	 {
	 		sprintf(testText,"Testing Memory Segment:%02x",seg);
			SetBank(seg);
			if(!TestSeg(seg,testText))
				err = TRUE;
			putchar('\n');
	 	 }
	// now test com ram
	if(testComRam)
	 {
		sprintf(testText,"Testing Communication Memory");
		outportb ( slaveBank, 0x80);
		if(!TestSeg(0x20,testText))
			err = TRUE;
		putchar('\n');
	 }
	if(testSegCon)
	 {
		if(!TestSegHits())
			err = TRUE;
		putchar('\n');
	 }
	return(err);
}

//==============================================================================

CmpStr(unsigned char far *ptr1, unsigned char far *ptr2)
{
	if(*ptr1++ != *ptr2++)
		return(TRUE);

	while(*ptr1 && *ptr2)
	 {
		if(*ptr1++ != *ptr2++)
			return(TRUE);
	 }
	return(FALSE);
}

//==============================================================================

#ifdef GENESIS
FLAG
LoadTestRom(void)
{
	unsigned char far *buffPtr;
	int i;

	SetBank(0);
	outportb(slaveCtrl,DB_CTRL_HOSTBUS);

							// copy boot rom into slave memory
	buffPtr = slaveBuffer;
	for(i=0;i<testromSize;i++)
		*buffPtr++ = testrom[i];

						// reset genesis
	outportb(slaveCtrl,0x60);		// kts change these to equates
	delay(DELAYTIME);		       // wait for slave to notice
	outportb(slaveCtrl,0xe0);

	delay(500*7);					// wait for sega copyright to go by

	outportb(slaveCtrl,DB_CTRL_HOSTBUS);
	if(CmpStr(slaveBuffer+1,(unsigned char far *)testString))
		return(FALSE);
	return(TRUE);
}
#endif

//==============================================================================

void
DrawBoard( void )
	{
	clrscr();
	gotoxy( 1, 1 );

	char t[132];
	FILE* fp = fopen( "board.txt", "rt" );

	while ( fgets( t, 132, fp ) )
		printf( t );

	fclose( fp );


	}


void
main(int argc,char *argv[])
{
	FILE *inFile;
	FLAG bad = FALSE;
	int size;					// of board (in megabits)

	unsigned long inLen;
	unsigned long saddr;
	int argCount;

	printf("BTest V2.0 (c) Developer Resources 1991,92\nDR "BOARDNAME" Board(tm) Hardware Tester \nBy Kevin T. Seghetti\n");

	LoadEnvVariables();
	InitSlaveIO(slaveBase,slaveBuffer);

	if(argc > 1)
	 {
		argCount = 1;
		while(argCount < argc)
	 	 {
			if(*argv[argCount] == '-')				// must be command line switch
				switch(argv[argCount][1])
			 	 {
					case 'S':
					case 's':
						testSegCon = TRUE;
						break;
					case 'E':
					case 'e':
						extensive = TRUE;
						break;
					case 'R':
					case 'r':
						testRam = FALSE;
						break;
					case 'C':
					case 'c':
						testComRam = FALSE;
						break;

					case 'N':
					case 'n':
						testNMI = FALSE;
						break;

					case 'B':
					case 'b':
						attemptBoot = FALSE;
						break;
					case 'H':
					case 'h':
					case '?':
						fprintf(stdout,"Btest docs \n\
-s = Do Segment Test\n\
-e = Do extensive test.\n\
-r = Don't do RAM test.\n\
-c = Don't test comm ram.\n\
-n = Don't test NMI\n\
-B = Don't attempt to boot.\n");
						break;

				 }
			argCount++;
		 }
	 }

	if(CheckForOtherMem())
	 {
		unsigned int slaveSeg;
		slaveSeg = ((unsigned long)slaveBuffer)>>16;
		printf("\
Some other memory is in conflict with the "BOARDNAME" board\n\
Try removing any extended memory drivers you may have loaded\n\
or exclude the memory area between %04X and %04X\n\
",slaveSeg,slaveSeg+0xfff);
		exit(5);
	 }
	if( size = CheckForBoard())
	 {
		DrawBoard();
		printf( "%d Megabits\n", size );
		TestBoardMemory();
	 }
	else
	 {
		printf("No "BOARDNAME" board found\n");
		exit(5);
	 }


#ifdef GENESIS
// now lets see if we can get some code to run on the slave
	if (attemptBoot)
	{
		printf("Booting"PLATFORMNAME"\n");
		outportb(slaveWorm,0xaa);
		if(LoadTestRom())
	 	{
			printf("Testing Wormhole\n");
    		if(inportb(slaveWorm) != 0xaa)
		 	{
				bad = TRUE;
				printf("Wormhole from slave to host bad\n");
		 	}
			if(*slaveBuffer != 0xaa)
		 	{
				bad = TRUE;
				printf("Wormhole from host to slave bad\n");
		 	}
			if(!bad)
				printf("Wormhole test complete\n");

			if(!bad && testNMI)
		 	{
				printf("Testing NMI\n");
									// NMI genesis
				outportb(slaveCtrl,0xa0);
				outportb(slaveCtrl,0xe0);
		 	}
			delay(10);
			if(inportb(slaveWorm) == 0x12)
				printf("NMI functioning\n");
			else
				printf("NMI not working\n");

	 	}
		else
	 	{
			printf("No"PLATFORMNAME" found or com ram bad\n");
			exit(5);
	 	}
	}
#endif
	printf("Board Test Complete\n");
	exit(0);
}

//==============================================================================
