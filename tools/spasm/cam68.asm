;==============================================================================
; cam68.asm: check addressing mode, addressing mode validation routines for 68000
;==============================================================================

.CODE
;==============================================================================

CAM_A	PROC	NEAR
	lea	bx,Uppercase

	MOV	AX,DI	;CHECK LEN=2
	SUB	AX,SI   ;
	CMP	AX,1    ;
	JNZ	@@CA1     ;

	MOV	AX,WORD PTR [PARAM_FIELD+SI]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah

	CMP	AX,'PS'
	JNZ	@@CA0
	MOV	AX,'7A'
@@CA0:
	CMP	AL,'A'
	JNZ	@@CA1
	CMP	AH,'0'
	JC	@@CA1
	CMP	AH,'7'
	JA	@@CA1
	SUB	AH,30H
	MOV	AREG,AH
	CLC
	RET
@@CA1:
	STC
	RET
CAM_A	ENDP

;==============================================================================

CAM_CCR	PROC	NEAR
	lea	bx,Uppercase

	MOV	AX,WORD PTR [PARAM_FIELD+SI]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah

	cmp	ax,'CC'
	JNZ	@@CC2

	mov	al,[PARAM_FIELD+SI+2]
	xlatb
	cmp	al,'R'
	JNZ	@@CC2
;	CMP	[PARAM_FIELD+SI],'C'
;	JNZ	@@CC2
;	CMP	[PARAM_FIELD+SI+1],'C'
;	JNZ	@@CC2
;	CMP	[PARAM_FIELD+SI+2],'R'
;	JNZ	@@CC2
	CMP	[PARAM_FIELD+SI+3],0
	JZ	@@CC1
	CMP	[PARAM_FIELD+SI+3],','
	JNZ	@@CC2
@@CC1:
	CLC
	RET
@@CC2:
	STC
	RET
CAM_CCR	ENDP

;------------------------------------------------------------------------------

CAM_SR	PROC	NEAR
	lea	bx,Uppercase

	MOV	AX,WORD PTR [PARAM_FIELD+SI]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah

	cmp	ax,'RS'
	JNZ	@@SR2

;	CMP	[PARAM_FIELD+SI],'S'
;	JNZ	@@SR2
;	CMP	[PARAM_FIELD+SI+1],'R'
;	JNZ	@@SR2
	CMP	[PARAM_FIELD+SI+2],0
	JZ	@@SR1
	CMP	[PARAM_FIELD+SI+2],','
	JNZ	@@SR2
@@SR1:
	CLC
	RET
@@SR2:
	STC
	RET
CAM_SR	ENDP

;------------------------------------------------------------------------------

CAM_USP	PROC	NEAR
	lea	bx,Uppercase

	MOV	AX,WORD PTR [PARAM_FIELD+SI]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah

	CMP	ax,'SU'
	JNZ	@@USP2

	MOV	al,[PARAM_FIELD+SI+2]
	xlatb
	CMP	al,'P'
	JNZ	@@USP2

;	CMP	[PARAM_FIELD+SI],'U'
;	JNZ	@@USP2
;	CMP	[PARAM_FIELD+SI+1],'S'
;	JNZ	@@USP2
;	CMP	[PARAM_FIELD+SI+2],'P'
;	JNZ	@@USP2
	CMP	[PARAM_FIELD+SI+3],0
	JZ	@@USP1
	CMP	[PARAM_FIELD+SI+3],','
	JNZ	@@USP2
@@USP1:
	CLC
	RET
@@USP2:
	STC
	RET
CAM_USP	ENDP

;==============================================================================

CAM_D	PROC	NEAR
	lea	bx,Uppercase

	MOV	AX,DI	;CHECK LEN=2
	SUB	AX,SI   ;
	CMP	AX,1    ;
	JNZ	@@CD1     ;

	MOV	AX,WORD PTR [PARAM_FIELD+SI]
	xlatb
	CMP	AL,'D'
	JNZ	@@CD1
	CMP	AH,'0'
	JC	@@CD1
	CMP	AH,'7'
	JA	@@CD1
	SUB	AH,30H
	MOV	DREG,AH
	CLC
	RET
