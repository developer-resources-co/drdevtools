//=============================================================================
//  tab816.c: 658/16 dissasembler
//  By Kevin T. Seghetti, (c) 1991 Developer Resources
//=============================================================================

char *opText[] =
{
	"ADC",  //  0
	"AND",  //  1
	"ASL",  //  2
	"BCC",  //  3
	"BCS",  //  4
	"BEQ",  //  5
	"BNE",  //  6
	"BMI",  //  7
	"BPL",  //  8
	"BVC",  //  9
	"BVS",  // 10
	"BIT",  // 11
	"BRK",  // 12
	"CLC",  // 13
	"CLD",  // 14
	"CLI",  // 15
	"CLV",  // 16
	"CMP",  // 17
	"CPX",  // 18
	"CPY",  // 19
	"DEC",  // 20
	"DEX",  // 21
	"DEY",  // 22
	"EOR",  // 23
	"INC",  // 24
	"INX",  // 25
	"INY",  // 26
	"JMP",  // 27
	"JSR",  // 28
	"LDA",  // 29
	"LDX",  // 30
	"LDY",  // 31
	"LSR",  // 32
	"NOP",  // 33
	"ORA",  // 34
	"PHA",  // 35
	"PHP",  // 36
	"PLA",  // 37
	"PLP",  // 38
	"ROL",  // 39
	"ROR",  // 40
	"RTI",  // 41
	"RTS",  // 42
	"SBC",  // 43
	"SEC",  // 44
	"SED",  // 45
	"SEI",  // 46
	"STA",  // 47
	"STX",  // 48
	"STY",  // 49
	"TAX",  // 50
	"TAY",  // 51
	"TSX",  // 52
	"TSY",  // 53
	"TXA",  // 54
	"TXS",  // 55
	"TYA",  // 56
	"???",  // 57
	"BRA",  // 58
	"BRL",  // 59
	"COP",  // 60
	"JML",  // 61
	"JSL",  // 62
	"MVN",  // 63
	"MVP",  // 64
	"PEA",  // 65
	"PEI",  // 66
	"PER",  // 67
	"PHB",  // 68
	"PHD",  // 69
	"PHK",  // 70
	"PHX",	// 71
	"PHY",	// 72
	"PLB",  // 73
	"PLD",  // 74
	"PLX",	// 75
	"PLY",	// 76
	"REP",  // 77
	"RTL",  // 78
	"SEP",  // 79
	"STP",  // 80
	"STZ",	// 81
	"TCD",  // 82
	"TCS",  // 83
	"TDC",  // 84
	"TRB",	// 85
	"TSB",	// 86
	"TSC",  // 87
	"TXY",  // 88
	"TYX",  // 89
	"WAI",  // 90
	"WDM",  // 91
	"XBA",  // 92
	"XCE"   // 93
};


#define OP_UNDEFINED 57


// this table contains the mnemonics for 8/16
unsigned char opTable[256] =

