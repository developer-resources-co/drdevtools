;==============================================================================
; opcode.asm: op-code assembler for spasm/65816
;==============================================================================

;==============================================================================
DATA	SEGMENT	'DATA'
;------------------------------------------------------------------------------

;========================================
;       65816 SPECIFIC ADDITIONS
;========================================

; bit field usage for the 816
;SDB2		DB	0	;80:SM 40:unused (rest is same)

; variables

LA_STACK		DB	40H DUP (?)
LI_STACK		DB	40H DUP (?)
LA_SP		DW	0
LI_SP		DW	0

; segment variables/flags

dataAssume	db	0			; starts at bank 0
directAssume	dw	0			; starst at bottom
addressingSeg	db	0			;  0 = code, 1 = data, 2 = abs(no thinking)

SEG_CODE		EQU	0
SEG_DATA		EQU	1
SEG_ABS		EQU	2

;==============================================================================

PARSE_TABLE LABEL WORD
	DW	OFFSET	PARSE_A
	DW	OFFSET	PARSE_B
	DW	OFFSET	PARSE_C
	DW	OFFSET	PARSE_D
	DW	OFFSET	PARSE_E
	DW	OFFSET	PARSE_PSEUDO
	DW	OFFSET	PARSE_PSEUDO
	DW	OFFSET	PARSE_PSEUDO
	DW	OFFSET	PARSE_I
	DW	OFFSET	PARSE_J
	DW	OFFSET	PARSE_PSEUDO
	DW	OFFSET	PARSE_L
	DW	OFFSET	PARSE_M
	DW	OFFSET	PARSE_N
	DW	OFFSET	PARSE_O
	DW	OFFSET	PARSE_P
	DW	OFFSET	PARSE_PSEUDO
	DW	OFFSET	PARSE_R
	DW	OFFSET	PARSE_S
	DW	OFFSET	PARSE_T
	DW	OFFSET	PARSE_PSEUDO
	DW	OFFSET	PARSE_PSEUDO
	DW	OFFSET	PARSE_W
	DW	OFFSET	PARSE_X
	DW	OFFSET	PARSE_PSEUDO
	DW	OFFSET	PARSE_PSEUDO

;------------------------------------------------------------------------------

GI_TABLE	DB	00H	;ADDRESS MODE MASKS GROUP I
	DB	09H		;1
	DB	0DH		;2
	DB	0FH		;3
	DB	05H		;4
	DB	00H		;5
	DB	00H		;6
	DB	11H		;7
	DB	17H		;8
	DB	01H		;9
	DB	15H		;10
	DB	19H		;11
	DB	1DH		;12
	DB	1FH		;13
	DB	19H		;14
	DB	00H		;15
	DB	00H		;16
	DB	00H		;17
	DB	12H		;18
	DB	07H		;19
	DB	00H		;20
	DB	00H		;21
	DB	03H		;22
	DB	13H		;23

DATA	ENDS

;==============================================================================
.CODE
;------------------------------------------------------------------------------

;==============================================================================
; inputs:
;	ds:si -> begin of param
;	ds:di -> end of param

PARSE_MNE	PROC	NEAR	;GET FIRST LETTER AND BRANCH ACCORDINGLY

	AND	SDB2,NOT	1			;ERROR ALERT ON!

	CMP	[MNE_LEN],3	;IS "MNE_FIELD" THREE LETTERS LONG?
	JNZ	P1
	MOV	BL,[MNE_FIELD]	;THEN PARSE AS A MNEMONIC
	SUB	BL,'A'
	CMP	BL,'Z'
	JA	P1
	XOR	BH,BH
	SHL	BX,1
	MOV	AX,WORD	PTR	[MNE_FIELD+1]
	JMP	[PARSE_TABLE+BX]

P1:	JMP	PARSE_PSEUDO	;TRY PARSING AS PSEUDO-OP

PARSE_MNE	ENDP

;==============================================================================

PARSE_A	PROC	NEAR
	CMP	AX,'CD'     ;ADC?
	JZ	@@ADC
	CMP	AX,'DN'     ;AND?
	JZ	@@AND
	CMP	AX,'LS'     ;ASL?
	JZ	@@ASL
	JMP	PARSE_PSEUDO

@@ADC:	MOV	AL,60H
	JMP	GROUPI

@@AND:	MOV	AL,20H
	JMP	GROUPI

@@ASL:	XOR	AL,AL
	CALL	ASC
	MOV	AL,0AH
	JMP	ISC

PARSE_A	ENDP

;------------------------------------------------------------------------------

PARSE_B	PROC	NEAR

	CALL	CNV_B		;CHECK FOR ALTERNATE "B" MNEMONICS

	CMP	AL,'C'      ;BCx?
	JZ	P_BC
	CMP	AL,'R'      ;BRx?
	JZ	P_BR
	CMP	AL,'V'      ;BVx?
	JZ	P_BV

	CMP	AX,'QE'     ;BEQ?
	JZ	P_BEQ
	CMP	AX,'TI'     ;BIT?
	JZ	P_BIT
	CMP	AX,'IM'     ;BMI?
	JZ	P_BMI
	CMP	AX,'EN'     ;BNE?
	JZ	P_BNE
	CMP	AX,'LP'     ;BPL?
	JZ	P_BPL
	JMP	PARSE_PSEUDO

P_BEQ:	MOV	AL,0F0H
	JMP	GROUPIII
P_BIT:	MOV	AL,20H
	JMP	GROUPVI
