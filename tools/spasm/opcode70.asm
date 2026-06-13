;==============================================================================
; opcode70.asm: op-code assembler for spasm/SPC700
;==============================================================================


	if 0
** adc
* addw
** and
* and1
** asl

* bbc
* bbs
** bcc
** bcs
** beq
** bmi
** bne
** bpl
** bra
** brk
** bvc
** bvs

** cmp
* cmpw
* call
* clr1
** clrc
** clrp
** clrv
* cbne

** dec
* decw
* div
** di
* daa
* das
* dbnz

** ei
** eor
* eor1

** inc
* incw

* jmp

** lsr

** mov
* mov1
* movw
* mul

** nop
* not1
** notc

** or
* or1

* pcall
* pop
* push

** ret
** reti
** rol
** ror

** sbc
* subw
* set1
** setc
** setp
** stop
** sleep

* tcall
* tclr1
* tset1

* xcn
	endif



;==============================================================================
DATA	SEGMENT	'DATA'
;------------------------------------------------------------------------------

;========================================
;       SPC700 SPECIFIC ADDITIONS
;========================================

; variables

porgl		dw	0
porgh		db	0

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
	DW	OFFSET	PARSE_PSEUDO
	DW	OFFSET	PARSE_X
	DW	OFFSET	PARSE_PSEUDO
	DW	OFFSET	PARSE_PSEUDO

;------------------------------------------------------------------------------

GI_TABLE	DB	00H	;ADDRESS MODE MASKS GROUP I
	DB	08H		;1	Immediate addressing		
	DB	00H		;2	Absolute				
	DB	00H		;3	Absolute Long			
	DB	00H		;4	Direct				
	DB	00H		;5	Accumulator			
	DB	00H		;6	Implied				
	DB	00H		;7	Direct Indirect Indexed		
	DB	00H		;8	Direct Indirect Long Indexed		
	DB	00H		;9	Direct Indexed Indirect		
	DB	00H		;10	Direct Indexed with X		
	DB	00H		;11	Direct Indexed with Y		
	DB	00H		;12	Absolute Indexed with X		
	DB	00H		;13	Absolute Long Indexed with X		
	DB	00H		;14	Absolute Indexed with Y		
	DB	00H		;15	Program Counter Relative 		
	DB	00H		;16	Program Counter Relative Long		
	DB	00H		;17	Absolute Indirect			
	DB	00H		;18	Direct Indirect			
	DB	00H		;19	Direct Indirect Long		
	DB	00H		;20	Absolute Indexed Indirect		
	DB	00H		;21	Stack				
	DB	00H		;22	Stack Relative			
	DB	00H		;23	Stack Relative Indirect Indexed	
	DB	00H		;24	Block Source Bank, Destination Bank

DATA	ENDS

;==============================================================================
.CODE
;------------------------------------------------------------------------------

;==============================================================================
; inputs:
;	ds:si -> begin of param
;	ds:di -> end of param

PARSE_MNE	PROC	NEAR	;GET FIRST LETTER AND BRANCH ACCORDINGLY

	AND	SDB2,NOT 1	;ERROR ALERT ON!

	MOV	BL,[MNE_FIELD]	;THEN PARSE AS A MNEMONIC
	SUB	BL,'A'
	CMP	BL,'Z'
	JA	P1
	XOR	BH,BH
	SHL	BX,1
	ifdef	__80386__
	mov	eax,dword ptr [mne_field+1]
	else
	MOV	AX,WORD PTR [MNE_FIELD+1]
	MOV	CX,WORD PTR [MNE_FIELD+3]	; ??? too many sometimes?
	endif
	JMP	[PARSE_TABLE+BX]
P1:	JMP	PARSE_PSEUDO
PARSE_MNE	ENDP

;==============================================================================


	if 0
PARSE_A	PROC	NEAR
	OPCODE	<ADC>,088h,NOT_DONE
	OPCODE	<ADDW>,07Ah,NOT_DONE
	OPCODE	<AND>,028h,NOT_DONE
	OPCODE	<AND1>,04Ah,NOT_DONE
	OPCODE	<ASL>,01Ch,NOT_DONE
PARSE_A	ENDP
	endif


PARSE_A	PROC	NEAR
	CMP	AX,'CD'		; ADC?
	jne	@@Not_ADC
	;----- ADC
	mov	si,OFFSET ADC_TABLE
	jmp	GROUPII
@@Not_ADC:

	cmp	ax,'DD'		; ADDW?
	jne	@@Not_ADDW
	cmp	cl,'W'
	jne	@@Not_ADDW
	;----- ADDW
	mov	al,07Ah
	jmp	NOT_DONE
