//=============================================================================
//	dis816.c: 65816 single-line dissasembler
//=============================================================================

#include "global.hpp"
#include "general.hpp"
#include "memory.hpp"
#include "tab816.cpp"

static int pTable[3] =
{
	0,							// 1
	9,
	20
};

//=============================================================================
// print into buffer in hex

char*
Print16BitsLabCheck(char *buffer, ULONG num)
	{
	_symbolList* sPtr;
	if(printCheckForSymbol)
		if ( sPtr = FindHexSymbol(num) )
			return( PrintString( buffer, sPtr->Name() ) );
//  else

	*buffer++ = '$';
	return Print16Bits( buffer, (UWORD)num );
	}


char *
Print8BitsLabCheck(char *buffer, ULONG num)
	{
	_symbolList* sPtr;

	if(printCheckForSymbol)
		if ( sPtr = FindHexSymbol(num) )
			return PrintString( buffer, sPtr->Name() );
//	else
	*buffer++ = '$';
	return Print8Bits( buffer, (UWORD)num );
	}

//=============================================================================

// 0 = 0808
// 1 = 1608
// 2 = 0816
// 3 = 1616

//=============================================================================
// dismodes:
// 0 = op-codes only
// 1 = addresses & op-codes
// 2 = addresses, hex & op-codes