P_BMI:	MOV	AL,30H
	JMP	GROUPIII
P_BNE:	MOV	AL,0D0H
	JMP	GROUPIII
P_BPL:	MOV	AL,10H
	JMP	GROUPIII

P_BC:	CMP	AH,'C'      ;BCC?
	JZ	P_BCC
	CMP	AH,'S'      ;BCS?
	JZ	P_BCS
	JMP	PARSE_PSEUDO

P_BCC:	MOV	AL,90H
	JMP	GROUPIII
P_BCS:	MOV	AL,0B0H
	JMP	GROUPIII

P_BR:	CMP	AH,'A'      ;BRA?
	JZ	P_BRA
	CMP	AH,'K'      ;BRK?
	JZ	P_BRK
	CMP	AH,'L'      ;BRL?
	JZ	P_BRL
	JMP	PARSE_PSEUDO

P_BRA:
	MOV	AL,80H
	JMP	GROUPIII
P_BRK:
	XOR	AL,AL
	JMP	WEIRDOS
P_BRL:
	MOV	AL,82H
	JMP	GROUPIII

P_BV:
	CMP	AH,'C'      ;BVC?
	JZ	P_BVC
	CMP	AH,'S'      ;BVS?
	JZ	P_BVS
	JMP	PARSE_PSEUDO
P_BVC:
	MOV	AL,50H
	JMP	GROUPIII
P_BVS:
	MOV	AL,70H
	JMP	GROUPIII
PARSE_B	ENDP

;------------------------------------------------------------------------------

PARSE_C	PROC	NEAR

	CALL	CNV_C		;CHECK FOR ALTERNATE "C" MNEMONICS

	CMP	AL,'L'      ;CLx?
	JZ	@@CL
	CMP	AL,'P'      ;CPx?
	JZ	@@CP
	CMP	AX,'PM'     ;CMP?
	JZ	@@CMP
	CMP	AX,'PO'     ;COP?
	JZ	@@COP
	JMP	PARSE_PSEUDO

@@CMP:
	MOV	AL,0C0H
	JMP	GROUPI

@@COP:
	MOV	AL,2
	JMP	WEIRDOS

@@CP:
	CMP	AH,'X'      ;CPX?
	JZ	@@CPX
	CMP	AH,'Y'      ;CPY?
	JZ	@@CPY
@@CPX:
	MOV	AL,0E0H
	JMP	GROUPVII
@@CPY:
	MOV	AL,0C0H
	JMP	GROUPVII

@@CL:
	CMP	AH,'C'      ;CLC?
	JZ	@@CLC
	CMP	AH,'D'      ;CLD?
	JZ	@@CLD
	CMP	AH,'I'      ;CLI?
	JZ	@@CLI
	CMP	AH,'V'      ;CLV?
	JZ	@@CLV
	JMP	PARSE_PSEUDO
@@CLC:	MOV	AL,18H
	JMP	ISC
@@CLD:	MOV	AL,0D8H
	JMP	ISC
@@CLI:	MOV	AL,58H
	JMP	ISC
@@CLV:	MOV	AL,0B8H
	JMP	ISC
PARSE_C	ENDP

;------------------------------------------------------------------------------

PARSE_D	PROC	NEAR

	CALL	CNV_D		;SEARCH FOR ALTERNATE "D" MNEMONICS

	CMP	AL,'E'
	JNZ	@@D1
	CMP	AH,'C'      ;DEC?
	JZ	@@DEC
	CMP	AH,'X'      ;DEX?
	JZ	@@DEX
	CMP	AH,'Y'      ;DEY?
	JZ	@@DEY
@@D1:	JMP	PARSE_PSEUDO
@@DEC:	MOV	AL,0C0H
	CALL	ASC
	MOV	AL,3AH
	JMP	ISC

@@DEX:	MOV	AL,0CAH
	JMP	ISC

@@DEY:	MOV	AL,88H
	JMP	ISC
PARSE_D	ENDP

;------------------------------------------------------------------------------

PARSE_E	PROC	NEAR
	CMP	AX,'RO'     ;EOR?
	JZ	@@EOR
	JMP	PARSE_PSEUDO

@@EOR:	MOV	AL,40H
	JMP	GROUPI
PARSE_E	ENDP

;------------------------------------------------------------------------------

PARSE_I	PROC	NEAR

	CALL	CNV_I		;SEARCH FOR ALTERNATE "I" MNEMONICS

	CMP	AL,'N'
	JNZ	@@I1
	CMP	AH,'C'      ;INC?
	JZ	@@INC
	CMP	AH,'X'      ;INX?
	JZ	@@INX
	CMP	AH,'Y'      ;INY?
	JZ	@@INY
@@I1:	JMP	PARSE_PSEUDO
@@INC:	MOV	AL,0E0H
	CALL	ASC
	MOV	AL,1AH
	JMP	ISC
@@INX:	MOV	AL,0E8H
	JMP	ISC
@@INY:	MOV	AL,0C8H
	JMP	ISC
PARSE_I	ENDP

;------------------------------------------------------------------------------

PARSE_J	PROC	NEAR
	CMP	AL,'M'      ;JMx?
	JZ	@@JM
	CMP	AL,'S'      ;JSx?
	JZ	@@JS
@@J1:	JMP	PARSE_PSEUDO
@@JM:	CMP	AH,'L'      ;JML?
	JNZ	@@JMP
	MOV	AL,0DCH
	JMP	GROUPIV