@@CD1:
	STC
	RET
CAM_D	ENDP

;==============================================================================

CAM_EA	PROC	NEAR		;CHECK ADDRESSING MODE FOR EFFECTIVE ADDRESS
	lea	bx,Uppercase

	MOV	ax,WORD PTR [PARAM_FIELD+SI+2]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah
	push	ax

	MOV	AX,WORD PTR [PARAM_FIELD+SI]	;STORE PIECES IN REG'S
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah
	pop	bx

	MOV	BP,DI				;SET BP TO FIELD LEN
	SUB	BP,SI

	CMP	AL,'#'				;IMMEDIATE?
	JNZ	@@CEA
	JMP	@@CEA7_4

@@CEA:
	CMP	BP,1
	JNZ	@@CEA2
	CMP	AL,'D'			;000?
	JNZ	@@CEA1
	CMP	AH,'0'
	JC	@@I5
	CMP	AH,'7'
	JA	@@I5
	XOR	AH,30H
	MOV	MOD_REG,AH
	MOV	AMQR,0
	CLC
	RET

@@CEA1:
	CMP	AX,'PS'
	JNZ	@@CEA1_1
	MOV	AX,'7A'
@@CEA1_1:
	CMP	AL,'A'			;001?
	JNZ	@@I5
	CMP	AH,'0'
	JC	@@I5
	CMP	AH,'7'
	JA	@@I5
	XOR	AH,38H
	MOV	MOD_REG,AH
	MOV	AMQR,1
	CLC
	RET

@@I5:
	JMP	@@CEA5			;INTERSTATE 5

@@CEA2:
	CMP	BP,3
	JNZ	@@CEA3
	CMP	AH,'S'
	JNZ	@@CEA2_1
	CMP	BL,'P'
	JNZ	@@CEA2_1
	MOV	AH,'A'
	MOV	BL,'7'
@@CEA2_1:
	CMP	AX,'A('			;010?
	JNZ	@@I5
	CMP	BH,')'
	JNZ	@@I5
	CMP	BL,'0'
	JC	@@I5
	CMP	BL,'7'
	JA	@@I5
	XOR	BL,20H
	MOV	MOD_REG,BL
	MOV	AMQR,2
	CLC
	RET

@@CEA3:
	CMP	BP,4
	JNZ	@@CEA5
	CMP	AH,'S'
	JNZ	@@CEA3_1
	CMP	BL,'P'
	JNZ	@@CEA3_1
	MOV	AH,'A'
	MOV	BL,'7'
@@CEA3_1:
	CMP	AX,'A('			;011?
	JNZ	@@CEA4
	CMP	BH,')'
	JNZ	@@CEA5
	CMP	[PARAM_FIELD+SI+4],'+'
	JNZ	@@CEA5
	CMP	BL,'0'
	JC	@@CEA5
	CMP	BL,'7'
	JA	@@CEA5
	XOR	BL,28H
	MOV	MOD_REG,BL
	MOV	AMQR,3
	CLC
	RET

@@CEA4:
	CMP	AX,'(-'			;100?
	JNZ	@@CEA5
	CMP	BX,'PS'
	JNZ	@@CEA4_1
	MOV	BX,'7A'
@@CEA4_1:
	CMP	BL,'A'
	JNZ	@@CEA5
	CMP	[PARAM_FIELD+SI+4],')'
	JNZ	@@CEA5
	CMP	BH,'0'
	JC	@@CEA5
	CMP	BH,'7'
	JA	@@CEA5
	XOR	BH,10H
	MOV	MOD_REG,BH
	MOV	AMQR,4
	CLC
	RET

@@CEA5:
	CMP	BP,4
	JC	@@I71
	MOV	AX,WORD PTR [PARAM_FIELD+DI-2]
	lea	bx,Uppercase
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah
	MOV	BX,WORD PTR [PARAM_FIELD+DI-5]
	CMP	[PARAM_FIELD+DI],')'	;101?
	JNZ	@@I71
	CMP	[PARAM_FIELD+DI-3],'('
	JNZ	@@CEA6
	CMP	AX,'CP'
	JNZ	@@CEA5X
	JMP	@@CEA7_2			;
