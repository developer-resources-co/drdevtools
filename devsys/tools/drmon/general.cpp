//=============================================================================
//  general.c: ordinary stuff
//=============================================================================

#include <ctype.h>

#include "base.hpp"
#include "global.hpp"
#include "list.hpp"
#include "gadget.hpp"
#include "object.hpp"
#include "listrect.hpp"
#include "symbol.hpp"
#include "general.hpp"
#include "screen.hpp"


FLAG printCheckForSymbol = boolean::TRUE;

//=============================================================================

char
hexTable[16] =
{
	'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
};

//=============================================================================
// print nibble into buffer in hex

char *
Print4Bits(char *buffer,UBYTE num)
{
	*buffer++ = hexTable[num & 0xf];
	return(buffer);
}

//=============================================================================
// print byte into buffer in hex
char * far
Print8Bits(char *buffer,UBYTE num)
{
	*buffer++ = hexTable[(num>>4) & 0xf];
	*buffer++ = hexTable[num & 0xf];
	return(buffer);
}

//=============================================================================
// print word into buffer in hex
char * far
Print16Bits(char *buffer,UWORD num)
{
	*buffer++ = hexTable[(num>>12) & 0xf];
	*buffer++ = hexTable[(num>>8) & 0xf];
	*buffer++ = hexTable[(num>>4) & 0xf];
	*buffer++ = hexTable[num & 0xf];
	return(buffer);
}

//=============================================================================

char *
PrintRaw24Bits(char *buffer,ULONG num)
{
	*buffer++ = hexTable[(num>>20) & 0xf];
	*buffer++ = hexTable[(num>>16) & 0xf];
	*buffer++ = hexTable[(num>>12) & 0xf];
	*buffer++ = hexTable[(num>>8) & 0xf];
	*buffer++ = hexTable[(num>>4) & 0xf];
	*buffer++ = hexTable[num & 0xf];
	return(buffer);
}

//=============================================================================
// print into buffer in hex

char *
Print24Bits(char *buffer,ULONG num)
{
#ifdef SNES
	_symbolList *sPtr;
	if(printCheckForSymbol)
		if((sPtr = FindHexSymbol(num)) != NULL)
			return(PrintString(buffer,sPtr->Name()));
#endif
	*buffer++ = '$';
	return(PrintRaw24Bits(buffer, num));
}

//=============================================================================
// print long into buffer in hex

char * far
PrintRaw32Bits(char *buffer,ULONG num)
{
	*buffer++ = hexTable[(num>>28) & 0xf];
	*buffer++ = hexTable[(num>>24) & 0xf];
	*buffer++ = hexTable[(num>>20) & 0xf];
	*buffer++ = hexTable[(num>>16) & 0xf];
	*buffer++ = hexTable[(num>>12) & 0xf];
	*buffer++ = hexTable[(num>>8) & 0xf];
	*buffer++ = hexTable[(num>>4) & 0xf];
	*buffer++ = hexTable[num & 0xf];
	return(buffer);
}

//=============================================================================

char * far
PrintBinary32Bits(char *buffer,ULONG num)
{
	int i;
	for(i=32;i != 0;i--)
		*buffer++ = (num >> ((i-1)) & 1) + '0';
	return(buffer);
}

//=============================================================================

char * far
Print32Bits(char *buffer,ULONG num)
{
#ifdef GENESIS
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"
	_symbolList *sPtr;
	if(printCheckForSymbol)
		if(sPtr = FindHexSymbol(num))
			return(PrintString(buffer,sPtr->Name()));
#pragma GCC diagnostic pop
#endif
		return(PrintRaw32Bits(buffer, num));
}

//=============================================================================
// print safe ascii char into buffer

char *
PrintASCII(char *buffer, unsigned char num)
{
	if(num < ' ')
		num = 250;
	else if(num > 0x7f)
		num = 249;
	*buffer++ = num;
	return(buffer);
}

//=============================================================================

char *
PrintDecimal16Bits(char *buffer, UWORD num)
{
	int count;
	count = '0';
	while(num >= 10000)
	 {
		count++;
		num -= 10000;
	 }
	 *buffer++ = count;

	count = '0';
	while(num >= 1000)
	 {
		count++;
		num -= 1000;
	 }
	 *buffer++ = count;
	count = '0';
	while(num >= 100)
	 {
		count++;
		num -= 100;
	 }
	 *buffer++ = count;
	count = '0';
	while(num >= 10)
	 {
		count++;
		num -= 10;
	 }
	 *buffer++ = count;
	 *buffer++ = '0'+num;
	 return(buffer);
}

//=============================================================================