@@JMP:	CMP	AH,'P'      ;JMP?
	JNZ	@@J1
	MOV	AL,4CH
	JMP	GROUPIV
@@JS:	CMP	AH,'L'      ;JSL?
	JNZ	@@JSR
	MOV	AL,22H
	JMP	GROUPIV
@@JSR:	CMP	AH,'R'      ;JSR?
	JNZ	@@J1
	MOV	AL,20H
	JMP	GROUPIV
PARSE_J	ENDP

;------------------------------------------------------------------------------

PARSE_L	PROC	NEAR
	CMP	AX,'AD'     ;LDA?
	JZ	@@LDA
	CMP	AX,'XD'     ;LDX?
	JZ	@@LDX
	CMP	AX,'YD'     ;LDY?
	JZ	@@LDY
	CMP	AX,'RS'     ;LSR?
	JZ	@@LSR
	JMP	PARSE_PSEUDO

@@LDA:	MOV	AL,0A0H
	JMP	GROUPI

@@LDX:	MOV	AL,0A2H
	JMP	GROUPV
@@LDY:	MOV	AL,0A0H
	JMP	GROUPV
@@LSR:	MOV	AL,40H
	CALL	ASC
	MOV	AL,4AH
	JMP	ISC
PARSE_L	ENDP

;------------------------------------------------------------------------------

PARSE_M	PROC	NEAR
	CMP	AX,'NV'     ;MVN?
	JZ	@@MVN
	CMP	AX,'PV'     ;MVP?
	JZ	@@MVP
	JMP	PARSE_PSEUDO
@@MVN:	MOV	AL,54H
	JMP	GROUPVII
@@MVP:	MOV	AL,44H
	JMP	GROUPVII
PARSE_M	ENDP

;------------------------------------------------------------------------------

PARSE_N	PROC	NEAR
	CMP	AX,'PO'     ;NOP?
	JZ	@@NOP
	JMP	PARSE_PSEUDO

@@NOP:	MOV	AL,0EAH
	JMP	ISC
PARSE_N	ENDP

;------------------------------------------------------------------------------

PARSE_O	PROC	NEAR
	CMP	AX,'AR'     ;ORA?
	JZ	@@ORA
	JMP	PARSE_PSEUDO

@@ORA:	XOR	AL,AL
	JMP	GROUPI
PARSE_O	ENDP

;------------------------------------------------------------------------------

PARSE_P	PROC	NEAR
	CMP	AL,'E'      ;PEx
	JZ	@@PE
	CMP	AL,'H'      ;PHx
	JZ	@@PH
	CMP	AL,'L'      ;PLx
	JZ	@@PL
	JMP	PARSE_PSEUDO

@@PE:	CMP	AH,'A'      ;PEA?
	JZ	@@PEA
	CMP	AH,'I'      ;PEI?
	JZ	@@PEI
	CMP	AH,'R'      ;PER?
	JZ	@@PER
	JMP	PARSE_PSEUDO

@@PEA:
	MOV	AL,0F4H
	JMP	WEIRDOS
@@PEI:
	MOV	AL,0D4H
	JMP	WEIRDOS
@@PER:
	MOV	AL,62H
	JMP	WEIRDOS

@@PH:
	CMP	AH,'A'      ;PHA?
	JZ	@@PHA
	CMP	AH,'B'      ;PHB?
	JZ	@@PHB
	CMP	AH,'D'      ;PHD?
	JZ	@@PHD
	CMP	AH,'K'      ;PHK?
	JZ	@@PHK
	CMP	AH,'P'      ;PHP?
	JZ	@@PHP
	CMP	AH,'X'      ;PHX?
	JZ	@@PHX
	CMP	AH,'Y'      ;PHY?
	JZ	@@PHY
	JMP	PARSE_PSEUDO

@@PHA:
	MOV	AL,48H
	JMP	ISC
@@PHB:
	MOV	AL,8BH
	JMP	ISC
@@PHD:
	MOV	AL,0BH
	JMP	ISC
@@PHK:
	MOV	AL,4BH
	JMP	ISC
@@PHP:
	MOV	AL,8
	JMP	ISC
@@PHX:
	MOV	AL,0DAH
	JMP	ISC
@@PHY:
	MOV	AL,05AH
	JMP	ISC

@@PL:
	CMP	AH,'A'      ;PLA?
	JZ	@@PLA
	CMP	AH,'B'      ;PLB?
	JZ	@@PLB
	CMP	AH,'D'      ;PLD?
	JZ	@@PLD
	CMP	AH,'P'      ;PLP?
	JZ	@@PLP
	CMP	AH,'X'      ;PLX?
	JZ	@@PLX
	CMP	AH,'Y'      ;PLY?
	JZ	@@PLY
	JMP	PARSE_PSEUDO

@@PLA:
	MOV	AL,68H
	JMP	ISC
@@PLB:
	MOV	AL,0ABH
	JMP	ISC
@@PLD:
	MOV	AL,2BH
	JMP	ISC
@@PLP:
	MOV	AL,28H
	JMP	ISC
@@PLX:
	MOV	AL,0FAH
	JMP	ISC
@@PLY:
	MOV	AL,7AH
	JMP	ISC

PARSE_P	ENDP

;------------------------------------------------------------------------------

PARSE_R	PROC	NEAR
	CMP	AX,'PE'     ;REP?
	JZ	@@REP
	CMP	AX,'LO'     ;ROL?
	JZ	@@ROL
	CMP	AX,'RO'     ;ROR?
	JZ	@@ROR
	CMP	AL,'T'      ;RTx?
	JZ	@@RT
	JMP	PARSE_PSEUDO

