ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ  		 68000 Processor Help 		         บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ            (C) 1991, Developer Resources               บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ


ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

ABCD:	;Add Binary Coded Decimal

  Syntax:  ABCD Dy,Dx
           ABCD -(Ay),-(Ax)

  Size:  (Byte)

  Adds the source operand to the destination operand
  along with the extend bit, and stores the result in
  the destination location.  The addition is performed
  using binary coded decimal arithmetic.  The operands
  are packed BCD numbers.

  Cycle Chart:

  ษอออออออออออออออออออออป
  บ       Clocks        บ
  วฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤถ
  บ  Dy,Dx  ณ-(Ay),-(Ax)บ
  วฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤถ
  บ   B6    ณ    B18    บ
  ศอออออออออฯอออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ U ณ * ณ U ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

ADD:	;ADD
ADDA:	;ADD to Address register

  Syntax:  ADD <ea>,Dn
           ADD Dn,<ea>
           ADD <ea>,An ;a.k.a."ADDA" -- doesn't set flags

  Size:  ADD  = (Byte, Word, Long)
         ADDA = (Word, Long)

  Adds the source operand to the destination operand
  using binary addition, and stores the result in the
  destination location.

  Cycle Chart:  (See also: "ADDI")

  ษอออออออออออออัออออออออออออออออออออออออออออออออออป
  บ  Effective  ณ              Clocks              บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  <ea>,Dn  ณ  Dn,<ea>  ณ <ea>,An  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ BW4   L8  ณ <ฤฤฤx ฺฤ> ณ W8   L8  บ
  บ An          ณ  W4   L8  ณ     xฤู   ณ W8   L8  บ
  บ (An)        ณ BW8   L14 ณ BW12  L20 ณ W12  L14 บ
  บ (An)+       ณ BW8   L14 ณ BW12  L20 ณ W12  L14 บ
  บ -(An)       ณ BW10  L16 ณ BW14  L22 ณ W14  L16 บ
  บ di16(An)    ณ BW12  L18 ณ BW16  L24 ณ W16  L18 บ
  บ di8(An,Xn)  ณ BW14  L20 ณ BW18  L26 ณ W18  L20 บ
  บ addr.W      ณ BW12  L18 ณ BW16  L24 ณ W16  L18 บ
  บ addr.L      ณ BW16  L22 ณ BW20  L28 ณ W20  L22 บ
  บ di16(PC)    ณ BW12  L18 ณ    ---    ณ W16  L18 บ
  บ di8(PC,Xn)  ณ BW14  L20 ณ    ---    ณ W18  L20 บ
  บ #<data>     ณ See: ADDI ณ    ---    ณ W12  L16 บ
  ศอออออออออออออฯอออออออออออฯอออออออออออฯออออออออออผ

  Condition Codes:
  (except <ea>,An)

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

ADDI:	;ADD Immediate

  Syntax:  ADDI #<data>,<ea>  ;"ADD" may be subsituted
                              ; for "ADDI" with SPASM68K)

  Size:  (Byte, Word, Long)

  Adds the immediate data to the destination oper-
  and using binary addition, and stores the result
  in the destination location.

  Cycle Chart:  (See also: "ADD" & "ADDA")

  ษอออออออออออออัออออออออออออออป
  บ  Effective  ณ    Clocks    บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ #<data>,<ea> บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ  BW8    L20  บ
  บ An          ณ  See:  ADDA  บ
  บ (An)        ณ  BW16   L28  บ
  บ (An)+       ณ  BW16   L28  บ
  บ -(An)       ณ  BW18   L30  บ
  บ di16(An)    ณ  BW20   L32  บ
  บ di8(An,Xn)  ณ  BW22   L34  บ
  บ addr.W      ณ  BW20   L32  บ
  บ addr.L      ณ  BW24   L36  บ
  บ di16(PC)    ณ     ----     บ
  บ di8(PC,Xn)  ณ     ----     บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

