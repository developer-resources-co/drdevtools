;=======]======================================================================
; cam816.asm: check/determine  addressing mode, addressing mode validation routines
;=============================================================================

DATA	SEGMENT	'DATA'

AM8		DB	0
AM16		DB	0
AM24		DB	0
DATA	ENDS

;=============================================================================

.code

ADDRESSING_MODES ENUM {
	ADDRMODE_IMMEDIATE = 1			;Immediate addressing			#
	ADDRMODE_ABSOLUTE			;Absolute				a
	ADDRMODE_ABSOLUTELONG			;Absolute Long				al
	ADDRMODE_DIRECT				;Direct					d
	ADDRMODE_ACCUMULATOR			;Accumulator				A
	ADDRMODE_IMPLIED			;Implied					i
	ADDRMODE_DIRECTINDIRECTINDEXED		;Direct Indirect Indexed			(d),y
	ADDRMODE_DIRECTINDIRECTLONGINDEXED	;Direct Indirect Long Indexed		[d],y
	ADDRMODE_DIRECTINDEXEDINDIRECT		;Direct Indexed Indirect			(d,x)
	ADDRMODE_DIRECTINDEXEDX			;Direct Indexed with X			d,x
	ADDRMODE_DIRECTINDEXEDY			;Direct Indexed with Y			d,y
	ADDRMODE_ABSOLUTEINDEXEDX		;Absolute Indexed with X			a,x
	ADDRMODE_ABSOLUTELONGINDEXEDX		;Absolute Long Indexed with X		al,x
	ADDRMODE_ABSOLUTEINDEXEDY		;Absolute Indexed with Y			a,y
	ADDRMODE_PCRELATIVE			;Program Counter Relative 		r
	ADDRMODE_PCRELATIVELONG			;Program Counter Relative Long		rl
	ADDRMODE_ABSOLUTEINDRECT		;Absolute Indirect			(a)
	ADDRMODE_DIRECTINDIRECT			;Direct Indirect				(d)
	ADDRMODE_DIRECTINDIRECTLONG		;Direct Indirect Long			[d]
	ADDRMODE_ABSOULTEINDEXEDINDIRECT	;Absolute Indexed Indirect		(a,x)
	ADDRMODE_STACK				;Stack					s
	ADDRMODE_STACKRELATIVE			;Stack Relative				d,s
	ADDRMODE_STACKRELATIVEINDIRECTINDEXED	;Stack Relative Indirect Indexed		(d,s),y
	ADDRMODE_BLOCKMOVE			;Block Source Bank, Destination Bank	xyc
}

;==============================================================================
;	addressing mode list
;
;       1       Immediate addressing			#
;       2	Absolute				a
;       3       Absolute Long				al
;       4	Direct					d
;       5	Accumulator				A
;       6	Implied					i
;       7	Direct Indirect Indexed			(d),y
;       8	Direct Indirect Long Indexed		[d],y
;       9	Direct Indexed Indirect			(d,x)
;       10	Direct Indexed with X			d,x
;       11	Direct Indexed with Y			d,y
;       12	Absolute Indexed with X			a,x
;       13	Absolute Long Indexed with X		al,x
;       14	Absolute Indexed with Y			a,y
;       15	Program Counter Relative 		r
;       16	Program Counter Relative Long		rl
;       17	Absolute Indirect			(a)
;       18	Direct Indirect				(d)
;       19	Direct Indirect Long			[d]
;	20	Absolute Indexed Indirect		(a,x)
;	21	Stack					s
;	22	Stack Relative				d,s
;	23	Stack Relative Indirect Indexed		(d,s),y
;	24	Block Source Bank, Destination Bank	xyc
;==============================================================================
; Inputs:
; Outputs:
;   	dl = addressing code(0 = not found)
;	bl:cx = operand value
;------------------------------------------------------------------------------

CAM_AD	PROC	NEAR		;CHECK ADDRESSING MODE FOR "a" & "d" OPERATORS
				;RETURN ADDRESSING CODE IN DL; 0 = NOT FOUND.
				;OPERAND VALUE IN BL:CX

	PUSH	AX		;SAVE AX BECAUSE AL USUALLY HAS OPCODE DATA
	MOV	DI,[PARAM_LAST]

				; al = 1st char, cl,ch,dl,dh = last 4 chars
	CMP	[PARAM_FIELD],'#'      ;IMMEDIATE?
	JNZ	@@NotImmediate
	JMP	CheckImmediate		;GO AWAY THEN!