@@REP:
	MOV	AL,0C2H
	JMP	GROUPVII
@@ROL:
	MOV	AL,20H
	CALL	ASC
	MOV	AL,2AH
	JMP	ISC
@@ROR:
	MOV	AL,60H
	CALL	ASC
	MOV	AL,6AH
	JMP	ISC

@@RT:
	CMP	AH,'I'      ;RTI?
	JZ	@@RTI
	CMP	AH,'L'      ;RTL?
	JZ	@@RTL
	CMP	AH,'S'      ;RTS?
	JZ	@@RTS
	JMP	PARSE_PSEUDO

@@RTI:
	MOV	AL,40H
	JMP	ISC
@@RTL:
	MOV	AL,6BH
	JMP	ISC
@@RTS:
	MOV	AL,60H
	JMP	ISC

PARSE_R	ENDP

;------------------------------------------------------------------------------

PARSE_S	PROC	NEAR

	CALL	CNV_S		;SEARCH FOR ALTERNATE "S" MNEMONICS

	CMP	AX,'CB'     ;SBC?
	JZ	@@SBC
	CMP	AL,'E'      ;SEx?
	JZ	@@SE
	CMP	AL,'T'      ;STx?
	JZ	@@ST
	JMP	PARSE_PSEUDO

@@SBC:
	MOV	AL,0E0H
	JMP	GROUPI

@@SE:
	CMP	AH,'C'      ;SEC?
	JZ	@@SEC
	CMP	AH,'D'      ;SED?
	JZ	@@SED
	CMP	AH,'I'      ;SEI?
	JZ	@@SEI
	CMP	AH,'P'      ;SEP?
	JZ	@@SEP
	JMP	PARSE_PSEUDO

@@SEC:
	MOV	AL,38H
	JMP	ISC
@@SED:
	MOV	AL,0F8H
	JMP	ISC
@@SEI:
	MOV	AL,78H
	JMP	ISC
@@SEP:
	MOV	AL,0E2H
	JMP	GROUPVII

@@ST:
	CMP	AH,'A'      ;STA?
	JZ	@@STA
	CMP	AH,'P'      ;STP?
	JZ	@@STP
	CMP	AH,'X'      ;STX?
	JZ	@@STX
	CMP	AH,'Y'      ;STY?
	JZ	@@STY
	CMP	AH,'Z'      ;STZ?
	JZ	@@STZ

@@STA:
	MOV	AL,80H
	JMP	GROUPI
@@STP:
	MOV	AL,0DBH
	JMP	ISC
@@STX:
	MOV	AL,82H
	JMP	GROUPV
@@STY:
	MOV	AL,80H
	JMP	GROUPV
@@STZ:
	MOV	AL,60H
	JMP	GROUPV
PARSE_S	ENDP

;------------------------------------------------------------------------------

PARSE_T	PROC	NEAR

	CALL	CNV_T		;SEARCH FOR ALTERNATE "T" MNEMONICS

	CMP	AL,'S'      ;TSx?
	JZ	@@TS
	CMP	AL,'X'      ;TXx?
	JZ	@@TX
	CMP	AX,'XA'     ;TAX?
	JZ	@@TAX
	CMP	AX,'YA'     ;TAY?
	JZ	@@TAY
	CMP	AX,'DC'     ;TCD?
	JZ	@@TCD
	CMP	AX,'SC'     ;TCS?
	JZ	@@TCS
	CMP	AX,'CD'     ;TDC?
	JZ	@@TDC
	CMP	AX,'BR'     ;TRB?
	JZ	@@TRB
	CMP	AX,'AY'     ;TYA?
	JZ	@@TYA
	CMP	AX,'XY'     ;TYX?
	JZ	@@TYX
	JMP	PARSE_PSEUDO

@@TS:
	CMP	AH,'B'      ;TSB?
	JZ	@@TSB
	CMP	AH,'C'      ;TSC?
	JZ	@@TSC
	CMP	AH,'X'      ;TSX?
	JZ	@@TSX
	JMP	PARSE_PSEUDO

@@TSB:
	XOR	AL,AL
	JMP	GROUPVI
@@TSC:
	MOV	AL,3BH
	JMP	ISC
@@TSX:
	MOV	AL,0BAH
	JMP	ISC

@@TX:
	CMP	AH,'A'      ;TXA?
	JZ	@@TXA
	CMP	AH,'S'      ;TXS?
	JZ	@@TXS
	CMP	AH,'Y'      ;TXY?
	JZ	@@TXY

@@TXA:
	MOV	AL,8AH
	JMP	ISC
@@TXS:
	MOV	AL,9AH
	JMP	ISC
@@TXY:
	MOV	AL,9BH
	JMP	ISC
@@TAX:
	MOV	AL,0AAH
	JMP	ISC
@@TAY:
	MOV	AL,0A8H
	JMP	ISC
@@TCD:
	MOV	AL,5BH
	JMP	ISC
@@TCS:
	MOV	AL,1BH
	JMP	ISC
@@TDC:
	MOV	AL,7BH
	JMP	ISC
@@TRB:
	MOV	AL,10H
	JMP	GROUPVI
@@TYA:
	MOV	AL,98H
	JMP	ISC
@@TYX:
	MOV	AL,0BBH
	JMP	ISC

PARSE_T	ENDP

;------------------------------------------------------------------------------