ADDQ:	;ADD Quick

  Syntax:  ADDQ #<data>,<ea>

  Size:  (Byte, Word, Long)

  Adds the immediate value (range = 1 to 8) to the dest-
  ination operand using binary addition, and stores the
  result in the destination location.  If the destina-
  tion is an address register, then only Word or Long
  sizes are allowed and condition codes are not affected.

  Cycle Chart:

  ษอออออออออออออัออออออออออออออป
  บ  Effective  ณ    Clocks    บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ #<data>,<ea> บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ  BW4    L8   บ
  บ An          ณ   W8    L8   บ
  บ (An)        ณ  BW12   L20  บ
  บ (An)+       ณ  BW12   L20  บ
  บ -(An)       ณ  BW14   L22  บ
  บ di16(An)    ณ  BW16   L24  บ
  บ di8(An,Xn)  ณ  BW18   L26  บ
  บ addr.W      ณ  BW16   L24  บ
  บ addr.L      ณ  BW20   L28  บ
  บ di16(PC)    ณ     ----     บ
  บ di8(PC,Xn)  ณ     ----     บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:
  (except #<data>,An)

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

ADDX:	;ADD with eXtend

  Syntax:  ADDX Dy,Dx
           ADDX -(Ay),-(Ax)

  Size:  (Byte, Word, Long)

  Adds the source operand to the destination operand
  along with the extend bit, and stores the result in
  the destination location.

  Cycle Chart:

  ษอออออออออออออออออออออป
  บ       Clocks        บ
  วฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤถ
  บ  Dy,Dx  ณ-(Ay),-(Ax)บ
  วฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤถ
  บ BW4  L8 ณ BW18  L30 บ
  ศอออออออออฯอออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

AND:	;AND logical

  Syntax:  AND <ea>,Dn
           AND Dn,<ea>

  Size:  (Byte, Word, Long)

  ANDs the source operand with the destination operand
  and stores the result in the destination location.

  Cycle Chart:  (See also: "ANDI")

  ษอออออออออออออัอออออออออออออออออออออออป
  บ  Effective  ณ        Clocks         บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  <ea>,Dn  ณ  Dn,<ea>  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ BW4   L8  ณ <ฤฤฤx     บ
  บ An          ณ    ---    ณ    ---    บ
  บ (An)        ณ BW8   L14 ณ BW12  L20 บ
  บ (An)+       ณ BW8   L14 ณ BW12  L20 บ
  บ -(An)       ณ BW10  L16 ณ BW14  L22 บ
  บ di16(An)    ณ BW12  L18 ณ BW16  L24 บ
  บ di8(An,Xn)  ณ BW14  L20 ณ BW18  L26 บ
  บ addr.W      ณ BW12  L18 ณ BW16  L24 บ
  บ addr.L      ณ BW16  L22 ณ BW20  L28 บ
  บ di16(PC)    ณ BW12  L18 ณ    ---    บ
  บ di8(PC,Xn)  ณ BW14  L20 ณ    ---    บ
  บ #<data>     ณ See: ANDI ณ    ---    บ
  ศอออออออออออออฯอออออออออออฯอออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

ANDI:	;AND with Immediate

  Syntax:  ANDI #<data>,<ea>  ;"AND" may be subsituted
           ANDI #<data>,CCR   ; for "ANDI" with SPASM68K
           ANDI #<data>,SR

  Size:  #<data>,<ea> = (Byte, Word, Long)
         #<data>,CCR  = (Byte)
         #<data>,SR   = (Word)

  ANDs the immediate data to the destination operand
  and stores the result in the destination location.

  Cycle Chart:  (See also: "AND")

  ษอออออออออออออัออออออออออออออป
  บ  Effective  ณ    Clocks    บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ #<data>,<ea> บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ  BW8    L20  บ
  บ An          ณ     ----     บ
  บ (An)        ณ  BW16   L28  บ
  บ (An)+       ณ  BW16   L28  บ
  บ -(An)       ณ  BW18   L30  บ
  บ di16(An)    ณ  BW20   L32  บ
  บ di8(An,Xn)  ณ  BW22   L34  บ
  บ addr.W      ณ  BW20   L32  บ
  บ addr.L      ณ  BW24   L36  บ
  บ di16(PC)    ณ     ----     บ
  บ di8(PC,Xn)  ณ     ----     บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ CCR         ณ     B20      บ
  บ SR          ณ     W20      บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:

      #<data>,<ea>            #<data>,CCR

    X   N   Z   V   C       X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ   ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ   ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู   ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

ASL:	;Arithmetic Shift Left
ASR:	;Arithmetic Shift Right

  Syntax:  ASd Dx,Dy
           ASd #<data>,Dy
           ASd <ea>
           (where d is direction, L or R)

  Size:  (Byte, Word, Long)

  Arithmetically shifts the bits of the operand in the
  direction (L or R) specified.  The carry bit receives
  the last bit shifted out of the operand.  The shift
  count for the shifting of a register may be specified
  by a data register or an immediate value.  Data regis-
  ters can be shifted as a Byte, a Word, or a Long.  Mem-
  ory operands are restricted to a Word.

  Operation Diagrams:

    ASL:
    ฺฤฤฤฤฤฤฤฤฤฟ        ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฟ      ฺฤฤฤฤฤฤฤฤฤฟ
    ณ    C    ณ<ฤฤฤยฤฤฤด   OPERAND   ณ<ฤฤฤฤฤด    0    ณ
    ภฤฤฤฤฤฤฤฤฤู    ณ   ภฤฤฤฤฤฤฤฤฤฤฤฤฤู      ภฤฤฤฤฤฤฤฤฤู
    ฺฤฤฤฤฤฤฤฤฤฟ    ณ
    ณ    X    ณ<ฤฤฤู
    ภฤฤฤฤฤฤฤฤฤู

    ASR:
         ฺฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ            ฺฤฤฤฤฤฤฤฤฤฟ
    ฺฤฤฤ>ณ MSB ณ    OPERAND    รฤฤฤฤฤยฤฤฤฤฤ>ณ    C    ณ
    ณ    ภฤฤยฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู     ณ      ภฤฤฤฤฤฤฤฤฤู
    ภฤฤฤฤฤฤฤู                        ณ      ฺฤฤฤฤฤฤฤฤฤฟ
                                     ภฤฤฤฤฤ>ณ    X    ณ
                                            ภฤฤฤฤฤฤฤฤฤู

  Cycle Chart:

  ษออออออออออออออออออออออออออออป
  บ          Clocks            บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dx,Dy       ณ BW6+2s L8+2s บ
  บ #<data>,Dy  ณ BW6+2s L8+2s บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ (An)        ณ     W12      บ
  บ (An)+       ณ     W12      บ
  บ -(An)       ณ     W14      บ
  บ di16(An)    ณ     W16      บ
  บ di8(An,Xn)  ณ     W18      บ
  บ addr.W      ณ     W16      บ
  บ addr.L      ณ     W20      บ
  บ di16(PC)    ณ     ---      บ
  บ di8(PC,Xn)  ณ     ---      บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

BCC:	;Branch if Carry Clear
BCS:	;Branch if Carry Set
BEQ:	;Branch if EQual
BGE:	;Branch if Greater or Equal
BGT:	;Branch if Greater Than
BHI:	;Branch if High
BLE:	;Branch if Less or Equal
BLS:	;Branch if Low or Same
BLT:	;Branch if Less Than
BMI:	;Branch if MInus (negative)
BNE:	;Branch if Not Equal
BPL:	;Branch if PLus (positive)
BRA:	;BRanch Always
BVC:    ;Branch if oVerflow Clear
BVS:    :Branch if oVerflow Set

  Syntax:  Bcc <label>

  Size:  (Relative8, Relative16)

  If the specified condtion is true, program execution
  continues at location (PC)+displacement.  Branches
  using an 8-bit relative displacement are known as
  "short" branches.

  Cycle Chart:

  ษอออออออออออออัออออออออออออออออออออป
  บ  Effective  ณ       Clocks       บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤถ
  บ          ณ cc false ณ cc true บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤถ
  บ Relative8   ณ     8    ณ   10    บ
  บ Relative16  ณ    12    ณ   10    บ
  ศอออออออออออออฯออออออออออฯอออออออออผ

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

BCHG:	;Bit test and CHanGe

  Syntax:  BCHG Dn,<ea>
           BCHG #<data>,<ea>

  Size: (Byte, Long)

  Tests a bit in the destination operand and sets the Z
  condition code appropriately, then inverts the spec-
  ified bit in the destination.  When the destination
  is a data register, size is Long.  When the destina-
  tion is a memory location the operation is a Byte
  operation.

  Cycle Chart:

  ษอออออออออออออัออออออออออออออออออออออออป
  บ  Effective  ณ        Clocks          บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  Dn,<ea>  ณ#<data>,<ea>บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ    L8     ณ    L12     บ
  บ An          ณ    ---    ณ    ---     บ
  บ (An)        ณ    B12    ณ    B16     บ
  บ (An)+       ณ    B12    ณ    B16     บ
  บ -(An)       ณ    B14    ณ    B18     บ
  บ di16(An)    ณ    B16    ณ    B20     บ
  บ di8(An,Xn)  ณ    B18    ณ    B22     บ
  บ addr.W      ณ    B16    ณ    B20     บ
  บ addr.L      ณ    B20    ณ    B24     บ
  บ di16(PC)    ณ    ---    ณ    ---     บ
  บ di8(PC,Xn)  ณ    ---    ณ    ---     บ
  ศอออออออออออออฯอออออออออออฯออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ - ณ * ณ - ณ - ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

BCLR:	;Bit test and CLeaR

  Syntax:  BCLR Dn,<ea>
           BCLR #<data>,<ea>

  Size: (Byte, Long)

  Tests a bit in the destination operand and sets the Z
  condition code appropriately, then clears the spec-
  ified bit in the destination.  When the destination
  is a data register, size is Long.  When the destina-
  tion is a memory location the operation is a Byte
  operation.

  Cycle Chart:

  ษอออออออออออออัออออออออออออออออออออออออป
  บ  Effective  ณ        Clocks          บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  Dn,<ea>  ณ#<data>,<ea>บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ    L10    ณ    L14     บ
  บ An          ณ    ---    ณ    ---     บ
  บ (An)        ณ    B12    ณ    B16     บ
  บ (An)+       ณ    B12    ณ    B16     บ
  บ -(An)       ณ    B14    ณ    B18     บ
  บ di16(An)    ณ    B16    ณ    B20     บ
  บ di8(An,Xn)  ณ    B18    ณ    B22     บ
  บ addr.W      ณ    B16    ณ    B20     บ
  บ addr.L      ณ    B20    ณ    B24     บ
  บ di16(PC)    ณ    ---    ณ    ---     บ
  บ di8(PC,Xn)  ณ    ---    ณ    ---     บ
  ศอออออออออออออฯอออออออออออฯออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ - ณ * ณ - ณ - ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

BSET:	;Bit test and SET

  Syntax:  BSET Dn,<ea>
           BSET #<data>,<ea>

  Size: (Byte, Long)

  Tests a bit in the destination operand and sets the Z
  condition code appropriately, then sets the specified
  bit in the destination.  When the destination is a
  data register, size is Long.  When the destination is
  a memory location the operation is a Byte operation.

  Cycle Chart:

  ษอออออออออออออัออออออออออออออออออออออออป
  บ  Effective  ณ        Clocks          บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  Dn,<ea>  ณ#<data>,<ea>บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ    L8     ณ    L12     บ
  บ An          ณ    ---    ณ    ---     บ
  บ (An)        ณ    B12    ณ    B16     บ
  บ (An)+       ณ    B12    ณ    B16     บ
  บ -(An)       ณ    B14    ณ    B18     บ
  บ di16(An)    ณ    B16    ณ    B20     บ
  บ di8(An,Xn)  ณ    B18    ณ    B22     บ
  บ addr.W      ณ    B16    ณ    B20     บ
  บ addr.L      ณ    B20    ณ    B24     บ
  บ di16(PC)    ณ    ---    ณ    ---     บ
  บ di8(PC,Xn)  ณ    ---    ณ    ---     บ
  ศอออออออออออออฯอออออออออออฯออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ - ณ * ณ - ณ - ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

BSR:    ;Branch to SubRoutine

  Syntax:  BSR <label>

  Size:  (Relative8, Relative16)

  Pushes the long-word address of the instruction im-
  ediately following the BSR instruction onto the sys-
  tem stack.  Program execution then continues at lo-
  cation (PC) + displacement.

  Cycle Chart:

  ษอออออออออออออัออออออออป
  บ  Effective  ณ Clocks บ
  บ   Address   ณ    บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤถ
  บ Relative8   ณ   18   บ
  บ Relative16  ณ   18   บ
  ศอออออออออออออฯออออออออผ

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

BTST:	;Bit TeST

  Syntax:  BTST Dn,<ea>
           BTST #<data>,<ea>

  Size: (Byte, Long)

  Tests a bit in the destination operand and sets the Z
  condition code appropriately.  When the destination is
  a data register, size is Long.  When the destination
  is a memory location the operation is a Byte operation.

  Cycle Chart:

  ษอออออออออออออัออออออออออออออออออออออออป
  บ  Effective  ณ        Clocks          บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  Dn,<ea>  ณ#<data>,<ea>บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ    L6     ณ    L10     บ
  บ An          ณ    ---    ณ    ---     บ
  บ (An)        ณ    B8     ณ    B12     บ
  บ (An)+       ณ    B8     ณ    B12     บ
  บ -(An)       ณ    B10    ณ    B14     บ
  บ di16(An)    ณ    B12    ณ    B16     บ
  บ di8(An,Xn)  ณ    B16    ณ    B20     บ
  บ addr.W      ณ    B12    ณ    B16     บ
  บ addr.L      ณ    B16    ณ    B20     บ
  บ di16(PC)    ณ    B12    ณ    B18     บ
  บ di8(PC,Xn)  ณ    B14    ณ    B20     บ
  บ #<data>     ณ    B8     ณ    ---     บ
  ศอออออออออออออฯอออออออออออฯออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ - ณ * ณ - ณ - ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

CHK:	;CHecK register against bounds

  Syntax:  CHK <ea>,Dn

  Size: (Word)

  Compares the value in the data register specified in
  the instruction to zero and the upper bound (effective
  address operand).  The upper bound is a twos comple-
  ment integer.  If the register value is less than zero
  or greater than the upper bound, a CHK instruction ex-
  ception, vector number 6, occurs.

  Cycle Chart:

  ษอออออออออออออัออออออออออออออออออออออออป
  บ             ณ        Clocks          บ
  บ  Effective  รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ   Address   ณ  Dn,<ea>  ณ  Dn,<ea>   ศออป
  บ             ณ In-Bounds ณ Out-Of-Bounds บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤหออผ
  บ Dn          ณ    W10    ณ    W44     บ
  บ An          ณ    ---    ณ    ---     บ
  บ (An)        ณ    W10    ณ    W48     บ
  บ (An)+       ณ    W10    ณ    W48     บ
  บ -(An)       ณ    W16    ณ    W50     บ
  บ di16(An)    ณ    W18    ณ    W52     บ
  บ di8(An,Xn)  ณ    W20    ณ    W54     บ
  บ addr.W      ณ    W18    ณ    W52     บ
  บ addr.L      ณ    W22    ณ    W56     บ
  บ di16(PC)    ณ    W18    ณ    W52     บ
  บ di8(PC,Xn)  ณ    W20    ณ    W54     บ
  บ #<data>     ณ    W14    ณ    W48     บ
  ศอออออออออออออฯอออออออออออฯออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ U ณ U ณ U ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

CLR:	;CLeaR an operand

  Syntax:  CLR <ea>

  Size: (Byte, Word, Long)

  Clears the destination operand to zero.  Address reg-
  isters are not allowed.

  Cycle Chart:

  ษอออออออออออออัออออออออออออป
  บ  Effective  ณ   Clocks   บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ    <ea>    บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ BW4    L6  บ
  บ An          ณ    ----    บ
  บ (An)        ณ BW12   L20 บ
  บ (An)+       ณ BW12   L20 บ
  บ -(An)       ณ BW16   L22 บ
  บ di16(An)    ณ BW16   L24 บ
  บ di8(An,Xn)  ณ BW18   L26 บ
  บ addr.W      ณ BW16   L24 บ
  บ addr.L      ณ BW20   L28 บ
  บ di16(PC)    ณ    ----    บ
  บ di8(PC,Xn)  ณ    ----    บ
  บ #<data>     ณ    ----    บ
  ศอออออออออออออฯออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ 0 ณ 1 ณ 0 ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

CMP:	;CoMPare
CMPA:	;CoMPare to Address register

  Syntax:  CMP <ea>,Dn
           CMP <ea>,An    ;a.k.a."CMPA"

  Size:  CMP  = (Byte, Word, Long)
         CMPA = (Word, Long)

  Compares the source operand to the destination operand
  (as if the source was subtracted from the destination)
  and sets the condition codes according to the result.
  If either operand is an address register, then the op-
  eration can be a Word or Long operation only.

  Cycle Chart:  (See also: "CMPI")

  ษอออออออออออออัอออออออออออออออออออออออป
  บ  Effective  ณ      Clocks           บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  <ea>,Dn  ณ  <ea>,An  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ BW4   L6  ณ W6    L6  บ
  บ An          ณ  W4   L6  ณ W6    L6  บ
  บ (An)        ณ BW8   L14 ณ W10   L14 บ
  บ (An)+       ณ BW8   L14 ณ W10   L14 บ
  บ -(An)       ณ BW10  L16 ณ W14   L16 บ
  บ di16(An)    ณ BW12  L18 ณ W14   L18 บ
  บ di8(An,Xn)  ณ BW14  L20 ณ W16   L20 บ
  บ addr.W      ณ BW12  L18 ณ W14   L18 บ
  บ addr.L      ณ BW16  L22 ณ W18   L22 บ
  บ di16(PC)    ณ BW12  L18 ณ W14   L18 บ
  บ di8(PC,Xn)  ณ BW14  L20 ณ W16   L20 บ
  บ #<data>     ณ See: CMPI ณ W10   L14 บ
  ศอออออออออออออฯอออออออออออฯอออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

CMPI:	;CoMPare Immediate

  Syntax:  CMPI #<data>,<ea>  ; "CMP" may be subsituted
                              ; for "CMPI" with SPASM68K)

  Size:  (Byte, Word, Long)

  Compares the source operand to the destination operand
  (as if the source was subtracted from the destination)
  and sets the condition codes according to the result.

  Cycle Chart:  (See also: "CMP" & "CMPA")

  ษอออออออออออออัออออออออออออออป
  บ  Effective  ณ    Clocks    บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ #<data>,<ea> บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ  BW8    L14  บ
  บ An          ณ  See:  CMPA  บ
  บ (An)        ณ  BW12   L20  บ
  บ (An)+       ณ  BW12   L20  บ
  บ -(An)       ณ  BW14   L22  บ
  บ di16(An)    ณ  BW16   L24  บ
  บ di8(An,Xn)  ณ  BW18   L26  บ
  บ addr.W      ณ  BW16   L24  บ
  บ addr.L      ณ  BW20   L28  บ
  บ di16(PC)    ณ     ----     บ
  บ di8(PC,Xn)  ณ     ----     บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