@@Not_ADDW:

	cmp	ax,'DN'		; AND?
	jne	@@Not_ANDx

	or	cl,cl
	jne	@@Not_AND
	;----- AND
	mov	si,OFFSET AND_TABLE
	jmp	GROUPII

@@Not_AND:
	cmp	cl,'1'
	jne	@@Not_AND1
	;----- AND1
	mov	al,04Ah
	jmp	NOT_DONE
@@Not_AND1:
@@Not_ANDx:

	cmp	ax,'LS'		; ASL?
	jne	@@Not_ASL
	;----- ASL
	mov	si,OFFSET ASL_TABLE
	jmp	GROUPI

@@Not_ASL:
	jmp	parse_pseudo
PARSE_A	ENDP

;------------------------------------------------------------------------------

PARSE_B	PROC	NEAR

	cmp	ax,'CB'		; BBC?
	jne	@@Not_BBC
	;----- BBC
	mov	al,003
	jmp	NOT_DONE
@@Not_BBC:

	cmp	ax,'SB'		; BBS?
	jne	@@Not_BBS
	;----- BBS
	mov	al,003h
	jmp	NOT_DONE
@@Not_BBS:

	cmp	ax,'CC'		; BCC?
	jne	@@Not_BCC
	;----- BCC
	mov	al,090h
	jmp	GROUPIII
@@Not_BCC:

	cmp	ax,'SC'		; BCS?
	jne	@@Not_BCS
	;----- BCS
	mov	al,0B0h
	jmp	GROUPIII
@@Not_BCS:

	cmp	ax,'QE'		; BEQ?
	jne	@@Not_BEQ
	;----- BEQ
	mov	al,0F0h
	jmp	GROUPIII
@@Not_BEQ:

	cmp	ax,'IM'		; BMI?
	jne	@@Not_BMI
	;----- BMI
	mov	al,030h
	jmp	GROUPIII
@@Not_BMI:

	cmp	ax,'EN'		; BNE?
	jne	@@Not_BNE
	;----- BNE
	mov	al,0D0h
	jmp	GROUPIII
@@Not_BNE:

	cmp	ax,'LP'		; BPL?
	jne	@@Not_BPL
	;----- BPL
	mov	al,010h
	jmp	GROUPIII
@@Not_BPL:

	cmp	ax,'AR'		; BRA?
	jne	@@Not_BRA
	;----- BRA
	mov	al,02Fh
	jmp	GROUPIII
@@Not_BRA:

	cmp	ax,'KR'		; BRK?
	jne	@@Not_BRK
	;----- BRK
	mov	al,00Fh
	jmp	ISC
@@Not_BRK:

	cmp	ax,'CV'		; BVC?
	jne	@@Not_BVC
	;----- BVC
	mov	al,050h
	jmp	GROUPIII
@@Not_BVC:

	cmp	ax,'SV'		; BVS?
	jne	@@Not_BVS
	;----- BVS
	mov	al,070h
	jmp	GROUPIII
@@Not_BVS:

	JMP	PARSE_PSEUDO

PARSE_B	ENDP

;------------------------------------------------------------------------------

PARSE_C	PROC	NEAR

	cmp	ax,'PM'		; CMPx?
	jne	@@Not_CMPx
	or	cl,cl		; CMP?
	jnz	@@Not_CMP
	;----- CMP
	mov	si,OFFSET CMP_TABLE
	jmp	GROUPII
@@Not_CMP:
	cmp	cl,'W'		; CMPW?
	jne	@@Not_CMPW
	;----- CMPW
	mov	al,05Ah
	jmp	NOT_DONE
@@Not_CMPW:
@@Not_CMPx:

	cmp	ax,'LA'		; CALL?
	jne	@@Not_CALL
	cmp	cl,'L'
	jne	@@Not_CALL
	;----- CALL
	mov	al,03Fh
	jmp	NOT_DONE
@@Not_CALL:

	cmp	ax,'RL'		; CLRx?
	jne	@@Not_CLRx
	cmp	cl,'1'		; CLR1?
	jne	@@Not_CLR1
	;----- CLR1
	mov	al,002h
	jmp	NOT_DONE
@@Not_CLR1:

	cmp	cl,'C'		; CLRC?
	jne	@@Not_CLRC
	;----- CLRC
	mov	al,060h
	jmp	ISC
@@Not_CLRC:

	cmp	cl,'P'		; CLRP?
	jne	@@Not_CLRP
	;----- CLRP
	mov	al,020h
	jmp	ISC
@@Not_CLRP:

	cmp	cl,'V'		; CLRV?
	jne	@@Not_CLRV
	;----- CLRV
	mov	al,0E0h
	jmp	ISC