{
	// msb: 0
	12,									    // 0
	34,
	60, 				// cop
	34, 				// ora
	86,					// tsb
	34,
	2,
	34,					// ora
	36,
	34,
	2,
	69,					// phd
	86,					// tsb
	34,
	2,
	34,					// ora

	// msb: 1
	8,										// 1
	34,
	34,					// ora
	34,					// ora
	85,					// trb
	34,
	2,
	34,					// ora
	13,
	34,
	24,					// inc a
	83,					// tcs
	85,					// trb
	34,
	2,
	34,					// ora

	// msb: 2
	28,										// 2
	1,
	62,					// jsl
	1,					// and
	11,
	1,
	39,
	1,					// and
	38,
	1,
	39,
	74,					// pld
	11,
	1,
	39,
	1,					// and

	// msb: 3
	7,										// 3
	1,
	1,					// and
	1,					// and
	11,					// bit
	1,
	39,
	1,					// and
	44,
	1,
	20, 				// dec
	87,					// tsc
	11,
	1,
	39,
	1,					// and

	// msb: 4
	41,										// 4
	23,
	91,					// wdm(kts note, maybe we should say ??? instead)
	23,					// eor
	64,					// mvp
	23,
	32,
	23,					// eor
	35,
	23,
	32,
	70,					// phk
	27,
	23,
	32,
	23,					// eor

	// msb: 5
	9,										// 5
	23,
	23,					// eor
	23,					// eor
	63,					// mvn
	23,
	32,
	23,					// eor
	15,
	23,
	72,					// phy
	82,					// tcd
	27,					// jmp
	23,
	32,
	23,					// eor

	// msb: 6
	42,										// 6
	0,
	67,					// per
	0,					// adc
	81,					// stz
	0,
	40,
	0,					// adc
	37,
	0,
	40,
	78,					// rtl
	27,
	0,
	40,
	0,					// adc

	// msb: 7
	10,										// 7
	0,
	0,					// adc
	0,					// adc
	81,					// stz
	0,
	40,
	0,					// adc
	46,
	0,
	76,					// ply
	84,					// tdc
	27,					// jmp
	0,
	40,
	0,					// adc

	// msb: 8
	58,					// bra
	47,
	59,					// brl
	47,					// sta
	49,
	47,
	48,
	47,					// sta
	22,
	11,					// bit
	54,
	68,					// phb
	49,
	47,
	48,
	47,					// sta

	// msb: 9
	3, 										// 9
	47,
	47,					// sta
	47,					// sta
	49,
	47,
	48,
	47,					// sta
	56,
	47,
	55,
	88,					// txy
	81,					// stz
	47,
	81,					// stz
	47,					// sta

	// msb: a
	31,										// A
	29,
	30,
	29,					// lda
	31,
	29,
	30,
	29,					// lda
	51,
	29,
	50,
	73,
	31,
	29,
	30,
	29,					// lda

	// msb: b
	4,										// B
	29,
	29,
	29,
	31,
	29,
	30,
	29,					// lda
	16,
	29,
	52,
	89,					// tyx
	31,
	29,
	30,
	29,					// lda

	// msb: c
	19,										// C
	17,
	77,					// rep
	17,					// cmp
	19,
	17,
	20,
	17,					// cmp
	26,
	17,
	21,
	90,					// wai
	19,
	17,
	20,
	17,					// cmp

	// msb: d
	6,										// D
	17,
	17,					// cmp
	17,					// cmp
	66,					// pei
	17,
	20,
	17, 				// cmp
	14,
	17,
	71,					// phx
	80,					// stp
	61,					// jml
	17,
	20,
	17,					// cmp

	// msb: e
	18,										// E
	43,
	79,					// sep
	43,					// sbc
	18,
	43,
	24,
	43,					// sbc
	25,
	43,
	33,
	92,					// xba
	18,
	43,
	24,
	43,					// sbc

	// msb: f
	5,  									// f
	43,
	43,					// sbc
	43,					// sbc
	65,					// pea
	43,
	24,
	43,					// sbc
	45,
	43,
	75,					// plx
	93,					// xce
	28,					// jsr
	43,
	24,
	43
};

//=============================================================================

/*

addressing modes for this table:

0	none(implied) & stack
1	nn		absolute
2	nn,X	absolute X
3	nn,Y	absolute Y
4	A		accumulator
5	#n		immediate
6	(n,X)	indirect indexed x(pre)
7	(n),Y	indirect indexed y(post)
8	(nn)	absolute indirect
9	n		pc relative
10	n		zero page
11	n,X		zero page X
12	n,Y		zero page Y


(65C02 addressing modes)

13	(n)		indirect(without indexing)
14	(nn,X)	absolute indexed indirect


(8/16 extended addressing modes)

15	nn		pc relative long(16 bit)
16	n,S		stack relative
17	(n,S),Y	stack relative indirect indexed Y
18	n,n		block move
19	nnn		absolute long
20	nnn,X	absolute long indexed X
21	[nn]	absolute indirect long
22	[n]		direct page indirect long
23	[n],y	direct page indirect long indexed Y

// invented addressing modes

24	n		interrupt addressing(for BRK & COP)



*/