@@CEA5X:
	; kts added 8/9/92
	cmp	ax,'PS'
	jnz	@@NotSP
	mov	ax,'7A'
@@NotSP:
	; end kts addition
	CMP	AH,'0'
	JC	@@CEA6
	CMP	AH,'7'
	JA	@@CEA6
	XOR	AH,18H
	MOV	MOD_REG,AH
	SUB	DI,4
	CALL	PARSE_EXP
	MOV	XW1,CX
	MOV	AMQR,5
	CLC
	RET

@@I71:
	JMP	@@CEA7_1

@@CEA6:
	MOV	DX,7		;110?
	CMP	BP,6
	JC	@@I71
	MOV	XW1,0
	MOV	CL,[PARAM_FIELD+DI-6]
	CMP	BL,','
	JNZ	@@CEA6Y
	CMP	AX,'W.'
	JZ	@@CEA6X
	CMP	AX,'L.'
	JNZ	@@I71
	MOV	XW1,800H
@@CEA6X:
	lea	bx,Uppercase
	MOV	ax,WORD PTR [PARAM_FIELD+DI-7]
	xlatb
	xchg    al,ah
	xlatb
	xchg	al,ah

	push	ax
	MOV	AX,WORD PTR [PARAM_FIELD+DI-4]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah
	pop	bx
	MOV	CL,[PARAM_FIELD+DI-8]
	MOV	DX,9
	JMP	@@CEA6Z
@@CEA6Y:
	CMP	[PARAM_FIELD+DI-3],','
	JNZ	@@CEA7_1
@@CEA6Z:
	CMP	CL,'('
	JNZ	I80
	CMP	BX,'CP'
	JZ	I73
	; kts added 8/9/92
	cmp	bx,'PS'
	jnz	@@NotSP2
	mov	bx,'7A'
@@NotSP2:
	; end kts addition

	CMP	BL,'A'
	JNZ	I80
	CMP	BH,'0'
	JC	I80
	CMP	BH,'7'
	JA	I80
	CMP	AL,'D'
	JZ	@@CEA6Q
	CMP	AL,'A'
	JNZ	I80
	OR	XW1,8000H
@@CEA6Q:
	CMP	AH,'0'
	JC	I80
	CMP	AH,'7'
	JA	I80
	MOV	MOD_REG,BH
	AND	AH,7
	ROR	AH,1
	ROR	AH,1
	ROR	AH,1
	ROR	AH,1
	OR	BYTE PTR [XW1+1],AH
	SUB	DI,DX
	JC	@@CEA6T
	XOR	CL,CL
	CMP	BP,9
	JNC	@@NOFIX
	CMP	[PARAM_FIELD+SI],'('
	JZ	@@FIX
@@NOFIX:
	CALL	PARSE_EXP
@@FIX:
	OR	BYTE PTR [XW1],CL
@@CEA6T:
	MOV	AMQR,6
        CLC
        RET

I73:
	JMP	@@CEA7_3
I80:
	JMP	@@CEA8		;INTERSTATE 80

@@CEA7_1:
	CALL	PARSE_EXP	;111.001
	MOV	XW1,BX
	MOV	XW2,CX
	MOV	MOD_REG,111001B
	MOV	AMQR,7
	CLC
	RET

@@CEA7_2:
	SUB	DI,4		;111.010
	CALL	PARSE_EXP
	SUB	CX,2		;PC SEXO THINGY
	SUB	CX,[OBJPTRL]
	MOV	XW1,CX
	MOV	MOD_REG,111010B
	MOV	AMQR,9
	CLC
	RET

@@CEA7_3:
	CMP	AL,'D'		;111.011
	JZ	@@CEA7Q
	CMP	AL,'A'
	JNZ	I80
	OR	XW1,8000H
@@CEA7Q:
	CMP	AH,'0'
	JC	@@CEA8
	CMP	AH,'7'
	JA	@@CEA8
	AND	AH,7
	ROR	AH,1
	ROR	AH,1
	ROR	AH,1
	ROR	AH,1
	OR	BYTE PTR [XW1+1],AH
	SUB	DI,DX
	CALL	PARSE_EXP
	SUB	CL,2				;PC SEXO THINGY
	SUB	CL,BYTE PTR [OBJPTRL]
	OR	BYTE PTR [XW1],CL
	MOV	MOD_REG,111011B
	MOV	AMQR,10
        CLC
        RET