unsigned int
Disassem(unsigned long addr, char far *_inBuff, char *outBuff,int disMode)
{
	int pad;
	unsigned char cd,byte,addrMode;
	unsigned int offset,i,tempint;
	unsigned char far *tBuff;
	char *oBuff;
	ULONG templong;
	oBuff = outBuff;
	unsigned char* inBuff = (unsigned char*)_inBuff;	// for easy unsigned
	int index = *inBuff;

	pad = pTable[disMode];
	addrMode = addrTable[index];
	offset = opSizeTable[addrMode];

	if(addrMode == 5)
	 {
		if(op5AddrTable[index] == 2)
			offset = 2;
		else
			switch(procMode)
		 	 {
				case 0: 			// normal, do nothing
					break;
				case 1:
					if(!op5AddrTable[index])
						offset = 3;
					break;
				case 2:
					if(op5AddrTable[index])
						offset = 3;
					break;
				case 3:
					offset = 3;
					break;
		 	 }
	 }

	cd = opTable[index];
//	if(cd == 0xff)
//		cd = OP_UNDEFINED;

	*outBuff = 0;

	if(disMode > 0)
		outBuff += sprintf(outBuff,"%06lx: ",addr);

	if(disMode > 1)
	 {
		tBuff = inBuff;
		for(i=0;i<offset;++i)
			outBuff += sprintf(outBuff,"%02x ",*tBuff++);
	 }

	inBuff++;						// skip op-code index
	PadString(oBuff,pad);
	outBuff = oBuff + pad;
	//outBuff += sprintf(outBuff,"%s  ",opText[cd]);
	outBuff = PrintString(outBuff,opText[cd]);
	*outBuff++ = ' ';
	*outBuff++ = ' ';

	switch(addrMode)
	 {
		case 0:
			*outBuff = 0;
			break;
		case 1:
			//outBuff += sprintf(outBuff,"$%04x",*(unsigned int far *)inBuff);
			*outBuff++ = '|';
			//*outBuff++ = '$';
			*Print16BitsLabCheck(outBuff,*(unsigned int far *)inBuff) = 0;
			break;
		case 2:
			//outBuff += sprintf(outBuff,"$%04x,X",*(unsigned int far *)inBuff);
			*outBuff++ = '|';
			//*outBuff++ = '$';
			outBuff = Print16BitsLabCheck(outBuff,*(unsigned int far *)inBuff);
			*PrintString(outBuff,",X") = 0;
			break;
		case 3:
			//outBuff += sprintf(outBuff,"$%04x,Y",*(unsigned int far *)inBuff);
			*outBuff++ = '|';
			//*outBuff++ = '$';
			outBuff = Print16BitsLabCheck(outBuff,*(unsigned int far *)inBuff);
			*PrintString(outBuff,",Y") = 0;
			break;
		case 4:
			//outBuff += sprintf(outBuff,"A");
			*outBuff++ = 'A';
			*outBuff = 0;
			break;
		case 5:					// immediate addressing mode, only this one
								// needs to check which mode we are in to
								// properly dissasemble
			if(offset == 3)
			 {
				//outBuff += sprintf(outBuff,"#$%04x",*(unsigned int far *)inBuff);
				*outBuff++ = '#';
				//*outBuff++ = '$';
				*Print16BitsLabCheck(outBuff,*(unsigned int far *)inBuff) = 0;
			 }
			else
			 {
				//outBuff += sprintf(outBuff,"#$%02x",*inBuff);
				*outBuff++ = '#';
				//*outBuff++ = '$';
				*Print8BitsLabCheck(outBuff,*inBuff) = 0;
			 }
			break;
		case 6:
			//outBuff += sprintf(outBuff,"($%02x,X)",*inBuff);
			*outBuff++ = '(';
			//*outBuff++ = '$';
			outBuff = Print8BitsLabCheck(outBuff,*inBuff);
			*outBuff++ = ',';
			*outBuff++ = 'X';
			*outBuff++ = ')';
			*outBuff = 0;
			break;
		case 7:
			//outBuff += sprintf(outBuff,"($%02x),Y",*inBuff);
			*outBuff++ = '(';
			//*outBuff++ = '$';
			outBuff = Print8BitsLabCheck(outBuff,*inBuff);
			*outBuff++ = ')';
			*outBuff++ = ',';
			*outBuff++ = 'Y';
			*outBuff = 0;
			break;
		case 8:
			//outBuff += sprintf(outBuff,"($%04x)",*(unsigned int far *)inBuff);
			*outBuff++ = '(';
			*outBuff++ = '|';
			//*outBuff++ = '$';
			outBuff = Print16BitsLabCheck(outBuff,*(unsigned int far *)inBuff);
			*outBuff++ = ')';
			*outBuff = 0;
			break;
		case 9:
			//outBuff += sprintf(outBuff,"$%04lx",(unsigned long)addr+offset+(char)*inBuff);
			//*outBuff++ = '$';
			*Print16BitsLabCheck(outBuff,(unsigned long)addr+offset+(char)*inBuff) = 0;
			break;
		case 10:
			//outBuff += sprintf(outBuff,"$%02x",*inBuff);
			//*outBuff++ = '$';
			*Print8BitsLabCheck(outBuff,*inBuff) = 0;
			break;
		case 11:
			//outBuff += sprintf(outBuff,"$%02x,X",*inBuff);
			//*outBuff++ = '$';
			outBuff = Print8BitsLabCheck(outBuff,*inBuff);
			*outBuff++ = ',';
			*outBuff++ = 'X';
			*outBuff = 0;
			break;
		case 12:
			//outBuff += sprintf(outBuff,"$%02x,Y",*inBuff);
			//*outBuff++ = '$';
			outBuff = Print8BitsLabCheck(outBuff,*inBuff);
			*outBuff++ = ',';
			*outBuff++ = 'Y';
			*outBuff = 0;
			break;
// 65C02 addressing modes:
		case 13:
			//outBuff += sprintf(outBuff,"($%02x)",*inBuff);
			*outBuff++ = '(';
			//*outBuff++ = '$';
			outBuff = Print8BitsLabCheck(outBuff,*inBuff);
			*outBuff++ = ')';
			*outBuff = 0;
			break;
		case 14:
			//outBuff += sprintf(outBuff,"($%04x,X)",*(unsigned int far *)inBuff);
			*outBuff++ = '(';
			*outBuff++ = '|';
			//*outBuff++ = '$';
			outBuff = Print16BitsLabCheck(outBuff,*(unsigned int far *)inBuff);
			*outBuff++ = ',';
			*outBuff++ = 'X';
			*outBuff++ = ')';
			*outBuff = 0;
			break;
// 8/16 addressing modes:
		case 15:
			tempint = *inBuff;
			tempint += ((unsigned long)*(inBuff+1)) << 8;
			//outBuff += sprintf(outBuff,"$%06lx",(unsigned long)addr+offset+tempint);
			//*outBuff++ = '$';
			templong = addr & 0xff0000;
			templong += (addr + offset + tempint) & 0xffff;
			*Print24Bits(outBuff,templong) = 0;
			break;
		case 16:
			//outBuff += sprintf(outBuff,"$%02x,S",*inBuff);
			//*outBuff++ = '$';
			outBuff = Print8BitsLabCheck(outBuff,*inBuff);
			*outBuff++ = ',';
			*outBuff++ = 'S';
			*outBuff = 0;
			break;
		case 17:
			//outBuff += sprintf(outBuff,"($%02x,S),Y",*inBuff);
			*outBuff++ = '(';
			//*outBuff++ = '$';
			outBuff = Print8BitsLabCheck(outBuff,*inBuff);
			*outBuff++ = ',';
			*outBuff++ = 'S';
			*outBuff++ = ')';
			*outBuff++ = ',';
			*outBuff++ = 'Y';
			*outBuff = 0;
			break;
		case 18:
			//outBuff += sprintf(outBuff,"$%02x,$%02x",*inBuff,*(inBuff+1));
			//*outBuff++ = '$';
			outBuff = Print8BitsLabCheck(outBuff,*(inBuff+1));
			*outBuff++ = ',';
			//*outBuff++ = '$';
			*Print8BitsLabCheck(outBuff,*(inBuff)) = 0;
			break;
		case 19:
			templong = *inBuff;
			templong += (unsigned long)*(inBuff+1) << 8;
			templong += ((unsigned long)*(inBuff+2)) << 16;
			//outBuff += sprintf(outBuff,"$>%06lx",templong);
			*outBuff++ = '>';
			//*outBuff++ = '$';
			*Print24Bits(outBuff,templong) = 0;
			break;
		case 20:
			templong = *inBuff;
			templong += (unsigned long)*(inBuff+1) << 8;
			templong += ((unsigned long)*(inBuff+2)) << 16;
			//outBuff += sprintf(outBuff,"$>%06lx,X",templong);
			*outBuff++ = '>';
			//*outBuff++ = '$';
			outBuff = Print24Bits(outBuff,templong);
			*outBuff++ = ',';
			*outBuff++ = 'X';
			*outBuff = 0;
			break;
		case 21:                           // kts changed for Erik, its his fault(used to be [])
			//outBuff += sprintf(outBuff,"($%04x)",*(unsigned int far *)inBuff);
			*outBuff++ = '(';
			//*outBuff++ = '$';
			outBuff = Print16BitsLabCheck(outBuff,*(unsigned int far *)inBuff);
			*outBuff++ = ')';
			*outBuff = 0;
			break;
		case 22:
			//outBuff += sprintf(outBuff,"[$%02x]",*inBuff);
			*outBuff++ = '[';
			//*outBuff++ = '$';
			outBuff = Print8BitsLabCheck(outBuff,*inBuff);
			*outBuff++ = ']';
			*outBuff = 0;
			break;
		case 23:
			//outBuff += sprintf(outBuff,"[$%02x],Y",*inBuff);
			*outBuff++ = '[';
			//*outBuff++ = '$';
			outBuff = Print8BitsLabCheck(outBuff,*inBuff);
			*outBuff++ = ']';
			*outBuff++ = ',';
			*outBuff++ = 'Y';
			*outBuff = 0;
			break;
		case 24:
			//outBuff += sprintf(outBuff,"$%02x",*inBuff);
			*outBuff++ = '$';
			*Print8Bits(outBuff,*inBuff) = 0;
			break;							// interrupts, print signature
		default:
			break;
	 }

	return(offset);
}

//=============================================================================