CMPM:	;CoMPare Memory

  Syntax:  CMPM (Ay)+,(Ax)+

  Size:  (Byte, Word, Long)

  Compares the source operand to the destination operand
  (as if the source was subtracted from the destination)
  and sets the condition codes according to the result.

  Clocks:  BW12, L20

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

DBCC:	;Decrement and Branch if Carry Clear
DBCS:	;Decrement and Branch if Carry Set
DBEQ:	;Decrement and Branch if EQual
DBGE:	;Decrement and Branch if Greater or Equal
DBGT:	;Decrement and Branch if Greater Than
DBHI:	;Decrement and Branch if High
DBLE:	;Decrement and Branch if Less or Equal
DBLS:	;Decrement and Branch if Low or Same
DBLT:	;Decrement and Branch if Less Than
DBMI:	;Decrement and Branch if MInus (negative)
DBNE:	;Decrement and Branch if Not Equal
DBPL:	;Decrement and Branch if PLus (positive)
DBRA:	;Decrement and BRanch Always
DBVC:   ;Decrement and Branch if oVerflow Clear
DBVS:   ;Decrement and Branch if oVerflow Set

  Syntax:  DBcc Dn,<label>

  Size:  (Word)

  If the specified condtion is false, the source oper-
  and is decremented by 1; program execution continues
  at location (PC)+displacement if the source operand
  is not equal to -1 ($FFFF), otherwise the PC falls
  through to the next instruction.

  Cycle Chart:

  ษอออออออออออออออัออออออออออออออออออออออออออออออออออออป
  บ               ณ              Clocks                บ
  บ   Effective   รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ    Address    ณ  cc true  ณ cc false ณ  cc false   บ
  บ               ณ(no branch)ณ & branch ณ & no branch บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn,Relative16 ณ    W12    ณ   W10    ณ     W14     บ
  ศอออออออออออออออฯอออออออออออฯออออออออออฯอออออออออออออผ

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

