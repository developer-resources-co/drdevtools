//=============================================================================
//  general.hpp: general.cpp header file
//=============================================================================

#ifndef DRMON_general_H
#define DRMON_general_H

//=============================================================================

extern FLAG printCheckForSymbol;
extern char hexTable[16];

char *
Print4Bits(char *buffer,UBYTE num);

char *
Print8Bits(char *buffer,UBYTE num);

char *
Print16Bits(char *buffer,UWORD num);

char *
PrintRaw24Bits(char *buffer,ULONG num);

char *
Print24Bits(char *buffer,ULONG num);

char *
Print32Bits(char *buffer,ULONG num);

char *
PrintRaw32Bits(char *buffer,ULONG num);

void
FillString(char*buffer, unsigned char fill, int len);

char *
PrintASCII(char *buffer, unsigned char num);

char *
PrintString(char *buffer, const char* string);

char *
PrintBinary32Bits(char *buffer,ULONG num);

char *
PrintDecimal32Bits(char *buffer, ULONG num);

char *
PrintDecimal16Bits(char *buffer, UWORD num);

void
CopyMem(char *dBuff,char *sBuff,unsigned int len);

int
StrToNLLen(char*tPtr);

void
PadString(char*buffer,int offset);

void
UnPadString(char*buffer);

char *
SkipSpace(char *s);

char*
DupString(const char *sourceString);

char*
FindNull(char*sourceString);

char*
FindChar(char*sourceString,unsigned char c);

void
UpCaseString(char*sPtr);

extern void
CopyScreen(char *dBuff,char *sBuff,unsigned int len);

char *
KillTrailNL(char *s);

//=============================================================================

#endif

//=============================================================================