@@Not_CLRV:
@@Not_CLRx:

	cmp	ax,'NB'		; CBNE?
	jne	@@Not_CNBE
	cmp	cl,'E'
	jne	@@Not_CNBE
	;----- CNBE
	mov	al,02Eh
	jmp	NOT_DONE
@@Not_CNBE:

	JMP	PARSE_PSEUDO

PARSE_C	ENDP

;------------------------------------------------------------------------------

PARSE_D	PROC	NEAR

	cmp	ax,'CE'		; DECx?
	jne	@@Not_DECx

	or	cl,cl
	jne	@@Not_DEC
	;----- DEC
	mov	si,OFFSET DEC_TABLE
	jmp	GROUPI
@@Not_DEC:
	cmp	cl,'W'		; DECW?
	jne	@@Not_DECW
	;----- DECW
	mov	al,01Ah
	jmp	NOT_DONE
@@Not_DECW:
@@Not_DECx:

	cmp	ax,'VI'		; DIV?
	jne	@@Not_DIV
	;----- DIV
	mov	al,09Eh
	jmp	NOT_DONE
@@Not_DIV:

	cmp	al,'I'		; DI?
	jne	@@Not_DI
	;----- DI
	mov	al,0C0h
	jmp	ISC
@@Not_DI:

	cmp	ax,'AA'		; DAA?
	jne	@@Not_DAA
	;----- DAA
	mov	al,0DFh
	jmp	GROUPVIII
@@Not_DAA:

	cmp	ax,'SA'		; DAS?
	jne	@@Not_DAS
	;----- DAS
	mov	al,0BEh
	jmp	GROUPVIII
@@Not_DAS:

	cmp	ax,'NB'		; DBNZ?
	jne	@@Not_DBNZ
	cmp	cl,'Z'
	jne	@@Not_DBNZ
	;----- DBNZ
	mov	al,06Eh
	jmp	NOT_DONE
@@Not_DBNZ:

	JMP	PARSE_PSEUDO

PARSE_D	ENDP

;------------------------------------------------------------------------------

PARSE_E	PROC	NEAR

	cmp	ax,'RO'		; EOR?
	jnz	@@Not_EOR

	cmp	cl,'1'
	jnz	@@Not_EOR1
	;----- EOR1
	mov	al,08Ah
	jmp	NOT_DONE
@@Not_EOR1:
	or	cl,cl
	jnz	@@Not_EOR
	;----- EOR
	mov	si,OFFSET EOR_TABLE
	jmp	GROUPII
@@Not_EOR:

	cmp	al,'I'		; EI?
	jnz	@@Not_EI
	mov	al,0A0h
	jmp	ISC
@@Not_EI:

	JMP	PARSE_PSEUDO

PARSE_E	ENDP

;------------------------------------------------------------------------------

PARSE_I	PROC	NEAR

	cmp	ax,'CN'		; INCx?
	jne	@@Not_INCx

	or	cl,cl		; INC?
	jnz	@@Not_INC
	;----- INC
	mov	si,OFFSET INC_TABLE
	jmp	GROUPI
@@Not_INC:

	cmp	cl,'W'		; INCW?
	jnz	@@Not_INCW
	;----- INCW
	mov	al,03Ah
	jmp	NOT_DONE
@@Not_INCW:
@@Not_INCx:

	JMP	PARSE_PSEUDO
PARSE_I	ENDP

;------------------------------------------------------------------------------

PARSE_J	PROC	NEAR

	cmp	ax,'PM'		; JMP?
	jne	@@Not_JMP
	;----- JMP
	mov	al,05Fh
	jmp	NOT_DONE
@@Not_JMP:

	JMP	PARSE_PSEUDO

PARSE_J	ENDP

;------------------------------------------------------------------------------

PARSE_L	PROC	NEAR

	cmp	ax,'RS'		; LSR?
	jne	@@Not_LSR
	;----- LSR
	mov	si,OFFSET LSR_TABLE
	jmp	GROUPI
@@Not_LSR:

	JMP	PARSE_PSEUDO
PARSE_L	ENDP

;------------------------------------------------------------------------------

PARSE_M	PROC	NEAR

	cmp	ax,'VO'		; MOVx?
	jne	@@Not_MOVx

	or	cl,cl		; MOV?
	jnz	@@Not_MOV
	;----- MOV
	mov	si,OFFSET MOV_TABLE
	jmp	GROUPII
@@Not_MOV:

	cmp	cl,'W'		; MOVW?
	jne	@@Not_MOVW
	;----- MOVW
	mov	al,0BAh
	jmp	NOT_DONE