unsigned char
addrTable[256] =
{
// 0
	24,		//
	6,
	24,     //
	16,		//
	10,		// direct
	10,
	10,
	22,		//
	0,
	5,
	4,
	0,
	1,		//
	1,
	1,
	19,		//

	9,						// 1
	7,
	13,		//
	17,		//
	10,		//
	11,
	11,
	23,		//
	0,
	3,
	4,		//
	0,      //
	1, 		//
	2,
	2,
	20,		//

	1,				   		// 2
	6,
	19,		//
	16,		//
	10,
	10,
	10,
	22, 	//
	0,
	5,
	4,
	0,		//
	1,
	1,
	1,
	19,		//

	9,						// 3
	7,
	13,		//
	17,		//
	11,		//
	11,
	11,
	23,		//
	0,
	3,
	4,		//
	0,  	//
	2,		//
	2,
	2,
	20,		//

	0,						// 4
	6,
	0,		//
	16,		//
	18,		//
	10,
	10,
	22,		//
	0,
	5,
	4,
	0,		//
	1,
	1,
	1,
	19,

	9,						// 5
	7,
	13,		//
	17,		//
	18,		//
	11,
	11,
	23,		//
	0,
	3,
	0,		//
	0,		//
	19,		//
	2,
	2,
	20,		//

	0,						// 6
	6,
	15,		//
	16,		//
	10,		//
	10,
	10,
	22,		//
	0,
	5,
	4,
	0,		//
	8,
	1,
	1,
	19,		//

	9,						// 7
	7,
	13,		//
	17,		//
	11,		//
	11,
	11,
	23,		//
	0,
	3,
	0,		//
	0,		//
	14, 	//
	2,
	2,
	20,		//
// 8
	9,		//
	6,
	15,		//
	16,		//
	10,
	10,
	10,
	22,		//
	0,
	5,		//
	0,
	0,		//
	1,
	1,
	1,
	19,		//

	9,						// 9
	7,
	13, 	//
	17,		//
	11,
	11,
	12,
	23,		//
	0,
	3,
	0,
	0,		//
	1,		//
	2,
	2,		//
	20,		//

	5,						// a
	6,
	5,
	16,		//
	10,
	10,
	10,
	22,		//
	0,
	5,
	0,
	0,		//
	1,
	1,
	1,
	19,

	9,						// b
	7,
	13,		//
	17,		//
	11,
	11,
	12,
	23,		//
	0,
	3,
	0,
	0,		//
	2,
	2,
	3,
	20,		//

	5,						// c
	6,
	5,		//
	16,		//
	10,
	10,
	10,
	22, 	//
	0,
	5,
	0,
	0,      //
	1,
	1,
	1,
	19,		//

	9,						// d
	7,
	13,		//
	17,		//
	13,		//
	11,
	11,
	23,		//
	0,
	3,
	0,		//
	0,      //
	21,		//
	2,
	2,
	20,		//

	5,					// e
	6,
	5,		//
	16,		//
	10,
	10,
	10,
	22,		//
	0,
	5,
	0,
	0,		//
	1,
	1,
	1,
	19,		//

	9,					// f
	7,
	13,		//
	17,		//
	1,      //
	11,
	11,
	23,		//
	0,
	3,
	0,		//
	0,      //
	14,		//
	2,
	2,
	20		//
};

//=============================================================================

unsigned char
opSizeTable[25] =
{
	1,						 	//	0	none
	3,						 	//	1	nn		absolute
	3,						 	//	2	nn,X	absolute X
	3,						 	//	3	nn,Y	absolute Y
	1,						 	//	4	A		accumulator
	2,						 	//	5	#n		immediate
	2,						 	//	6	(n,X)	ind x
	2,						 	//	7	(n),Y	ind y
	3,						 	//	8	(nn)	indirect
	2,						 	//	9	n		relative
	2,						 	//	10	n		zero page
	2,						 	//	11	n,X		zero page X
	2,						 	//	12	n,Y		zero page Y
	2,							//  13  (n)     zero page indirect
	3,							//  14  (nn,X)	absolute indexed indirect
	3,							//  15  nn		pc relative long(16 bit)
	2,							//  16  n,S		stack relative
	2,							//  17  (n,S),Y	stack relative indirect indexed Y
	3,							//  18  n,n     block move
	4,							//  19  nnn		absolute long
	4,							//  20  nnn,X	absolute long indexed X
	3,							//  21  [nn]	absolute indirect long
	2,							//  22  [n]		direct page indirect long
	2,							//  23  [n],Y	direct page indirect long indexed Y
	2							//  24		    interrupt modes, skip byte
};

//=============================================================================

unsigned char
op5AddrTable[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,          // 0
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,         // a
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,			// c
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//=============================================================================