PARSE_W	PROC	NEAR
	CMP	AX,'IA'     ;WAI?
	JZ	@@WAI
	CMP	AX,'MD'     ;WDM?
	JZ	@@WDM
	JMP	PARSE_PSEUDO
@@WAI:	MOV	AL,0CBH
	JMP	ISC
@@WDM:	MOV	AL,42H
	JMP	ISC
PARSE_W	ENDP


PARSE_X	PROC	NEAR
	CMP	AX,'AB'     ;XBA?
	JZ	@@XBA
	CMP	AX,'EC'     ;XCE?
	JZ	@@XCE
	JMP	PARSE_PSEUDO
@@XBA:	MOV	AL,0EBH
	JMP	ISC
@@XCE:	MOV	AL,0FBH
	JMP	ISC
PARSE_X	ENDP

;==============================================================================

CNV2STD	PROC	NEAR		;CONVERT ALTERNATE MNEMONIC TO STANDARD

CNV_B:	CMP	AX,'TL'     ;BLT -> BCC
	JNZ	CNV_B1
	MOV	AX,'CC'
	RET
CNV_B1:	CMP	AX,'EG'     ;BGE -> BCS
	JNZ	CNV_B2
	MOV	AX,'SC'
CNV_B2:	RET

CNV_C:	CMP	AX,'AM'     ;CMA -> CMP
	JNZ	CNV_B2
	MOV	AH,'P'
	RET

CNV_D:	CMP	AX,'AE'     ;DEA -> ASSEMBLE *NOW* AS "DEC A"
	JNZ	CNV_B2
	MOV	AL,3AH		;OPCODE
	JMP	ISC		;IMPLIED SHORTCUT

CNV_I:	CMP	AX,'AN'     ;INA -> ASSEMBLE *NOW* AS "INC A"
	JNZ	CNV_B2
	MOV	AL,1AH
	JMP	ISC

CNV_S:	CMP	AX,'AW'     ;SWA -> ASSEMBLE *NOW* AS "XBA"
	JNZ	CNV_B2
	MOV	AL,0EBH
	JMP	ISC

CNV_T:	CMP	AX,'DA'     ;TAD -> ASSEMBLE *NOW* AS "TCD"
	JNZ	CNV_T2
	MOV	AL,5BH
	JMP	ISC
CNV_T2:	CMP	AX,'SA'     ;TSA -> ASSEMBLE *NOW* AS "TCS"
	JNZ	CNV_T3
	MOV	AL,1BH
	JMP	ISC
CNV_T3:	CMP	AX,'AD'     ;TDA -> ASSEMBLE *NOW* AS "TDC"
	JNZ	CNV_T4
	MOV	AL,7BH
	JMP	ISC
CNV_T4:	CMP	AX,'AS'     ;TSA -> ASSEMBLE *NOW* AS "TSC"
	JNZ	CNV_B2
	MOV	AL,3BH
	JMP	ISC

CNV2STD	ENDP

;==============================================================================
; adc,and,cmp,eor,lda,ora,sbc,sta
;------------------------------------------------------------------------------

GROUPI	PROC	NEAR		;ARITHMETIC/BOOLEAN OPERATIONS

	mov	addressingSeg,SEG_DATA
	CALL	CAM_AD		;FIND ADRESSING MODE
;   	dl = addressing code(0 = not found)
;	bl:cx = operand value
	XOR	DH,DH
	MOV	DI,DX
	MOV	AH,[GI_TABLE+DI];LOOK UP BIT MASK
	OR	AH,AH		;WAS MODE VALID?
	JZ	@@Error		;COMPLAIN IF NOT
	OR	AL,AH		;ABRACADABRA!
	; kts 05-18-94 03:11pm, immediate not allowed for sta
	cmp     al,089h
	jz	@@Error
GroupIEntry:
	CALL	STORE_BYTE	;STORE OPCODE
	MOV	AL,CL		;LOAD & STORE 8-BIT OPERAND
	CALL	STORE_BYTE

	CMP	AH,9		;IMMEDIATE?
	JNZ	@@NotImm

	TEST	SDB1,80H	;LONG-A OFF?  IF SO THEN DONE
	JZ	@@Ret
@@NotImm:
	TEST	AH,8		;WORD OR LONG OP?
	JZ	@@Ret
	MOV	AL,CH		;STORE ANOTHER 8 BITS
	CALL	STORE_BYTE
	AND	AH,0FH
	CMP	AH,0FH		;LONG OP?
	JNZ	@@Ret
	MOV	AL,BL		;STORE ANOTHER 8 BITS
	CALL	STORE_BYTE
	RET
@@Error:
	MOV	AX,9		;INVALID ADDRESSING MODE
	JMP	ERROR
@@Ret:
	RET
GROUPI	ENDP

;==============================================================================
; ??
;------------------------------------------------------------------------------

GROUPII	PROC	NEAR		;INC,DEC,ROTATES, & SHIFTS
	mov	addressingSeg,SEG_DATA
	CALL	CAM_AD
	OR	DL,DL		;ILLEGAL ADRESSING MODE?
	JZ	@@Error
	CMP	DL,2		;CONVERT ADRESSING MODES INTO MASKS
	JZ	GII1
	CMP	DL,4
	JZ	GII2
	CMP	DL,10
	JZ	GII3
	CMP	DL,12
	JZ	GII4
	JMP	@@Error
GII1:	MOV	AH,0EH
	JMP	GII5
GII2:	MOV	AH,06H
	JMP	GII5
