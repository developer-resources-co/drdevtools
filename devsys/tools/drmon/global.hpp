//=============================================================================
//      globals: stuff everyone should include
//
// WBNIV        11 Nov 92       Changed ULONG, UWORD, UBYTE, etc. to typedefs
//
//=============================================================================

#ifndef DRMON_GLOBAL_H
#define DRMON_GLOBAL_H

#include "compat.hpp"
#include <dos.h>

#ifdef __BORLANDC__

#undef  far
#define far
#undef  near
#define near
#undef  _far
#define _far
#undef  _near
#define _near

//#define farfree free
//#define farmalloc malloc

#define TRUE TRUE
#define FALSE FALSE

//?void delay( unsigned );

#endif

//=============================================================================

#ifdef SYSTEMGEN
#define GENESIS
#endif

#ifdef SYSTEMSNES
#define SNES
#endif

#ifdef DEBUGCOFF
#define SYMBOL_CASE_SENSITIVE
#endif

#ifdef DEBUGDR
#endif

#if defined( DEBUGZARDOZ )
#define SYMBOL_CASE_SENSITIVE
#endif

#define DEBUG

//==============================================================================

//#define TRUE 1
//#define FALSE 0
typedef unsigned int FLAG;

typedef unsigned long ULONG;
typedef unsigned short UWORD;
typedef unsigned char UBYTE;

#define until( exp )    while ( !( exp ) )


const unsigned long OUT_OF_BAND = 0xFFFFFFFF;

//typedef unsigned int errorcode;
//extern unsigned char textBuffer[];

//=============================================================================
// file extensions

#define EXT_HELP "HLP"
#define EXT_PLATFORM "PLT"
#define EXT_PROCESSOR "PRC"
#define EXT_SCRIPT "SCR"

#define HELPNAME "Help"

//=============================================================================
// platform specific defines


#ifdef SNES
#define PORTBASE 0x318
#ifdef EMUL
#define PROGNAME "SNESMon Demo"
#else
#define PROGNAME "SNESMon"
#endif
#define WORDSWAP 0
#define PROCESSORNAME "658/16"
#define PLATFORMNAME "SNES"
#endif



#ifdef GENESIS
#define PORTBASE 0x318
#ifdef EMUL
#define PROGNAME "GenMon Demo"
#else
#define PROGNAME "GenMon"
#endif
#define WORDSWAP 1
#define PROCESSORNAME "68000"
#define PLATFORMNAME "Genesis"
#endif

#ifdef MASTERSYSTEM
#define PROCESSORNAME "Z80"
#define PLATFORMNAME "MasterSystem"
#endif

#ifdef NES
#define PROCESSORNAME "65C02"
#define PLATFORMNAME "NES"
#endif

//=============================================================================
// standard includes

#include <stdio.h>
#include <stdlib.h>
//#include <alloc.h>
#include <string.h>
#include <assert.h>

#if defined( DOSX286 ) || defined( __OS2__ )
#define farmalloc malloc
#define farfree free
#endif

//=============================================================================

#endif

//=============================================================================