DIVS:	;DIVide Signed

  Syntax:  DIVS <ea>,Dn

  Size:  (Word)

  Divides the signed destination operand by the signed
  source operand and stores the signed result in the
  destination.  The result is a quotient in the lower
  word (least significant 16-bits) and the remainder is
  in the upper word (most significant 16-bits) of the
  result.  The sign of the remainder is the same as the
  sign of the dividend.  If the source operand is zero,
  a divide-by-zero exception occurs.  If dividend is
  larger than a signed-word, oVerflow = 1

  Cycle Chart:

  ษอออออออออออออัออออออออออป
  บ  Effective  ณ  Clocks  บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  <ea>,Dn บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ   W158   บ
  บ An          ณ    --    บ
  บ (An)        ณ   W162   บ
  บ (An)+       ณ   W162   บ
  บ -(An)       ณ   W162   บ
  บ di16(An)    ณ   W166   บ
  บ di8(An,Xn)  ณ   W168   บ
  บ addr.W      ณ   W166   บ
  บ addr.L      ณ   W170   บ
  บ di16(PC)    ณ   W166   บ
  บ di8(PC,Xn)  ณ   W168   บ
  บ #<data>     ณ   W162   บ
  ศอออออออออออออฯออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ * ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

DIVU:	;DIVide Unsigned

  Syntax:  DIVU <ea>,Dn

  Size:  (Word)

  Divides the unsigned destination operand (long) by the
  unsigned source operand (word) and stores the unsigned
  result in the destination.  The result is a quotient
  in the lower word (least significant 16-bits) and the
  remainder is in the upper word (most significant 16-
  bits) of the result.  If the source operand is zero,
  a divide-by-zero exception occurs.  If the dividend is
  larger than an unsigned word, oVerflow = 1.

  Cycle Chart:

  ษอออออออออออออัออออออออออป
  บ  Effective  ณ  Clocks  บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ <ea>,Dn  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ   W140   บ
  บ An          ณ    --    บ
  บ (An)        ณ   W144   บ
  บ (An)+       ณ   W144   บ
  บ -(An)       ณ   W146   บ
  บ di16(An)    ณ   W148   บ
  บ di8(An,Xn)  ณ   W150   บ
  บ addr.W      ณ   W148   บ
  บ addr.L      ณ   W150   บ
  บ di16(PC)    ณ   W148   บ
  บ di8(PC,Xn)  ณ   W150   บ
  บ #<data>     ณ   W144   บ
  ศอออออออออออออฯออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ * ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

EOR:	;Exclusive-OR logical

  Syntax:  EOR Dn,<ea>

  Size:  (Byte, Word, Long)

  Exclusive-ORs the source operand with the destination
  operand and stores the result in the destination loca-
  tion.

  Cycle Chart:  (See also: "EORI")

  ษอออออออออออออัอออออออออออป
  บ  Effective  ณ  Clocks   บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  Dn,<ea>  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ BW4   L8  บ
  บ An          ณ    ---    บ
  บ (An)        ณ BW8   L14 บ
  บ (An)+       ณ BW8   L14 บ
  บ -(An)       ณ BW10  L16 บ
  บ di16(An)    ณ BW12  L18 บ
  บ di8(An,Xn)  ณ BW14  L20 บ
  บ addr.W      ณ BW12  L18 บ
  บ addr.L      ณ BW16  L22 บ
  บ di16(PC)    ณ BW12  L18 บ
  บ di8(PC,Xn)  ณ BW14  L20 บ
  ศอออออออออออออฯอออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

EORI:	;Exclusive-OR with Immediate

  Syntax:  EORI #<data>,<ea>  ;"EOR" may be subsituted
           EORI #<data>,CCR   ; for "EORI" with SPASM68K
           EORI #<data>,SR

  Size:  #<data>,<ea> = (Byte, Word, Long)
         #<data>,CCR  = (Byte)
         #<data>,SR   = (Word)

  Exlusive-ORs the immediate data to the destination op-
  erand and stores the result in the destination location.

  Cycle Chart:  (See also: "EOR")

  ษอออออออออออออัออออออออออออออป
  บ  Effective  ณ    Clocks    บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ #<data>,<ea> บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ  BW8    L20  บ
  บ An          ณ     ----     บ
  บ (An)        ณ  BW16   L28  บ
  บ (An)+       ณ  BW16   L28  บ
  บ -(An)       ณ  BW18   L30  บ
  บ di16(An)    ณ  BW20   L32  บ
  บ di8(An,Xn)  ณ  BW22   L34  บ
  บ addr.W      ณ  BW20   L32  บ
  บ addr.L      ณ  BW24   L36  บ
  บ di16(PC)    ณ     ----     บ
  บ di8(PC,Xn)  ณ     ----     บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ CCR         ณ     B20      บ
  บ SR          ณ     W20      บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:

      #<data>,<ea>          #<data>,CCR or SR

    X   N   Z   V   C       X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ   ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ   ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู   ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

EXG:	;EXchanGe registers

  Syntax:  EXG Dx,Dy
           EXG Ax,Ay
           EXG Dx,Ay
           EXG Ay,Dx

  Size:  (Long)

  Exchanges the contents of two 32-bit registers.

  Clocks: 6

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

EXT:	;sign EXTend

  Syntax:  EXT Dx

  Size:  (Word, Long)

  Extends a byte in a data register to a word, or a word
  in a data register to a long-word, by replicating the
  sign bit to the left.

  Clocks: 4

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

ILLEGAL:  ;take ILLEGAL instruction trap

  Syntax:  ILLEGAL

  Forces an illegal instruction exception, vector number
  4.  All other illegal instruction bit patterns are re-
  served for future extension of the instruction set and
  should not be used to force an exception.

  Clocks: 34

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

JMP:	;JuMP

  Syntax:  JMP <ea>

  Program execution continues at the effective address
  specified by the instruction.  The addressing mode for
  the effective address must be a control addressing mode.

  ษอออออออออออออัออออออออป
  บ  Effective  ณ Clocks บ
  บ   Address   รฤฤฤฤฤฤฤฤถ
  บ          ณ  <ea>  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤถ
  บ Dn          ณ   --   บ
  บ An          ณ   --   บ
  บ (An)        ณ    8   บ
  บ (An)+       ณ   --   บ
  บ -(An)       ณ   --   บ
  บ di16(An)    ณ   10   บ
  บ di8(An,Xn)  ณ   14   บ
  บ addr.W      ณ   10   บ
  บ addr.L      ณ   12   บ
  บ di16(PC)    ณ   10   บ
  บ di8(PC,Xn)  ณ   22   บ
  บ #<data>     ณ   --   บ
  ศอออออออออออออฯออออออออผ

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

JSR:	;Jump to SubRoutine

  Syntax:  JSR <ea>

  Pushes the long-word address of the instruction im-
  ediately following the JSR instruction onto the sys-
  tem stack.  Program execution then continues at the
  address specified in the instruction.

  ษอออออออออออออัออออออออป
  บ  Effective  ณ Clocks บ
  บ   Address   รฤฤฤฤฤฤฤฤถ
  บ          ณ  <ea>  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤถ
  บ Dn          ณ   --   บ
  บ An          ณ   --   บ
  บ (An)        ณ   16   บ
  บ (An)+       ณ   --   บ
  บ -(An)       ณ   --   บ
  บ di16(An)    ณ   18   บ
  บ di8(An,Xn)  ณ   22   บ
  บ addr.W      ณ   18   บ
  บ addr.L      ณ   20   บ
  บ di16(PC)    ณ   18   บ
  บ di8(PC,Xn)  ณ   22   บ
  บ #<data>     ณ   --   บ
  ศอออออออออออออฯออออออออผ

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

LEA:	;Load Effective Address

  Syntax:  LEA <ea>,An

  Size: (Long)

  Loads the effective address into the specified address
  register.  All 32 bits of the address register are
  affected by this instruction.

  ษอออออออออออออัออออออออป
  บ  Effective  ณ Clocks บ
  บ   Address   รฤฤฤฤฤฤฤฤถ
  บ          ณ  <ea>  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤถ
  บ Dn          ณ   --   บ
  บ An          ณ   --   บ
  บ (An)        ณ   L4   บ
  บ (An)+       ณ   --   บ
  บ -(An)       ณ   --   บ
  บ di16(An)    ณ   L8   บ
  บ di8(An,Xn)  ณ   L12  บ
  บ addr.W      ณ   L8   บ
  บ addr.L      ณ   L12  บ
  บ di16(PC)    ณ   L8   บ
  บ di8(PC,Xn)  ณ   L12  บ
  บ #<data>     ณ   --   บ
  ศอออออออออออออฯออออออออผ

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

LINK:   ;LINK and allocate

  Syntax:  LINK An,#<disp16>

  Size: (Word)

  Pushes the contents of the specified address register
  onto the stack.  Then loads the updateed stack pointer
  into the address register.  Finally, adds the displace-
  ment value to the stack pointer.  The displacement val-
  ue is sign-extended to 32-bits; the user should specify
  a negative displacement in order to allocate stack area.

  Clocks: 16

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