GII3:	MOV	AH,16H
	JMP	GII5
GII4:	MOV	AH,1EH
GII5:	OR	AL,AH		;ABRACADABRA
	CALL	STORE_BYTE	;STORE OPCODE
	MOV	AL,CL
	CALL	STORE_BYTE	;STORE 8-BIT DATA
	TEST	AH,8		;WORD OP?
	JZ	GII6
	MOV	AL,CH
	CALL	STORE_BYTE
GII6:
	RET

@@Error:
	MOV	AX,9		;INVALID ADDRESSING MODE
	JMP	ERROR

GROUPII	ENDP

;==============================================================================
; beq,bmi,bne,bpl,bcc,bcs,bra,brl,bvc,bvs
;------------------------------------------------------------------------------

GROUPIII	PROC	NEAR	;BRANCHES

	CMP	AL,82H		;BRL?
	JNZ	@@GIII1
	CALL	CAM_RL
	CMP	DL,16
	JNZ	@@GIII2
	CALL	STORE_BYTE	;STORE OPCODE
	MOV	AL,CL
	CALL	STORE_BYTE	;ADDRESS LOW
	MOV	AL,CH
	JMP	STORE_BYTE	;ADDRESS HI

@@GIII1:
	CALL	CAM_R
	CMP	DL,15
	JNZ	@@GIII2
	CALL	STORE_BYTE	;STORE OPCODE
	MOV	AL,CL
	JMP	STORE_BYTE	;8-BIT ADDRESS

@@GIII2:
	MOV	AX,9		;ILLEGAL ADDRESSING MODE
	JMP	ERROR

GROUPIII	ENDP

;==============================================================================
; jml,jmp,jsl,jsr
;------------------------------------------------------------------------------

GROUPIV	PROC	NEAR		;JUMPS

	mov	addressingSeg,SEG_CODE
	CALL	CAM_AD


	; kts removed 04-16-94 04:11pm, causes assembler to reserve 16 bits,
	; then back-patch 24 if addressing mode "al"
;	TEST	SDB1,4		;FORWARD REFERENCE?
;	JZ	@@GIV
;	CMP	DL,3		;IF SO THEN CONVERT "al" TO "a"
;	JNZ	@@GIV
;	MOV	DL,2
;@@GIV:
	CMP	DL,4		;CONVERT "d" TO "a"
	JZ	@@GIV0
	CMP	DL,18		;CONVERT "(d)" TO "(a)"
	JZ	@@GIV2A
	CMP	DL,9		;CONVERT "(d,X)" TO "(a,X)"
	JZ	@@GIV3A
	OR	DL,DL
	JZ	@@GIV6		;ERROR

	CMP	DL,2		;AM 2?
	JNZ	@@GIV1
@@GIV0:
	CMP	AL,22H
	JZ	@@GIV5		;OVERRIDE (a); ASSEMBLE LONG (al)
	CMP	AL,4CH
	JZ	@@GIV4		;ASSEMBLE WORD
	CMP	AL,20H
	JZ	@@GIV4		;ASSEMBLE WORD
	AND	AL,7FH
	CMP	AL,5CH
	JZ	@@GIV5		;ASSEMBLE LONG
	JMP	@@GIV6

@@GIV1:
	CMP	DL,3		;AM 3?  LONG OPERAND?
	JNZ	@@GIV2
	CMP	AL,22H
	JZ	@@GIV5		;ASSEMBLE LONG
	ADD	AL,10H
	CMP	AL,5CH
	JZ	@@GIV5		;ASSEMBLE LONG
	CMP	AL,0ECH
	JNZ	@@GIV6
	MOV	AL,5CH
	JMP	@@GIV5

@@GIV2:
	CMP	DL,17		;AM 17?
	JNZ	@@GIV3
@@GIV2A:
	CMP	AL,0DCH
	JZ	@@GIV4		;ASSEMBLE WORD
	ADD	AL,20H
	CMP	AL,6CH
	JZ	@@GIV4		;ASSEMBLE WORD
	JMP	@@GIV6

@@GIV3:
	CMP	DL,20		;AM 20?
	JNZ	@@GIV6		;ERROR
@@GIV3A:
	ADD	AL,30H
	CMP	AL,7CH
	JZ	@@GIV4		;ASSEMBLE WORD
	ADD	AL,0ACH
	CMP	AL,0FCH
	JZ	@@GIV4		;ASSEMBLE WORD
	JMP	@@GIV6

@@GIV4:
	CALL	STORE_BYTE	;STORE OPCODE   ;WORD STORE
	MOV	AL,CL
	CALL	STORE_BYTE	;STORE LSBYTE
	MOV	AL,CH
	JMP	STORE_BYTE	;STORE MSBYTE

@@GIV5:
	CALL	STORE_BYTE	;STORE OPCODE   ;LONG STORE
	MOV	AL,CL
	CALL	STORE_BYTE	;STORE LSBYTE
	MOV	AL,CH
	CALL	STORE_BYTE	;STORE MSBYTE
	MOV	AL,BL
	JMP	STORE_BYTE	;STORE SEGMENT

@@GIV6:
	MOV	AX,9		;ILLEGAL ADDRESSING MODE
	JMP	ERROR
GROUPIV	ENDP

;==============================================================================
; ldx,ldy,stx,sty,stz
;------------------------------------------------------------------------------

