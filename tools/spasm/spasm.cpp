
//=============================================================================
// spasm.c: condom for spaz68K and spasm816(trojan ribbed)
//=============================================================================

#include	<stdio.h>
#include	<stdlib.h>
#include	<dos.h>
#include	<alloc.h>
#include	<string.h>
#include	<process.h>
#include	<time.h>
#include	<dir.h>

//============================================================================

typedef unsigned long ULONG;
typedef unsigned short UWORD;
typedef unsigned char UBYTE;

//============================================================================

#include	"global.hpp"
#include	"error.hpp"
#include	"express.hpp"

//=============================================================================

#define FLAG unsigned char
#define TRUE 1
#define FALSE 0

#define ULONG unsigned long
#define UWORD unsigned int
#define UBYTE unsigned char
//#define byte unsigned char

#define ERR_NOINPUT 5
#define ERR_FILENOTFOUND 5
#define ERR_FILENOTOPENED 5
#define ERR_NOMEM 5
#define ERR_BOARD 10

//==============================================================================

#ifdef SNES

//#define FAMICARD

#ifdef FAMICARD
#define PORTBASE 0x304
#else
#define PORTBASE 0x318
#endif

#define WORDSWAP 0
#define PROCESSORNAME "65816"
#define PLATFORMNAME "SNES"

void RestoreSNESVectors(void);

#endif

#ifdef GENESIS
#define PORTBASE 0x318
#define WORDSWAP 1
#define	PROCESSORNAME "68000"
#define PLATFORMNAME "Genesis"
#endif

#ifdef SPC700
#define PORTBASE 0x000
#define PROCESSORNAME "SPC700"
#define PLATFORMNAME "SONY"
#endif


//==============================================================================

void Cleanup(int err);
extern "C" unsigned long SwapLong (unsigned long a);
extern "C" unsigned int SwapWord (unsigned int a);

//==============================================================================
// new board stuff

unsigned int slaveCtrl,slaveBank,slaveWorm;
unsigned char far *slaveBufferCmd;
unsigned char far *slaveBufferData;

//=============================================================================
// parser stuff
unsigned char expBuffer[251];
token	tokenStream[251];	/* reasonably large */
token	*tsPtr;				/* pointer into the stream */
char 	*isPtr;				/* pointer to the input stream */
ULONG answer;

//=============================================================================
// other globals

void far *memChunk = NULL;
int memSizeSeg;
extern "C" void SPASM(void);
extern "C" unsigned long far *FindSymbol(char far *symbol);

extern unsigned char far SRC_FILE[MAXPATH];

extern unsigned char far SLD_FILE[MAXPATH];
extern unsigned char far SLDFLAG;

extern unsigned char far bCaseSensitive;
extern unsigned char far SYSTEMTYPE;
extern unsigned char far SYSTEM_FILE[MAXPATH];

extern unsigned char far MAP_FILE[MAXPATH];
extern unsigned char far MAPFLAG;

extern unsigned char far SYM_FILE[MAXPATH];
extern unsigned char far SYMFLAG;

extern unsigned char far LIST_FILE[MAXPATH];
extern unsigned char far bBackpatchListingFile;

extern unsigned int far SPASM_MAJOR;
extern unsigned int far SPASM_MINOR;
extern unsigned int far SPASM_REV;


#ifdef SNES
//extern unsigned int far SFXBASE;
//extern unsigned int far SFXMEMSEG;
#endif
extern unsigned char far ERRORLEVEL;

#ifdef BENCHMARK
extern long lines_assembled;
#endif
extern unsigned char szVersion[];
#include "version.h"

//=============================================================================

unsigned int slaveBase = PORTBASE;
unsigned char far *slaveBuffer = (unsigned char far *)0xD0000000;

//------------------------------------------------------------------------------

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
#ifdef SNES
//	SFXBASE = slaveCtrl;
//	SFXMEMSEG = ((unsigned long)slaveBuffer)>>16;
#endif
}

//=============================================================================