LSL:	;Logical Shift Left
LSR:	;Logical Shift Right

  Syntax:  LSd Dx,Dy
           LSd #<data>,Dy
           LSd <ea>
           (where d is direction, L or R)

  Size:  (Byte, Word, Long)

  Logically shifts the bits of the operand in the dir-
  ection (L or R) specified.  The carry bit receives
  the last bit shifted out of the operand.  The shift
  count for the shifting of a register may be specified
  by a data register or an immediate value.  Data regis-
  ters can be shifted as a Byte, a Word, or a Long.  Mem-
  ory operands are restricted to a Word.

  Operation Diagrams:

    LSL:
    ฺฤฤฤฤฤฤฤฤฤฟ        ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฟ      ฺฤฤฤฤฤฤฤฤฤฟ
    ณ    C    ณ<ฤฤฤยฤฤฤด   OPERAND   ณ<ฤฤฤฤฤด    0    ณ
    ภฤฤฤฤฤฤฤฤฤู    ณ   ภฤฤฤฤฤฤฤฤฤฤฤฤฤู      ภฤฤฤฤฤฤฤฤฤู
    ฺฤฤฤฤฤฤฤฤฤฟ    ณ
    ณ    X    ณ<ฤฤฤู
    ภฤฤฤฤฤฤฤฤฤู

    LSR:
    ฺฤฤฤฤฤฤฤฤฤฟ     ฺฤฤฤฤฤฤฤฤฤฤฤฟ           ฺฤฤฤฤฤฤฤฤฤฟ
    ณ    0    ณฤฤฤฤ>ณ  OPERAND  รฤฤฤฤยฤฤฤฤฤ>ณ    C    ณ
    ภฤฤฤฤฤฤฤฤฤู     ภฤฤฤฤฤฤฤฤฤฤฤู    ณ      ภฤฤฤฤฤฤฤฤฤู
                                     ณ      ฺฤฤฤฤฤฤฤฤฤฟ
                                     ภฤฤฤฤฤ>ณ    X    ณ
                                            ภฤฤฤฤฤฤฤฤฤู

  Cycle Chart:

  ษออออออออออออออออออออออออออออป
  บ          Clocks            บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dx,Dy       ณ BW6+2s L8+2s บ
  บ #<data>,Dy  ณ BW6+2s L8+2s บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ (An)        ณ     W12      บ
  บ (An)+       ณ     W12      บ
  บ -(An)       ณ     W14      บ
  บ di16(An)    ณ     W16      บ
  บ di8(An,Xn)  ณ     W18      บ
  บ addr.W      ณ     W16      บ
  บ addr.L      ณ     W20      บ
  บ di16(PC)    ณ     ---      บ
  บ di8(PC,Xn)  ณ     ---      บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ 0 ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

MOVE:	;MOVE data from source to destination
MOVEA:  ;MOVE data from source to Address register

  Syntax:  MOVE <ea>,<ea>
           MOVE <ea>,An  ;a.k.a. "MOVEA"--flags not set

  Size:  <ea>,<ea> =  (Byte, Word, Long)
         An,<ea>   =  (Word, Long)
         <ea>,An   =  (Word, Long)
         <ea>,CCR  =  (Byte)
         CCR,<ea>  =  (Byte)
         <ea>,SR   =  (Word)
         SR,<ea>   =  (Word)
         An,USP    =  (Long)
         USP,An    =  (Long)

  Moves the data at the source to the destination loca-
  tion, and sets the condition codes according to the
  data.  If the destination is an address register, only
  Word or Long operations are allowed--word operands be-
  ing sign extended to 32-bits; condition codes are not
  affected in this instance.

  Cycle Chart:

  ษอออออออออออออัอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
  บ   Source    ณ  Destination Operand -ฤ-ฤฤ-ฤฤฤ-ฤฤฤฤฤ>                                                           บ
  บ   Operand   รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤยฤฤฤฤยฤฤฤฤฤยฤฤฤฤฤยฤฤฤฤฤฤฤฤยฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤยฤฤฤฤฤยฤฤฤฤฤถ
  บ          ณ    Dn     ณ    An    ณ(An)ณ(An)+ณ-(An)ณdi16(An)ณaddr.Wณ di8(An,Xn)ณ   addr.L  ณ CCR ณ SR  ณ USP บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤลฤฤฤฤมฤฤฤฤฤมฤฤฤฤฤลฤฤฤฤฤฤฤฤมฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤลฤฤฤฤฤลฤฤฤฤฤถ
  บ Dn          ณ BW4   L4  ณ W4   L4  ณ  BW8      L12  ณ  BW12    L16  ณ BW14  L18 ณ BW16  L20 ณ B12 ณ W12 ณ --- บ
  บ An          ณ  W4   L4  ณ W4   L4  ณ  BW8      L12  ณ  BW12    L16  ณ BW14  L18 ณ BW16  L20 ณ --- ณ --- ณ L4  บ
  บ (An)        ณ BW8   L12 ณ W8   L12 ณ  BW12     L20  ณ  BW16    L24  ณ BW18  L26 ณ BW24  L28 ณ B16 ณ W16 ณ --- บ
  บ (An)+       ณ BW8   L12 ณ W8   L12 ณ  BW12     L20  ณ  BW16    L24  ณ BW18  L26 ณ BW24  L28 ณ B16 ณ W16 ณ --- บ
  บ -(An)       ณ BW10  L14 ณ W10  L14 ณ  BW14     L22  ณ  BW18    L26  ณ BW20  L28 ณ BW26  L30 ณ B18 ณ W18 ณ --- บ
  บ di16(An)    ณ BW12  L16 ณ W12  L16 ณ  BW16     L24  ณ  BW20    L28  ณ BW22  L30 ณ BW28  L32 ณ B20 ณ W20 ณ --- บ
  บ di8(An,Xn)  ณ BW14  L18 ณ W14  L18 ณ  BW18     L26  ณ  BW22    L30  ณ BW24  L32 ณ BW30  L34 ณ B22 ณ W22 ณ --- บ
  บ addr.W      ณ BW12  L16 ณ W12  L16 ณ  BW16     L24  ณ  BW20    L28  ณ BW22  L30 ณ BW28  L32 ณ B20 ณ W20 ณ --- บ
  บ addr.L      ณ BW16  L20 ณ W16  L20 ณ  BW20     L28  ณ  BW24    L32  ณ BW26  L34 ณ BW32  L36 ณ B24 ณ W24 ณ --- บ
  บ di16(PC)    ณ BW12  L16 ณ W12  L16 ณ  BW16     L24  ณ  BW20    L28  ณ BW22  L30 ณ BW28  L32 ณ B20 ณ W20 ณ --- บ
  บ di8(PC,Xn)  ณ BW14  L18 ณ W14  L18 ณ  BW18     L26  ณ  BW22    L30  ณ BW24  L32 ณ BW30  L34 ณ B22 ณ W22 ณ --- บ
  บ #<data>     ณ BW8   L12 ณ W8   L12 ณ  BW12     L20  ณ  BW16    L24  ณ BW18  L26 ณ BW24  L28 ณ B16 ณ W16 ณ --- บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤลฤฤฤฤฤลฤฤฤฤฤถ
  บ SR          ณ     W6    ณ   ---    ณ   W12    ณ W14 ณ      W16      ณ    W18    ณ    W20    ณ --- ณ --- ณ --- บ
  บ USP         ณ    ---    ณ    L4    ณ   ---    ณ --- ณ      ---      ณ    ---    ณ    ---    ณ --- ณ --- ณ --- บ
  ศอออออออออออออฯอออออออออออฯออออออออออฯออออออออออฯอออออฯอออออออออออออออฯอออออออออออฯอออออออออออฯอออออฯอออออฯอออออผ

  Condition Codes:
  (except <ea>,An / USP,An / An,USP)

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

