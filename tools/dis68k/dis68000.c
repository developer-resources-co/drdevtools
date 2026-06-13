#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dis68.h"
#include "alloc.h"


#define BYTE 0
#define WORD 1
#define LONG 2

//#include "general.c"

#define STRINGLENGTH 20

//--------------------------------------------------------------------------
// Inline macros
//--------------------------------------------------------------------------

#define PARSEEA(a)                             \
    {                                          \
        EA_parser[(a)>>3]((a)&7);              \
    }


#define ADDTEMP                 \
    {                       \
        strcat (outputstring,tempstring);   \
        outputposition += strlen (tempstring);  \
    }


#define ADDSTRING(a)                \
    {                       \
        strcat (outputstring,a);        \
        outputposition += strlen (a);       \
    }


#define MAKESTRING(a)               \
    {                               \
        tempstring[0]='$';               \
        *Print16Bits (&tempstring[1],(a))=0;     \
    }

#define MAKEADDRESS(a)               \
    {                               \
        tempstring[0]='$';               \
        *Print32Bits (&tempstring[1],(a))=0;     \
    }

#define MAKESTRINGSHORT(a)               \
    {                               \
        tempstring[0]='$';               \
        *Print8Bits (&tempstring[1],(a))=0;     \
    }

#define ADDCHAR(a)              \
    {                       \
         outputstring[outputposition++]=(a);   \
         outputstring[outputposition]=0;        \
    }


//--------------------------------------------------------------------------
// character tables
//--------------------------------------------------------------------------
char sizeext[4][4]=
{
    ".B",
    ".W",
    ".L",    // sometimes 10 = long
    ".L"     // sometimes 11 = long
};

char Bthing[4][5]=
{
    "BTST",
    "BCHG",
    "BCLR",
    "BSET"
};


char ConditionCodes[16][4]=
{
    "T",     // T ?
    "F",     // F ?
    "HI",
    "LS",
    "CC",
    "CS",
    "NE",
    "EQ",
    "VC",
    "VS",
    "PL",
    "MI",
    "GE",
    "LT",
    "GT",
    "LE"
};


char rots[8][6]=
{
    "ASR",
    "LSR",
    "ROXR",
    "ROR",

    "ASL",
    "LSL",
    "ROXL",
    "ROL"
};

char OtherFunction[8][6] =
{
    "RESET",
    "NOP",
    "STOP",
    "RTE",
    "FOO",
    "RTS",
    "TRAPV",
    "RTR"
};

//--------------------------------------------------------------------------
// globals
//--------------------------------------------------------------------------
char *outputstring;
short int current_size;
short int outputposition;
char tempstring[20];
unsigned short int far *ext_words;
unsigned long address;


//--------------------------------------------------------------------------
// First layer parsing functions
//--------------------------------------------------------------------------
unsigned short int far *((*top_nybble_parser[16]) (unsigned short int far *buffer)) =
{
    Parse0,
    Parse1,
    Parse2,
    Parse3,
    Parse4,
    Parse5,
    Parse6,
    Parse7,
    Parse8,
    Parse9,
    Parse10,
    Parse11,
    Parse12,
    Parse13,
    Parse14,
    Parse15
};

//--------------------------------------------------------------------------
void (*Zero_parser[16]) (unsigned short int number) =
{
    ParseORI,
    ParseBthings,
    ParseANDI,
    ParseBthings,
    ParseSUBI,
    ParseBthings,
    ParseADDI,
    ParseBthings,
    ParseBthings2,
    ParseBthings,
    ParseEORI,
    ParseBthings,
    ParseCMPI,
    ParseBthings,
    ParseNot68K,    // 68010 instruction
    ParseBthings
};

//--------------------------------------------------------------------------
void (*Four_parser[16]) (unsigned short int number) =
{
    ParseNEGX,
    ParseCHKLEA,
    ParseCLR,
    ParseCHKLEA,
    ParseNEG,
    ParseCHKLEA,
    ParseNOT,
    ParseCHKLEA,
    ParseNBCD,
    ParseCHKLEA,
    ParseTST,
    ParseCHKLEA,
    ParseMOVEM,
    ParseCHKLEA,
    ParseFourMisc,
    ParseCHKLEA
};

void (*FourMiscFunction[16]) (unsigned short int number) =
{
    ParseNot68K,
    ParseNot68K,
    ParseNot68K,
    ParseNot68K,
    ParseTRAP,
    ParseLINK,
    ParseMOVEUSP,
    ParseOther,
    ParseJUMPS,
    ParseJUMPS,
    ParseJUMPS,
    ParseJUMPS,
    ParseJUMPS,
    ParseJUMPS,
    ParseJUMPS,
    ParseJUMPS
};