void
DoFileName(unsigned char *name, unsigned char far *dest,unsigned char *ext)
{
	unsigned char far *nPtr;
	if(*name)
	 {
		_fstrcpy(dest,(unsigned char far *)name);
		if(!_fstrchr(dest,'.'))
			_fstrcat(dest,(unsigned char far *)ext);
	 }
}

//=============================================================================

int
main(int argc,char *argv[])
{
    long memSize;
	int argCount;
	char szProgName[ _MAX_FNAME ];// base name of argv[0] for printing program name

	// For filename translations later
	char szDrive[ MAXDRIVE ];
	char szDir[ MAXDIR ];
	char szFile[ MAXFILE ];
	char szExt[ MAXEXT ];

	SYSTEMTYPE = 0;
#ifdef  FAMICARD
//	SYSTEMTYPE = 3;									// choose SFX board
#endif
#ifdef SPC700
	SYSTEMTYPE = 2;									// Write to disk by default
#endif
	SYMFLAG = FALSE;
	MAPFLAG = FALSE;
	SLDFLAG = FALSE;
	*SYM_FILE = 0;
	*SLD_FILE = 0;
	*MAP_FILE = 0;
	*SYSTEM_FILE = 0;
	*LIST_FILE = 0;


#if 0
	{ // Create major/minor/rev version numbers
	unsigned char *_szVersion = szVersion;

	_szVersion += sscanf( ++_szVersion, "%d", &SPASM_MAJOR );
	_szVersion += sscanf( ++_szVersion, "%d", &SPASM_MINOR );
	_szVersion += sscanf( ++_szVersion, "%d", &SPASM_REV );
//	sscanf( szVersion, "[a-zA-Z ]*%d.%d.%d", &SPASM_MAJOR, &SPASM_MINOR, &SPASM_REV );
	}
#else
	SPASM_MAJOR = rmj;
	SPASM_MINOR = rmm;
	SPASM_REV   = rup;
#endif

		_splitpath( argv[0], NULL, NULL, szProgName, NULL );
		strlwr( szProgName );

#if 0
	fprintf( stderr, "%s %s (c) 1991,92,93 Developer Resources.  All Rights Reserved.\n\
",szProgName, szVersion);
#else
	fprintf( stderr, "%s Version %d.%d.%d\n"
		"Copyright 1991,92,93,94 Developer Resources.  All Rights Reserved.\n",
		szProgName, rmj, rmm, rup );
#endif

	argCount = 1;
	while(argCount < argc)
	 {
		if(*argv[argCount] == '-')				// must be command line switch
			switch(argv[argCount][1])
			 {
				case 'n':
				case 'N':
					SYSTEMTYPE = 1;
					break;
				case 'w':
				case 'W':
					SYSTEMTYPE = 2;
					DoFileName(&argv[argCount][2],SYSTEM_FILE,".bin");
					break;
				case 'o':
				case 'O':
					SYSTEMTYPE = 3;
					break;
				case 's':
				case 'S':
					SYMFLAG = TRUE;
					DoFileName(&argv[argCount][2],SYM_FILE,".sym");
					break;
				case 'm':
				case 'M':
					MAPFLAG = TRUE;
					DoFileName(&argv[argCount][2],MAP_FILE,".map");
					break;
				case 'L':
					bBackpatchListingFile = TRUE;
				case 'l':
					DoFileName(&argv[argCount][2],LIST_FILE,".lst");
					break;
				case 'd':
				case 'D':
					SLDFLAG = TRUE;
					DoFileName(&argv[argCount][2],SLD_FILE,".sld");
					break;
				case 'c':
				case 'C':
					bCaseSensitive = TRUE;
					break;
			 }
		else break;
		argCount++;
	 }

	if(argc-argCount < 1)
	 {
		printf(PROCESSORNAME" Assembler\n\
By Erik Anderson, Kevin T. Seghetti, and William B. Norris IV\n"
#ifdef SPC700
"SPC700 additions by William B. Norris IV\n"
#endif
#ifdef Z80
"Z80 additions by Kevin T. Seghetti\n"
#endif
"Usage: %s <switches> sourcefile\n\
Switches:\n\
	-c: Case-sensitive symbols\n\
	-d: Write source debug info\n\
	-s: Write symbol file\n\
	-m: Write map file\n\
	-l: Set list filename\n\
	-n: No Development System, send output nowhere\n\
	-w: No Development System, send output to disk\n\
	To override default filenames, follow switch with filename\n\
",szProgName);
		Cleanup(ERR_NOINPUT);
	 }

	LoadEnvVariables();
	InitSlaveIO(slaveBase,slaveBuffer);

#ifdef SNES
	if ( SYSTEMTYPE == 3 )
		{ // Establish communication OK with board.

		int counter = 30;
		while (--counter && (inportb(slaveWorm) != 0xA3))
			delay( 10 );

		if	( !counter )
			{
			printf( "Slave dead\n" );
			Cleanup( ERR_BOARD );
			}
		}
#endif

	// copy source file name into assembly buffer
	fnsplit( argc <= argCount ? "test.asm" : argv[argCount],
		szDrive, szDir, szFile, szExt );

	char lSRC_FILE[_MAX_PATH];
	fnmerge( lSRC_FILE, szDrive, szDir, szFile, *szExt ? szExt : ".asm" );
	_fstrcpy( SRC_FILE, lSRC_FILE );

	char lSLD_FILE[_MAX_PATH];
	_fstrcpy( lSLD_FILE, *SLD_FILE ? SLD_FILE : SRC_FILE );

	// copy source level debugging file name into assembly buffer
	fnsplit( lSLD_FILE,	szDrive, szDir, szFile, szExt );

	if(!*SLD_FILE)
		strcpy(szExt,".sld");

	fnmerge( lSLD_FILE, szDrive, szDir, szFile, *szExt ? szExt : ".sld" );
	_fstrcpy( SLD_FILE, lSLD_FILE );

	// copy destination file name into assembly buffer
	if(SYSTEMTYPE == 2  && !*SYSTEM_FILE)
		{
		_fstrcpy(SYSTEM_FILE,SRC_FILE);
		_fstrcpy(_fstrchr(SYSTEM_FILE,'.'),(unsigned char far *)".bin");
		}


	// copy map file name into assembly buffer
	if(MAPFLAG  && !*MAP_FILE)
	 {
		_fstrcpy(MAP_FILE,SRC_FILE);
		_fstrcpy(_fstrchr(MAP_FILE,'.'),(unsigned char far *)".map");
	 }


	// copy symbol file name into assembly buffer
	if(SYMFLAG && !*SYM_FILE)
		{
		_fstrcpy(SYM_FILE,SRC_FILE);
		_fstrcpy(_fstrchr(SYM_FILE,'.'),(unsigned char far *)".sym");
		}


	if(!*LIST_FILE)
		{
		_fstrcpy(LIST_FILE,SRC_FILE);
		_fstrcpy(_fstrchr(LIST_FILE,'.'),(unsigned char far *)".lst");
		}

	// set up to call assembly, giving all available memory to it
    memSize = farcoreleft();
    memChunk = farmalloc(memSize);
    memSizeSeg = memSize >> 4;

	{
#ifdef BENCHMARK
	long t_start, t_end, t_diff;
	long far *dos_tod = (long far *)MK_FP(0x40,0x6C);

	t_start = *dos_tod;
#endif

	SPASM();

#ifdef BENCHMARK
	t_end = *dos_tod;
	t_diff = t_end - t_start;
	if ( lines_assembled )
		{
		printf( "\nLines assembled: %ld  (%ld lines/min)\n",
			lines_assembled, lines_assembled*1092/t_diff );
		}
#endif
	}

#ifdef SNES
	if ( SYSTEMTYPE == 3 )
		RestoreSNESVectors();
#endif

	Cleanup(ERRORLEVEL);					// will auto-free memchunk
}