@@CEA7_4:
	ADD	SI,1		;111.100
	CALL	PARSE_EXP
	MOV	XW1,CX
	CMP	SEYES,80H		; is it long?
	JNZ	@@CEA7_4X
	MOV	XW1,BX
	MOV	XW2,CX
	JMP	@@CEA7_4Y

@@CEA7_4X:
					; insure upper word is zero
	OR	BX,BX
	JNZ	@@CEA7_4ZZ
	CMP	SEYES,0			; is it byte
	JNZ	@@CEA7_4Y
	OR	CH,CH
	JNZ	@@CEA7_4Z
@@CEA7_4Y:
	MOV	MOD_REG,111100B
	MOV	AMQR,8
	CLC
	RET


@@CEA7_4ZZ:			; check if upper word is ffffh
	cmp	bx,0ffffh
	jnz	@@CEA7_4Z		; nope, is really an error
	test	ch,080h		; since negative, insure uppermost
				; bit of lower word is also on
	jnz	@@CEA7_4Y		; no error
				; otherwise, fall through to error

@@CEA7_4Z:
	MOV	AX,6		;ILLEGAL SIZE FOR OPERAND
	CALL	ERROR
	JMP	@@CEA7_4Y

@@CEA8:	STC
	RET

CAM_EA	ENDP

;==============================================================================

CAM_I	PROC	NEAR		;CHECK ADDRESSING MODE FOR IMMEDIATE
				;CF->CLR = YES
				;VALUE IN IDWH:IDWL

	CMP	[PARAM_FIELD+SI],'#'
	JNZ	@@CI
	INC	SI
	CALL	PARSE_EXP
	MOV	IDWH,BX
	MOV	IDWL,CX
	CLC
	RET
@@CI:
	STC
	RET

CAM_I	ENDP

;=============================================================================

CAM_M	PROC	NEAR		;CHECK ADDRESSING MODE FOR MULITPLE REGISTERS

	lea	bx,Uppercase
	MOV	RLM,0
@@M:
	MOV	al,[PARAM_FIELD+SI]
	xlatb
	mov	ch,al
	CMP	ch,'A'
	JZ	@@M1
	CMP	ch,'D'
	JZ	@@M1
@@M0:
	STC
	RET
@@M1:
	MOV	ah,[PARAM_FIELD+SI+1]
	CMP	ah,'0'
	JC	@@M0
	CMP	ah,'7'
	JA	@@M0
	MOV	CL,[PARAM_FIELD+SI+2]
	CMP	CL,'/'
	JZ	@@M3
	OR	CL,CL
	JZ	@@M3
	CMP	CL,','
	JZ	@@M3
	CMP	CL,'-'
	JNZ	@@M0
	mov	al,[PARAM_FIELD+SI+3]
	xlatb
	CMP	ch,al
	JNZ	@@M0
	MOV	DL,[PARAM_FIELD+SI+4]
	CMP	DL,'0'
	JC	@@M0
	CMP	DL,'7'
	JA	@@M0
	CMP	DL,ch
	JAE	@@M0
@@M2:
	CALL	@@M5
	INC	ah
	CMP	ah,DL
	JBE	@@M2
	ADD	SI,6
@@M2X:
	CMP	SI,DI
	JBE	@@M
	CLC
	RET

@@M3:
	CALL	@@M5
	ADD	SI,3
	JMP	@@M2X

;-----------------------------------------------------------------------------

@@M5:
	MOV	CL,ah
	AND	CL,7
	INC	CL
	CMP	ch,'A'
	JNZ	@@M6
	ADD	CL,8
@@M6:
	XOR	BP,BP
	STC
	RCL	BP,CL
	OR	RLM,BP
	RET

;=============================================================================

RRLM:						; called from opcode68
	XOR	BX,BX
	MOV	CX,10H
