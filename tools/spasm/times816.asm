; times816.asm -- 65816 opcode execution timings

	IF 0
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

	ENDIF

DATA	SEGMENT	'DATA'
opcode816Times LABEL BYTE
	db 7	;   Stack / Interrupt           BRK           00     2*  7(7)
	db 6	;   DP Indexed,X                ORA (dp,X)    01     2   6(1,2)
	db 7	;   Stack / Interrupt           COP const     02     2*  7(1)
	db 4	;   Stack Relative (SR)         ORA sr,S      03     2   4(1)
	db 5	;   Direct Page                 TSB dp        04     2   5(1,2)
	db 3	;   Direct Page (DP)            ORA dp        05     2   3(1,2)
	db 5	;   Direct Page (DP)            ASL dp        06     2   5(4,2)
	db 6	;   DP Indirect Long            ORA (dp)      07     2   6(1,2)
	db 3	;   Stack (Push)                PHP           08     1   3
	db 2	;   Immediate                   ORA #const    09     2*  2(1)
	db 2	;   Accumulator                 ASL A         0A     1   2
	db 4	;   Stack (Push)                PHD           0B     1   4
	db 6	;   Implied                     TSB addr      0C     3   6(1)
	db 4	;   Absolute                    ORA addr      0D     3   4(1)
	db 6	;   Absolute                    ASL addr      0E     3   6(4)
	db 5	;   Absolute Long               ORA long      0F     4   5(1)
	db 2	;   Program Counter Relative    BPL nearlabel 10     2   2(5,6)
	db 5	;   DP Indexed Indirect,X       ORA (dp),Y    11     2   5(1,2,3)
	db 5	;   DP Indirect                 ORA (dp)      12     2   5(1,2)
	db 7	;   SR Indirect Indexed,Y       ORA (sr,S),Y  13     2   7(1)
	db 5	;   Direct Page                 TRB dp        14     2   5(1,2)
	db 4	;   DP Indirect,X               ORA dp,X      15     2   4(1,2)
	db 6	;   DP Indexed,X                ASL (dp,X)    16     2   6(4,2)
	db 6	;   DP Indirect Indexed,Y       ORA [dp],Y    17     2   6(1,2)
	db 2	;   Implied                     CLC           18     1   2
	db 4	;   Absolute Indexed,Y          ORA addr,Y    19     3   4(1,3)
	db 2	;   Accumulator                 INC A         1A     1   2
	db 2	;   Implied                     TXS           1B     1   2
	db 6	;   Implied                     TRB addr      1C     3   6(1)
	db 4	;   Absolute Indexed,X          ORA addr,X    1D     3   4(1,3)
	db 7	;   Absolute Indexed,X          ASL addr,X    1E     3   7(4)
	db 5	;   Absolute Long Indexed,X     ORA long,X    1F     4   5(1)
	db 6	;   Absolute                    JSR addr      20     3   6
	db 6	;   DP Indexed,X                AND (dp,X)    21     2   6(1,2)
	db 8	;   Absolute Long               JSR long      22     4   8
	db 4	;   DP Indirect Long Indexed,Y  AND sr,S      23     2   4(1)
	db 3	;   Direct Page (DP)            BIT dp        24     2   3(1,2)
	db 3	;   Direct Page (DP)            AND dp        25     2   3(1,2)
	db 5	;   Direct Page (DP)            ROL dp        26     2   5(1,2)
	db 6	;   DP Indirect Long            AND (dp)      27     2   6(1,2)
	db 4	;   Stack (Pull)                PLP           28     1   4
	db 2	;   Immediate                   AND #const    29     2*  2(1)
	db 2	;   Accumulator                 ROL A         2A     1   2
	db 5	;   Stack (Pull)                PLD           2B     1   5
	db 4	;   Absolute                    BIT addr      2C     3   4(1)
	db 4	;   Absolute                    AND addr      2D     3   4(1)
	db 6	;   Absolute                    ROL addr      2E     3   6(1)
	db 5	;   Absolute Long               AND long      2F     4   5(1)
	db 2	;   Program Counter Relative    BMI nearlabel 30     2   2(5,6)
	db 5	;   DP Indexed Indirect,X       AND (dp),Y    31     2   5(1,2,3)
	db 5	;   DP Indirect                 AND (dp)      32     2   5(1,2)
	db 7	;   Stack Relative (SR)         AND (sr,S),Y  33     2   7(1)
	db 4	;   DP Indexed,X                BIT (dp,X)    34     2   4(1,2)
	db 4	;   DP Indirect,X               AND dp,X      35     2   4(1,2)
	db 6	;   DP Indexed,X                ROL (dp,X)    36     2   6(1,2)
	db 6	;   DP Indirect Indexed,Y       AND [dp],Y    37     2   6(1,2)
	db 2	;   Implied                     SEC           38     1   2
	db 4	;   Absolute Indexed,Y          AND addr,Y    39     3   4(1,3)
	db 2	;   Accumulator                 DEC A         3A     1   2
	db 2	;   Implied                     TSC           3B     1   2
	db 4	;   Absolute Indexed,X          BIT addr,X    3C     3   4(1,3)
	db 4	;   Absolute Indexed,X          AND addr,X    3D     3   4(1,3)
	db 7	;   Absolute Indexed,X          ROL addr,X    3E     3   7(1)
	db 5	;   Absolute Long Indexed,X     AND long,X    3F     4   5(1)
	db 6	;   Stack (RTI)                 RTI           40     1   6(1)
	db 6	;   DP Indexed,X                EOR (dp,X)    41     2   6(1,2)
	db 0	;                               WDM           42     2*  0(*)
	db 4	;   Stack Relative (SR)         EOR sr,S      43     2   4(1)
	db 7	;   Block Move                  MVP srcbk,    44     3   7(*)
	db 3	;   Direct Page (DP)            EOR dp        45     2   3(1,2)
	db 5	;   Direct Page (DP)            LSR dp        46     2   5(1,2)
	db 6	;   DP Indirect Long            EOR (dp)      47     2   6(1,2)
	db 3	;   Stack (Push)                PHA           48     1   3(1)
	db 2	;   Immediate                   EOR #const    49     2*  2(1)
	db 2	;   Accumulator                 LSR A         4A     1   2
	db 3	;   Stack (Push)                PHA           4B     1   3(1)
	db 3	;   Absolute                    JMP addr      4C     3   3
	db 4	;   Absolute                    EOR addr      4D     3   4(1)
	db 6	;   Absolute                    LSR addr      4E     3   6(1)
	db 5	;   Absolute Long               EOR long      4F     4   5(1)
	db 2	;   Program Counter Relative    BVS nearlabel 50     2   2(5,6)
	db 5	;   DP Indexed Indirect,X       EOR (dp),Y    51     2   5(1,2,3)
	db 5	;   DP Indirect                 EOR (dp)      52     2   5(1,2)
	db 7	;   SR Indirect Indexed,Y       EOR (sr,S),Y  53     2   7(1)
	db 7	;   Block Move                  MVN srcbk,    54     3   7(*)
	db 4	;   DP Indirect,X               EOR dp,X      55     2   4(1,2)
	db 6	;   DP Indexed,X                LSR (dp,X)    56     2   6(1,2)
	db 6	;   DP Indirect Indexed,Y       EOR [dp],Y    57     2   6(1,2)
	db 2	;   Implied                     CLI           58     1   2
	db 4	;   Absolute Indexed,Y          EOR addr,Y    59     3   4(1,3)
	db 3	;   Stack (Push)                PHY           5A     1   3(1)
	db 2	;   Implied                     TAX           5B     1   2
	db 4	;   Absolute Long               JMP long      5C     4   4
	db 4	;   Absolute Indexed,X          EOR addr,X    5D     3   4(1,3)
	db 7	;   Absolute Indexed,X          LSR addr,X    5E     3   7(1)
	db 5	;   Absolute Long Indexed,X     EOR long,X    5F     4   5(1)
	db 6	;   Stack (RTS)                 RTS           60     1   6
	db 6	;   DP Indexed,X                ADC (dp,X)    61     2   6(1,2)
	db 6	;   PER (Program Counter        PER label     62     3   6
	db 4	;   DP Indirect Long Indexed,Y  ADC sr,S      63     2   4(1)
	db 3	;   Direct Page (DP)            STZ dp        64     2   3(1,2)
	db 3	;   Direct Page (DP)            ADC dp        65     2   3(1,2)
	db 5	;   Direct Page (DP)            ROR dp        66     2   5(1,2)
	db 6	;   DP Indirect Long            ADC (dp)      67     2   6(1,2)
	db 4	;   Stack (Pull)                PLA           68     1   4(1)
	db 2	;   Immediate                   ADC #const    69     2*  2(1)
	db 2	;   Accumulator                 ROR A         6A     1   2
	db 6	;   Stack (RTL)                 RTL           6B     1   6
	db 5	;   Absolute Indirect           JMP (addr)    6C     3   5
	db 4	;   Absolute                    ADC addr      6D     3   4(1)
	db 6	;   Absolute                    ROR addr      6E     3   6(1)
	db 5	;   Absolute Long               ADC long      6F     4   5(1)
	db 2	;   Program Counter Relative    BVS nearlabel 70     2   2(5,6)
	db 5	;   DP Indexed Indirect,X       ADC (dp),Y    71     2   5(1,2,3)
	db 5	;   DP Indirect                 ADC (dp)      72     2   5(1,2)
	db 7	;   Stack Relative (SR)         ADC (sr,S),Y  73     2   7(1)
	db 4	;   DP Indexed,Y                STZ dp,Y      74     2   4(1,2)
	db 4	;   DP Indirect,X               ADC dp,X      75     2   4(1,2)
	db 6	;   DP Indexed,X                ROR (dp,X)    76     2   6(1,2)
	db 6	;   DP Indirect Indexed,Y       ADC [dp],Y    77     2   6(1,2)
	db 2	;   Implied                     SEI           78     1   2
	db 4	;   Absolute Indexed,Y          ADC addr,Y    79     3   4(1,3)
	db 4	;   Stack (Pull)                PLY           7A     1   4(1)
	db 2	;   Implied                     TDC           7B     1   2
	db 6	;   Absolute Indexed Indirect   JMP (addr,X)  7C     3   6
	db 4	;   Absolute Indexed,X          ADC addr,X    7D     3   4(1,3)
	db 7	;   Absolute Indexed,X          ROR addr,X    7E     3   7(1)
	db 5	;   Absolute Long Indexed,X     ADC long,X    7F     4   5(1)
	db 2	;   Program Counter Relative    BRA nearlabel 80     2   2(5,6)
	db 6	;   DP Indexed,X                STA (dp,X)    81     2   6(1,2)
	db 4	;   Program Counter Relative    BRL farlabel  82     3   4
	db 4	;   Stack Relative (SR)         STA sr,S      83     2   4(1)
	db 3	;   Direct Page (DP)            STY dp        84     2   3(1,2)
	db 3	;   Direct Page (DP)            STA dp        85     2   3(1,2)
	db 2	;   Direct Page (DP)            STX dp        86     2   2(1,2)
	db 6	;   DP Indirect Long            STA (dp)      87     2   6(1,3)
	db 2	;   Implied                     DEY           88     1   2
	db 2	;   Immediate                   BIT #const    89     2*  2(1)
	db 2	;   Implied                     TXA           8A     1   2
	db 3	;   Stack (Push)                PHB           8B     1   3
	db 4	;   Absolute                    STY addr      8C     3   4(1)
	db 4	;   Absolute                    STA addr      8D     3   4(1)
	db 4	;   Absolute                    STX addr      8E     3   4(1)
	db 5	;   Absolute Long               STA long      8F     4   5(1)
	db 2	;   Program Counter Relative    BCC nearlabel 90     2   2(5,6)
	db 6	;   DP Indexed Indirect,X       STA (dp),Y    91     2   6(1,2)
	db 5	;   DP Indirect                 STA (dp)      92     2   5(1,2)
	db 7	;   SR Indirect Indexed,Y       STA (sr,S),Y  93     2   7(1)
	db 4	;   DP Indexed,X                STY dp,X      94     2   4(1,2)
	db 4	;   DP Indirect,X               STA dp,X      95     2   4(1,2)
	db 4	;   DP Indexed,Y                STX dp,Y      96     2   4(1,2)
	db 6	;   DP Indirect Indexed,Y       STA [dp],Y    97     2   6(1,2)
	db 2	;   Implied                     TYA           98     1   2
	db 5	;   Absolute Indexed,Y          STA addr,Y    99     3   5(1)
	db 2	;   Implied                     TXS           9A     1   2
	db 2	;   Implied                     TXY           9B     1   2
	db 4	;   Absolute                    STZ addr      9C     3   4(1)
	db 5	;   Absolute Indexed,X          STA addr,X    9D     3   5(1)
	db 5	;   Absolute Indexed,X          STZ addr,X    9E	   3   5(1)
	db 5	;   Absolute Long Indexed,X     STA long,X    9F     4   5(1)
	db 2	;   Immediate                   LDY #const    A0     2*  2(8)
	db 6	;   DP Indexed,X                LDA (dp,X)    A1     2   6(1,2)
	db 2	;   Immediate                   LDX #const    A2     2*  2(8)
	db 4	;   Stack Relative (SR)         LDA sr,S      A3     2   4(1)
	db 3	;   Direct Page (DP)            LDY dp        A4     2   3(8,2)
	db 3	;   Direct Page (DP)            LDA dp        A5     2   3(1,2)
	db 3	;   Direct Page (DP)            LDX dp        A6     2   3(8,2)
	db 6	;   DP Indirect Long            LDA (dp)      A7     2   6(1,2)
	db 2	;   Implied                     TAY           A8     1   2
	db 2	;   Immediate                   LDA #const    A9     2*  2(1)
	db 2	;   Implied                     TAX           AA     1   2
	db 4	;   Stack (Pull)                PLB           AB     1   4
	db 4	;   Absolute                    LDY addr      AC     3   4(8)
	db 4	;   Absolute                    LDA addr      AD     3   4(1)
	db 4	;   Absolute                    LDX addr      AE     3   4(8)
	db 5	;   Absolute Long               LDA long      AF     4   5(1)
	db 2	;   Program Counter Relative    BCS nearlabel B0     2   2(5,6)
	db 5	;   DP Indexed Indirect,X       LDA (dp),Y    B1     2   5(1,2,3)
	db 5	;   DP Indirect                 LDA (dp)      B2     2   5(1,2)
	db 7	;   SR Indirect Indexed,Y       LDA (sr,S),Y  B3     2   7(1)
	db 6	;   DP Indexed,Y                LDY (dp,X)    B4     2   6(8,2)
	db 4	;   DP Indirect,X               LDA dp,X      B5     2   4(1,2)
	db 6	;   DP Indexed,Y                LDX (dp,X)    B6     2   6(8,2)
	db 6	;   DP Indirect Indexed,Y       LDA [dp],Y    B7     2   6(1,2)
	db 2	;   Implied                     CLV           B8     1   2
	db 4	;   Absolute Indexed,Y          LDA addr,Y    B9     3   4(1,3)
	db 2	;   Implied                     TSX           BA     1   2
	db 2	;   Implied                     TYX           BB     1   2
	db 4	;   Absolute Indexed,X          LDY addr,X    BC     3   4(8,3)
	db 4	;   Absolute Indexed,X          LDA addr,X    BD     3   4(1,3)
	db 4	;   Absolute Indexed,X          LDX addr,X    BE     3   4(8,3)
	db 5	;   Absolute Long Indexed,X     LDA long,X    BF     4   5(1)
	db 2	;   Immediate                   CPY #const    C0     2*  2(1)
	db 6	;   DP Indexed Indirect,X       CMP (dp,X)    C1     2   6(1,2)
	db 3	;   Immediate                   REP #const    C2     2   3
	db 7	;   Stack Relative (SR)         CMP sr,S      C3     2   7(1)
	db 3	;   Direct Page (DP)            CPY dp        C4     2   3(1,2)
	db 3	;   Direct Page (DP)            CMP dp        C5     2   3(1,2)
	db 5	;   Direct Page (DP)            DEC dp        C6     2   5(1,2)
	db 6	;   DP Indirect Long            CMP [dp]      C7     2   6(1,2)
	db 2	;   Implied                     INY           C8     1   2
	db 2	;   Immediate                   CMP #const    C9     2*  2(1)
	db 2	;   Implied                     DEX           CA     1   2
	db 3	;   Implied                     WAI           CB     1   3(1)
	db 4	;   Absolute                    CPY addr      CC     3   4(1)
	db 4	;   Absolute                    CMP addr      CD     3   4(1)
	db 6	;   Absolute                    DEC addr      CE     3   6(1)
	db 5	;   Absolute Long               CMP long      CF     4   5(1)
	db 2	;   Program Counter Relative    BNE nearlabel D0     2   2(5,6)
	db 5	;   DP Indexed Indirect,Y       CMP (dp),Y    D1     2   5(1,2,3)
	db 5	;   DP Indirect                 CMP (dp)      D2     2   5(1,2)
	db 7	;   SR Indirect Indexed,Y       CMP (sr,S),Y  D3     2   7(1)
	db 6	;   Stack                       PEI (dp)      D4     2   6(1)
	db 4	;   DP Indirect,X               CMP dp,X      D5     2   4(1,2)
	db 6	;   DP Indexed,X                DEC (dp,X)    D6     2   6(1,2)
	db 6	;   DP Indirect Long Indexed,Y  CMP [dp],Y    D7     2   6(1,2)
	db 2	;   Implied                     CLD           D8     1   2
	db 4	;   Absolute Indexed,Y          CMP addr,Y    D9     3   4(1,3)
	db 3	;   Stack (Push)                PHX           DA     1   3(1)
	db 3	;   Implied                     STP           DB     1   3(1)
	db 6	;   Absolute Indirect Long      JMP [addr]    DC     3   6
	db 4	;   Absolute Indexed,X          CMP addr,X    DD     3   4(1,3)
	db 7	;   Absolute Indexed,X          DEC addr,X    DE     3   7(1,3)
	db 5	;   Absolute Long Indexed,X     CMP long,X    DF     4   5(1)
	db 2	;   Immediate                   CPX #const    E0     2*  2(8)
	db 6	;   DP Indexed,X                SBC (dp,X)    E1     2   6(1,2)
	db 3	;   Immediate                   SEP #const    E2     2   3
	db 4	;   Stack Relative (SR)         SBC sr,S      E3     2   4(1)
	db 3	;   Direct Page (DP)            CPX dp        E4     2   3(8,2)
	db 3	;   Direct Page (DP)            SBC dp        E5     2   3(1,2)
	db 3	;   Direct Page (DP)            INC dp        E6     2   3(1,2)
	db 6	;   DP Indirect Long            SBC (dp)      E7     2   6(1,2)
	db 2	;   Implied                     INX           E8     1   2
	db 2	;   Immediate                   SBC #const    E9     2*  2(1)
	db 2	;   Implied                     NOP           EA     1   2
	db 3	;   Implied                     XBA           EB     1   3
	db 4	;   Absolute                    CPX addr      EC     3   4(8)
	db 4	;   Absolute                    SBC addr      ED     3   4(1)
	db 4	;   Absolute                    INC addr      EE     3   4(1)
	db 5	;   Absolute Long               SBC long      EF     4   5(1)
	db 2	;   Program Counter Relative    BCS nearlabel F0     2   2(5,6)
	db 5	;   DP Indexed Indirect,X       SBC (dp),Y    F1     2   5(1,2,3)
	db 5	;   DP Indirect                 SBC (dp)      F2     2   5(1,2)
	db 7	;   SR Indirect Indexed,Y       SBC (sr,S),Y  F3     2   7(1)
	db 5	;   Stack (Absolute)            PEA addr      F4     3   5
	db 4	;   DP Indirect,X               SBC dp,X      F5     2   4(1,2)
	db 6	;   DP Indexed,X                INC (dp,X)    F6     2   6(1,2)
	db 6	;   DP Indirect Indexed,Y       SBC [dp],Y    F7     2   6(1,2)
	db 2	;   Implied                     SED           F8     1   2
	db 4	;   Absolute Indexed,Y          SBC addr,Y    F9     3   4(1,3)
	db 4	;   Stack (Pull)                PLX           FA     1   4(1)
	db 2	;   Implied                     XCE           FB     1   2
	db 8	;   Absolute Indexed Indirect   JSR (addr,X)  FC     3   8
	db 4	;   Absolute Indexed,X          SBC addr,X    FD     3   4(1,3)
	db 4	;   Absolute Indexed,X          INC addr,X    FE     3   4(1,3)
	db 5	;   Absolute Long Indexed,X     SBC long,X    FF     4   5(1)

opcode816TimesFlags LABEL BYTE
	db 0 OR (1 SHL (7-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (4-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (4-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (5-1)) OR (1 SHL (6-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (4-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (4-1))
	db 0 OR (1 SHL (1-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (5-1)) OR (1 SHL (6-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (5-1)) OR (1 SHL (6-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1))
	db 0
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0
	db 0
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (5-1)) OR (1 SHL (6-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1))
	db 0
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (5-1)) OR (1 SHL (6-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (5-1)) OR (1 SHL (6-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (8-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (8-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (8-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (8-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0
	db 0
	db 0 OR (1 SHL (8-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (8-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (5-1)) OR (1 SHL (6-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (8-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (8-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0
	db 0
	db 0 OR (1 SHL (8-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (8-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (5-1)) OR (1 SHL (6-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (8-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (8-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1))
	db 0
	db 0
	db 0 OR (1 SHL (8-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (1-1))
	db 0 OR (1 SHL (5-1)) OR (1 SHL (6-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (2-1))
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1))
	db 0
	db 0
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1)) OR (1 SHL (3-1))
	db 0 OR (1 SHL (1-1))




DATA	ENDS


.CODE
CalculateOpcodeTiming:
	mov	ch,al
	lea	bx,opcode816Times
	xlatb
	mov	cl,al
	mov	al,ch
	lea	bx,opcode816TimesFlags
	xlatb
	test	al,1	;  (1) Add 1 cycle if m=0 (16-bit memory/accumulator)
	jz	@@Not16A
	test	sdb1,80h
	jz	@@Not16A
	inc	cl
@@Not16A:

	test	al,2	;  (2) Add 1 cycle if low byte of Direct Page register is other than zero (DL<>0)
	jz	@@Not_DP
	cmp	directAssume,0
	je	@@Not_DP
	inc	cl
@@Not_DP:

  	test	al,4	;  (3) Add 1 cycle if adding index crosses a page boundary

  	test	al,8	;  (4) Add 2 cycles if m=0 (16-bit memory/accumulator)
	jz	@@Not16A_2
	test	sdb1,80h
	jz	@@Not16A_2
	add	cl,2
@@Not16A_2:

	test	al,10h	;  (5) Add 1 cycle if branch is taken

;;	test	al,20h	;  (6) Add 1 cycle if branch taken crosses page coundary on
;;     			;	65816's 6502 emulation mode

	test	al,40h	;  (7) Add 1 cycle for 65816 native mode (e=0)
	jz	@@NotNative
	inc	cl
@@NotNative:

	test	al,80h	;  (8) Add 1 cycle if x=0 (16-bit index registers)
	jz	@@Not16X
	test	sdb1,40h
	jz	@@Not16X
	inc	cl
@@Not16X:

	mov	al,cl
	ret