@@NotImmediate:

	lea	bx,Uppercase

	MOV	DX,WORD	PTR [PARAM_FIELD+DI-1]
	mov	al,[PARAM_FIELD+DI-1]
	xlatb
	mov	dl,al
	mov	al,[PARAM_FIELD+DI]
	xlatb
	mov	dh,al

	MOV	CX,WORD	PTR [PARAM_FIELD+DI-3]
	mov	al,[PARAM_FIELD+DI-3]
	xlatb
	mov	cl,al
	mov	al,[PARAM_FIELD+DI-2]
	xlatb
	mov	ch,al

	mov	al,[PARAM_FIELD]

	CMP	AL,'('      ;7,9,17,18,20,23?
	JZ	@@DoLeftParen
	CMP	AL,'['      ;8,19?
	JZ	@@CAD3
	CMP	DX,'X,'     ;10,12,13?
	JZ	@@CAD4
	CMP	DX,'Y,'     ;11,14?
	JZ	@@CAD5
	CMP	DX,'S,'     ;22?
	JZ	@@CAD6
	JMP	@@CAD7		;2,3,4?

@@DoLeftParen:
	CMP	DX,'Y,'     ;7,23?
	JNZ	@@DoLeftParenNotY
	JMP	@@CAD8
@@DoLeftParenNotY:
	CMP	CH,','      ;9,20?
	JNZ	@@DoLeftParenNotComma
	JMP	@@CAD10
@@DoLeftParenNotComma:
	CMP	DH,')'      ;17,18?
	JZ	@@CAD11
@@SYNTAXERROR:
	MOV	AX,5		;SYNTAX ERROR
	JMP	@@Error

@@CAD3:
	CMP	DH,']'      ;19?
	JNZ	@@CAD3A
	JMP	@@CAD12
@@CAD3A:
	CMP	DX,'Y,'     ;8?
	JNZ	@@SYNTAXERROR
	CMP	CH,']'
	JNZ	@@SYNTAXERROR
	MOV	SI,1		;OKAY 8
	SUB	DI,3
	MOV	AM8,8
	MOV	AM16,0		; kts 11-25-92 01:50am
	MOV	AM24,0
	JMP	@@ParseExp
@@CAD4:
	XOR	SI,SI		;OKAY 10,12,13
	SUB	DI,2
	MOV	AM8,10
	MOV	AM16,12
	MOV	AM24,13
	JMP	@@ParseExp

@@CAD5:
	XOR	SI,SI		;OKAY 11,14
	SUB	DI,2
	MOV	AM8,11
	MOV	AM16,14
	MOV	AM24,0			; kts 11-25-92 01:54am
	JMP	@@ParseExp
@@CAD6:
	XOR	SI,SI		;OKAY 22
	SUB	DI,2
	MOV	AM8,22
	MOV	AM16,0			; kts 11-25-92 02:01am
	MOV	AM24,0			; kts
	mov	addressingSeg,SEG_ABS
	JMP	@@ParseExp

@@CAD7:
	XOR	SI,SI		;OKAY 2,3,4
	MOV	AM8,4
	MOV	AM16,2
	MOV	AM24,3
	JMP	@@ParseExp

@@CAD8:
	CMP	CH,')'      ;7,23?
	JZ	@@CAD8A
	JMP	@@SYNTAXERROR
@@CAD8A:
	CMP	[PARAM_FIELD+DI-4],','  ;23?
	JZ	@@CAD9
	MOV	SI,1		;OKAY 7
	SUB	DI,3
	MOV	AM8,7
	MOV	AM16,0			; kts 11-25-92 01:48am
	MOV	AM24,0			; kts
	JMP	@@ParseExp
@@CAD9:
	CMP	CL,'S'          ;23?
	JZ	@@CAD9A
	JMP	@@SYNTAXERROR
@@CAD9A:
	MOV	SI,1		;OKAY 23
	SUB	DI,5
	MOV	AM8,23
	MOV	AM16,0  		; kts 11-25-92 02:22am
	MOV	AM24,0			; kts
	mov	addressingSeg,SEG_ABS
	JMP	@@ParseExp
@@CAD10:
	CMP	dl,'X'		; ensure correct reg used
	jz	@@Cad10Ok
	jmp	@@SYNTAXERROR