@@THE_LOOP:
	SHL	AX,1
	RCR	BX,1
	LOOP	@@THE_LOOP
	MOV	AX,BX
	RET

CAM_M	ENDP

;------------------------------------------------------------------------------

CAM_R	PROC	NEAR		;CHECK ADDRESSING MODE FOR RELATIVE DISPLACEMENT

	CALL	PARSE_EXP
	TEST	SDB1,4		;IF FWD REF THEN DON'T COMPLAIN
	JNZ	@@C4
	MOV	AX,OBJPTRL	;CALCULATE RELATIVE OFFSET
	ADD	AX,2
	SBB	BX,0
	SUB	CX,AX
	SBB	BX,OBJPTRH
	CMP	BX,0FFFFH
	JZ	@@C2
        OR	BL,BL
	JZ	@@C3

@@C1:
	MOV	AX,8		;VIVA LA SPAGHETTI!
	CALL	ERROR
	JMP	@@C4

@@C2:
	CMP	CX,8000H
	JB	@@C1
	JMP	@@C5

@@C3:
	CMP	CX,7FFFH
	JA	@@C1
	JMP	@@C5

@@C4:
	MOV     RDW,4E71H
	RET

@@C5:
	MOV	RDW,CX
	RET

CAM_R	ENDP

;------------------------------------------------------------------------------

CAM_RS	PROC	NEAR		;CAMARO RALLY SPORT

	CALL	PARSE_EXP
	TEST	SDB1,4		;IF FWD REF THEN DON'T COMPLAIN
	JNZ	@@X4
	MOV	AX,OBJPTRL	;CALCULATE RELATIVE OFFSET
	ADD	AX,2			; move forward to next instruction,
	ADC	BX,0			; since relative branches are calculated from there
	SUB	CX,AX
	SBB	BX,OBJPTRH		; bx:cx now contains delta
	CMP	BX,0FFFFH
	JZ	@@X2
	OR	BX,BX
	JZ	@@X3

@@X1:
	MOV	AX,8		;VIVA LA SPAGHETTI!
	CALL	ERROR
@@X4:
	MOV     RDB,0FEH	; ??
	RET

@@X2:
	CMP	CX,0FF80H
	JB	@@X1
@@X5:				; its inside of range
	MOV	RDB,cl
	OR	CX,CX		; make sure it isn't an offset of zero, since than isn't allowed
	JZ	@@X1
	RET

@@X3:
	CMP	CX,7FH
	JA	@@X1
	JMP	@@X5

CAM_RS	ENDP

;------------------------------------------------------------------------------

CCC	PROC	NEAR		;RETURN VALUE 0 -> 15 IN DL
				;(DL=16)=INVALID
	XOR	DL,DL
	CMP	AX,'T'
	JZ	@@CCCP
	INC	DL
	CMP	AX,'F'
        JZ      @@CCCP
        INC     DL
        CMP     AX,'IH'
        JZ      @@CCCP
        INC     DL
        CMP     AX,'SL'
        JZ      @@CCCP
        INC     DL
        CMP     AX,'CC'
        JZ      @@CCCP
        CMP     AX,'SH'
        JZ      @@CCCP
        INC     DL
        CMP     AX,'SC'
        JZ      @@CCCP
        CMP     AX,'OL'
        JZ      @@CCCP
        INC     DL
        CMP     AX,'EN'
        JZ      @@CCCP
	INC	DL
        CMP     AX,'QE'
	JZ	@@CCCP
	INC	DL
	CMP	AX,'CV'
	JZ	@@CCCP
	INC	DL
	CMP	AX,'SV'
	JZ	@@CCCP
	INC	DL
	CMP	AX,'LP'
	JZ	@@CCCP
	INC	DL
	CMP	AX,'IM'
	JZ	@@CCCP
	INC	DL
	CMP	AX,'EG'
	JZ	@@CCCP
	INC	DL
	CMP	AX,'TL'
	JZ	@@CCCP
	INC	DL
	CMP	AX,'TG'
	JZ	@@CCCP
	INC	DL
	CMP	AX,'EL'
	JZ	@@CCCP
	INC	DL
@@CCCP:
	RET

CCC	ENDP

;==============================================================================