@@Not_MOVW:

	cmp	cl,'1'		; MOV1?
	jne	@@Not_MOV1
	;----- MOV1
	mov	al,0AAh
	jmp	NOT_DONE
@@Not_MOV1:
@@Not_MOVx:

	cmp	ax,'LU'		; MUL?
	jne	@@Not_MUL
	;----- MUL
	mov	al,0CFh
	jmp	NOT_DONE
@@Not_MUL:

	JMP	PARSE_PSEUDO

PARSE_M	ENDP

;------------------------------------------------------------------------------

PARSE_N	PROC	NEAR

	cmp	ax,'PO'		; NOP?
	jne	@@Not_NOP
	;----- NOP
	mov	al,00h
	jmp	ISC
@@Not_NOP:

	cmp	ax,'TO'		; NOT1?
	jne	@@Not_NOT1
	cmp	cl,'1'
	jne	@@Not_NOT1
	;----- NOT1
	mov	al,0EAh
	jmp	NOT_DONE
@@Not_NOT1:
	cmp	cl,'C'		; NOTC?
	jne	@@Not_NOTC
	;----- NOTC
	mov	al,0EDh
	jmp	ISC
@@Not_NOTC:

	JMP	PARSE_PSEUDO

PARSE_N	ENDP

;------------------------------------------------------------------------------

PARSE_O	PROC	NEAR

	cmp	ax,'1R'		; OR1?
	jne	@@Not_OR1
	;----- OR1
	mov	al,0Ah
	jmp	NOT_DONE
@@Not_OR1:
	cmp	al,'R'		; OR?
	jne	@@Not_OR
	;----- OR
	mov	si,OFFSET OR_TABLE
	jmp	GROUPII
@@Not_OR:

	JMP	PARSE_PSEUDO

PARSE_O	ENDP

;------------------------------------------------------------------------------

PARSE_P	PROC	NEAR

	cmp	ax,'PO'		; POP?
	jne	@@Not_POP
	;----- POP
;	mov	si,OFFSET POP_TABLE
	jmp	GROUPI
@@Not_POP:

	cmp	ax,'SU'		; PUSH?
	jne	@@Not_PUSH
	cmp	cl,'H'
	jne	@@Not_PUSH
	;----- PUSH
;	mov	si,OFFSET PUSH_TABLE
	jmp	GROUPI
@@Not_PUSH:

	cmp	ax,'AC'		; PCALL?
	jne	@@Not_PCALL
	cmp	cx,'LL'
	jne	@@Not_PCALL
	;----- PCALL
	mov	al,04Fh
	jmp	NOT_DONE
@@Not_PCALL:

	JMP	PARSE_PSEUDO

PARSE_P	ENDP

;------------------------------------------------------------------------------

PARSE_R	PROC	NEAR

	cmp	ax,'TE'		; RETx?
	jne	@@Not_RETx

	or	cl,cl		; RET?
	jnz	@@Not_RET
	;----- RET
	mov	al,06Fh
	jmp	ISC
@@Not_RET:
	cmp	cl,'I'		; RETI?
	jne	@@Not_RETI
	;----- RETI
	mov	al,07Fh
	jmp	ISC
@@Not_RETI:
@@Not_RETx:

	cmp	ax,'LO'		; ROL?
	jne	@@Not_ROL
	;----- ROL
	mov	si,OFFSET ROL_TABLE
	jmp	GROUPI
@@Not_ROL:

	cmp	ax,'RO'		; ROR?
	jne	@@Not_ROR
	;----- ROR
	mov	si,OFFSET ROR_TABLE
	jmp	GROUPI
@@Not_ROR:

	JMP	PARSE_PSEUDO

PARSE_R	ENDP

;------------------------------------------------------------------------------

PARSE_S	PROC	NEAR

	cmp	ax,'CB'		; SBC?
	jne	@@Not_SBC
	;----- SBC
	mov	si,OFFSET SBC_TABLE
	jmp	GROUPII
@@Not_SBC:

	cmp	ax,'BU'		; SUBW?
	jne	@@Not_SUBW
	cmp	cl,'W'
	jne	@@Not_SUBW
	;----- SUBW
	mov	al,09Ah
	jmp	NOT_DONE
@@Not_SUBW:

	cmp	ax,'TE'		; SETx?
	jne	@@Not_SETx

	cmp	cl,'1'		; SET1?
	jne	@@Not_SET1
	;----- SET1
	mov	al,002
	jmp	NOT_DONE
@@Not_SET1:

	cmp	cl,'C'		; SETC?
	jne	@@Not_SETC
	;----- SETC
	mov	al,080h
	jmp	ISC
