
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary
  (4) Add 2 cycles if m=0 (16-bit memory/accumulator)
  (5) Add 1 cycle if branch is taken
  (6) Add 1 cycle if branch taken crosses page coundary on
      65816's 6502 emulation mode
  (7) Add 1 cycle for 65816 native mode (e=0)
  (8) Add 1 cycle if x=0 (16-bit index registers)




иммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммм╩
╨                      65816 Processor Help   	                ╨
гддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд╤
╨              Copyright 1991-1994 Developer Resources          ╨
╨                      All Rights Reserved.                     ╨
хммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммм╪

Assembler Syntax

	Immediates
	<	Lower word
	>	Upper word
	^	Bank


	Addressing (?)

	< forced direct-page addressing
	| forced 16-bit addressing
	> forced 24-bit addressing



ммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммм

ADC	Add With Carry

  ADC description

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              * *         * *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   ADC #const    69     2*  2(1)
  Absolute                    ADC addr      6D     3   4(1)
  Absolute Long               ADC long      6F     4   5(1)
  Direct Page (DP)            ADC dp        65     2   3(1,2)
  DP Indirect                 ADC (dp)      72     2   5(1,2)
  DP Indirect Long            ADC (dp)      67     2   6(1,2)
  Absolute Indexed,X          ADC addr,X    7D     3   4(1,3)
  Absolute Long Indexed,X     ADC long,X    7F     4   5(1)
  Absolute Indexed,Y          ADC addr,Y    79     3   4(1,3)
  DP Indirect,X               ADC dp,X      75     2   4(1,2)
  DP Indexed,X                ADC (dp,X)    61     2   6(1,2)
  DP Indexed Indirect,X       ADC (dp),Y    71     2   5(1,2,3)
  DP Indirect Indexed,Y       ADC [dp],Y    77     2   6(1,2)
  DP Indirect Long Indexed,Y  ADC sr,S      63     2   4(1)
  Stack Relative (SR)         ADC (sr,S),Y  73     2   7(1)
  SR Indirect Indexed,Y
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


AND:	And Accumulator with Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   AND #const    29     2*  2(1)
  Absolute                    AND addr      2D     3   4(1)
  Absolute Long               AND long      2F     4   5(1)
  Direct Page (DP)            AND dp        25     2   3(1,2)
  DP Indirect                 AND (dp)      32     2   5(1,2)
  DP Indirect Long            AND (dp)      27     2   6(1,2)
  Absolute Indexed,X          AND addr,X    3D     3   4(1,3)
  Absolute Long Indexed,X     AND long,X    3F     4   5(1)
  Absolute Indexed,Y          AND addr,Y    39     3   4(1,3)
  DP Indirect,X               AND dp,X      35     2   4(1,2)
  DP Indexed,X                AND (dp,X)    21     2   6(1,2)
  DP Indexed Indirect,X       AND (dp),Y    31     2   5(1,2,3)
  DP Indirect Indexed,Y       AND [dp],Y    37     2   6(1,2)
  DP Indirect Long Indexed,Y  AND sr,S      23     2   4(1)
  Stack Relative (SR)         AND (sr,S),Y  33     2   7(1)
  SR Indirect Indexed,Y
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary
  (4) Add 1 cycle if 65C02 and d=1 (decimal mode, 65C02)