MOVEM:	;MOVE Multiple registers

  Syntax:  MOVEM register list,<ea>
           MOVEM <ea>,register list

  Size:  (Word, Long)

  Moves the contents of selected registers to or from
  consecutive memory locations starting at the location
  specified by the effective address.  In the case of
  a word transfer to either address or data registers,
  each word is sign extended to 32 bits, and the result-
  ing long-word is loaded into the associated register.
  If the effective address is specified by the predecre-
  ment mode, only a register-to-memory operation is al-
  lowed.

  ษอออออออออออออัอออออออออออออออออออออออออออออออป
  บ  Effective  ณ            Clocks             บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ reg-list,<ea> ณ <ea>,reg-list บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ      ---      ณ      ---      บ
  บ An          ณ      ---      ณ      ---      บ
  บ (An)        ณ W8+4r   L8+8r ณ W12+4r  L8+8r บ
  บ (An)+       ณ      ---      ณ W12+4r  L8+8r บ
  บ -(An)       ณ W8+4r   L8+8r ณ      ---      บ
  บ di16(An)    ณ W12+4r L12+8r ณ W16+4r L16+8r บ
  บ di8(An,Xn)  ณ W14+4r L14+8r ณ W18+4r L18+8r บ
  บ addr.W      ณ W12+4r L12+8r ณ W16+4r L16+8r บ
  บ addr.L      ณ W16+4r L16+8r ณ W20+4r L20+8r บ
  บ di16(PC)    ณ      ---      ณ W16+4r L16+8r บ
  บ di8(PC,Xn)  ณ      ---      ณ W18+4r L18+8r บ
  ศอออออออออออออฯอออออออออออออออฯอออออออออออออออผ

  Condition Codes:

   Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

MOVEP:	;MOVE Peripheral Data

  Syntax:  MOVEP Dx,di16(Ay)
           MOVEP di16(Ay),Dx

  Size:  (Word, Long)

  Moves data between a data register and alternate bytes
  within the address space starting at the location spe-
  cified and incrementing by two.  The high-order byte is
  transferred last.  The memory address is specified in
  the address register indirect plus 16-bit displacement
  addressing mode.  The instruction was originally des-
  igned for interfacing 8-bit peripherals on a 16-bit
  data bus, such as the MC68000 bus.

  Clocks:  W16, L24

  Condition Codes:

    Not Affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

MOVEQ:	;MOVE Quick

  Syntax:  MOVEQ #<data>,Dn

  Size:  (Long)

  Moves a byte of immediate data to a 32-bit data regis-
  ter.  The data is sign extended into a long-word.

  Clocks: 4

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

MULS:	;MULtiply Signed
MULU:   ;MULtiply Unsigned

  Syntax:  MULS <ea>,Dn
           MULU <ea>,Dn

  Size:  (Word)

  Multiplies the destination operand by the source oper-
  and and stores the result in the destination.  The mul-
  tiplier and multiplicand are both word operands, and
  the result is a long-word operand.  All 32 bits of the
  product are saved in the destination data register.

  Cycle Chart:

  ษอออออออออออออัอออออออออป
  บ  Effective  ณ Clocks  บ
  บ   Address   รฤฤฤฤฤฤฤฤฤถ
  บ          ณ <ea>,Dn บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ   W70   บ
  บ An          ณ    --   บ
  บ (An)        ณ   W74   บ
  บ (An)+       ณ   W74   บ
  บ -(An)       ณ   W76   บ
  บ di16(An)    ณ   W78   บ
  บ di8(An,Xn)  ณ   W80   บ
  บ addr.W      ณ   W78   บ
  บ addr.L      ณ   W82   บ
  บ di16(PC)    ณ   W78   บ
  บ di8(PC,Xn)  ณ   W80   บ
  บ #<data>     ณ   W74   บ
  ศอออออออออออออฯอออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ * ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

NBCD:	;Negate Binary Coded Decimal

  Syntax:  NBCD <ea>

  Size:  (Byte)

  Subtracts the destination operand and the extend bit
  from zero.  The operation is performed using binary
  coded decimal arithmetic.  The packed BCD result is
  saved in the destination location.  This instruction
  produces the tens complement of the destination if
  the extend bit is zero, or the nines complement if
  the extend bit is one.

  Cycle Chart:

  ษอออออออออออออัออออออออออป
  บ  Effective  ณ  Clocks  บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ   <ea>   บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ    B6    บ
  บ An          ณ    --    บ
  บ (An)        ณ    B12   บ
  บ (An)+       ณ    B12   บ
  บ -(An)       ณ    B14   บ
  บ di16(An)    ณ    B16   บ
  บ di8(An,Xn)  ณ    B18   บ
  บ addr.W      ณ    B16   บ
  บ addr.L      ณ    B20   บ
  บ di16(PC)    ณ    --    บ
  บ di8(PC,Xn)  ณ    --    บ
  ศอออออออออออออฯออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ U ณ * ณ U ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

NEG:	;Negate
NEGX:   ;Negate with Extend

  Syntax:  NEG  <ea>
           NEGX <ea>

  Size: (Byte, Word, Long)

  Subtracts the destination operand (along with extend
  bit if NEGX) from zero.

  Cycle Chart:

  ษอออออออออออออัออออออออออออป
  บ  Effective  ณ   Clocks   บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ    <ea>    บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ BW4    L6  บ
  บ An          ณ    ----    บ
  บ (An)        ณ BW12   L20 บ
  บ (An)+       ณ BW12   L20 บ
  บ -(An)       ณ BW16   L22 บ
  บ di16(An)    ณ BW16   L24 บ
  บ di8(An,Xn)  ณ BW18   L26 บ
  บ addr.W      ณ BW16   L24 บ
  บ addr.L      ณ BW20   L28 บ
  บ di16(PC)    ณ    ----    บ
  บ di8(PC,Xn)  ณ    ----    บ
  บ #<data>     ณ    ----    บ
  ศอออออออออออออฯออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

NOP:	;No OPeration

  Syntax:  NOP

  Performs no operation.  The processor state, other than
  the program counter, is unaffected.  Execution contin-
  ues with the instruction following the NOP instruction.

  Clocks: 4

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

NOT:	;NOT logical

  Syntax:  NOT <ea>

  Size: (Byte, Word, Long)

  Calculates the ones complement of the destination oper-
  and stores the result in the destination loacation,
  i.e., inverts all bits within the destination.

  Cycle Chart:

  ษอออออออออออออัออออออออออออป
  บ  Effective  ณ   Clocks   บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ    <ea>    บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ BW4    L6  บ
  บ An          ณ    ----    บ
  บ (An)        ณ BW12   L20 บ
  บ (An)+       ณ BW12   L20 บ
  บ -(An)       ณ BW16   L22 บ
  บ di16(An)    ณ BW16   L24 บ
  บ di8(An,Xn)  ณ BW18   L26 บ
  บ addr.W      ณ BW16   L24 บ
  บ addr.L      ณ BW20   L28 บ
  บ di16(PC)    ณ    ----    บ
  บ di8(PC,Xn)  ณ    ----    บ
  บ #<data>     ณ    ----    บ
  ศอออออออออออออฯออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

OR:	;OR logical

  Syntax:  OR Dn,<ea>

  Size:  (Byte, Word, Long)

  ORs the source operand with the destination operand and
  stores the result in the destination location.

  Cycle Chart:  (See also: "ORI")

  ษอออออออออออออัอออออออออออป
  บ  Effective  ณ  Clocks   บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  Dn,<ea>  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ BW4   L8  บ
  บ An          ณ    ---    บ
  บ (An)        ณ BW8   L14 บ
  บ (An)+       ณ BW8   L14 บ
  บ -(An)       ณ BW10  L16 บ
  บ di16(An)    ณ BW12  L18 บ
  บ di8(An,Xn)  ณ BW14  L20 บ
  บ addr.W      ณ BW12  L18 บ
  บ addr.L      ณ BW16  L22 บ
  บ di16(PC)    ณ BW12  L18 บ
  บ di8(PC,Xn)  ณ BW14  L20 บ
  ศอออออออออออออฯอออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

ORI:	;OR with Immediate

  Syntax:  ORI #<data>,<ea>  ;"OR" may be subsituted
           ORI #<data>,CCR   ; for "ORI" with SPASM68K
           ORI #<data>,SR

  Size:  #<data>,<ea> = (Byte, Word, Long)
         #<data>,CCR  = (Byte)
         #<data>,SR   = (Word)

  ORs the immediate data to the destination operand and
  stores the result in the destination location.

  Cycle Chart:  (See also: "OR")

  ษอออออออออออออัออออออออออออออป
  บ  Effective  ณ    Clocks    บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ #<data>,<ea> บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ  BW8    L20  บ
  บ An          ณ     ----     บ
  บ (An)        ณ  BW16   L28  บ
  บ (An)+       ณ  BW16   L28  บ
  บ -(An)       ณ  BW18   L30  บ
  บ di16(An)    ณ  BW20   L32  บ
  บ di8(An,Xn)  ณ  BW22   L34  บ
  บ addr.W      ณ  BW20   L32  บ
  บ addr.L      ณ  BW24   L36  บ
  บ di16(PC)    ณ     ----     บ
  บ di8(PC,Xn)  ณ     ----     บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ CCR         ณ     B20      บ
  บ SR          ณ     W20      บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:

      #<data>,<ea>          #<data>,CCR or SR

    X   N   Z   V   C       X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ   ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ   ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู   ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