GROUPV	PROC	NEAR		;OTHER LD'S AND ST'S
				;APOLOGIES FOR LACK OF COMMENTS

	CMP	AL,0A0H		;LDX #, LDY #  ?
	JC	GV1
	CALL	CAM_I
	OR	DL,DL
	JZ	GV1
	CALL	STORE_BYTE
	MOV	AL,CL
	CALL	STORE_BYTE
	TEST	SDB1,40H	;LONG I OFF?
	JZ	GV0A
	MOV	AL,CH
	JMP	STORE_BYTE
GV0A:
	RET
GV0:
	MOV	AX,9		;ILLEGAL ADDRESSING MODE
	JMP	ERROR

GV1:
	mov	addressingSeg,SEG_DATA
	CALL	CAM_AD
	CMP	DL,2
	JNZ	GV2
	CMP	AL,60H
	JNZ	GV1A
	MOV	AL,90H
GV1A:
	OR	AL,0CH
	JMP	GV7
GV2:
	CMP	DL,4
	JNZ	GV3
	OR	AL,4
	JMP	GV8
GV3:
	CMP	DL,10
	JNZ	GV4
	OR	AL,14H
	CMP	AL,0B6H
	JZ	GV0
	CMP	AL,96H
	JZ	GV0
	JMP	GV8
GV4:
	CMP	DL,11
	JNZ	GV5
	OR	AL,16H
	CMP	AL,0B6H
	JZ	GV8
	CMP	AL,96H
	JZ	GV8
	JMP	GV0
GV5:
	CMP	DL,12
	JNZ	GV6
	OR	AL,1CH
	CMP	AL,0BCH
	JZ	GV7
	ADD	AL,22H
	CMP	AL,9EH
	JZ	GV7
GV6:
	CMP	DL,14
	JNZ	GV0
	OR	AL,1EH
	CMP	AL,0BEH
	JNZ	GV0
GV7:
	CALL	STORE_BYTE
	MOV	AL,CL
	CALL	STORE_BYTE
	MOV	AL,CH
	JMP	STORE_BYTE
GV8:
	CALL	STORE_BYTE
	MOV	AL,CL
	JMP	STORE_BYTE

GROUPV	ENDP

;==============================================================================
; bit,tsb,trb
;------------------------------------------------------------------------------

GROUPVI	PROC	NEAR		;BITWISE OPS

	CMP	AL,20H		;BIT?
	JNZ	GVI1
	CALL	CAM_I		;#?
	OR	DL,DL
	JZ	GVI1
	MOV	AX,980H		;"BIT #" IS LIKE A "GROUP I" INSTRUCTION
	OR	AL,AH		;ABRACADABRA!
	JMP	GroupIEntry
GVI1:
	mov	addressingSeg,SEG_DATA
	CALL	CAM_AD
	CMP	DL,2		;CONVERT ADDRESSING MODE # INTO MASK
	JNZ	GVI2
	MOV	AH,0CH
	JMP	GVI6
GVI2:
	CMP	DL,4
	JNZ	GVI3
	MOV	AH,4
	JMP	GVI6
GVI3:
	CMP	DL,10
	JNZ	GVI4
	MOV	AH,14H
	JMP	GVI5
GVI4:
	CMP	DL,12
	JNZ	GVI8
	MOV	AH,1CH
GVI5:
	CMP	AL,10H
	JBE	GVI8
GVI6:
	OR	AL,AH		;ABRACADABRA
	CALL	STORE_BYTE
	MOV	AL,CL
	CALL	STORE_BYTE
	TEST	AH,8
	JZ	GVI7
	MOV	AL,CH
	JMP	STORE_BYTE
GVI7:
	RET
GVI8:
	MOV	AX,9		;ILLEGAL ADDRESSING MODE
	JMP	ERROR

GROUPVI	ENDP

;==============================================================================
; cpx,cpy,mvn,mvp,rep,sep
;------------------------------------------------------------------------------

GROUPVII	PROC	NEAR	;LEFTOVERS

	TEST	AL,2		;REP OR SEP?
	JNZ	GVII7
	TEST	AL,4		;MVN OR MVP?
	JNZ	GVII9
	mov	addressingSeg,SEG_DATA
	CALL	CAM_AD		;CPX OR CPY
	CMP	DL,1
	JZ	GVII1
	CMP	DL,2
	JZ	GVII2
	CMP	DL,4
	JZ	GVII3
	JMP	IllegalAddrMode		;ERROR 9
GVII1:
	XOR	AH,AH
	JMP	GVII4
GVII2:
	MOV	AH,0CH
	JMP	GVII4
GVII3:
	MOV	AH,4
GVII4:
	OR	AL,AH		;ABRACADABRA!
	CALL	STORE_BYTE	;STORE OPCODE
	MOV	AL,CL
	CALL	STORE_BYTE	;STORE 8-BIT DATA
	OR	AH,AH		;IMMDEDIATE?
	JNZ	GVII5
	TEST	SDB1,40H	;LONG I ON?
	JNZ	GVII6
	RET
GVII5:
	TEST	AH,8		;8-BIT DATA?
	JZ	GVII11
GVII6:
	MOV	AL,CH
	JMP	STORE_BYTE

	;----- REP or SEP
GVII7:
	CALL	CAM_I		;REP OR SEP FOLLOWED BY IMMEDIATE?
	OR	DL,DL
	JNZ	GVII8		;JUMP IF YES
	JMP	IllegalAddrMode		;ELSE ERROR 9