ASL	Shift Memory or Accumulator Left

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           * *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Accumulator                 ASL A         0A     1   2
  Absolute                    ASL addr      0E     3   6(4)
  Direct Page (DP)            ASL dp        06     2   5(4,2)
  Absolute Indexed,X          ASL addr,X    1E     3   7(4)
  DP Indexed,X                ASL (dp,X)    16     2   6(4,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (4) Add 2 cycles if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


BCC     Branch if Carry Clear

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Program Counter Relative    BCC nearlabel 90     2   2(5,6)
			  (or BLT nearlabel)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (5) Add 1 cycle if branch is taken
  (6) Add 1 cycle if branch taken crosses page coundary on
      65816's 6502 emulation mode


BCS     Branch if Carry Set

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Program Counter Relative    BCS nearlabel B0     2   2(5,6)
			  (or BGE nearlabel)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (5) Add 1 cycle if branch is taken
  (6) Add 1 cycle if branch taken crosses page coundary on
      65816's 6502 emulation mode


BEQ	Branch if Equal

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Program Counter Relative    BCS nearlabel F0     2   2(5,6)
			  (or BGE nearlabel)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (5) Add 1 cycle if branch is taken
  (6) Add 1 cycle if branch taken crosses page coundary on
      65816's 6502 emulation mode


BIT	Test Memory Bits against Accumulator

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *   (Other than immediate)
					  *   (Immediate only)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   BIT #const    89     2*  2(1)
  Absolute                    BIT addr      2C     3   4(1)
  Direct Page (DP)            BIT dp        24     2   3(1,2)
  Absolute Indexed,X          BIT addr,X    3C     3   4(1,3)
  DP Indexed,X                BIT (dp,X)    34     2   4(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


BMI	Branch if Minus

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Program Counter Relative    BMI nearlabel 30     2   2(5,6)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (5) Add 1 cycle if branch is taken
  (6) Add 1 cycle if branch taken crosses page coundary on
      65816's 6502 emulation mode


BNE	Branch if Not Equal

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Program Counter Relative    BNE nearlabel D0     2   2(5,6)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (5) Add 1 cycle if branch is taken
  (6) Add 1 cycle if branch taken crosses page coundary on
      65816's 6502 emulation mode


BPL	Branch if Plus

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Program Counter Relative    BPL nearlabel 10     2   2(5,6)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (5) Add 1 cycle if branch is taken
  (6) Add 1 cycle if branch taken crosses page coundary on
      65816's 6502 emulation mode


BRA	Branch Always

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Program Counter Relative    BRA nearlabel 80     2   2(5,6)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (5) Add 1 cycle if branch is taken
  (6) Add 1 cycle if branch taken crosses page coundary on
      65816's 6502 emulation mode


BRK	Software Break

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
  !!!
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack / Interrupt           BRK           00     2*  7(7)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) BRK is 1 byte, but program value pushed onto stack is
      incremented by 2 allowing for optional signature
  (7) Add 1 cycle for 65816 native mode (e=0)


BRL	Branch Always Long

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Program Counter Relative    BRL farlabel  82     3   4
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


BVC	Branch if Overflow Clear

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Program Counter Relative    BVS nearlabel 50     2   2(5,6)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (5) Add 1 cycle if branch is taken
  (6) Add 1 cycle if branch taken crosses page coundary on
      65816's 6502 emulation mode


BVS	Branch if Overflow Set

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Program Counter Relative    BVS nearlabel 70     2   2(5,6)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (5) Add 1 cycle if branch is taken
  (6) Add 1 cycle if branch taken crosses page coundary on
      65816's 6502 emulation mode


CLC	Clear Carry Flag

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
					    c
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     CLC           18     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


CLD	Clear Decimal Mode Flag

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		                      *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     CLD           D8     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


CLI	Clear Interrupt Disable Flag

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		                        *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     CLI           58     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


CLV	Clear Overflow Flag

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		                *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     CLV           B8     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


CMP	Compare Accumulator with Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           * *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   CMP #const    C9     2*  2(1)
  Absolute                    CMP addr      CD     3   4(1)
  Absolute Long               CMP long      CF     4   5(1)
  Direct Page (DP)            CMP dp        C5     2   3(1,2)
  DP Indirect                 CMP (dp)      D2     2   5(1,2)
  DP Indirect Long            CMP [dp]      C7     2   6(1,2)
  Absolute Indexed,X          CMP addr,X    DD     3   4(1,3)
  Absolute Long Indexed,X     CMP long,X    DF     4   5(1)
  Absolute Indexed,Y          CMP addr,Y    D9     3   4(1,3)
  DP Indirect,X               CMP dp,X      D5     2   4(1,2)
  DP Indexed Indirect,X       CMP (dp,X)    C1     2   6(1,2)
  DP Indexed Indirect,Y       CMP (dp),Y    D1     2   5(1,2,3)
  DP Indirect Long Indexed,Y  CMP [dp],Y    D7     2   6(1,2)
  Stack Relative (SR)         CMP sr,S      C3     2   7(1)
  SR Indirect Indexed,Y       CMP (sr,S),Y  D3     2   7(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


COP	Co-Processor Enable

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		                      * *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack / Interrupt           COP const     02     2*  7(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)


CPX	Compare Index Register X with Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           * *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   CPX #const    E0     2*  2(8)
  Absolute                    CPX addr      EC     3   4(8)
  Direct Page (DP)            CPX dp        E4     2   3(8,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if x=0 (16-bit index registers)
  (8) Add 1 cycle if x=0 (16-bit index registers)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


CPY	Compare Index Register Y with Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           * *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   CPY #const    C0     2*  2(1)
  Absolute                    CPY addr      CC     3   4(1)
  Direct Page (DP)            CPY dp        C4     2   3(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


DEC	Decrement

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Accumulator                 DEC A         3A     1   2
  Absolute                    DEC addr      CE     3   6(1)
  Direct Page (DP)            DEC dp        C6     2   5(1,2)
  Absolute Indexed,X          DEC addr,X    DE     3   7(1,3)
  DP Indexed,X                DEC (dp,X)    D6     2   6(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


DEX	Decrement Index Register X

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     DEX           CA     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


DEY	Decrement Index Register Y

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     DEY           88     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


EOR	Exclusive-OR Accumulator with Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   EOR #const    49     2*  2(1)
  Absolute                    EOR addr      4D     3   4(1)
  Absolute Long               EOR long      4F     4   5(1)
  Direct Page (DP)            EOR dp        45     2   3(1,2)
  DP Indirect                 EOR (dp)      52     2   5(1,2)
  DP Indirect Long            EOR (dp)      47     2   6(1,2)
  Absolute Indexed,X          EOR addr,X    5D     3   4(1,3)
  Absolute Long Indexed,X     EOR long,X    5F     4   5(1)
  Absolute Indexed,Y          EOR addr,Y    59     3   4(1,3)
  DP Indirect,X               EOR dp,X      55     2   4(1,2)
  DP Indexed,X                EOR (dp,X)    41     2   6(1,2)
  DP Indexed Indirect,X       EOR (dp),Y    51     2   5(1,2,3)
  DP Indirect Indexed,Y       EOR [dp],Y    57     2   6(1,2)
  Stack Relative (SR)         EOR sr,S      43     2   4(1)
  SR Indirect Indexed,Y       EOR (sr,S),Y  53     2   7(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


INC	Increment

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Accumulator                 INC A         1A     1   2
  Absolute                    INC addr      EE     3   4(1)
  Direct Page (DP)            INC dp        E6     2   3(1,2)
  Absolute Indexed,X          INC addr,X    FE     3   4(1,3)
  DP Indexed,X                INC (dp,X)    F6     2   6(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


INX	Increment Index Register X

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     INX           E8     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


INY	Increment Index Register Y

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     INY           C8     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


JMP	Jump

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Absolute                    JMP addr      4C     3   3
  Absolute Indirect           JMP (addr)    6C     3   5
  Absolute Indexed Indirect   JMP (addr,X)  7C     3   6
  Absolute Long               JMP long      5C     4   4
			  (or JML long)
  Absolute Indirect Long      JMP [addr]    DC     3   6
			  (or JML [addr])
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


JSL	Jump to Subroutine Long (Inter-Bank)

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Absolute Long               JSR long      22     4   8
		          (or JSL long)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


JSR	Jump to Subroutine

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Absolute                    JSR addr      20     3   6
  Absolute Indexed Indirect   JSR (addr,X)  FC     3   8
  Absolute Long               JSR long      22     4   8
			  (or JSL long)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


LDA	Load Accumulator from Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   LDA #const    A9     2*  2(1)
  Absolute                    LDA addr      AD     3   4(1)
  Absolute Long               LDA long      AF     4   5(1)
  Direct Page (DP)            LDA dp        A5     2   3(1,2)
  DP Indirect                 LDA (dp)      B2     2   5(1,2)
  DP Indirect Long            LDA (dp)      A7     2   6(1,2)
  Absolute Indexed,X          LDA addr,X    BD     3   4(1,3)
  Absolute Long Indexed,X     LDA long,X    BF     4   5(1)
  Absolute Indexed,Y          LDA addr,Y    B9     3   4(1,3)
  DP Indirect,X               LDA dp,X      B5     2   4(1,2)
  DP Indexed,X                LDA (dp,X)    A1     2   6(1,2)
  DP Indexed Indirect,X       LDA (dp),Y    B1     2   5(1,2,3)
  DP Indirect Indexed,Y       LDA [dp],Y    B7     2   6(1,2)
  Stack Relative (SR)         LDA sr,S      A3     2   4(1)
  SR Indirect Indexed,Y       LDA (sr,S),Y  B3     2   7(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


LDX	Load Index Register X from Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   LDX #const    A2     2*  2(8)
  Absolute                    LDX addr      AE     3   4(8)
  Direct Page (DP)            LDX dp        A6     2   3(8,2)
  Absolute Indexed,X          LDX addr,X    BE     3   4(8,3)
  DP Indexed,Y                LDX (dp,X)    B6     2   6(8,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if x=0 (16-bit index registers)
  (1) Add 1 cycle if x=0 (16-bit index registers)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


LDY	Load Index Register Y from Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   LDY #const    A0     2*  2(8)
  Absolute                    LDY addr      AC     3   4(8)
  Direct Page (DP)            LDY dp        A4     2   3(8,2)
  Absolute Indexed,X          LDY addr,X    BC     3   4(8,3)
  DP Indexed,Y                LDY (dp,X)    B4     2   6(8,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if x=0 (16-bit index registers)
  (1) Add 1 cycle if x=0 (16-bit index registers)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


LSR	Logical Shift Memory or Accumulator Right

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           * *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Accumulator                 LSR A         4A     1   2
  Absolute                    LSR addr      4E     3   6(1)
  Direct Page (DP)            LSR dp        46     2   5(1,2)
  Absolute Indexed,X          LSR addr,X    5E     3   7(1)
  DP Indexed,X                LSR (dp,X)    56     2   6(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 2 cycles if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


MVN	Block Move Next

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Block Move                  MVN srcbk,    54     3   7(*)
				  destbk
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) 7 cycles per byte moved

  IMPORTANT NOTE:
	The Super Nintendo implementation of the MVN and MVP
	commands cause the data bank to be set to the bank of
	the (source or destination).


MVP	Block Move Previous

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Block Move                  MVP srcbk,    44     3   7(*)
				  dstbk
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) 7 cycles per byte moved

  IMPORTANT NOTE:
	The Super Nintendo implementation of the MVN and MVP
	commands cause the data bank to be set to the bank of
	the (source or destination).


NOP	No Operation

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     NOP           EA     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


ORA	OR Accumulator with Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   ORA #const    09     2*  2(1)
  Absolute                    ORA addr      0D     3   4(1)
  Absolute Long               ORA long      0F     4   5(1)
  Direct Page (DP)            ORA dp        05     2   3(1,2)
  DP Indirect                 ORA (dp)      12     2   5(1,2)
  DP Indirect Long            ORA (dp)      07     2   6(1,2)
  Absolute Indexed,X          ORA addr,X    1D     3   4(1,3)
  Absolute Long Indexed,X     ORA long,X    1F     4   5(1)
  Absolute Indexed,Y          ORA addr,Y    19     3   4(1,3)
  DP Indirect,X               ORA dp,X      15     2   4(1,2)
  DP Indexed,X                ORA (dp,X)    01     2   6(1,2)
  DP Indexed Indirect,X       ORA (dp),Y    11     2   5(1,2,3)
  DP Indirect Indexed,Y       ORA [dp],Y    17     2   6(1,2)
  Stack Relative (SR)         ORA sr,S      03     2   4(1)
  SR Indirect Indexed,Y       ORA (sr,S),Y  13     2   7(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


PEA	Push Effective Absolute Address

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Absolute)            PEA addr      F4     3   5
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


PEI	Push Effective Indirect Address

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack                       PEI (dp)      D4     2   6(1)
   (Direct Page Indirect)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if low byte of Direct Page is other than zero (DL<>0)


PER	Push Effective PC Relative Indirect Address

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  PER (Program Counter        PER label     62     3   6
   Relative Long)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


PHA	Push Accumulator

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Push)                PHA           48     1   3(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)


PHB	Push Bank Register

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Push)                PHB           8B     1   3
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)


PHD	Push Direct Page Register

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Push)                PHD           0B     1   4
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


PHK	Push Program Bank Register

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Push)                PHA           4B     1   3(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)


PHP	Push Processor Status Register

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Push)                PHP           08     1   3
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


PHX	Push Index Register

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Push)                PHX           DA     1   3(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if x=0 (16-bit index registers)


PHY	Push Index Register

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Push)                PHY           5A     1   3(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if x=0 (16-bit index registers)


PLA	Pull Accumulator

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
			      *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Pull)                PLA           68     1   4(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)


PLB	Pull Data Bank Register

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
			      *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Pull)                PLB           AB     1   4
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


PLD	Pull Direct Page Register

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
			      *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Pull)                PLD           2B     1   5
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