//=============================================================================
// SNES note: this is just here to make the expression parser happy, if anyone tries
// to read memory from the assembler, it will just return 0.

#ifdef GENESIS
extern "C" long ReadMemFromC(unsigned long addr);

long MemRead(long addr,byte size)
{
    unsigned long foo = 0;
    foo = ReadMemFromC(addr);
	switch (size)
	{
		case	1:
			return(foo & 0xff);
		case	2:
			return(SwapWord(foo)&0xffff);

		case	3:
			foo &= 0xffffff;
			return(SwapLong(foo));
		default:
			return(SwapLong(foo));
	}
}
#endif

#ifdef SNES

long MemRead(long l,byte s)
{
    return( 0L );
}
#endif

#ifdef SPC700
long MemRead(long l,byte s)
{
    return( 0L );
}
#endif

//=============================================================================

//unsigned long far *nii;

//unsigned long far *LookupVar(char * s)
unsigned long far LookupVar(char * s)
{
	ulong far* sym;
//    nii = FindSymbol ((char far *) s);
//         return ((unsigned long far *)nii);

    sym = FindSymbol ((char far *) s);
	if(sym)
	    return(*sym);
	else
		return(OUT_OF_BAND);
}

//=============================================================================

errorcode EvalVar( ULONG * lvalue, token * * p )
{
	int	i;
	ULONG j,k;
	j = 0L;
	for (i = 0; i < 32 ; i += 8 ) {
		k = (ULONG) ((ULONG)(*(*p)++) << i);
		j |= k;
		}
//	*lvalue = *(ULONG *)j ;
	*lvalue = j;
	return NOERR;
}