GVII8:
	;----- See if the address we're about to assemble at (OBJPTR(L/H))
	;-----  is the same as the address of a symbol@@
	;-----  If so, update the symbol's A/I flags
	;^^^^^ NOT DONE YET

	CALL	STORE_BYTE	;STORE OP-CODE
	xchg	AL,CL
	CALL	STORE_BYTE	;STORE IMMDEIATE DATA

	;----- Set LONGA/LONGI flags based on data (AL)
	cmp	cl,0E2h			; REP?
	je	@@AutoREP

	test	al,10h		; turn on INDEX?
	jz	@@not_longi_sep
	or	sdb1,40h
@@not_longi_sep:

	test	al,20h
	jz	@@not_longa_sep
	or	sdb1,80h
@@not_longa_sep:
	ret

@@AutoREP:
	test	al,10h
	jz	@@not_longi_rep
	and	sdb1,NOT 40h
@@not_longi_rep:

	test	al,20h
	jz	@@not_longa_rep
	and	sdb1,NOT 80h
@@not_longa_rep:
	ret


	;----- MVN/MVP
GVII9:
	CALL	CAM_XYC		;MVP OR MVN FOLLOWED BY TWO BYTES?
	OR	DL,DL
	JNZ	GVII10
	JMP	IllegalAddrMode		;COMPLAIN IF NOT

GVII10:
	CALL	STORE_BYTE	;STORE OP-CODE
	MOV	AL,CL
	call	STORE_BYTE	;STORE DSTBLK
	MOV	AL,BL
	jmp	STORE_BYTE	;STORE SRCBLK
GVII11:
	RET
GROUPVII	ENDP

;==============================================================================
; brk,cop,pea,pei,per
;------------------------------------------------------------------------------

WEIRDOS	PROC	NEAR		;SPECIAL CASE INSTRUCTIONS

	CMP	AL,2		;BRK OR COP?
	JA	@@NotBrkOrCop
	CMP	[PARAM_FIELD],0	;NO OPERAND?
	JNZ	@@HasOperand
	JMP	STORE_BYTE	;OKAY THEN, YOU ASKED FOR IT!

@@HasOperand:
	mov	addressingSeg,SEG_CODE
	CALL	CAM_AD		;CHECK FOR CODE BYTE
	CMP	DL,4
	JNZ	@@WEIRD4		;PARAMETER NOT VALID
@@WriteOpAnd8Data:
	CALL	STORE_BYTE	;STORE OPCODE
	MOV	AL,CL
	JMP	STORE_BYTE	;STORE CODE BYTE

@@NotBrkOrCop:
	mov	addressingSeg,SEG_DATA
	CMP	AL,0D4H		;PEI?
	JNZ	@@NotPEI
	CALL	CAM_AD

	TEST	SDB1,4		;FORWARD REFERENCE?
	JNZ	@@WriteOpAnd8Data		; SO FAKE IT(KTS)

	CMP	DL,18		;(d) ADDRESSING MODE?
	JZ	@@WriteOpAnd8Data		;THEN IT'S COOL
	JMP	IllegalAddrMode		;ERROR 9

@@NotPEI:
	CMP	AL,0F4H		;PEA?
	JNZ	@@WEIRD4
	CALL	CAM_AD

	TEST	SDB1,4		;FORWARD REFERENCE?
	JNZ	@@WriteOpAnd16Data	; SO FAKE IT(KTS)

	CMP	DL,4
	JZ	@@WriteOpAnd16Data
	CMP	DL,2
	JZ	@@WriteOpAnd16Data
	cmp	dl,3
	jz	@@WriteOpAnd16Data
	cmp	dl,1
	jz	@@WriteOpAnd16Data
	JMP	IllegalAddrMode

@@WEIRD4:
	mov	addressingSeg,SEG_CODE
	CALL	CAM_RL		;PER
	TEST	SDB1,4		;FORWARD REFERENCE?
	JNZ	@@WriteOpAnd16Data		; SO FAKE IT(KTS)

	OR	DL,DL
	JZ	IllegalAddrMode

@@WriteOpAnd16Data:
	CALL	STORE_BYTE	;STORE OPCODE  (PEA OR PER)
	MOV	AL,CL
	CALL	STORE_BYTE	;STORE LOW BYTE
	MOV	AL,CH
	JMP	STORE_BYTE	;STORE HIGH BYTE

IllegalAddrMode:
	MOV	AX,9		;ILLEGAL ADDRESSING MODE
	JMP	ERROR

WEIRDOS	ENDP

;==============================================================================

ISC	PROC	NEAR		;"IMPLIED" SHORTCUT

	CMP	[PARAM_FIELD],0	;DID USER TRY TO ADD A PARAMETER?
	JZ	ISC1
	MOV	AX,2
	JMP	ERROR
ISC1:	JMP	STORE_BYTE

ISC	ENDP

;==============================================================================

ASC	PROC	NEAR		;"ACCUMULATOR" ADDRESSING MODE SHORTCUT
	MOV	DX,WORD PTR [PARAM_FIELD]
	CMP	DX,'A'          ;JUST AN "A" IN THE PARAMETER FIELD?
	jz	@@ImpliedA
	cmp	dx,'a'
	je	@@ImpliedA
	cmp	dl,0
	jz	@@ImpliedA

	;----- Not register (a or implied a) addressing mode.
	;-----  Deal with memory location.
	ADD	SP,2
	JMP	GROUPII

@@ImpliedA:
	MOV	PARAM_FIELD,0	;BLANK IT OUT AND RETURN; USE "ISC"
	RET

ASC	ENDP

;==============================================================================