PLP	Pull Status Flags

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
			      *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Pull)                PLP           28     1   4
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


PLX	Pull Register X from Stack

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
			      *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Pull)                PLX           FA     1   4(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if x=0 (16-bit index registers)


PLY	Pull Register Y from Stack

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
			      *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (Pull)                PLY           7A     1   4(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if x=0 (16-bit index registers)


REP	Reset Status Bits

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *       * * * * (emulation e=1)
			      * * * * * * * * (emulation e=0)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   REP #const    C2     2   3
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


ROL	Rotate Memory or Accumulator Left

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           * *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Accumulator                 ROL A         2A     1   2
  Absolute                    ROL addr      2E     3   6(1)
  Direct Page (DP)            ROL dp        26     2   5(1,2)
  Absolute Indexed,X          ROL addr,X    3E     3   7(1)
  DP Indexed,X                ROL (dp,X)    36     2   6(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 2 cycles if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


ROR	Rotate Memory or Accumulator Right   			^^^text describing instruction and/or diagram

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           * *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Accumulator                 ROR A         6A     1   2
  Absolute                    ROR addr      6E     3   6(1)
  Direct Page (DP)            ROR dp        66     2   5(1,2)
  Absolute Indexed,X          ROR addr,X    7E     3   7(1)
  DP Indexed,X                ROR (dp,X)    76     2   6(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 2 cycles if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


RTI	Return from Interrupt

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (RTI)                 RTI           40     1   6(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle for 65816 native mode (e=0)


RTL	Return from Subroutine Long

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (RTL)                 RTL           6B     1   6
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


RTS	Return from Subroutine

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Stack (RTS)                 RTS           60     1   6
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


SBC	Subtract with Borrow from Accumulator

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              * *         * *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   SBC #const    E9     2*  2(1)
  Absolute                    SBC addr      ED     3   4(1)
  Absolute Long               SBC long      EF     4   5(1)
  Direct Page (DP)            SBC dp        E5     2   3(1,2)
  DP Indirect                 SBC (dp)      F2     2   5(1,2)
  DP Indirect Long            SBC (dp)      E7     2   6(1,2)
  Absolute Indexed,X          SBC addr,X    FD     3   4(1,3)
  Absolute Long Indexed,X     SBC long,X    FF     4   5(1)
  Absolute Indexed,Y          SBC addr,Y    F9     3   4(1,3)
  DP Indirect,X               SBC dp,X      F5     2   4(1,2)
  DP Indexed,X                SBC (dp,X)    E1     2   6(1,2)
  DP Indexed Indirect,X       SBC (dp),Y    F1     2   5(1,2,3)
  DP Indirect Indexed,Y       SBC [dp],Y    F7     2   6(1,2)
  Stack Relative (SR)         SBC sr,S      E3     2   4(1)
  SR Indirect Indexed,Y       SBC (sr,S),Y  F3     2   7(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Add 1 byte if m=0 (16-bit memory/accumulator)
  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)
  (3) Add 1 cycle if adding index crosses a page boundary


SEC	Set Carry Flag

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
					    *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     SEC           38     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


SEC	Set Carry Flag

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
					    *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     SEC           38     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


SEC	Set Carry Flag

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
					    *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     SEC           38     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


SEC	Set Carry Flag

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
					    *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     SEC           38     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


SED     Set Decimal Mode Flag

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		                      *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     SED           F8     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


SEI	Set Interrupt Disable Flag

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		                        *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     SEI           78     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


SEP     Set Status Bits

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              * *     * * * * (emulation mode e=1)
                              * * * * * * * * (emulation mode e=0)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Immediate                   SEP #const    E2     2   3
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


STA	Store Accumulator to Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Absolute                    STA addr      8D     3   4(1)
  Absolute Long               STA long      8F     4   5(1)
  Direct Page (DP)            STA dp        85     2   3(1,2)
  DP Indirect                 STA (dp)      92     2   5(1,2)
  DP Indirect Long            STA (dp)      87     2   6(1,3)
  Absolute Indexed,X          STA addr,X    9D     3   5(1)
  Absolute Long Indexed,X     STA long,X    9F     4   5(1)
  Absolute Indexed,Y          STA addr,Y    99     3   5(1)
  DP Indirect,X               STA dp,X      95     2   4(1,2)
  DP Indexed,X                STA (dp,X)    81     2   6(1,2)
  DP Indexed Indirect,X       STA (dp),Y    91     2   6(1,2)
  DP Indirect Indexed,Y       STA [dp],Y    97     2   6(1,2)
  Stack Relative (SR)         STA sr,S      83     2   4(1)
  SR Indirect Indexed,Y       STA (sr,S),Y  93     2   7(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


STP	Stop the Processor

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     STP           DB     1   3(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Uses 3 cycles to shut the processor down; additional
      cycles are required by reset to restart it


STX	Store Index Register X to Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Absolute                    STX addr      8E     3   4(1)
  Direct Page (DP)            STX dp        86     2   2(1,2)
  DP Indexed,Y                STX dp,Y      96     2   4(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


STY	Store Index Register Y to Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Absolute                    STY addr      8C     3   4(1)
  Direct Page (DP)            STY dp        84     2   3(1,2)
  DP Indexed,X                STY dp,X      94     2   4(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if x=0 (16-bit index register)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


STZ	Store Zero to Memory

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Absolute                    STZ addr      9C     3   4(1)
  Direct Page (DP)            STZ dp        64     2   3(1,2)
  Absolute Indexed,X          STZ addr,X    9E	   3   5(1)
  DP Indexed,Y                STZ dp,Y      74     2   4(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


TAX	Transfer Accumulator to Index Register X

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TAX           AA     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TAY	Transfer Accumulator to Index Register Y

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TAY           A8     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TCD	Transfer 16-Bit Accumulator to Direct Page Register

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TAX           5B     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TCS	Transfer Accumulator to Stack Pointer

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TXS           1B     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TCD	Transfer Direct Page Register to 16-Bit Accumulator

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TDC           7B     1   2
                          (or TDA)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TRB	Test and Reset Memory Bits Against Accumulator

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		                          *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TRB addr      1C     3   6(1)
  Direct Page                 TRB dp        14     2   5(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 2 cycles if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


TSB	Test and Set Memory Bits Against Accumulator

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		                          *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TSB addr      0C     3   6(1)
  Direct Page                 TSB dp        04     2   5(1,2)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Add 2 cycles if m=0 (16-bit memory/accumulator)
  (2) Add 1 cycle if low byte of Direct Page register
      is other than zero (DL<>0)


TSC	Transfer Stack Pointer to 16-Bit Accumulator

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TSC           3B     1   2
                          (or TSA)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TSX	Transfer Stack Pointer to Index Register X

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TSX           BA     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TXA	Transfer Index Register X to Accumulator

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TXA           8A     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TXS	Transfer Index Register X to Stack Pointer

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TXS           9A     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TXY	Transfer Index Register X to Y

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TXY           9B     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TYA	Transfer Index Register Y to Accumulator

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TYA           98     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


TYX	Transfer Index Registers Y to X

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     TYX           BB     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


WAI	Wait for Interrupt

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     WAI           CB     1   3(1)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (1) Uses 3 cycles to shut the processor down, additional
      cycles are required by interrupt to restart it


WDM	Reserved for Future Expansion      y

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
                              WDM           42     2*  0(*)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд

  (*) Byte and cycle count subject to change in future
      processors which expand WDM into 2-byte opcode portions
      of instructions of varying lengths


XBA	Exchange the B and A Accumulators

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		              *           *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     XBA           EB     1   3
                          (or SWA)
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд


XCE	Exchange Carry and Emulation Bits

  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Flags Affected              N V M X D I Z C
		                  * /       *
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Addressing Mode             Syntax      Opcode  # of  # of
					   (Hex) Bytes Cycles
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
  Implied                     XCE           FB     1   2
  ддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