char *
PrintDecimal32Bits(char *buffer, ULONG num)
{
	int count;

	count = '0';
	while(num >= 1000000000)
	 {
		count++;
		num -= 1000000000;
	 }
	 *buffer++ = count;

	count = '0';
	while(num >= 100000000)
	 {
		count++;
		num -= 100000000;
	 }
	 *buffer++ = count;

	count = '0';
	while(num >= 10000000)
	 {
		count++;
		num -= 10000000;
	 }
	 *buffer++ = count;

	count = '0';
	while(num >= 1000000)
	 {
		count++;
		num -= 1000000;
	 }
	 *buffer++ = count;

	count = '0';
	while(num >= 100000)
	 {
		count++;
		num -= 100000;
	 }
	 *buffer++ = count;

	count = '0';
	while(num >= 10000)
	 {
		count++;
		num -= 10000;
	 }
	 *buffer++ = count;

	count = '0';
	while(num >= 1000)
	 {
		count++;
		num -= 1000;
	 }
	 *buffer++ = count;
	count = '0';
	while(num >= 100)
	 {
		count++;
		num -= 100;
	 }
	 *buffer++ = count;
	count = '0';
	while(num >= 10)
	 {
		count++;
		num -= 10;
	 }
	 *buffer++ = count;
	 *buffer++ = '0'+num;
	 return(buffer);
}

//=============================================================================

char *
PrintString(char *buffer, const char* string)
{
	while(*string)
		*buffer++ = *string++;
	return(buffer);
}

//=============================================================================

void
CopyScreen(char far *dBuff,char far *sBuff,unsigned int len)
{
	// Copy len bytes. The old body did `len /= 4` then copied `long`s — correct only
	// where sizeof(long)==4 (16-bit DOS); on 64-bit Linux long is 8 bytes, so it copied
	// 2x len and ran screenSize bytes past both buffers (garbled the screen at widths
	// whose over-copy hit live heap, and corrupted the heap). memcpy copies exactly len.
	memcpy(dBuff, sBuff, len);
}

//=============================================================================

void
CopyMem(char far *dBuff,char far *sBuff,unsigned int len)
{
	// Same 64-bit long fix as CopyScreen: copy exactly len bytes (was len/=4 + long
	// copy, which copied 2x on a 64-bit host where long is 8 bytes, not 4).
	memcpy(dBuff, sBuff, len);
}

//=============================================================================
// string functions
//=============================================================================
// count # of chars before newline or end of string

int
StrToNLLen(char*tPtr)
{
	int strLen;
	strLen = 0;

	while(*tPtr != '\n' && *tPtr != 0)
	 {
		strLen++;
		tPtr++;
	 }
	return(strLen);
}

//=============================================================================

void
FillString(char*buffer, unsigned char fill, int len)
{
	while(len--)
		*buffer++ = fill;
}

//=============================================================================
// pad string with spaces to specified length(used by dissasembler)
void
PadString(char*buffer,int offset)
{
	int i;
	i = 0;
	while(*buffer)
	 {
		buffer++;
		i++;
	 }

	while(i<offset)					// kts note: was <=
	 {
		*buffer++ = ' ';
		i++;
	 }
	*buffer = 0;
}

//=============================================================================
// remove padding from string

void
UnPadString(char*buffer)
{
//	while(*buffer++)
//		;

	while(*buffer)					// kts 12-30-92 02:37am(above is WRONG!)
		buffer++;

	buffer--;
	while(*buffer == ' ')
	while(isspace(*buffer))
		buffer--;

	*(++buffer) = '\0';
}

//=============================================================================

char *
KillTrailNL(char *s)
{
	if (s && *s)
	if (s[strlen(s)-1] == '\n')
		s[strlen(s)-1] = 0;
	return s;
}

//=============================================================================

char *
SkipSpace(char *s)
{
	if(s && *s)
		while(isspace(*s)) s++;
	return(s);
}

//=============================================================================

char*
DupString(const char *sourceString)
{
	char*destString;
//	UnPadString(sourceString);					// KTS 02-01-93 01:29am

	destString = (char *)malloc(1+strlen(sourceString));
	if(destString)
		strcpy(destString,sourceString);
	return(destString);
}

//=============================================================================

char*
FindNull(char*sourceString)
{
	while(*sourceString)
		sourceString++;
	return(sourceString);
}

//=============================================================================

char*
FindChar(char*sourceString,unsigned char c)
{
	while(*sourceString)
	 {
		if(*sourceString == c)
			break;
		sourceString++;
	 }
	return(sourceString);
}

//=============================================================================

void
UpCaseString(char*sPtr)
{
	while(*sPtr)
	 {
		if(*sPtr >= 'a' && *sPtr <= 'z')
				*sPtr = *sPtr - ('a'-'A');
		sPtr++;
	 }
}

//=============================================================================

