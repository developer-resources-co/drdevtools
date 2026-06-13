//=============================================================================
//	globals: stuff everyone should include
//=============================================================================

#ifdef SYSTEMGEN
#define GENESIS
#endif

#ifdef SYSTEMSNES
#define SNES
#endif

#define DEBUG 1

typedef unsigned int errorcode;
extern unsigned char textBuffer[];

//=============================================================================
// platform specific defines

#ifdef SNES
#define PLATFORMNAME "SNES"
#endif

#ifdef GENESIS
#define PLATFORMNAME "Genesis"
#endif

extern unsigned char szVersion[];


//=============================================================================
// standard includes

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <string.h>

//=============================================================================