@@Not_SETC:

	cmp	cl,'P'		; SETP?
	jne	@@Not_SETP
	;----- SETP
	mov	al,040h
	jmp	ISC
@@Not_SETP:
@@Not_SETx:

	cmp	ax,'OT'		; STOP?
	jne	@@Not_STOP
	cmp	cl,'P'
	jne	@@Not_STOP
	;----- STOP
	mov	al,0FFh
	jmp	ISC
@@Not_STOP:

	cmp	ax,'EL'		; SLEEP?
	jne	@@Not_SLEEP
	cmp	cx,'PE'
	jne	@@Not_SLEEP
	;----- SLEEP
	mov	al,0EFh
	jmp	ISC
@@Not_SLEEP:

	JMP	PARSE_PSEUDO

PARSE_S	ENDP

;------------------------------------------------------------------------------

PARSE_T	PROC	NEAR

	cmp	ax,'AC'		; TCALL?
	jne	@@Not_TCALL
	cmp	cx,'LL'
	jne	@@Not_TCALL
	;----- TCALL
	mov	al,001h
	jmp	NOT_DONE
@@Not_TCALL:

	cmp	ax,'ES'		; TSET1?
	jne	@@Not_TSET1
	cmp	cx,'1T'
	jne	@@Not_TSET1
	;----- TSET1
	mov	al,00Eh
	jmp	NOT_DONE
@@Not_TSET1:

	cmp	ax,'LC'		; TCLR1?
	jne	@@Not_TCLR1
	cmp	cx,'1R'
	jne	@@Not_TCLR1
	;----- TCLR1
	mov	al,04Eh
	jmp	NOT_DONE
@@Not_TCLR1:

	JMP	PARSE_PSEUDO

PARSE_T	ENDP

;------------------------------------------------------------------------------

PARSE_X	PROC	NEAR
	cmp	ax,'NC'		; XCN?
	jne	@@Not_XCN
	;----- XCN
	mov	al,09Fh
	jmp	GROUPVIII
@@Not_XCN:

	JMP	PARSE_PSEUDO

PARSE_X	ENDP

;==============================================================================


WriteCodeBytes	PROC

	;----- Retrieve opcode from table
	dec	dl
	dec	dh

	mov	al,18		; number of addressing modes
	mul	dh
	xor	dh,dh
	add	ax,dx
	add	si,ax
	mov	al,[si]
	call	store_byte	; opcode

	xor	bx,bx
	xor	cx,cx
	mov	bl,al		; offset into # of bytes table
	mov	cl,[OPCODE_BYTES+bx]
	dec	cx		; FIX TABLE!

	jcxz	@@DoneWriteOperand
	; cx = # of bytes to write out from buffer
	push	ds
	pop	es
	mov	si,offset OPERAND_BYTES
	call	StoreBin
@@DoneWriteOperand:

	ret

WriteCodeBytes	ENDP


; 1 operand opcodes
GROUPI	PROC	NEAR

	mov	am,0
	call	_cam_ad
	mov	sam,al
	mov	dx,am
	call	WriteCodeBytes
	ret

GROUPI	ENDP


; 2 operand opcodes
GROUPII	PROC	NEAR

	call	cam_ad
	call	WriteCodeBytes
	ret

GROUPII	ENDP

;==============================================================================
; bra,beq,bne,bcs,bcc,bvs,bvc,bmp,bpl
;------------------------------------------------------------------------------

GROUPIII	PROC	NEAR	;BRANCHES

	CALL	CAM_R
	CMP	DL,15
	jne	GIII2
	CALL	STORE_BYTE	;STORE OPCODE
	MOV	AL,CL
	JMP	STORE_BYTE	;8-BIT ADDRESS

GIII2:	MOV	AX,9		;ILLEGAL ADDRESSING MODE
	JMP	ERROR

GROUPIII	ENDP


; Must be accumulator mode
GROUPVIII	PROC

	call	cam_ad
	cmp	dl,5		; ACCUMULATOR
	jne	GVIII2
	jmp	store_byte	; opcode
GVIII2:
	mov	ax,9
	jmp	error

GROUPVIII	ENDP

;==============================================================================

ISC	PROC	NEAR		;"IMPLIED" SHORTCUT

	CMP	[PARAM_FIELD],0	;DID USER TRY TO ADD A PARAMETER?
	JZ	ISC1
	MOV	AX,2
	JMP	ERROR
ISC1:	JMP	STORE_BYTE

ISC	ENDP

;==============================================================================

NOT_DONE	PROC	NEAR

	RET

NOT_DONE	ENDP

