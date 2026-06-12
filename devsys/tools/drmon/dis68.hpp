//==============================================================================
//	dis68.h:
//==============================================================================

void PadLine (void);
void FinishLogicFunctions (unsigned short int number);
unsigned short int GetExtendedWord (void);
unsigned short int GetWord (unsigned short int *buffer);

//----------------------------------------------------------------------------
//Opcode parsers
//----------------------------------------------------------------------------
void ParseBthings2 (unsigned short int number);
void ParseMOVEP (unsigned short int number);
void ParseBthings (unsigned short int number);
void ParseADDI (unsigned short int number);
void ParseCLR (unsigned short int number);
void ParseNEGX (unsigned short int number);
void ParseNEG (unsigned short int number);
void ParseNOT (unsigned short int number);
void ParseNBCD (unsigned short int number);
void ParseTST (unsigned short int number);
void ParseMOVEM (unsigned short int number);
void ParseFourMisc (unsigned short int number);
void ParseTRAP (unsigned short int number);
void ParseLINK (unsigned short int number);
void ParseMOVEUSP (unsigned short int number);
void ParseOther (unsigned short int number);
void ParseJUMPS (unsigned short int number);
void ParseCHKLEA (unsigned short int number);
void ParseSUBI (unsigned short int number);
void ParseCMPI (unsigned short int number);
void ParseORI (unsigned short int number);
void ParseANDI (unsigned short int number);
void ParseMUL (unsigned short int number);
void ParseEXG (unsigned short int number);
void ParseEXG2 (unsigned short int number);
void ParseABCD (unsigned short int number);
void ParseEORI (unsigned short int number);
void ParseADDSUBQ (unsigned short int number);
void ParseDIV (unsigned short int number);
void ParseSBCD (unsigned short int number);
void ParseSUBX (unsigned short int number);
void ParseADDX (unsigned short int number);
void ParseCMP (unsigned short int number);
void ParseAND (unsigned short int number);
void ParseSUB (unsigned short int number);
void ParseOr (unsigned short int number);
void ParseADD (unsigned short int number);
void ParseEOR (unsigned short int number);

//--------------------------------------------------------------------------
// Field Parsing
//--------------------------------------------------------------------------
void ParseImmediat (unsigned short int number);
void ParseTindes (unsigned short int number);
void ParseSzDestin (unsigned short int number);
void ParseSzDestinI (unsigned short int number);
void ParseSz (unsigned short int number);
void ParseDr (unsigned short int number);
void ParseAr (unsigned short int number);
void ParseRn (unsigned short int number);
void ParseCond (unsigned short int number);
void ParseDSzEA(unsigned short int number);
void ParseRegOpEA (unsigned short int number);
void ParseRegOpEA2 (unsigned short int number);
void ParseRegList (unsigned short int number,int reversed);
void Parse8BitDisplacement (signed char displace);
void Parse16BitDisplacement (signed int displace);


//--------------------------------------------------------------------------
// EA parsers
//--------------------------------------------------------------------------
void ParseImmediateEA (unsigned short int number);
void ParsePCDisplaceIndex (unsigned short int number);
void ParsePCDisplace (unsigned short int number);
void Parse16Abs (unsigned short int number);
void Parse32Abs (unsigned short int number);
void ParseMiscEA (unsigned short int number);
void ParseNot68K (unsigned short int number);
void ParseArDisplaceIndex (unsigned short int number);
void ParseArDisplace (unsigned short int number);
void ParseArIndirect (unsigned short int number);
void ParseArIndirect_postinc (unsigned short int number);
void ParseArIndirect_predec (unsigned short int number);

//--------------------------------------------------------------------------
// top level parsers
//--------------------------------------------------------------------------

unsigned short int *Parse0(unsigned short int *buffer);
unsigned short int *Parse1(unsigned short int *buffer);
unsigned short int *Parse2(unsigned short int *buffer);
unsigned short int *Parse3(unsigned short int *buffer);
unsigned short int *Parse4(unsigned short int *buffer);
unsigned short int *Parse5(unsigned short int *buffer);
unsigned short int *Parse6(unsigned short int *buffer);
unsigned short int *Parse7(unsigned short int *buffer);
unsigned short int *Parse8(unsigned short int *buffer);
unsigned short int *Parse9(unsigned short int *buffer);
unsigned short int *Parse10(unsigned short int *buffer);
unsigned short int *Parse11(unsigned short int *buffer);
unsigned short int *Parse12(unsigned short int *buffer);
unsigned short int *Parse13(unsigned short int *buffer);
unsigned short int *Parse14(unsigned short int *buffer);
unsigned short int *Parse15(unsigned short int *buffer);


//--------------------------------------------------------------------------
unsigned short int *diss (unsigned short int *buffer,char *output);
//unsigned int far Disassem(unsigned long addr,unsigned char *inBuff,unsigned char *outBuff,int disMode);
unsigned int Disassem(unsigned long addr,unsigned char *inBuff,unsigned char *outBuff,int disMode);

//==============================================================================

