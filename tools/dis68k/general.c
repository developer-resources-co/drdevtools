//=============================================================================
//  general.c: ordinary stuff
//=============================================================================

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
char *
Print8Bits(char *buffer,UBYTE num)
{
	*buffer++ = hexTable[(num>>4) & 0xf];
	*buffer++ = hexTable[num & 0xf];
	return(buffer);
}

//=============================================================================
// print word into buffer in hex
char *
Print16Bits(char *buffer,UWORD num)
{
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
	*buffer++ = hexTable[(num>>20) & 0xf];
	*buffer++ = hexTable[(num>>16) & 0xf];
	*buffer++ = hexTable[(num>>12) & 0xf];
	*buffer++ = hexTable[(num>>8) & 0xf];
	*buffer++ = hexTable[(num>>4) & 0xf];
	*buffer++ = hexTable[num & 0xf];
	return(buffer);
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
#if GENESIS
	struct _symbolList *sPtr;
	if(sPtr = FindHexSymbol(num))
		return(PrintString(buffer,sPtr->name));
	else
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
PrintString(char *buffer, char *string)
{
	while(*string)
		*buffer++ = *string++;
	return(buffer);
}

//=============================================================================

void
CopyMem(char far *dBuff,char far *sBuff,unsigned int len)
{
    unsigned int i;
	long far *source,far *dest;
	source = (long far *)sBuff;
	dest = (long far *)dBuff;
	len /= 4;
    for(i=0;i<len;++i)
	*dest++ = *source++;
}

//=============================================================================
// string functions
//=============================================================================
// count # of chars before newline or end of string

int
StrToNLLen(unsigned char *tPtr)
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
// pad string with spaces to specified length(used by dissasembler)
void
PadString(unsigned char *buffer,int offset)
{
	int i;
	i = 0;
	while(*buffer)
	 {
		buffer++;
		i++;
	 }

	if(i < offset)
	 {
		while(i<=offset)
		 {
			*buffer++ = ' ';
			i++;
		 }
	 }
	*buffer = 0;
}

//=============================================================================
// remove padding from string

void
UnPadString(unsigned char *buffer)
{
	while(*buffer)
		buffer++;

	buffer--;
	while(*buffer == ' ')
		buffer--;

	buffer++;
	*buffer = 0;
}

//=============================================================================