#if 0
errorcode EvalVar( unsigned long * lvalue, token * * p )
{
	int	i;
	unsigned char *tValue;
	unsigned char far *vPtr;
	unsigned long j,k;
	tValue = (unsigned char *)lvalue;
	j = 0L;
	for (i = 0; i < 32 ; i += 8 ) {
		k = (unsigned long) ((unsigned long)(*(*p)++) << i);
		j |= k;
		}

	//erik:low to high
	//pc: sp, ss

	vPtr = (unsigned char far *)j;
	*tValue++ = *vPtr++;
	*tValue++ = *vPtr++;
	*tValue++ = *vPtr++;
	*tValue++ = *vPtr++;
	return NOERR;
}
#endif

//=============================================================================
// actual expression parser interface, called from assembly

int
DoExpr()
{
	int error;
		tsPtr = tokenStream;
		isPtr = expBuffer;

		error = TokenizeExpression(&tsPtr,&isPtr);
		tsPtr = tokenStream;

		if(!error)
			error = EvalExpression(&answer,&tsPtr);
		return(error);
}

//=============================================================================

void
Cleanup(int err)
{
	if(memChunk != 0)
		farfree(memChunk);
	exit(err);
}

//=============================================================================

#ifdef SNES
void
RestoreSNESVectors(void)
{

#ifdef FAMICARD
	unsigned char far *baddr;
	unsigned int delayCount;

	// check and see if rom is running
	if(inportb(slaveWorm) == 0xA3)
	 {
		// rom appears to be running, lets tell it to update its vectors
		outportb(slaveCtrl,0x6c);
		outportb(slaveBank,0xf);			// go to comm page
		baddr = 0xd000d100;
		*baddr = 6;							// restore vectors command
		outportb(slaveCtrl,0x68);
		outportb(slaveWorm,2);
		delayCount = 0xfffe;
		while((inportb(slaveWorm) & 1) && delayCount--);
		outportb(slaveWorm,0);
		if(delayCount ==  0xffff)
		 {
			printf("Slave Dead\n");
			return;
		 }

		delayCount = 0xfffe;
		while(!(inportb(slaveWorm) & 1) && delayCount--);
		if(delayCount ==  0xffff)
			printf("Slave Dead\n");
     }
#else

#endif
}


#endif

#if 0
unsigned char *
FindDot(unsigned char *string)
{
	while(*string != 0)
		if(*string++ == '.')
			return(--string);
	return(NULL);
}

//=============================================================================

unsigned char far *
FindFarDot(unsigned char far *string)
{
	while(*string != 0)
		if(*string++ == '.')
			return(--string);
	return(NULL);
}

//=============================================================================

void
FarStrCpy(unsigned char far *dest,unsigned char far *src)
{
	while(*dest++ = *src++);
}

//=============================================================================

void
FarStrCat(unsigned char far *dest,unsigned char far *src)
{
	while(*dest++);
	dest--;
	while(*dest++ = *src++);
}


#endif
//=============================================================================