PEA:	;Push Effective Address

  Syntax:  PEA <ea>

  Size: (Long)

  Pushes the effective address onto the stack.  The ef-
  fective address is a long-word address

  ษอออออออออออออัออออออออป
  บ  Effective  ณ Clocks บ
  บ   Address   รฤฤฤฤฤฤฤฤถ
  บ          ณ  <ea>  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤถ
  บ Dn          ณ   --   บ
  บ An          ณ   --   บ
  บ (An)        ณ   L12  บ
  บ (An)+       ณ   --   บ
  บ -(An)       ณ   --   บ
  บ di16(An)    ณ   L16  บ
  บ di8(An,Xn)  ณ   L20  บ
  บ addr.W      ณ   L16  บ
  บ addr.L      ณ   L12  บ
  บ di16(PC)    ณ   L16  บ
  บ di8(PC,Xn)  ณ   L20  บ
  บ #<data>     ณ   --   บ
  ศอออออออออออออฯออออออออผ

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

RESET:  ;RESET external devices

  Syntax:  RESET

  Asserts the ~RSTO signal for 124 clock periods, reset-
  ting all external devices.  The processor sate, other
  than the program counter, is unaffected and execution
  continues with the next instruction.

  Clocks: 132

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

ROL:	;ROtate Left
ROR:	;ROtate Right
ROXL:   ;ROtate with eXtend Left
ROXR:   ;ROtate with eXtend Right

  Syntax:  ROd  Dx,Dy
           ROd  #<data>,Dy
           ROd  <ea>
           ROXd Dx,Dy
           ROXd #<data>,Dy
           ROXd <ea>

           where d is direction, L or R

  Size:  (Byte, Word, Long)

  Rotates the bits of the operand in the direction spec-
  ified (L or R).  The extend bit is included in the rot-
  ation if ROXL or ROXR. (see diagrams below...)

  Operation Diagrams:

    ROL:           ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
    ฺฤฤฤฤฤฤฤฤฤฟ    ณ   ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฟ      ณ
    ณ    C    ณ<ฤฤฤมฤฤฤด   OPERAND   ณ<ฤฤฤฤฤู
    ภฤฤฤฤฤฤฤฤฤู        ภฤฤฤฤฤฤฤฤฤฤฤฤฤู

    ROR:
    ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
    ณ     ฺฤฤฤฤฤฤฤฤฤฤฤฟ    ณ      ฺฤฤฤฤฤฤฤฤฤฟ
    ภฤฤฤฤ>ณ  OPERAND  รฤฤฤฤมฤฤฤฤฤ>ณ    C    ณ
          ภฤฤฤฤฤฤฤฤฤฤฤู           ภฤฤฤฤฤฤฤฤฤู

    ROXL:         ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
    ฺฤฤฤฤฤฤฤฤฤฟ   ณ  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฟ     ฺฤฤฤฤฤฤฤฤฤฟ    ณ
    ณ    C    ณ<ฤฤมฤฤด   OPERAND   ณ<ฤฤฤฤด    X    ณ<ฤฤฤู
    ภฤฤฤฤฤฤฤฤฤู      ภฤฤฤฤฤฤฤฤฤฤฤฤฤู     ภฤฤฤฤฤฤฤฤฤู

    ROXR:
    ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
    ณ   ฺฤฤฤฤฤฤฤฤฤฟ     ฺฤฤฤฤฤฤฤฤฤฤฤฟ    ณ    ฺฤฤฤฤฤฤฤฤฤฟ
    ภฤฤฤด    X    ณฤฤฤฤ>ณ  OPERAND  รฤฤฤฤมฤฤฤ>ณ    C    ณ
        ภฤฤฤฤฤฤฤฤฤู     ภฤฤฤฤฤฤฤฤฤฤฤู         ภฤฤฤฤฤฤฤฤฤู

  Cycle Chart:

  ษออออออออออออออออออออออออออออป
  บ          Clocks            บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dx,Dy       ณ BW6+2s L8+2s บ
  บ #<data>,Dy  ณ BW6+2s L8+2s บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ (An)        ณ     W12      บ
  บ (An)+       ณ     W12      บ
  บ -(An)       ณ     W14      บ
  บ di16(An)    ณ     W16      บ
  บ di8(An,Xn)  ณ     W18      บ
  บ addr.W      ณ     W16      บ
  บ addr.L      ณ     W20      บ
  บ di16(PC)    ณ     ---      บ
  บ di8(PC,Xn)  ณ     ---      บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ 0 ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

RTE:  ;ReTurn from Exception

  Syntax:  RTE

  Loads the processor state information stored in the ex-
  ception stack frame located at the top of the stack in-
  to the processor.  The instruction examines the stack
  format field in the format/offset word to determine how
  much information must be restored.

  Clocks: 20

  Condition Codes:

    Set according to the condition code bits in the
    status register value restored from the stack.

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

RTR:  ;ReTurn and Restore status codes

  Syntax:  RTR

  Pulls the condition code and program counter values
  from the stack.  The previous condition codes and pro-
  gram counter values are lost.  The supervisor portion
  of the status register is unaffected.

  Clocks: 20

  Condition Codes:

    Set according to the condition code bits in the
    status register value restored from the stack.

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

RTS:  ;ReTurn from Subroutine

  Syntax:  RTS

  Pulls the program counter value from the stack.  The
  previous program counter value is lost.

  Clocks: 16

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

SBCD:	;Subtract Binary Coded Decimal

  Syntax:  SBCD Dy,Dx
           SBCD -(Ay),-(Ax)

  Size:  (Byte)

  Subtracts the source operand to the destination oper-
  and along with the extend bit, and stores the result
  in the destination location.  The subtraction is per-
  formed using binary coded decimal arithmetic.  The
  operands are packed BCD numbers.

  Cycle Chart:

  ษอออออออออออออออออออออป
  บ       Clocks        บ
  วฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤถ
  บ  Dy,Dx  ณ-(Ay),-(Ax)บ
  วฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤถ
  บ   B6    ณ    B18    บ
  ศอออออออออฯอออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ U ณ * ณ U ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

SCC:	;Set bits to 1s if Carry Clear
SCS:	;Set bits to 1s if Carry Set
SEQ:	;Set bits to 1s if EQual
SF:     ;Set bits to 1s if False (never true)
SGE:	;Set bits to 1s if Greater or Equal
SGT:	;Set bits to 1s if Greater Than
SHI:	;Set bits to 1s if High
SLE:	;Set bits to 1s if Less or Equal
SLS:	;Set bits to 1s if Low or Same
SLT:	;Set bits to 1s if Less Than
SMI:	;Set bits to 1s if MInus (negative)
SNE:	;Set bits to 1s if Not Equal
SPL:	;Set bits to 1s if PLus (positive)
ST:     ;Set bits to 1s if True (always true)
SVC:    ;Set bits to 1s if oVerflow Clear
SVS:    ;Set bits to 1s if oVerflow Set

  Syntax:  Scc <ea>

  Size:  (Byte)

  If the specified condtion is true, sets the byte
  specified by the effective address to TRUE (all
  ones).  Otherwise, sets that byte to FALSE (all
  zeros).

  Cycle Chart:

  ษอออออออออออออัอออออออออออออออออออออป
  บ             ณ       Clocks        บ
  บ  Effective  รฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤถ
  บ   Address   ณ   <ea>   ณ   <ea>   บ
  บ             ณ  cc=True ณ cc=False บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ    B6    ณ    B4    บ
  บ An          ณ    --    ณ    --    บ
  บ (An)        ณ    B12   ณ    B12   บ
  บ (An)+       ณ    B12   ณ    B12   บ
  บ -(An)       ณ    B14   ณ    B14   บ
  บ di16(An)    ณ    B16   ณ    B16   บ
  บ di8(An,Xn)  ณ    B18   ณ    B18   บ
  บ addr.W      ณ    B16   ณ    B16   บ
  บ addr.L      ณ    B20   ณ    B20   บ
  บ di16(PC)    ณ    --    ณ    --    บ
  บ di8(PC,Xn)  ณ    --    ณ    --    บ
  ศอออออออออออออฯออออออออออฯออออออออออผ

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

STOP:  ;load status register and STOP

  Syntax:  STOP #<data16>

  Moves the immediate operand into the status register
  (both user and supervisor portions), advances the pro-
  gram counter to point to the next instruction, and
  stops the fetching and executing of instructions.

  Clocks: 4

  Condition Codes:

    Set according to the immediate operand.

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