@@Cad10Ok:
	MOV	SI,1		;OKAY 9,20
	SUB	DI,3
	MOV	AM8,9
	MOV	AM16,20
	MOV	AM24,0			; kts 11-25-92 02:23am
	JMP	@@ParseExp
@@CAD11:
	MOV	SI,1		;OKAY 17,18
	SUB	DI,1
	MOV	AM8,18
	MOV	AM16,17
	MOV	AM24,0			; kts 11-25-92 01:55am
	JMP	@@ParseExp
@@CAD12:
	MOV	SI,1		;OKAY 19
	SUB	DI,1
	MOV	AM8,19
	MOV	AM16,0			; kts 11-25-92 01:47am
	MOV	AM24,0			; kts

@@ParseExp:
	CALL	PARSE_EXP	;PARSE THE EXPRESSION
	CMP	OVERRIDE,0	;DID HE USE OVERRIDE?
	JZ	@@FindAFit		;IF NOT THEN JUMP TO "FIND A FIT"
	MOV	DL,AM8
	DEC	OVERRIDE
	JZ	@@Ret
	MOV	DL,AM16
	DEC	OVERRIDE
	JZ	@@Ret
	MOV	DL,AM24
	JMP	@@Ret

@@FindAFit:				; find a fit
	OR	BH,BH		;DOES VALUE EXCEED 24 BITS?
	JNZ	@@ErrorOpSize		;JUMP IF SO

	TEST	SDB1,4		;FORWARD REFERENCE?
	JNZ	@@DoForwardReference

	cmp	addressingSeg,SEG_DATA
	jz	@@CheckData
	cmp	addressingSeg,SEG_CODE
	jz	@@CheckProg

	MOV	DL,AM24
	or	bl,bl
	jnz	@@Ret
@@Normal:
	mov	dl,AM16
	or	ch,ch
	jnz	@@Ret
	mov	dl,AM8
	jmp	@@Ret
; do abs
@@CheckProg:
				; code to check ranges for Program bank
	MOV	DL,AM24
	cmp	bx,objptrh
	jnz	@@Ret
	jmp	@@Normal

@@ErrorOpSize:
	MOV	AX,6		;EXPRESSION VALUE EXCEEDS 24 BITS
@@Error:
	CALL	ERROR
	XOR	DL,DL
@@Ret:
	POP	AX
	RET

;-----------------------------------------------------------------------------

@@CheckData:  		; code to check ranges for Data bank and direct page
				; check for direct page access
	or	bl,bl
	jnz	@@NotDirectPage			; if bank not zero, cannot be a direct page access
	mov	al,AM8
	or	al,al
	jz	@@NotDirectPage			; if no direct page addressing available, then don't try
	mov	ax,cx
	sub     ax,directAssume
	jc	@@NotDirectPage			; nope
	cmp	ax,256
	jnc	@@NotDirectPage
					; if here, can be converted into direct page
	mov	cx,ax
	mov	dl,AM8
	JMP	@@Ret

@@NotDirectPage:
	MOV	DL,AM24
	cmp	bl,dataAssume
	jnz	@@Ret			; if don't match, then must be 24 bit address
	MOV	DL,AM16			; check for 16-bit value
	JMP	@@Ret

;-----------------------------------------------------------------------------

@@DoForwardReference:
	mov	dl,AM24
	or	dl,dl
	jnz	@@DoForwardRef24
	mov	dl,AM16
	or	dl,dl
	jnz	@@Ret
	mov	dl,AM8
	jmp	@@Ret

@@DoForwardRef24:			; kts this means that forward references
					; to other banks will fuck up and overwrite
					; the next op-code
	cmp	dl,ADDRMODE_ABSOLUTELONG
	jne	@@NotAL
	mov	dl,ADDRMODE_ABSOLUTE
@@NotAL:
	jmp	@@Ret
CAM_AD	ENDP

;==============================================================================

CAM_I	PROC	NEAR		;CHECK ADDRESSING MODE FOR IMMEDIATE OPERATOR
				;RETURN ADDRESSING CODE (1) IN DL IF VALID
				;0 IF INVALID
				;OPERAND VALUE IN CX
	XOR	DL,DL
	CMP	[PARAM_FIELD],'#'
	JNZ	CAM_IRet
	PUSH	AX
CheckImmediate:
	MOV	DI,[PARAM_LAST]
	MOV	SI,1
	CALL	PARSE_EXP
	POP	AX
	MOV	DL,1
CAM_IRet:
	RET
