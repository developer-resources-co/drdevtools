//=============================================================================
//      globals: stuff everyone should include
//
// WBNIV        11 Nov 92       Changed ULONG, UWORD, UBYTE, etc. to typedefs
//
//=============================================================================

// actually, this file is for compatibility with drmon for the expression parser

#ifndef DRMON_GLOBAL_H
#define DRMON_GLOBAL_H

//============================================================================

typedef unsigned long ULONG;
typedef unsigned short UWORD;
typedef unsigned char UBYTE;


typedef unsigned long ulong;
typedef unsigned int uword;
typedef unsigned char ubyte;

typedef int word;
typedef char byte;

long MemRead(long addr,byte size);

const unsigned long OUT_OF_BAND = 0xFFFFFFFF;

//============================================================================

typedef int errorcode;

//============================================================================

#endif

//=============================================================================