SUB:	;SUBtract
SUBA:   ;SUBtract from Address register

  Syntax:  SUB <ea>,Dn
           SUB Dn,<ea>
           SUB <ea>,An ;a.k.a."SUBA"--doesn't set flags

  Size:  SUB  = (Byte, Word, Long)
         SUBA = (Word, Long)

  Subtracts the source operand from the destination op-
  erand using binary subtraction, and stores the result
  in the destination location.

  Cycle Chart:  (See also: "SUBI")

  ษอออออออออออออัออออออออออออออออออออออออออออออออออป
  บ  Effective  ณ              Clocks              บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ  <ea>,Dn  ณ  Dn,<ea>  ณ <ea>,An  บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ BW4   L8  ณ <ฤฤฤx ฺฤ> ณ W8   L8  บ
  บ An          ณ  W4   L8  ณ     xฤู   ณ W8   L8  บ
  บ (An)        ณ BW8   L14 ณ BW12  L20 ณ W12  L14 บ
  บ (An)+       ณ BW8   L14 ณ BW12  L20 ณ W12  L14 บ
  บ -(An)       ณ BW10  L16 ณ BW14  L22 ณ W14  L16 บ
  บ di16(An)    ณ BW12  L18 ณ BW16  L24 ณ W16  L18 บ
  บ di8(An,Xn)  ณ BW14  L20 ณ BW18  L26 ณ W18  L20 บ
  บ addr.W      ณ BW12  L18 ณ BW16  L24 ณ W16  L18 บ
  บ addr.L      ณ BW16  L22 ณ BW20  L28 ณ W20  L22 บ
  บ di16(PC)    ณ BW12  L18 ณ    ---    ณ W16  L18 บ
  บ di8(PC,Xn)  ณ BW14  L20 ณ    ---    ณ W18  L20 บ
  บ #<data>     ณ See: SUBI ณ    ---    ณ W12  L16 บ
  ศอออออออออออออฯอออออออออออฯอออออออออออฯออออออออออผ

  Condition Codes:
  (except <ea>,An)

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

SUBI:	;SUBtract Immediate

  Syntax:  SUBI #<data>,<ea>  ;"SUB" may be subsituted
                              ; for "SUBI" with SPASM68K

  Size:  (Byte, Word, Long)

  Subtracts the immediate data from the destination op-
  erand using binary subtraction, and stores the result
  in the destination location.

  Cycle Chart:  (See also: "SUB" & "SUBA")

  ษอออออออออออออัออออออออออออออป
  บ  Effective  ณ    Clocks    บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ #<data>,<ea> บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ  BW8    L20  บ
  บ An          ณ  See:  SUBA  บ
  บ (An)        ณ  BW16   L28  บ
  บ (An)+       ณ  BW16   L28  บ
  บ -(An)       ณ  BW18   L30  บ
  บ di16(An)    ณ  BW20   L32  บ
  บ di8(An,Xn)  ณ  BW22   L34  บ
  บ addr.W      ณ  BW20   L32  บ
  บ addr.L      ณ  BW24   L36  บ
  บ di16(PC)    ณ     ----     บ
  บ di8(PC,Xn)  ณ     ----     บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

SUBQ:	;SUBtract Quick

  Syntax:  SUBQ #<data>,<ea>

  Size:  (Byte, Word, Long)

  Subtracts the immediate value (range = 1 to 8) from
  the destination operand using binary subtraction, and
  stores the result in the destination location.  If
  the destination is an address register, only Word
  or Long sizes are allowed and condition codes are not
  affected.

  Cycle Chart:

  ษอออออออออออออัออออออออออออออป
  บ  Effective  ณ    Clocks    บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ #<data>,<ea> บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ  BW4    L8   บ
  บ An          ณ   W8    L8   บ
  บ (An)        ณ  BW12   L20  บ
  บ (An)+       ณ  BW12   L20  บ
  บ -(An)       ณ  BW14   L22  บ
  บ di16(An)    ณ  BW16   L24  บ
  บ di8(An,Xn)  ณ  BW18   L26  บ
  บ addr.W      ณ  BW16   L24  บ
  บ addr.L      ณ  BW20   L28  บ
  บ di16(PC)    ณ     ----     บ
  บ di8(PC,Xn)  ณ     ----     บ
  ศอออออออออออออฯออออออออออออออผ

  Condition Codes:
  (except #<data>,An)

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

SUBX:	;SUBtract with eXtend

  Syntax:  SUBX Dy,Dx
           SUBX -(Ay),-(Ax)

  Size:  (Byte, Word, Long)

  Subtracts the source operand from the destination op-
  erand along with the extend bit, and stores the result
  in the destination location.

  Cycle Chart:

  ษอออออออออออออออออออออป
  บ       Clocks        บ
  วฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤถ
  บ  Dy,Dx  ณ-(Ay),-(Ax)บ
  วฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤถ
  บ BW4  L8 ณ BW18  L30 บ
  ศอออออออออฯอออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ * ณ * ณ * ณ * ณ * ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

SWAP:    ;SWAP register halves

  Syntax:  SWAP Dn

  Size: (Word)

  Exchanges the 16-bit words (halves) of a data register.

  Clocks: 16

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

TAS:	;Test And Set an operand

  Syntax:  TAS <ea>

  Size:  (Byte)

  Tests and sets the byte operand addressed by the effec-
  tive address field.  The instruction tests the current
  value of the operand and sets the N and Z condition ap-
  propriately.  TAS also sets the high-order bit of the
  operand.  The operation uses a locked or read-modify-
  write transfer sequence.  This instruction supports use
  of a flag or semaphore to coordinate several processors.

  Cycle Chart:

  ษอออออออออออออัออออออออออป
  บ  Effective  ณ  Clocks  บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ   <ea>   บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ    B4    บ
  บ An          ณ    --    บ
  บ (An)        ณ    B14   บ
  บ (An)+       ณ    B14   บ
  บ -(An)       ณ    B16   บ
  บ di16(An)    ณ    B18   บ
  บ di8(An,Xn)  ณ    B20   บ
  บ addr.W      ณ    B18   บ
  บ addr.L      ณ    B22   บ
  บ di16(PC)    ณ    --    บ
  บ di8(PC,Xn)  ณ    --    บ
  ศอออออออออออออฯออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

TRAP:    ;TRAP

  Syntax:  TRAP #<vector>

  Causes a TRAP #<vector> exception.  The instruction
  adds the immediate operand (vector) of the instruction
  to 32 to obtain the vector number.  The range of vec-
  tor values is 0 - 15.

  Clocks: 38

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

TRAPV:    ;TRAP on oVerflow

  Syntax:  TRAPV

  If the overflow condition is set, causes a TRAPV ex-
  ception (vector number 7).  If the overflow condition
  is not ser, the processor performs no operation and
  execution continues with the next instruction.

  Clocks: 34 or 4

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

TST:	;TeST an operand

  Syntax:  TST <ea>

  Size: (Byte, Word, Long)

  Compares the operand with zero and sets the condition
  codes according to the results of the test.  The size
  of the operation is specified as byte, word, or long.

  Cycle Chart:

  ษอออออออออออออัออออออออออออป
  บ  Effective  ณ   Clocks   บ
  บ   Address   รฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ          ณ    <ea>    บ
  วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤฤถ
  บ Dn          ณ BW4    L4  บ
  บ An          ณ    ----    บ
  บ (An)        ณ BW6    L12 บ
  บ (An)+       ณ BW6    L12 บ
  บ -(An)       ณ BW10   L14 บ
  บ di16(An)    ณ BW12   L16 บ
  บ di8(An,Xn)  ณ BW14   L18 บ
  บ addr.W      ณ BW12   L16 บ
  บ addr.L      ณ BW16   L20 บ
  บ di16(PC)    ณ    ----    บ
  บ di8(PC,Xn)  ณ    ----    บ
  บ #<data>     ณ    ----    บ
  ศอออออออออออออฯออออออออออออผ

  Condition Codes:

    X   N   Z   V   C
  ฺฤฤฤยฤฤฤยฤฤฤยฤฤฤยฤฤฤฟ
  ณ - ณ * ณ * ณ 0 ณ 0 ณ
  ภฤฤฤมฤฤฤมฤฤฤมฤฤฤมฤฤฤู

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

UNLINK:   ;UNLINK and deallocate

  Syntax:  UNLINK An

  Loads the stack pointer from the specified address
  register with the long-word pulled from the top of
  the stack.

  Clocks: 12

  Condition Codes:

    Not affected

ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