CAM_I	ENDP

;==============================================================================

CAM_R	PROC	NEAR		;CHECK ADDRESSING MODE FOR RELATIVE OPERATOR
				;RETURN ADDERSSING CODE (15) IN DL
				;RELATIVE OFFSET VALUE IN CL
	PUSH	AX
	XOR	DL,DL
	MOV	DI,[PARAM_LAST]
	XOR	SI,SI
	CALL	PARSE_EXP
	TEST	SDB1,4		;IF FORWARD REFERENCE THEN DON'T COMPLAIN
	JNZ	CAMROK
;	TEST	SDB2,4
;	JNZ	CAMROK

	CMP	BX,OBJPTRH	;RELATIVE ADDRESS WITHIN 64K SEGMENT?
	JNZ	CAMR3
	MOV	AX,OBJPTRL	;CALCULATE RELATIVE OFFSET
	ADD	AX,2		; position to beginning of next instruction
	SUB	CX,AX
	CMP	CH,0FFH		;DOES IT FIT WITHIN 8 BITS?
	JZ	CAMR2		;negative branch
	OR	CH,CH
	JZ	CAMR1		;YES

CAMRERR:
	MOV	AX,8		;NO, OUT OF RANGE
	; dx contains distance off by
	CALL	ERROR
	XOR	CL,CL
CAMROK:
	POP	AX
	MOV	DL,15		; program counter relative
	RET
CAMR1:				; postive branch
	cmp	cl,080h
	jc	CAMROK

	;----- Calculate "off by n bytes"
	mov	dx,-127
	add	dx,cx
	jmp	CAMRERR

CAMR2:				; negative branch
	cmp	cl,080h
	jnc	CAMROK

	;----- Calculate "off by n bytes"
	mov	dx,-128
	sub	dx,cx
	jmp	CAMRERR

CAMR3:	MOV	AX,7
	CALL	ERROR
	POP	AX
	RET
CAM_R	ENDP

;==============================================================================

CAM_RL	PROC	NEAR		;CHECK ADDRESSING MODE FOR RELATIVE LONG OPERATOR
				;RETURN ADDERSSING CODE (16) IN DL
				;RELATIVE OFFSET IN CX
	PUSH	AX
	XOR	DL,DL
	MOV	DI,[PARAM_LAST]
	XOR	SI,SI
	CALL	PARSE_EXP
	TEST	SDB1,4		;BYPASS ERROR CHECKING IF FORWARD REFERENCE
	JNZ	CRL0
;	TEST	SDB2,4
;	JNZ	CRL0
	CMP	BX,OBJPTRH	;RELATIVE ADDRESS WITHIN 64K SEGMENT?
	JNZ	CRL1
	MOV	AX,OBJPTRL	;CALCULATE RELATIVE OFFSET
	ADD	AX,3
	SUB	CX,AX
CRL0:	POP	AX
	MOV	DL,10H
	RET
CRL1:	MOV	AX,7
	CALL	ERROR
	XOR	CX,CX
	JMP	CRL0

CAM_RL	ENDP

;==============================================================================

CAM_XYC	PROC	NEAR		;CHECK ADDRESSSING MODE FOR BLOCK MOVE OPERATOR
				;RETURN ADDRESSING CODE (24) IN DL
	PUSH	AX		;OPERAND VALUES IN BL & CL "SRCBLK,DSTBLK"
	XOR	DL,DL
	XOR	SI,SI
	MOV	DI,SI
@@CXYC1:
	INC	DI
	MOV	AL,[PARAM_FIELD+DI]
	OR	AL,AL
	JZ	@@CXYC3
	CMP	AL,','
	JNZ	@@CXYC1
	DEC	DI
	PUSH	DI
	CALL	PARSE_EXP
	POP	DI
	ADD	DI,2
	MOV	SI,DI
	CMP	[PARAM_FIELD+DI],0
	JZ	@@CXYC3
@@CXYC2:
	INC	DI
	MOV	AL,[PARAM_FIELD+DI]
	OR	AL,AL
	JNZ	@@CXYC2
	DEC	DI
	push	cx
;	MOV	BP,CX
	CALL	PARSE_EXP
	pop	bx
; 	MOV	BX,BP
	MOV	DL,24
	POP	AX
	RET

@@CXYC3:
	MOV	AX,5
	CALL	ERROR
	POP	AX
	RET

CAM_XYC	ENDP

;==============================================================================