char HEXCHAR[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

//--------------------------------------------------------------------------
// Effective Address Parsers
void (*EA_parser[8]) (unsigned short int number) =
{
    ParseDr,
    ParseAr,
    ParseArIndirect,
    ParseArIndirect_postinc,
    ParseArIndirect_predec,
    ParseArDisplace,
    ParseArDisplaceIndex,
    ParseMiscEA
};

//--------------------------------------------------------------------------
// Effective Address parsers, second layer for 111
void (*EA_misc_parser[8]) (unsigned short int number) =
{
    Parse16Abs,
    Parse32Abs,
    ParsePCDisplace,
    ParsePCDisplaceIndex,
    ParseImmediateEA,
    ParseNot68K,
    ParseNot68K,
    ParseNot68K
};


//---------------------------------------------------------------------------
//short int itoa (short int a,char *string,short int base)
//{
// sprintf (string,"%0x",a);
//}


unsigned short int GetWord (unsigned short int far *buffer)
{
    unsigned int a;
    unsigned char far *foo=(unsigned char far *)buffer;
    a = (*foo++)<<8;
    a |= *foo;
    return (a);
}



//----------------------------------------------------------------------------
// Support Functions
//---------------------------------------------------------------------------
void PadLine (void)
{
    while (outputposition<8)
        ADDCHAR(' ');
}

//--------------------------------------------------------------------------
void FinishLogicFunctions (unsigned short int number)
{
    if ((number&0x003f) == 0x3C)
        {
            if ((number&0x00C0))
                {
                    ADDSTRING(".W")
                    PadLine ();
                    MAKESTRING(GetExtendedWord () & 0xffff);
                    ADDTEMP;
                    ADDSTRING(",SR");

                }
             else
                {
                    ADDSTRING(".B")
                    PadLine ();
                    MAKESTRING(GetExtendedWord () & 0xff);
                    ADDTEMP;
                    ADDSTRING(",CCR");
                }
        }
    else
    ParseSzDestinI (number);
}


//--------------------------------------------------------------------------
unsigned short int GetExtendedWord (void)
{
    return (GetWord(ext_words++));
}

//----------------------------------------------------------------------------
// Opcode Parsers
//--------------------------------------------------------------------------
void ParseBthings2 (unsigned short int number)
{
    ADDSTRING(Bthing[(number>>6)&3]);
    PadLine ();
    MAKESTRING(GetExtendedWord ());
    ADDCHAR('#');
    ADDTEMP;
    ADDCHAR(',');
    PARSEEA((number&0x3f));
}

//--------------------------------------------------------------------------
void ParseBthings (unsigned short int number)
{
    if ((number & 0x0038) == 0x0008)
        {
        ParseMOVEP(number);
        return;
        }
    ADDSTRING(Bthing[(number>>6)&3]);
    PadLine ();
    ParseDr (number>>9);
    ADDCHAR(',');
    PARSEEA((number&0x3f));
}

void ParseMOVEP (unsigned short int number)
{
    ADDSTRING ("MOVEP");
    ParseSz(((number>>6)&1)+1);
    PadLine ();
    if (!(number & 0x080))
        {
              Parse16Abs (number);
              ADDCHAR('(');
              ParseAr(number&0x07);
              ADDCHAR(')');
              ADDCHAR(',');
              ParseDr((number>>9)&0x07);
        }
        else
        {
              ParseDr((number>>9)&0x07);
              ADDCHAR(',');
              Parse16Abs (number);
              ADDCHAR('(');
              ParseAr(number&0x07);
              ADDCHAR(')');
        }

}


//--------------------------------------------------------------------------
void ParseADDI (unsigned short int number)
{
    ADDSTRING("ADDI");
    ParseSzDestinI (number);
}

//--------------------------------------------------------------------------
void ParseCLR (unsigned short int number)
{
    if ((number & 0xC0) == 0xC0)
     {
        ADDSTRING("MOVE.W");
        PadLine ();
        ADDSTRING("CCR,");
        PARSEEA(number&0x3f);
     }
     else
    {
        ADDSTRING("CLR");
        ParseSzDestin (number);
    }
}

//----------------------------------------------------------------------------

void ParseNEGX (unsigned short int number)
{
    if ((number & 0xC0) == 0xC0)
     {
        ADDSTRING("MOVE.W");
        PadLine ();
        ADDSTRING("SR,");
        PARSEEA(number&0x3f);
     }
     else
     {
        ADDSTRING("NEGX");
        ParseSzDestin(number);
     }
}

//----------------------------------------------------------------------------
void ParseNEG (unsigned short int number)
{
    if ((number & 0xC0) == 0xC0)
     {
        ADDSTRING("MOVE.W");
        PadLine ();
        PARSEEA(number&0x3f);
        ADDSTRING(",CCR");
     }
     else
        {
        ADDSTRING("NEG");
        ParseSzDestin (number);
        }
}

//----------------------------------------------------------------------------
void ParseNOT (unsigned short int number)
{
    if ((number & 0xC0) == 0xC0)
     {
        ADDSTRING("MOVE.W");
        PadLine ();
        PARSEEA(number&0x3f);
        ADDSTRING(",SR");
     }
     else
        {
        ADDSTRING("NOT");
        ParseSzDestin (number);
        }
}

//----------------------------------------------------------------------------
void ParseNBCD (unsigned short int number)
{
    if ((number&0x00c0)==00)
        {
            ADDSTRING("NBCD.B");
            PadLine();
            PARSEEA(number&0x3f);
            return;
        }
    if ((number&0x00F8) == 0x040)
        {
            ADDSTRING("SWAP");
            PadLine();
            ParseDr (number&7);
            return;
        }
   if ((number&0x00C0) == 0x40)
        {
            ADDSTRING("PEA");
            PadLine ();
            PARSEEA(number&0x3f);
            return;
        }
   if ((number&0x038) == 00)
        {
            if (number & 0x040)
                ADDSTRING("EXT.L")
            else
                ADDSTRING("EXT.W");
            PadLine ();
            ParseDr(number&0x07);
            return;
        }
  ADDSTRING("MOVEM");
  ParseSz(((number>>6)&1)+1);
  PadLine();
  ParseRegList (GetExtendedWord(),((number&0x038)==0x20));
  ADDCHAR(',');
  PARSEEA(number&0x3f);
}

//----------------------------------------------------------------------------
void ParseTST (unsigned short int number)
{
    if ((number & 0x00c0) == 0x00c0)
    {
        if ((number & 0x3f) == 0x003C)
        {
            ADDSTRING("ILLEGAL");
            return;
        }
        ADDSTRING("TAS");
        PadLine();
        PARSEEA(number&0x03f);
        return;
    }
    ADDSTRING("TST");
    ParseSzDestin (number);
}

//----------------------------------------------------------------------------
void ParseMOVEM (unsigned short int number)
{
  int a;
  ADDSTRING("MOVEM");
  ParseSz(((number>>6)&1)+1);
  PadLine();
  a = GetExtendedWord();
  PARSEEA(number&0x3f);
  ADDCHAR(',');
  ParseRegList (a,((number&0x038)==0x20));
}

//----------------------------------------------------------------------------
void ParseFourMisc (unsigned short int number)
{
    int subnumber;
    subnumber = number & 0xf0;
    subnumber >>=4;
    FourMiscFunction[subnumber](number);
}

//----------------------------------------------------------------------------
void ParseTRAP (unsigned short int number)
{
    ADDSTRING("TRAP");
    PadLine ();
    ADDCHAR('$');
    ADDCHAR(HEXCHAR[number&0x0f]);
}

//----------------------------------------------------------------------------
void ParseLINK (unsigned short int number)
{
    if (number & 0x08)
        ADDSTRING("UNLK")
    else
        ADDSTRING("LINK");
    PadLine ();
    ParseAr(number&0x07);
    ADDCHAR(',');
    Parse16BitDisplacement(GetExtendedWord());
    ADDTEMP;
}

//----------------------------------------------------------------------------
void ParseMOVEUSP (unsigned short int number)
{
    ADDSTRING("MOVE");
    PadLine();
    if (number&0x08)
    {
        ADDSTRING("USP,");
        ParseAr(number&0x07);
    }
    else
    {
        ParseAr(number&0x07);
        ADDSTRING(",USP");
    }
}



//----------------------------------------------------------------------------
void ParseOther (unsigned short int number)
 {
    int subnumber;
    subnumber = number & 0x07;
    ADDSTRING(OtherFunction[subnumber]);
    PadLine();
    if (subnumber==2)
            {
                ADDCHAR(',');
                Parse16BitDisplacement(GetExtendedWord());
                ADDTEMP;
            }

}


//----------------------------------------------------------------------------
void ParseJUMPS (unsigned short int number)
{
    if (number&0x040)
        ADDSTRING("JMP")
    else
        ADDSTRING("JSR");
    PadLine();
    PARSEEA(number&0x3f);
}

void ParseCHKLEA (unsigned short int number)
{
    if (number&0x040)
        ADDSTRING("LEA")
    else
        ADDSTRING("CHK");
    PadLine();
    PARSEEA(number&0x3f);
    ADDCHAR(',');
    ParseAr((number>>9)&0x07);
}

//--------------------------------------------------------------------------
void ParseSUBI (unsigned short int number)
{
    ADDSTRING("SUBI");
    ParseSzDestinI (number);
}

//--------------------------------------------------------------------------
void ParseCMPI (unsigned short int number)
{
    ADDSTRING("CMPI");
    ParseSzDestinI (number);
}

//--------------------------------------------------------------------------
void ParseORI (unsigned short int number)
{
    ADDSTRING("ORI");
    FinishLogicFunctions (number);
}

//--------------------------------------------------------------------------
void ParseANDI (unsigned short int number)
{
    ADDSTRING("ANDI");
    FinishLogicFunctions (number);
}

//----------------------------------------------------------------------------
void ParseMUL (unsigned short int number)
{
    if (number&0x0100) ADDSTRING("MULS")
        else ADDSTRING("MULU");
    PadLine ();
    PARSEEA(number&0x3f);
    ADDCHAR(',');
    ParseDr ((number>>9)&0x7);
}

//----------------------------------------------------------------------------
void ParseEXG (unsigned short int number)
{
    ADDSTRING("EXG.L")
    PadLine ();
    if (number&0x8)
        {
            ParseAr ((number)&0x7);
            ADDCHAR(',');
            ParseAr ((number>>9)&0x7);
        }
        else
        {
            ParseDr ((number)&0x7);
            ADDCHAR(',');
            ParseDr ((number>>9)&0x7);
        }
}

//----------------------------------------------------------------------------
void ParseEXG2 (unsigned short int number)
{
    ADDSTRING("EXG.L")
    PadLine ();
    ParseDr ((number>>9)&0x7);
    ADDCHAR(',');
    ParseAr ((number)&0x7);
}

//----------------------------------------------------------------------------
void ParseABCD (unsigned short int number)
{
    ADDSTRING("ABCD.B")
    PadLine ();
    if (number & 8)
    {
        ParseArIndirect_predec ((number)&0x7);
        ADDCHAR(',');
        ParseArIndirect_predec ((number>>9)&0x7);
    }
    else
    {
        ParseDr ((number)&0x7);
        ADDCHAR(',');
        ParseDr ((number>>9)&0x7);
    }

}
//--------------------------------------------------------------------------
void ParseEORI (unsigned short int number)
{
    ADDSTRING("EORI");
    FinishLogicFunctions (number);
}

void ParseADDSUBQ (unsigned short int number)
{
    if (number&0x0100)
        ADDSTRING("SUBQ")
    else
        ADDSTRING("ADDQ");
    ParseSz ((number>>6)&3);
    PadLine ();
    ADDCHAR('#');
    ADDCHAR('$');
    if ((number>>9) & 0x07)
        *Print4Bits(tempstring,(number>>9)&7) = 0;
    else
        *Print4Bits(tempstring,8) = 0;
    ADDTEMP;
    ADDCHAR(',');
    PARSEEA(number&0x003f);
}
//---------------------------------------------------------------------------
void ParseDIV (unsigned short int number)
{
    ADDSTRING("DIV");
    if (number&0x0100)
        ADDCHAR('S')
    else
        ADDCHAR('U');
    PadLine ();
    PARSEEA(number&0x003f);
    ADDCHAR(',');
    ParseDr ((number>>9)&07);
}

//---------------------------------------------------------------------------
void ParseSBCD (unsigned short int number)
{
    void (*foo) (unsigned short int number);
    ADDSTRING("SBCD.B");
    PadLine ();
    if (number&0x08)
        foo = ParseArIndirect_predec;
    else
        foo = ParseDr;
    foo (number&0x7);
    ADDCHAR(',');
    foo ((number>>9)&0x7);
}

//---------------------------------------------------------------------------
void ParseSUBX (unsigned short int number)
{
    ADDSTRING("SUBX");
    ParseDSzEA(number);
}
//---------------------------------------------------------------------------
void ParseADDX (unsigned short int number)
{
    ADDSTRING("ADDX");
    ParseDSzEA(number);
}

//---------------------------------------------------------------------------
void ParseCMP (unsigned short int number)
{
    ADDSTRING("CMP");
    ParseRegOpEA2 (number);
}

//----------------------------------------------------------------------------
void ParseAND (unsigned short int number)
{
    ADDSTRING("AND");
    ParseRegOpEA (number);
}

//---------------------------------------------------------------------------
void ParseSUB (unsigned short int number)
{
    ADDSTRING("SUB");
    ParseRegOpEA (number);
}

//---------------------------------------------------------------------------
void ParseOr (unsigned short int number)
{
    ADDSTRING("OR");
    ParseRegOpEA (number);
}
//---------------------------------------------------------------------------
void ParseADD (unsigned short int number)
{
    ADDSTRING("ADD");
    ParseRegOpEA (number);
}

//---------------------------------------------------------------------------
void ParseEOR (unsigned short int number)
{
    ADDSTRING("EOR");
    ParseRegOpEA (number);
}



//--------------------------------------------------------------------------
// Field Parsing
//--------------------------------------------------------------------------
void ParseImmediat (unsigned short int number)
{
    ADDCHAR('#');
    MAKESTRING(number);
    ADDTEMP;
}

//--------------------------------------------------------------------------
void ParseTindes (unsigned short int number)
{
    short int number_buffer;
    short int number_buffer2;
    number_buffer = number & 7 ;
    number_buffer2= number >> 3;
    PARSEEA((number_buffer << 3 ) + number_buffer2);
}


//--------------------------------------------------------------------------
void ParseSzDestin (unsigned short int number)
{
    unsigned short int tempext;
    current_size = (number >> 6) & 0x03;
    ADDSTRING(sizeext[current_size]);
    PadLine ();
    PARSEEA((number&0x3f));
}

//----------------------------------------------------------------------------
void ParseSzDestinI (unsigned short int number)
{
    unsigned short int tempext;
    current_size = (number >> 6) & 0x03;
    ADDSTRING(sizeext[current_size]);
    PadLine ();
    ADDCHAR('#');
    if (current_size==LONG)
        {
        ADDCHAR('$');
        *Print32Bits (tempstring,(((long) GetExtendedWord()) << 16) + GetExtendedWord())=0;
        ADDTEMP;
        }
    else
    {
            tempext = GetExtendedWord ();
            if (current_size==BYTE) tempext&=0xff;
            MAKESTRING(tempext);
            ADDTEMP;
    }
    ADDCHAR(',');
    PARSEEA((number&0x3f));
}


//--------------------------------------------------------------------------
void ParseSz (unsigned short int number)
{
    current_size=number;
    if (current_size == 3) current_size=2;
    ADDSTRING(sizeext[number]);
}

//--------------------------------------------------------------------------
void ParseDr (unsigned short int number)
{
    ADDCHAR('D');
    ADDCHAR(number+'0');
}

//--------------------------------------------------------------------------
void ParseAr (unsigned short int number)
{
    ADDCHAR('A');
    ADDCHAR(number+'0');
}

//--------------------------------------------------------------------------
void ParseRn (unsigned short int number)
{
    if (number > 7)
        ADDCHAR('A')
    else
        ADDCHAR('D');
    ADDCHAR((number&0x07)+'0');
}

//--------------------------------------------------------------------------
void ParseCond (unsigned short int number)
{
    ADDSTRING(ConditionCodes[number]);
            PadLine ();
}

//---------------------------------------------------------------------------
void ParseDSzEA(unsigned short int number)
{
    void (*foo) (unsigned short int number);
    ParseSz((number>>6)&0x03);
            PadLine ();
    if (number&0x08)
        foo = ParseArIndirect_predec;
    else
        foo = ParseDr;
    foo (number&0x7);
    ADDCHAR(',');
    foo ((number>>9)&0x7);
}

//---------------------------------------------------------------------------
void ParseRegOpEA (unsigned short int number)
{
    int foo;
    foo =number&0x1c0;
    foo >>=6;
    if ((foo==7)||(foo==3))
        {
            foo = 1;
            ParseSz(((number>>8)&0x1) + 1);
        }
        else
        {
             foo=0;
             ParseSz((number>>6)&0x3);
        }
    PadLine ();
    if (((number & 0x0100) == 0x0100) & (!foo))
        {
        ParseDr ((number>>9)&0x07);
        ADDCHAR(',');
        PARSEEA(number&0x3f);
        }
    else
        {
        PARSEEA(number&0x3f);
        ADDCHAR(',');
        if (foo)
                ParseAr ((number>>9)&0x07);
        else
                ParseDr ((number>>9)&0x07);
        }
}

//---------------------------------------------------------------------------
void ParseRegOpEA2 (unsigned short int number)
{
    int foo;
    foo =number&0x1c0;
    foo >>=6;
    if ((foo==7)||(foo==3))
        {
            foo = 1;
            ParseSz(((number>>8)&0x1) + 1);
        }
        else
        {
             foo=0;
             ParseSz((number>>6)&0x3);
        }
    PadLine ();
    PARSEEA(number&0x3f);
    ADDCHAR(',');
    if (foo)
            ParseAr ((number>>9)&0x07);
    else
            ParseDr ((number>>9)&0x07);
}

void ParseRegList (unsigned short int number,int reversed)
{
    int a;
    int b;
    if (number==0) return;
    for (a=0;a<16;a++,number>>=1)
        if ((number) & 1 )
        {
          if (reversed) b = a ^ 15;
             else b = a;
          ParseRn(b);
          ADDCHAR('/');
        }
    outputposition--;
    outputstring[outputposition]=0;
}


//----------------------------------------------------------------------------
void Parse8BitDisplacement (signed char displace)
{
    MAKEADDRESS(address+((signed long) displace));
}
//----------------------------------------------------------------------------
void Parse16BitDisplacement (signed int displace)
{
    MAKEADDRESS(address+((signed long) displace));
}



//--------------------------------------------------------------------------
// EA parsers
//--------------------------------------------------------------------------
// #Immed
void ParseImmediateEA (unsigned short int number)
{
    ADDCHAR('#');
    if (current_size==LONG)
        {
        ADDCHAR('$');
        *Print32Bits (tempstring,(((long) GetExtendedWord()) << 16) + GetExtendedWord())=0;
        ADDTEMP;
        return;
        }
     MAKESTRING(GetExtendedWord ());
     ADDTEMP;
}

//--------------------------------------------------------------------------
// d8(PC,Rn)
void ParsePCDisplaceIndex (unsigned short int number)
{
    unsigned short int d8;
    unsigned short int Rn;
    int islong;
    Rn = GetExtendedWord ();
    d8 = Rn & 0x0ff;
    islong = Rn & 0x0800;
    Rn >>= 12;
    ADDCHAR('$');
    *Print8Bits(tempstring,d8)=0;
    ADDTEMP;
    ADDCHAR('(');
    address+=2;
    ADDCHAR('P');
    ADDCHAR('C');
    ADDCHAR(',');
    ParseRn (Rn);
    if (islong)
        {
            ADDCHAR('.');
            ADDCHAR('L');
        }
    ADDCHAR(')');
}

//--------------------------------------------------------------------------
// d16(PC)
void ParsePCDisplace (unsigned short int number)
{
    MAKESTRING(GetExtendedWord());
    ADDTEMP;
    ADDCHAR('(');
    ADDCHAR('P');
    ADDCHAR('C');
    address+=2;
//    MAKESTRING(address);
//    ADDTEMP;
    ADDCHAR(')');
}

//--------------------------------------------------------------------------
// addr.W
void Parse16Abs (unsigned short int number)
{
    MAKESTRING(GetExtendedWord ());
    ADDTEMP;
}

//--------------------------------------------------------------------------
// addr.L
void Parse32Abs (unsigned short int number)
{
    ADDCHAR('$');
    *Print32Bits (tempstring,(((long) GetExtendedWord()) << 16) + GetExtendedWord())=0;
    ADDTEMP;
//    MAKESTRING(GetExtendedWord());
//    ADDTEMP;
}

void ParseMiscEA (unsigned short int number)
{
    EA_misc_parser[number&0x07] (number);
}

//--------------------------------------------------------------------------
// not a valid 68000 addressing mode
void ParseNot68K (unsigned short int number)
{
    ADDSTRING("!68K");
}


//--------------------------------------------------------------------------
// d8(An,Rn)
void ParseArDisplaceIndex (unsigned short int number)
{
    unsigned short int d8;
    unsigned short int Rn;
    int islong;
    Rn = GetExtendedWord ();
    d8 = Rn & 0x0ff;
    islong = Rn & 0x0800;
    Rn >>= 12;
    ADDCHAR('$');
    *Print8Bits(tempstring,d8)=0;
    ADDTEMP;
    ADDCHAR('(');
    ADDCHAR('A');
    ADDCHAR(number+'0');
    ADDCHAR(',');
    ParseRn (Rn);
    if (islong)
        {
            ADDCHAR('.');
            ADDCHAR('L');
        }
    ADDCHAR(')');
}

//--------------------------------------------------------------------------
// d16(An)
void ParseArDisplace (unsigned short int number)
{
    unsigned short int d16;
    d16 = GetExtendedWord ();
    MAKESTRING(d16);
    ADDTEMP;
    ADDCHAR('(');
    ADDCHAR('A');
    ADDCHAR(number+'0');
    ADDCHAR(')');
}

//--------------------------------------------------------------------------
// (An)
void ParseArIndirect (unsigned short int number)
{
    ADDCHAR('(');
    ADDCHAR('A');
    ADDCHAR(number+'0');
    ADDCHAR(')');
}

//--------------------------------------------------------------------------
// (An)+
void ParseArIndirect_postinc (unsigned short int number)
{
    ADDCHAR('(');
    ADDCHAR('A');
    ADDCHAR(number+'0');
    ADDCHAR(')');
    ADDCHAR('+');
}

//--------------------------------------------------------------------------
// -(An)
void ParseArIndirect_predec (unsigned short int number)
{
    ADDCHAR('-');
    ADDCHAR('(');
    ADDCHAR('A');
    ADDCHAR(number+'0');
    ADDCHAR(')');
}


//--------------------------------------------------------------------------
// top level parsers
//--------------------------------------------------------------------------



unsigned short int far *Parse0(unsigned short int far *buffer)
{
    short int subnumber;
    unsigned int number;
    number = GetWord(buffer);
    subnumber = (number >> 8) &0x0f;
    Zero_parser[subnumber] (number);
    return (ext_words);
}

//--------------------------------------------------------------------------
unsigned short int far *Parse1(unsigned short int far *buffer)
{
    unsigned int number;
    number = GetWord(buffer);
    ADDSTRING("MOVE.B");
    PadLine ();
    current_size = BYTE;
    PARSEEA(number&0x3f);
    ADDCHAR(',');
    ParseTindes ((number>>6)&0x3f);
    return (ext_words);
}

//--------------------------------------------------------------------------
unsigned short int far *Parse2(unsigned short int far *buffer)
{
    unsigned int number;
    number = GetWord(buffer);
    ADDSTRING("MOVE.L");
    PadLine ();
    current_size = LONG;
    PARSEEA(number&0x3f);
    ADDCHAR(',');
    ParseTindes ((number>>6)&0x3f);
    return (ext_words);
}

//--------------------------------------------------------------------------
unsigned short int far *Parse3(unsigned short int far *buffer)
{
    unsigned int number;
    number = GetWord(buffer);
    ADDSTRING("MOVE.W");
    PadLine ();
    current_size = WORD;
    PARSEEA(number&0x3f);
    ADDCHAR(',');
    ParseTindes ((number>>6)&0x3f);
    return (ext_words);
}

//---------------------------------------------------------------------------
unsigned short int far *Parse4(unsigned short int far *buffer)
{
    short int subnumber;
    unsigned int number;
    number = GetWord(buffer);
    subnumber = (number >> 8) &0x0f;
    Four_parser[subnumber] (number);
    return (ext_words);
}

//---------------------------------------------------------------------------
unsigned short int far *Parse5(unsigned short int far *buffer)
{
    unsigned short int temp_ext;
    unsigned int number;
    number = GetWord(buffer);
    if ((number&0x00C0)!=0x00C0)
        ParseADDSUBQ (number);
    else
    if ((number&0x0038)==8)
        {
        ADDSTRING("DB");
        ParseCond ((number>>8)&0x0f);
        ParseDr (number&0x07);
        ADDCHAR(',');
        address +=2;
        Parse8BitDisplacement (GetExtendedWord() & 0xff);
        ADDTEMP;
        }
    else
        {
        ADDSTRING("S");
        ParseCond ((number>>8)&0x0f);
        PARSEEA(number&0x03f);
        }
   return (ext_words);
}


//---------------------------------------------------------------------------
unsigned short int far *Parse6(unsigned short int far *buffer)
{
    unsigned int number;
    number = GetWord(buffer);
    ADDSTRING("B");
    if (((number>>8)&0x0f)==1) ADDSTRING("SR")
        else ParseCond ((number>>8)&0x0f);
    PadLine ();
    address+=2;
    if (number&0x0ff)
        Parse8BitDisplacement(number&0x0ff);
    else
        Parse16BitDisplacement(GetExtendedWord());
    ADDTEMP;
    return (ext_words);
}


//--------------------------------------------------------------------------
unsigned short int far *Parse7(unsigned short int far *buffer)
{
    unsigned int number;
    short int regnumber;
    short int immed;
    number = GetWord(buffer);
    ADDSTRING("MOVEQ");
    PadLine ();
    regnumber = ((number)&0x0e00)>>9;
    ADDCHAR('#');
    ADDCHAR('$');
    *Print8Bits(tempstring,(number & 0xff) )=0;
    ADDTEMP;
    ADDCHAR(',');
    ParseDr (regnumber);
    return (ext_words);
}


//---------------------------------------------------------------------------
unsigned short int far *Parse8(unsigned short int far *buffer)
{
    unsigned int number;
    number = GetWord(buffer);
    if ((number&0xc0)==0xc0)
        ParseDIV (number);
    else
        if ((number&0x1f0)==0x100)
            ParseSBCD (number);
        else
            ParseOr (number);
    return (ext_words);
}
//---------------------------------------------------------------------------
unsigned short int far *Parse9(unsigned short int far *buffer)
{
    unsigned int number;
    number = GetWord(buffer);
    if ((number&0x0130)==0x0100)
        ParseSUBX(number);
    else
        ParseSUB (number);
    return (ext_words);
}

//--------------------------------------------------------------------------
unsigned short int far *Parse10(unsigned short int far *buffer)
{
    ADDSTRING("ALINE");
    PadLine ();
    MAKESTRING(GetWord(buffer)&0x0fff);
    ADDTEMP;
    return (ext_words);
}

//---------------------------------------------------------------------------
unsigned short int far *Parse11(unsigned short int far *buffer)
{
    unsigned int number;
    number = GetWord(buffer);
    if ((!(number&0x0100))||((number&0x00c0)==0x00c0))
        ParseCMP (number);
    else
        if ((number&0x038)!=0x08)
            ParseEOR (number&0xfeff);
        else
            {
            ADDSTRING("CMPM");
            ParseSz ((number>>6)&0x03);
            PadLine ();
            ParseArIndirect_postinc (number&0x07);
            ADDCHAR(',');
            ParseArIndirect_postinc ((number>>9)&0x07);
            }
    return (ext_words);
}



//---------------------------------------------------------------------------
unsigned short int far *Parse12(unsigned short int far *buffer)
{
    unsigned int number;
    number = GetWord(buffer);
    if ((number & 0x00C0) == 0x00C0) { ParseMUL (number);  return(ext_words); }
    if ((number & 0x01F0) == 0x0140) { ParseEXG (number);  return(ext_words); }
    if ((number & 0x01F8) == 0x0188) { ParseEXG2 (number); return(ext_words); }
    if ((number & 0x01F0) == 0x0100) { ParseABCD (number); return(ext_words); }
    ParseAND (number);
    return (ext_words);
}
//---------------------------------------------------------------------------
unsigned short int far *Parse13(unsigned short int far *buffer)
{
    unsigned int number;
    number = GetWord(buffer);
    if ((number&0x0130)==0x0100)
        ParseADDX(number);
    else
        ParseADD (number);
    return (ext_words);
}

//---------------------------------------------------------------------------
unsigned short int far *Parse14(unsigned short int far *buffer)
{
    unsigned int number;
    unsigned int number2;
    number = GetWord(buffer);
    if ((number&0x00c0)!=0x00c0)
    {
        ADDSTRING(rots[((number>>3)&0x03)+((number&0x0100)>>6)]);
        ParseSz((number>>6)&0x03);
        PadLine();
        number2= (number>>9)&0x07;
        if ((number&0x020))
            ParseDr (number2);
        else
            {
            ADDCHAR('#');
            MAKESTRINGSHORT((number2?number2:8));
            ADDTEMP;
            }
        ADDCHAR(',');
        ParseDr (number&0x07);
    }
    else
    {
        ADDSTRING(rots[((number>>9)&0x03)+((number&0x0100)>>6)]);
        ADDCHAR ('.');
        ADDCHAR ('W');
        PadLine();
        PARSEEA(number&0x03f);
    }
    return (ext_words);
}

//--------------------------------------------------------------------------
unsigned short int far *Parse15(unsigned short int far *buffer)
{
    ADDSTRING("FLINE");
    PadLine ();
    MAKESTRING(GetWord(buffer)&0x0fff);
    ADDTEMP;
    return (ext_words);
}



//--------------------------------------------------------------------------
// entry point
//--------------------------------------------------------------------------
unsigned short int far *diss (unsigned short int far *buffer,char *output)
{
    short int a;
    unsigned short int top_nybble;
    top_nybble = GetWord(buffer) >> 12;
    outputstring  = output;
    outputposition = 0;
    for (a=0;a<STRINGLENGTH;a++)
        outputstring[a]=0;
    ext_words=buffer;
    ext_words++;
    return (top_nybble_parser[top_nybble](buffer));
}

// dismodes:
// 0 = op-codes only
// 1 = addresses & op-codes
// 2 = addresses, hex & op-codes

unsigned int far Disassem(unsigned long addr,unsigned char far *inBuff,unsigned char *outBuff,int disMode)
{
    char Hexdump[60];
    char OpCodes[60];
    unsigned short int far *buffer;
    unsigned short int far *endbuffer;
    unsigned int deltabytes = 0;
    char *foo;
    int a,b;
    address = addr;
    b = 160;     // SLS HACK
    for (a=0;a<b;a++)
        outBuff[a] = 0;
    for (a=0;a<25;a++)
        Hexdump[a]=' ';
    Hexdump[a]=0;
    OpCodes[0]=0;
    buffer = (unsigned short int far *) inBuff;
    if (disMode>0)
        {
        PrintRaw32Bits(outBuff,addr);
        strcat (outBuff,":");
        }
    endbuffer = diss(buffer,OpCodes);
    deltabytes = endbuffer - buffer;
    if (disMode>1)
    {
        foo = Hexdump;
        for (a=0;a<deltabytes;a++)
            foo = Print16Bits(foo,GetWord((buffer++))) + 1;
        strcat (outBuff,Hexdump);
    }
    deltabytes <<=1;
    strcat (outBuff,OpCodes);
    return (deltabytes);
}
