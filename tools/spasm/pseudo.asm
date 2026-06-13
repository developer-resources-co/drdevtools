;==============================================================================
; pseudo.asm: pseudo-ops for spasm
;==============================================================================

DATA	SEGMENT	'DATA'

QUIT_TEXT	DB	10,'QUIT directive encountered.  Assembly aborted.',0
szAssert	db	'ASSERT ',0
szAssertSeperator	db	': ',0
szFailed	db	' failed.',0

MAX_REPT	equ	16
REPT_LEVEL	DW	0
REPT_COUNT	DW	MAX_REPT DUP (?)
REPT_STARTINGPOS	DD	MAX_REPT DUP (?)
REPT_LINENUM	DW	MAX_REPT DUP (?)

oldSDB1	db	0
oldSDB2	db	0

bchInString	db	?		; boolean and also the quote character!


DATA	ENDS
;==============================================================================
.CODE

UpcaseWord:
	lea	bx,Uppercase

	xlatb
	xchg	ah,al
	xlatb
	xchg	ah,al

	ret

;==============================================================================
; scan forward in param_field(staring at si) until comma found
; return offset of char past comma in si

FindComma:
@@Loop:
	MOV	AL,[PARAM_FIELD+si]
	inc	si
	OR	AL,AL
	JZ	@@Err
	CMP	AL,','
	JNZ	@@Loop
	ret

@@Err:
	mov	ax,5
	jmp	Error


;==============================================================================
; parse 2 expressions seperated by a comma
; Outputs:
;	dx:ax = first result
;	bx:cx = second result
;
;------------------------------------------------------------------------------

Parse2Exp	proc	near
	push	di
	push	si
	call	FindComma
	mov	di,si
	pop	si
	push	di
	dec	di
	dec	di
	call	parse_exp			; first parse left half
	pop	si			; si now points to second half
	pop	di
	mov	al,SDB1
	mov	oldSDB1,al
	mov	al,SDB2
	mov	oldSDB2,al
	push	cx
	push	bx
	call	parse_exp
	pop	dx
	pop	ax
	ret
Parse2Exp	endp

;==============================================================================

GRABVAL	PROC	NEAR		;GRABS VALUES FROM PARAM_FIELD
	MOV	SI,DI		;(FOR DIRECTIVES SUCH AS DB,DW,DL,AND MACROS)
	CMP	DI,PARAM_LAST
	JA	@@Nope
	cmp	bchInString,0
	jne	@@String

@@Loop:
	MOV	AL,[PARAM_FIELD+DI]
	CMP	AL,','          ;COMMA?
	JZ	@@Comma
	cmp	al,''''
	je	@@NewString
	cmp	al,'"'
	je	@@NewString
	INC	DI
	CMP	DI,PARAM_LAST
	JBE	@@Loop

@@Comma:
	DEC	DI
	PUSH	SI
	PUSH	DI
	CALL	PARSE_EXP
	POP	DI
	POP	SI
	ADD	DI,2
	CLC
	RET

@@NewString:
@@EndString:
	inc	di		; Move past starting '
	cmp	di,param_last	; End of field?
	ja	@@Nope

	add	si,2			; ??? Make sure comma here?

	xor	bchInString,al
	jz	@@Loop
@@String:
	mov	al,[param_field+di]
	inc	di		; Eat returned(?) character
	cmp	al,bchInString
	je	@@EndString
	mov	cl,al
	mov	ch,0		; clear out rest of returned value (bx:cx)
	xor	bx,bx
	clc
	ret

@@Err:
	MOV	AX,18
	CALL	ERROR
@@Nope:
	STC
	RET
GRABVAL	ENDP

;==============================================================================

PrintSymbolToListing:
	mov	al,bh			; wbn 24.2.94 -- print all 32 bits
	call	hexout
	mov	[list_prefix+listhexstart],ah
	mov	[list_prefix+listhexstart+1],al

	mov	al,bl
	call	hexout
	MOV	[LIST_PREFIX+LISTHEXSTART+2],AH
	MOV	[LIST_PREFIX+LISTHEXSTART+3],AL

	mov	al,ch
	call	hexout
	MOV	[LIST_PREFIX+LISTHEXSTART+4],AH
	MOV	[LIST_PREFIX+LISTHEXSTART+5],AL

	mov	al,cl
	call	hexout
	MOV	[LIST_PREFIX+LISTHEXSTART+6],AH
	MOV	[LIST_PREFIX+LISTHEXSTART+7],AL

	ret


PARSE_PSEUDO	PROC	NEAR	;PARSE PSEUDO-OP

;;	ifdef	GENESIS
	OR	SDB2,40H		      	;NOTE THIS ROUTINE WAS CALLED
;;	endif

	MOV	AX,WORD PTR [MNE_FIELD]	;STORE OP INTO REGISTERS
	;mov	eax,DWORD PTR [MNE_FIELD]
	MOV	BX,WORD PTR [MNE_FIELD+2]
;	MOV	CL,[MNE_FIELD+4]
	MOV	CX,word ptr [MNE_FIELD+4]		; kts 03-16-94 11:45pm
	XOR	SI,SI		;FOR (PARSE_EXP)
	MOV	DI,PARAM_LAST

	ifdef	SNES
	CMP	AX,'OL'                 ;"LONG" PSEUDO OP?
	JNZ	PP3
	CMP	BX,'GN'
	JNZ	PP3
	mov	ax,WORD PTR [PARAM_FIELD]		; on/off
	call	UpcaseWord
	CMP	CL,'A'          ;LONGA?
	JZ	PP1
	CMP	CL,'I'          ;LONGI?
	JZ	PP2
	MOV	AX,10		;INVALID PSEUDO-OP
	JMP	ERROR

PP3:	JMP	PP4

	;----- LONGA
PP1:	CMP	ax,'NO' ;"LONGA ON"
	JNZ	PP1A
	OR	SDB1,80H
	RET
PP1A:	CMP	ax,'FO' ;"LONGA OFF"
	JNZ	PP1B
	AND	SDB1,NOT 80H
	RET
PP1B:	CMP	ax,'UP' ;"LONGA PUSH"
	JNZ	PP1C
	;@@@CMP	WORD PTR [PARAM_FIELD+2],'HS'
	;@@@JNZ	PP1C
	JMP	PUSHLA
PP1C:	CMP	ax,'OP' ;"LONGA POP"
	JNZ	PP2D
	;@@@CMP	WORD PTR [PARAM_FIELD+2],'P'
	;@@@JNZ	PP2D
	JMP	POPLA

	;----- LONGI
PP2:
	CMP	ax,'NO' ;"LONGI ON"
	JNZ	PP2A
	OR	SDB1,40H
	RET
PP2A:
	CMP	ax,'FO' ;"LONGI OFF"
	JNZ	PP2B
	AND	SDB1,NOT 40H
	RET
PP2B:
	CMP	ax,'UP' ;"LONGI PUSH"
	JNZ	PP2C
	;@@@CMP	WORD PTR [PARAM_FIELD+2],'HS'
	;@@@JNZ	PP2C
	JMP	PUSHLI
PP2C:
	CMP	ax,'OP' ;"LONGI POP"
	JNZ	PP2D
	;@@@CMP	WORD PTR [PARAM_FIELD+2],'P'
	;@@@JNZ	PP2D
	JMP	POPLI

PP2D:
	MOV	AX,46			;ILLEGAL OR INVALID PARAMETER
	JMP	ERROR
	endif

;------------------------------------------------------------------------------
; org

PP4:	CMP	AX,'RO'         ;ORG?
	JNZ	PP5
	CMP	BL,'G'
	JNZ	PP5
	CALL	PARSE_EXP
	MOV	OBJPTRH,BX
	MOV	OBJPTRL,CX

	; aligns PORG with LORG -- kts added 4/23/92
	MOV	DELTA_L,0
	MOV	DELTA_H,0

;;	ifdef	GENESIS
	JMP	SEGSIZE_KLUDGE
;;	endif


PP5:
	CMP	AX,'EG'         ;GEQU?
	JNZ	@@PP6
	CMP	BX,'UQ'
	JZ	@@PP8
	JMP	PP9
@@PP6:
	CMP	AX,'QE'         ;EQU?
	JNZ	@@PP7
	CMP	BL,'U'
	JZ	@@PP8
@@PP7:
	CMP	AX,'='          ;=?
	JNZ	PP9
@@PP8:
	PUSH	DI
	CALL	FIND_SYMBOL	;FIND LAST SYMBOL DEFINED
	POP	DI
	JC	@@PP8A
	PUSH	ES		;CRAM [PARAM_FIELD] VALUE INTO IT.
	CALL	PARSE_EXP
	POP	ES
	TEST	SDB1,4		;NO HONKEY MO-FO-WARD REFERENCES ALLOWED HERE
;	JNZ	@@PP8X
;	TEST	SDB2,4
	JZ	@@PP8Y
;@@PP8X:
	MOV	AX,53
	JMP	ERROR
@@PP8Y:
	MOV	ES:[symbol_val_l],CX
	MOV	ES:[symbol_val_h],BX

	; kts added to show equate in symbol file, 4/25/92
	TEST	SDB2,8			;IS LIST FILE ON?
	JZ	@@Ret

	call	PrintSymbolToListing
@@Ret:
	RET

;------------------------------------------------------------------------------

@@PP8A:
	MOV	AX,5
	JMP	ERROR

PP9:
	CMP	AX,'SD'         ;DS?
	JNZ	@@Not_DS
	OR	BL,BL
	JZ	PP9X		; "ds" only (no . extension)?
	CMP	BL,'.'
	JNZ	@@Not_DS
PP9X:
	CALL	PARSE_EXP
	OR	BL,BL
	JNZ	PP9Y		; no extension implies bytes
	JCXZ	PP11
	ifndef GENESIS
	call	parse_extension
	endif
PP9Y:
	TEST	SDB1,4
	JNZ	PP10X
;	TEST	SDB2,4
;	JNZ	PP10X
	CMP	SEYES,0
	JZ	PP10
	SHL	CX,1
	RCL	BL,1
	CMP	SEYES,40H
	JZ	PP10
	SHL	CX,1
	RCL	BL,1
PP10:
	ifdef SNES
	mov	dh,BYTE PTR objptrl+1		; Calculate current page
	endif

	ADD	OBJPTRL,CX
	ADC	OBJPTRH,BX
	SUB	CSCOUNTL,CX
	SBB	CSCOUNTH,BX
	JNC	PP11
	MOV	AX,35
	JMP	ERROR
PP10X:
	MOV	AX,53
	jmp	error
PP11:

	ifdef SNES
	cmp	dh,BYTE PTR objptrl+1
	je	@@NoPageCrossing
	mov	ax,74
	call	error
@@NoPageCrossing:
	endif

	CMP	_MAPFLAG,0
	JZ	PP11A
	JMP	SEGSIZE_KLUDGE
PP11A:
	RET
@@Not_DS:

	CMP	AX,'NI'         ;INCB(IN)?
	JNZ	PP21
	CMP	BX,'BC'
	JNZ	PP20
	MOV	AX,INT21_OPENFILE	;OPEN FILE
	MOV	DX,OFFSET PARAM_FIELD
	CMP	IDL,0
	JZ	PP13
	CALL	INC_DIR
PP13:
	INT	21H
	JNC	PP14
	MOV	AX,12
	JMP	ERROR
PP14:
	MOV	INCHAND,AX
PP15:
	push	ds
	MOV	BX,INCHAND
	xor	dx,dx
	MOV	CX,FILE_BUFFER_SIZE
	mov	ax,BUFSEG2
	mov	ds,ax
	CALL	FileRead		; read $4000 bytes into memory
	pop	ds
	mov	cx,ax
	JCXZ	PP18		; if no more file, stop
	XOR	SI,SI		; start at top of rbuf2

PP17:
	MOV	ES,BUFSEG2
	call	StoreBin
;	MOV	AL,ES:[SI]
;	CALL	STORE_BYTE
;	INC	SI
;	LOOP	PP17
	JMP	PP15

PP18:
	MOV	AH,3EH		;CLOSE INCB FILE
	MOV	BX,INCHAND
	INT	21H
	RET

PP20:
	CMP	BX,'LC'         ;INCL(UDE)?
	JNZ	PP21
	JMP	PUSH_FILE

PP21:
	CMP	AX,'FI'         ;IF?
	JNZ	PP24
	OR	BL,BL
	JNZ	PP23
	CALL	PARSE_EXP
	MOV	AL,1
	OR	CX,BX
	JNZ	PP22
	XOR	AL,AL
PP22:
	JMP	PUSH_IF

PP23:
	CMP	BX,'ON'         ;IFNOT?
	JNZ	PP232ND
	CMP	CX,'T'		; note: checking for T and zero termination
	JNZ	PP232ND

	CALL	PARSE_EXP
	MOV	AL,1
	OR	CX,BX
	JZ	PP22
	XOR	AL,AL
	JMP	PUSH_IF

PP232ND:
	CMP	BX,'ED'         ;IFDEF?
	JNZ	@@Not_IFDEF
	CMP	CL,'F'
	JNZ	@@Not_IFDEF
	call	FindSymbolParamField
	MOV	AL,1
	JNC	PP22	; couldn't this go down to the PUSH_IF below?
	XOR	AL,AL
	JMP	PUSH_IF
@@Not_IFDEF:

	CMP	BX,'DN'         ;IFNDEF?
	JNZ	PP24
	CMP	CL,'E'
	JNZ	PP24
	call	FindSymbolParamField
	MOV	AL,1
	JC	PP22
	XOR	AL,AL
	JMP	PUSH_IF

PP24:
	CMP	AX,'NE'         ;ENDIF?
	JNZ	@@NextPsOp
	CMP	BX,'ID'
	JNZ	@@NextPsOp
	CMP	CL,'F'
	JNZ	@@NextPsOp
	CALL	POP_IF
	RET
@@NextPsOp:

PPElse:
	CMP	AX,'LE'         ;ELSE
	JNZ	g_PP25
	CMP	BX,'ES'
	JNZ	g_PP25
	XOR	BH,BH
	MOV	BL,IF_LEVEL
	OR	BL,BL
	JNZ	@@Ok
	MOV	AX,77
	JMP	ERROR
@@Ok:
	mov	al,[IF_FLAG+BX]
	xor	al,1
	mov	[IF_FLAG+BX],al
	RET

g_PP25:
	CMP	AX,'CD'         ;DC?
	JNZ	g_PP31
	OR	BL,BL
	JZ	g_PP25X
	CMP	BL,'.'
	JNZ	g_PP31
g_PP25X:
	mov	bchInString,0

	ifndef	GENESIS
	call	parse_extension
	endif

	MOV	GVFLAG,1
	MOV	AX,OBJPTRH
	MOV	GVH,AX
	MOV	AX,OBJPTRL
	MOV	GVL,AX
	CMP	SEYES,40H
	JZ	g_PPDW
	CMP	SEYES,80H
	JZ	g_PPDL

g_PPDB:
	MOV	DI,SI
g_PP26:
	CALL	GRABVAL
	JC	g_PP26B
	MOV	AL,CL
	PUSH	DI
	CALL	STORE_BYTE			; BYTE
	POP	DI
	JMP	g_PP26
g_PP26B:
	MOV	GVFLAG,0
	RET

g_PPDW:
	MOV	DI,SI
g_PP28:
	CALL	GRABVAL
	JC	g_PP26B
	PUSH	DI
	ifdef	INTEL
	xchg	cl,ch
	endif

	MOV	AL,CH
	CALL	STORE_BYTE			; WORD
	MOV	AL,CL
	CALL	STORE_BYTE

	POP	DI
	JMP	g_PP28

g_PPDL:
	MOV	DI,SI
g_PP30:
	CALL	GRABVAL
	JC	g_PP26B
	PUSH	DI
	ifdef	INTEL
	xchg	bl,bh
	xchg	cl,ch
	xchg	bx,cx
	endif
	MOV	AL,BH
	CALL	STORE_BYTE			; LONG
	MOV	AL,BL
	CALL	STORE_BYTE
	MOV	AL,CH
	CALL	STORE_BYTE
	MOV	AL,CL
	CALL	STORE_BYTE
	POP	DI
	JMP	g_PP30

g_PP31:
	ifdef	GENESIS
				; genesis dt and dtr (same as SNES?)
	CMP	AX,'TD'     ;DT?
	jz	@@Cont
;	JNZ	g_PP40
	jmp	g_PP40
@@Cont:
	OR	BL,BL
	JNZ	g_PP36
	CMP	[PARAM_FIELD],''''
	JNZ	g_PP39
	CMP	[PARAM_FIELD+DI],''''
	JNZ	g_PP39
g_PP32:
	INC	SI
	MOV	AL,[PARAM_FIELD+SI]
	CMP	AL,39
	JNZ	g_PP33
	CMP	[PARAM_FIELD+SI+1],''''
	JNZ	g_PP34
	INC	SI
g_PP33:
	PUSH	DI
	CALL	STORE_BYTE
	POP	DI
	CMP	SI,DI
	JNA	g_PP32
g_PP33A:
	RET
g_PP34:
	INC	SI
	MOV	AL,[PARAM_FIELD+SI]
	CMP	AL,''''
	JZ	g_PP32
	OR	AL,AL
	JZ	g_PP33A
	JMP	g_PP34

g_PP36:
	CMP	BX,'R'      ;DTR?
	JNZ	g_PP40
	CMP	[PARAM_FIELD],''''
	JNZ	g_PP39
	MOV	[PARAM_FIELD+DI],''''
	JNZ	g_PP39
g_PP37:
	DEC	DI
	MOV	AL,[PARAM_FIELD+DI]
	CMP	AL,''''
	JNZ	g_PP38
	CMP	[PARAM_FIELD+DI-1],''''
	JNZ	g_PP38B
	DEC	DI
g_PP38:
	PUSH	DI
	CALL	STORE_BYTE
	POP	DI
	CMP	DI,0
	JA	g_PP37
g_PP38A:
	RET
g_PP38B:
	DEC	DI
	JS	g_PP38A
	MOV	AL,[PARAM_FIELD+DI]
	CMP	AL,''''
	JZ	g_PP37
	JMP	g_PP38B
	RET

g_PP39:
	MOV	AX,5
	JMP	ERROR
g_PP40:
	endif
	;endif

	;ifdef	SNES
PP25:
	CMP	AX,'BD'         ;DB?
	JNZ	PP27
	OR	BL,BL
	JNZ	PP27

	mov	bchInString,0
	MOV	GVFLAG,1
	MOV	AX,OBJPTRH
	MOV	GVH,AX
	MOV	AX,OBJPTRL
	MOV	GVL,AX

	MOV	DI,SI
PP26:
	CALL	GRABVAL
	JC	PP26B
	MOV	AL,CL
	PUSH	DI
	CALL	STORE_BYTE
	POP	DI
	JMP	PP26
PP26B:
	MOV	GVFLAG,0
	RET

PP27:
	CMP	AX,'WD'         ;DW?
	JNZ	PP29
	OR	BL,BL
	JNZ	PP29
	MOV	DI,SI

	mov	bchInString,0
	MOV	GVFLAG,1
	MOV	AX,OBJPTRH
	MOV	GVH,AX
	MOV	AX,OBJPTRL
	MOV	GVL,AX

PP28:
	CALL	GRABVAL
	JC	PP26B
	PUSH	DI
	ifdef	MOTOROLA
	xchg	cl,ch
	endif
	MOV	AL,CL
	CALL	STORE_BYTE
	MOV	AL,CH
	CALL	STORE_BYTE
	POP	DI
	JMP	PP28

PP29:
	CMP	AX,'LD'         ;DL?
	JNZ	@@NotDL
	OR	BL,BL
	JNZ	@@NotDL

	;----- DL
	mov	bchInString,0
	MOV	GVFLAG,1
	MOV	AX,OBJPTRH
	MOV	GVH,AX
	MOV	AX,OBJPTRL
	MOV	GVL,AX

	MOV	DI,SI
PP30:
	CALL	GRABVAL
	JC	PP26B
	PUSH	DI
	ifdef	MOTOROLA
	xchg	bl,bh
	xchg	cl,ch
	xchg	bx,cx
	endif
	MOV	AL,CL
	CALL	STORE_BYTE
	MOV	AL,CH
	CALL	STORE_BYTE
	MOV	AL,BL
	CALL	STORE_BYTE
	POP	DI
	JMP	PP30
@@NotDL:

	cmp	ax,'DD'		; DD?
	jnz	@@NotDD
	or	bl,bl
	jnz	@@NotDD

	mov	bchInString,0
	mov	gvflag,1
	mov	ax,objptrh
	mov	gvh,ax
	mov	ax,objptrl
	mov	gvl,ax

	mov	di,si
@@DD_loop:
	call	grabval
	jc	@@DD_exit
	push	di
	ifdef	MOTOROLA
	xchg	bl,bh
	xchg	cl,ch
	xchg	bx,cx
	endif
	mov	al,cl
	call	store_byte
	mov	al,ch
	call	store_byte
	mov	al,bl
	call	store_byte
	mov	al,bh
	call	store_byte
	pop	di
	jmp	@@DD_loop
@@DD_exit:
	MOV	GVFLAG,0
	RET
@@NotDD:


PP31:
	CMP	AX,'TD'     ;DT?
	jz	@@ISTD
	jmp	PP40
;	JNZ	PP40
@@ISTD:
	OR	BL,BL
	JNZ	PP36
	CMP	[PARAM_FIELD],''''
	JNZ	PP39
	CMP	[PARAM_FIELD+DI],''''
	JNZ	PP39
PP32:
	INC	SI
	MOV	AL,[PARAM_FIELD+SI]
	CMP	AL,''''
	JNZ	PP33
	CMP	[PARAM_FIELD+SI+1],''''
	JNZ	PP34
	INC	SI
PP33:
	PUSH	DI
	CALL	STORE_BYTE
	POP	DI
	CMP	SI,DI
	JNA	PP32
PP33A:
	RET

PP34:	INC	SI
	MOV	AL,[PARAM_FIELD+SI]
	CMP	AL,''''
	JZ	PP32
	OR	AL,AL
	JZ	PP33A
	JMP	PP34

PP36:
	CMP	BX,'R'      ;DTR?
	JNZ	PP40
	CMP	[PARAM_FIELD],''''
	JNZ	PP39
	MOV	[PARAM_FIELD+DI],''''
	JNZ	PP39
PP37:
	DEC	DI
	MOV	AL,[PARAM_FIELD+DI]
	CMP	AL,''''
	JNZ	PP38
	CMP	[PARAM_FIELD+DI-1],''''
	JNZ	PP38B
	DEC	DI
PP38:
	PUSH	DI
	CALL	STORE_BYTE
	POP	DI
	CMP	DI,0
	JA	PP37
PP38A:
	RET

PP38B:
	DEC	DI
	JS	PP38A
	MOV	AL,[PARAM_FIELD+DI]
	CMP	AL,''''
	JZ	PP37
	JMP	PP38B
	RET

PP39:
	MOV	AX,5
	JMP	ERROR
	;endif


PP40:
	CMP	AX,'AM'         ;MACRO?
	JNZ	@@NotMacro
	CMP	BX,'RC'
	JNZ	@@NotMacro
	CMP	CL,'O'
	JNZ	@@NotMacro

	JMP	@@DoMacro

@@NotMacro:
	CMP	AX,'ED'         ;(OR DEFM?)
	JNZ	@@NotMacroOrDefm
	CMP	BX,'MF'
	JZ	@@DoMacro
@@NotMacroOrDefm:
	JMP	PP46

@@DoMacro:
	CALL	FIND_SYMBOL	;MAKE LAST SYMBOL INTO MACRO
	JC	@@Syntax
	XOR	DI,DI		;MAKE SURE MACRO IS <16 CHRS
	MOV	AX,DI
	MOV	CX,MACRO_NAME_LEN+2	; kts why 2?
	CLD                     ; verify name is not too long

	REPNZ	SCASB 			; scan forward to zero or until cx runs out
;	SCASB                      ; kts removed 04-16-94 06:54pm
	JCXZ	@@MacroTooLong		; if cx ran out, give macro name too long error

	MOV	AX,MACENDPTR	;DEFINE MACRO
	MOV	ES:[mac_ptr],AX		; store ptr to macro text
	MOV	ES:[mac_sigoffset],MACRO_SIG

@@NextLine:
	CALL	GET_LINE	;STORE MACRO DATA
	JNC	@@NotEOF
	MOV	AH,9		;END OF FILE ENCOUNTERED IN THE MACRO!
	MOV	DX,OFFSET ERROREOFINMACRO
	INT	21H
	JMP	T1

@@NotEOF:
	MOV	ES,MACSEG	;STORE 1 LINE OF MACRO DATA
	MOV	CX,DI		; get # of chars in line, di is left over from GET_LINE
	INC	CX
	MOV	SI,OFFSET LINE
	MOV	DI,MACENDPTR
;	CMP	DI,2F00H	;OUT OF ROOM?
	CMP	DI,MACRO_BUFFER_SIZE-1
	JNC	@@OutOfMacroSpace
	CLD
@@CopyLoop:
	LODSB
	CMP	AL,''''		;IF THERE'S A SINGLE QUOTE
	JZ	@@Quoted		;THEN DON'T TRY TO SAVE ROOM
				;OTHERWISE...

	CMP	AL,';'		;EXCLUDE COMMENTS
	JNZ	@@NotComment
	XOR	AL,AL
@@NotComment:
	CMP	AL,20H		;AND COMPRESS SPACES
	JNZ	@@NotSpace
	CMP	BYTE PTR [SI],20H
	JZ	@@CopyLoop
@@NotSpace:
	STOSB
	OR	AL,AL
	LOOPNZ	@@CopyLoop
	JMP	@@Done

@@Quoted:
	STOSB			;SAVE LINE VERBATIM
	dec	cx		; kts 03-10-94 04:03pm
	REP	MOVSB

@@Done:
	MOV	MACENDPTR,DI
	CALL	BLIF		;IS THIS THE END?
	JC	@@NextLine
	TEST	SDB1,2
	JNZ	@@NextLine
	CMP	WORD PTR [MNE_FIELD],'NE'   ;LOOK FOR "ENDM" DIRECTIVE
	JNZ	@@NextLine
	CMP	WORD PTR [MNE_FIELD+2],'MD'
	JNZ	@@NextLine
	RET

@@Syntax:
	MOV	AX,5			; syntax error
	JMP	ERROR

@@MacroTooLong:
	MOV	AX,3			; macro name must not exceed 15 chars
	JMP	ERROR

@@OutOfMacroSpace:					; out of macro space
	mov	ax,28
	jmp	error
	MOV	AH,9
;	MOV	DX,OFFSET ERROR28
;	INT	21H
;	mov	_ERRORLEVEL,DOS_RETURNCODE_SEVERE
;	JMP	T1

;-----------------------------------------------------------------------------

PP46:
	CMP	AX,'NE'         ;ENDM(ACRO)?
	JNZ	PP47
	CMP	BX,'MD'
	JNZ	PP47
	JMP	POP_MACRO

PP47:
	CMP	AX,'XE'         ;EXITM(ACRO)?
	JNZ	PP48
	CMP	BX,'TI'
	JNZ	PP48
	CMP	CL,'M'
	JNZ	PP48
				;JMP   POP_MACRO

POP_MACRO:
	CMP	MACLEVEL,0FFFFH
	JZ	@@PopErr

	; kts 02-21-94 10:19pm
	; validate rept level

	MOV	BX,MACLEVEL		; get maclevel before decrement

	DEC	MACLEVEL
	JNS	@@POPM1
	AND     SDB1,NOT 8
@@POPM1:


	SHL	BX,1
	mov	ax,[MACREPTLEVEL+BX]
	cmp	ax,REPT_LEVEL
	jnz	@@ReptErr
	; end kts 02-21-94 10:19pm

	RET

@@PopErr:
	mov	ax,75
	jmp	Error

@@ReptErr:
	mov	ax,76
	jmp	Error

PP48:
	CMP	AX,'NE'         ;END? (OF SOURCE CODE FILE)
	JNZ	@@not_END
	CMP	BX,'D'
	JNZ	@@not_END
	JMP	POP_FILE
@@not_END:

;==============================================================================

	CMP	AX,'LL'         ;LLCH(R)?
	JNZ	@@not_LLCHR
	CMP	BX,'HC'
	JNZ	@@not_LLCHR
	MOV	AL,[PARAM_FIELD]
	OR	AL,AL
	JZ	PP50
	MOV	LLCHR,AL
	RET
PP50:
	MOV	AX,26
	JMP	ERROR
@@not_LLCHR:

;==============================================================================

	CMP	AX,'ED'         ;DEFS(EG)?
	JNZ	@@not_DEFSEG
	CMP	BX,'SF'
	JNZ	@@not_DEFSEG

	MOV	AX,WORD	PTR	[PARAM_FIELD]		;WHAT SECTION TYPE?
	call	UpcaseWord

	CMP	AX,'AR'         ;RAM TYPE [1]?
	JZ	@@PP52
	CMP	AX,'OR'         ;ROM TYPE [2]?
	JZ	@@PP53
	CMP	AX,'OC'		; CODE type(same as rom)
	JZ	@@PP53

	CMP	AX,'AD'		; DATA type?
	JZ	@@PP54

	MOV	AX,30
	JMP	ERROR
@@PP52:
	MOV	BH,SEGTYPE_RAM
	JMP	@@PP55

@@PP53:
	MOV	BH,SEGTYPE_ROM
	JMP	@@PP55

@@PP54:
	MOV	BH,SEGTYPE_DATA
	JMP	@@PP55

@@PP55:
	PUSH	BX		; preserve 32-bit value
	CALL	FIND_SYMBOL	;FIND CORESPONDING SYMBOL
	POP	BX
	JNC	@@PP56
	MOV	AX,5
	JMP	ERROR

@@PP56:
	XOR	DI,DI		;FIND END OF STRING
@@PP57:
	INC	DI
	CMP	DI,10H
	JAE	PP61
	MOV	AL,ES:[DI]
	CMP	AL,3
	JA	@@PP57
	MOV	ES:[DI],BH	;STORE SECTION TYPE
	MOV	ES:[symbol_segment_type],BH	;STORE SECTION TYPE
	XOR	AX,AX		;STORE OBJPTRS AS 0

	MOV	ES:[symbol_segment_lorg_l],AX
	MOV	ES:[symbol_segment_lorg_h],AX

	MOV	ES:[symbol_segment_delta_l],AX	;STORE DELTAS AS 0
	MOV	ES:[symbol_segment_delta_h],AL
	MOV	ES:[symbol_segment_top_l],AX	;STORE SSKT AS 0
	MOV	ES:[symbol_segment_top_h],AL
	DEC	AX		;STORE COUNT AS $FFFFFF
	MOV	ES:[symbol_segment_count_l],AX
	MOV	ES:[symbol_segment_count_h],AL
	MOV	ES:[symbol_segment_bottom_l],AX	;STORE SSKB AS $FFFFFF
	MOV	ES:[symbol_segment_bottom_h],AL
	RET
@@not_DEFSEG:

;==============================================================================

	CMP	AX,'ES'         ;SEG(MENT)?
	JNZ	PP60A
	CMP	BL,'G'
	JNZ	PP60A

	CALL	FIND_SYMBOL	;VALID SYMBOL?
	JNC	PP61
	MOV	AX,16		;UNDEFINED
	JMP	ERROR

PP60A:
	JMP	PP64

PP61:
	MOV	BP,AX
	XOR	DI,DI		;MAKE SURE SEGMENT NAME IS <15 CHRS
	MOV	CX,10H
PP61A:
	INC	DI
	CMP	BYTE PTR ES:[DI],SEGTYPE_COUNT+1
	JC	PP61B
	LOOP	PP61A
	JCXZ	PP61C

PP61B:
	MOV	AX,BP
	OR	AL,AL		;LEGAL SYMBOL?
	JNZ	PP62
	MOV	AX,31		;ILLEGAL
	JMP	ERROR

PP61C:
	MOV	AX,34		;NAME TOO LONG
	JMP	ERROR

PP62:
	MOV	AL,es:[symbol_segment_type]		; KTS 03-16-94 03:54pm
	MOV	CSTYPE,AL	;STORE CURRENT SEGMET TYPE

	MOV	BX,CSPTR	;IS THIS THE FIRST SEGM?
	OR	BX,BX		;BYPASS IF SO
	JZ	PP63

	PUSH	ES		;SAVE STUFF ASSOCIATED WITH SEGMENT
	MOV	ES,BX

	MOV	AX,OBJPTRL	;SAVE OBJPTRS
	MOV	ES:[symbol_segment_lorg_l],AX
	MOV	AX,OBJPTRH
	MOV	ES:[symbol_segment_lorg_h],AX

	MOV	AX,DELTA_L	;SAVE DELTAS
	MOV	ES:[symbol_segment_delta_l],AX
	MOV	AX,DELTA_H
	MOV	ES:[symbol_segment_delta_h],AL		; kts downgrade to 24 bits

	MOV	AX,CSCOUNTL	;SAVE CSCOUNT
	MOV	ES:[symbol_segment_count_l],AX
	MOV	AX,CSCOUNTH
	MOV	ES:[symbol_segment_count_h],AL		; kts downgrade to 24 bits

	MOV	AX,SSKBL	;SAVE SSKB
	MOV	ES:[symbol_segment_bottom_l],AX
	MOV	AX,SSKBH
	MOV	ES:[symbol_segment_bottom_h],AL		; kts downgrade to 24 bits

	MOV	AX,SSKTL	;SAVE SSKT
	MOV	ES:[symbol_segment_top_l],AX
	MOV	AX,SSKTH
	MOV	ES:[symbol_segment_top_h],AL		; kts downgrade to 24 bits

	POP	ES

PP63:				;LOAD STUFF ASSOCIATED WITH SEGMENT

	MOV	CSPTR,ES	;LOAD OBJPTRS
	MOV	AX,ES:[symbol_segment_lorg_l]
	MOV	OBJPTRL,AX
	MOV	AX,ES:[symbol_segment_lorg_h]
	MOV	OBJPTRH,AX

	MOV	AX,ES:[symbol_segment_delta_l]	;LOAD DELTAS
	MOV	DELTA_L,AX
	xor	ax,ax
	MOV	AL,ES:[symbol_segment_delta_h]		; kts downgrade to 24 bits
	MOV	DELTA_H,AX

	MOV	AX,ES:[symbol_segment_count_l]	;LOAD CSCOUNT
	MOV	CSCOUNTL,AX
	xor	ax,ax
	MOV	al,ES:[symbol_segment_count_h]
	MOV	CSCOUNTH,AX

	MOV	AX,ES:[symbol_segment_bottom_l]	;LOAD SSKB
	MOV	SSKBL,AX
	xor	ax,ax
	MOV	AL,ES:[symbol_segment_bottom_h]		; kts downgrade to 24 bits
	MOV	SSKBH,AX

	MOV	AX,ES:[symbol_segment_top_l]	;LOAD SSKT
	MOV	SSKTL,AX
	xor	ax,ax
	MOV	AL,ES:[symbol_segment_top_h]		; kts downgrade to 24 bits
	MOV	SSKTH,AX

	RET

;==============================================================================

PP64:
	CMP	AX,'OP'         ;PORG?
	JNZ	PP64andahalf
	CMP	BX,'GR'
	JNZ	PP64andahalf
	OR	CL,CL
	JNZ	PP65

	CALL	PARSE_EXP	;COMPUTER DELTA
	MOV	AX,OBJPTRL
	SUB	CX,AX
	MOV	AX,OBJPTRH
	SBB	BX,AX

	MOV	DELTA_L,CX
	MOV	DELTA_H,BX
	RET

PP64andahalf:

	ifdef	SNES
; delta = org-porg
;==============================================================================

; delta += oldorg - org

	CMP	AX,'OL'         ;LORG?
	JNZ	PP65
	CMP	BX,'GR'
	JNZ	PP65
	OR	CL,CL
	JNZ	PP65

	CALL	PARSE_EXP	;COMPUTER DELTA
	push	cx			; remember new org
	push	bx
					; bl:cx = new lorg

	MOV	AX,OBJPTRL		; find delta between old and new lorg
	SUB	ax,cx
	MOV	cx,OBJPTRH
	SBB	cx,bx
					; now bx:ax = delta to add to delta
	mov	cx,DELTA_L
	add	ax,cx
	mov	cx,DELTA_H
	adc	cx,bx
					; now bx:ax = new delta
	mov	DELTA_L,ax
	mov	DELTA_H,bx

	pop	bx
	pop	cx

	MOV	OBJPTRH,BX
	MOV	OBJPTRL,CX
	RET
	endif

;==============================================================================

PP65:
	ifdef	SNES
	CMP	AX,'FS'         ;SFXM(AP)?
	JNZ	@@not_SFXMAP
	CMP	BX,'MX'
	JNZ	@@not_SFXMAP

	mov	ax,WORD PTR [PARAM_FIELD]
	call	UpcaseWord
	CMP	ax,'NO' ;SFXMAP ON?
	JZ	PP66
	CMP	ax,'FO' ;SFXMAP OFF?
	JZ	PP67
	MOV	AX,11
	JMP	ERROR
PP66:
	OR	SDB2,80H
	RET
PP67:
	AND	SDB2,NOT	80H
	RET
@@not_SFXMAP:
	endif

	CMP	AX,'IS'         ;SIZE?
	JNZ	@@not_SIZE
	CMP	BX,'EZ'
	JNZ	@@not_SIZE
	CALL	PARSE_EXP
	MOV	CSCOUNTL,CX
	MOV	CSCOUNTH,BX
	RET
@@not_SIZE:

;==============================================================================

	CMP	AX,'NI'         ;INCS(YM)?
	JNZ	@@Jmp
	CMP	BX,'SC'
	JZ	@@Ok72
@@Jmp:
	jmp	PP83
@@Ok72:
	MOV	AX,INT21_OPENFILE	;OPEN FILE
	MOV	DX,OFFSET PARAM_FIELD
	INT	21H
	JNC	PP74
	MOV	AX,12
	JMP	ERROR
PP74:
	MOV	INCHAND,AX
PP75:
	push	ds
	push	dx
	MOV	BX,INCHAND
	mov	ax,BUFSEG2
	mov	ds,ax
	xor	dx,dx
	MOV	CX,FILE_BUFFER_SIZE
	CALL	FileRead		;INITIATE
	mov	cx,ax
	pop	dx
	pop	ds
	SUB	CX,80H
	JBE	@@IncSymDone
	MOV	SI,80H

PP76:
	XOR	DI,DI
PP77:
	CALL	ReadByteBufSeg2		;LOAD SYMBOL NAME
	JCXZ	@@IncSymDone
	MOV	[LINE+DI],AL
	INC	DI
	OR	AL,AL
	JNZ	PP77

	CALL	ReadByteBufSeg2		;LOAD SYMBOL VALUE
	JCXZ	@@IncSymDone
	MOV	DL,AL
	CALL	ReadByteBufSeg2
	JCXZ	@@IncSymDone

	MOV	DH,AL
	CALL	ReadByteBufSeg2
	MOV	BL,AL
	CALL	ReadByteBufSeg2
	MOV	SVL,DX
	MOV	BH,AL
	MOV	SVH,BX

	PUSH	SI		;ADD THE SYMBOL
	PUSH	CX
	CALL	ADD_SYMBOL
	POP	CX
	POP	SI
	JCXZ	@@IncSymDone
	JMP	PP76

@@IncSymDone:
	MOV	AH,3EH		;CLOSE INCSYM FILE
	MOV	BX,OFFSET INCHAND
	INT	21H
	RET

;-----------------------------------------------------------------------------
; Inputs:
;	cx = # of bytes left in buffer
; Outputs:
;	cx = # of bytes left(if zero, end of file reached)


ReadByteBufSeg2:
	MOV	ES,BUFSEG2
	MOV	AL,ES:[SI]	;READ 1 BYTE FROM BUFFER
	INC	SI
	DEC	CX
	JNZ	@@RBBS2Ret
	PUSH	AX
	PUSH	BX

	push	ds
	push	dx
	MOV	BX,INCHAND
	mov	ax,BUFSEG2
	mov	ds,ax
	xor	dx,dx
	MOV	CX,FILE_BUFFER_SIZE
	CALL	FileRead
	mov	cx,ax				; get # of bytes actually read
	pop	dx
	pop	ds

	POP	BX
	POP	AX
	XOR	SI,SI
@@RBBS2Ret:
	RET

;==============================================================================

PP83:
	CMP	AX,'IL'                         ;LIST?
	JNZ	@@not_LIST
	CMP	BX,'TS'
	JNZ	@@not_LIST
	OR	CL,CL
	JNZ	@@not_list
	mov	ax,WORD PTR [PARAM_FIELD]
	call	UpcaseWord
	CMP	ax,'NO'
	JZ	@@LIST_On
	CMP	ax,'FO'
	JZ	@@LIST_Off
	MOV	AX,11
	JMP	ERROR
@@LIST_On:
	OR	SDB2,8
	JMP	LIST_START
@@LIST_Off:
	AND	SDB2,NOT 8
	RET
@@not_LIST:

;==============================================================================

	CMP	AX,'VE'				;EVEN
	JNZ	@@not_EVEN
	CMP	BX,'NE'
	JNZ	@@not_EVEN
	or	cl,cl
	jnz	@@not_EVEN
	TEST	OBJPTRL,1
	JZ	@@PP87
	ADD	OBJPTRL,1
	ADC	OBJPTRH,0
@@PP87:
 	RET
@@not_EVEN:

;==============================================================================

	CMP	AX,'NI'				;INCD(IR)
	JNZ	@@not_INCDIR
	CMP	BX,'DC'
	JNZ	@@not_INCDIR
	XOR	BX,BX
	CMP	PARAM_FIELD,BL
	JNZ	@@PP89
	MOV	IDL,BX
	RET
@@PP89:
	MOV	AL,[PARAM_FIELD+BX]
	MOV	[INCDIR+BX],AL
	MOV	IDL,BX
	INC	BX
	OR	AL,AL
	JNZ	@@PP89
	CMP	[INCDIR+BX-2],'\'
	JZ	@@PP89X
	CMP	[INCDIR+BX-2],'/'			; kts 12-24-92 04:30am
	JZ	@@PP89X
	MOV	WORD PTR [INCDIR+BX-1],'\'
	MOV	IDL,BX
@@PP89X:
	RET
@@not_INCDIR:

;==============================================================================

	CMP	AX,'ES'				;SET
	JNZ	@@not_SET
	CMP	BX,'T'
	JNZ	@@not_SET
	XOR	SI,SI
	MOV	DI,PARAM_LAST
@@PP91:
	MOV	AL,[PARAM_FIELD+SI]
	MOV	[SYMBOL_FIELD+SI],AL
	INC	SI
	CMP	AL,'='
	JZ	@@PP92
	CMP	SI,DI
	JL	@@PP91
	MOV	AX,5
	JMP	ERROR
@@PP92:
	MOV	[SYMBOL_FIELD+SI-1],0
	push	si
	push	di
	CALL	FIND_SYMBOL
	pop	di
	pop	si
	JC	@@PP93

	PUSH	ES

	CALL	PARSE_EXP
	TEST	SDB1,4		;NO HONKEY MO-FO-WARD REFERENCES ALLOWED HERE
	JZ	@@SetOk
	POP	ES
	MOV	AX,53
	JMP	ERROR
@@SetOk:
	POP	ES
	MOV	WORD PTR ES:[symbol_segment_lorg_h],BX
	MOV	ES:[symbol_segment_lorg_l],CX

	TEST	SDB2,8			;IS LIST FILE ON?
	JZ	@@Ret2
	call	PrintSymbolToListing
@@Ret2:
	RET

@@PP93:
	MOV	AX,16
	JMP	ERROR
	; end kts paste
@@not_SET:

;==============================================================================

	CMP	AX,'UQ'				;QUIT
	JNZ	@@Not_QUIT
	CMP	BX,'TI'
	JNZ	@@Not_QUIT
	;----- QUIT
	mov	si,offset quit_text
	mov	bx,STDOUT
	call	PrintLine
	jmp	terminate
@@Not_QUIT:

;==============================================================================

	ifndef	GENESIS
				; assume assu(me) 11-24-92 02:08pm
	cmp	ax,'SA'
	jnz	@@not_ASSUME
	cmp	bx,'US'
	jnz	@@not_ASSUME
					; assume code here

	mov	al,PARAM_FIELD+2
	cmp	al,':'				; ensure colon
	jnz	@@Err

	mov	ax,word ptr PARAM_FIELD
	call	UpcaseWord
	cmp	ax,'PD'				; check for DP(direct page)
	je	@@DP
	ifdef	SNES
	cmp	ax,'BD'				; check for DB(data bank)
	endif
	jnz	@@Err

	ifdef	SNES
	               		; assume code for data segment
	add	si,3
	CALL	PARSE_EXP		;COMPUTE DELTA (returns in bx:cx)
	if 0			; remove comments if you want guarantee,
				; else leave in for truncation
	or	cl,ch
	or	cl,bl
	or	cl,bh
	endif
	MOV	dataAssume,cl	; ignore rest of result
	ret
	endif

@@DP:               		; assume code for data segment
	add	si,3
	CALL	PARSE_EXP		;COMPUTE DELTA (returns in bx:cx)
	or	cx,bx		; WBNIV - or in bank (should be zero,
				;  if not, this will cause assembler
				;  to complain)
	ifdef	SPC700		; SPC700 direct page can only be 0 or 1
	cmp	cx,0100h
	je	@@SPC700_ValidDP
	or	cx,cx
	jz	@@SPC700_ValidDP
	mov	ax,62		; direct page must be $0 or $100
	jmp	error
@@SPC700_ValidDP:
	endif
	MOV	directAssume,cx
	ret

@@Err:
	mov	ax,5			; syntax error
	jmp	ERROR
@@not_ASSUME:
	endif

;==============================================================================

	CMP	AX,'IF'				;FILLCHAR
	JNZ	@@Not_FILLCHAR
	CMP	BX,'LL'
	JNZ	@@Not_FILLCHAR
;;	cmp	cl,'C'
;;	jne	@@Not_FILLCHAR
	mov	ax,WORD PTR [PARAM_FIELD]
	call	UpcaseWord
	CMP	ax,'NO'
	JZ	@@On
	CMP	ax,'FO'
	JZ	@@Off
	MOV	AX,11
	JMP	ERROR
@@On:
	OR	assembleFlags,AFF_FILLFILE
	ret

@@Off:
	AND	assembleFlags,NOT AFF_FILLFILE
	RET
@@Not_FILLCHAR:

;==============================================================================

	cmp	ax,'ER'			; REPT
	jne	@@Not_REPT
	cmp	bx,'TP'
	jne	@@Not_REPT

	;----- REPT
	call	parse_exp
	TEST	SDB1,4		;NO HONKEY MO-FO-WARD REFERENCES ALLOWED HERE
	JZ	@@REPT_notFwd
	MOV	AX,53
	JMP	ERROR
@@REPT_notFwd:
	; repeat count now in cx
	or	cx,cx
	jne	@@REPT_NotZero
	MOV	AX,78
	CALL	ERROR
	inc	cx				; assume 1
@@REPT_NotZero:
	cmp	rept_level,MAX_REPT		; 16
	jne	@@DoREPT
	mov	ax,63
	jmp	error

@@DoREPT:
	mov	si,src_level
	shl	si,1              		; look up current line in current source file
	mov	ax,[line_count+si]

	mov	bx,rept_level
	shl	bx,1
	dec	cx			; kts 03-16-94 01:02pm
	mov	[rept_count+bx],cx		; save # of repetitions
	mov	[rept_linenum+bx],ax		; save source line #

	; save position in file (should point to beginning of next line)
	mov	bx,[src_handle+si]
	mov	ax,04201h			; LSEEK, offset from current pos.
	xor	cx,cx
	xor	dx,dx
	int	21h				; actually, just read file position
	jnc	@@Rept_LSEEK_Ok
	jmp	severe

@@Rept_LSEEK_Ok:
	; remember where in the file the line after the rept is
	;----- Adjust filepos by -buffersize +offset
	mov	bx,[src_endptr+si]
	cmp	bx,1000h
	jc	@@src_endptrOk
	mov	bx,1000h
@@src_endptrOk:

	sub	ax,bx
	sbb	dx,0

	add	ax,[src_ptr+si]
	adc	dx,0

;	add	ax,2		; add 2 to skip past CR/LF (lame!)
;	adc	dx,0

	mov	si,rept_level
	shl	si,2
	mov	word ptr [rept_startingpos+si],ax
	mov	word ptr [rept_startingpos+2+si],dx

	inc	rept_level
	ret

@@Not_REPT:

;------------------------------------------------------------------------------

	cmp	ax,'NE'			; ENDR
	jne	@@Not_ENDR
	cmp	bx,'RD'
	jne	@@Not_ENDR

	;----- ENDR
	cmp	rept_level,0
	jne	@@DoENDR
	mov	ax,64
	jmp	error

@@DoENDR:
	mov	si,src_level
	shl	si,1

	mov	bx,rept_level
	dec	bx
	shl	bx,1

;	mov	ax,[rept_linenum+bx]		; adjust line_count since we
;	mov	[line_count+si],ax		; are move back in the source file

	dec	[rept_count+bx]			; update # of times to loop
	js	@@ENDR_Done

					; kts moved down 01-06-93 03:19pm
	mov	ax,[rept_linenum+bx]		; adjust line_count since we
	mov	[line_count+si],ax		; are move back in the source file

	push	si				; kts added 01-16-93 08:37pm
	mov	si,bx
	shl	si,1				; rept_level*4
	mov  	dx,word ptr [rept_startingpos+si]
	mov	cx,word ptr [rept_startingpos+2+si]
	mov	ah,042h                         ; this time actually seek in the
	mov	al,0				; source file
	pop	si
	mov	bx,[src_handle+si]		; was bx, which was wrong
	int	21h
	jnc	@@SetPosOk
	jmp	severe
@@SetPosOk:
	;----- Turn off listing for this line
	and	assembleFlags,NOT AFF_LISTLINE

	;----- Resync input buffer with new position
	push	es
	mov	ax,src_level			; get current source level
	xchg	al,ah				; src_level * 256
	add	ax,bufseg			; * 16 since seg offset = * 4096 = size of src buffer(fucking lame)
	push	ax
	pop	es
	call	ReadSourceChunk
	pop	es
	ret

@@ENDR_Done:
;	add	[line_count+si],2		; Adjust source line #
	dec	rept_level
	ret

@@Not_ENDR:

	cmp	ax,'IT'			; TITL
	jne	@@Not_TITL
	cmp	bx,'LT'
	jne	@@Not_TITL

	;----- TITL
	xor	si,si
@@TITL_Next:
	mov	al,[param_field+si]
	mov	[list_TITL+si],al
	or	al,0
	je	@@TITL_Done
	inc	si
	jmp	@@TITL_Next
@@TITL_Done:
	ret
@@Not_TITL:

	cmp	ax,'AP'			; PAGE
	jne	@@Not_PAGE
	cmp	bx,'EG'
	jne	@@Not_PAGE

	;----- PAGE
	call	parse_exp
	mov	list_linesPerPage,cx
	ret
@@Not_PAGE:

	CMP	AX,'RE'			; ERROR
	jne	@@Not_ERROR
	cmp	BX,'OR'
	jne	@@Not_ERROR

	;----- ERROR
	mov	ax,67
	lea	bx,param_field
	jmp	error

@@Not_ERROR:

	CMP	AX,'ES'			; SEVERE
	jne	@@Not_SEVERE
	cmp	BX,'EV'
	jne	@@Not_SEVERE

	;----- user severe
	mov	ax,69
	lea	bx,param_field
	jmp	error
@@Not_SEVERE:

	CMP	AX,'AW'			; WARNING
	jne	@@Not_WARNING
	cmp	BX,'NR'
	jne	@@Not_WARNING

	;----- print user waring
	mov	ax,68
	lea	bx,param_field
	jmp	error
@@Not_WARNING:

	CMP	AX,'RE'			; errlevel
	jne	@@Not_ERRLEVEL
	cmp	BX,'LR'
	jne	@@Not_ERRLEVEL

	;----- set errorlevel
	call	Parse2Exp			; first parse left half
;	dx:ax = first result
;	bx:cx = second result


	TEST	oldSDB1,4		;NO HONKEY MO-FO-WARD REFERENCES ALLOWED HERE
	JZ	@@ERRLEVEL_NotFwd
;	JNZ	@@ERRLEVEL_Fwd
;	TEST	oldSDB2,4
;	JNZ	@@ERRLEVEL_Fwd

	TEST	SDB1,4		;NO HONKEY MO-FO-WARD REFERENCES ALLOWED HERE
;	JNZ	@@ERRLEVEL_Fwd
;	TEST	SDB2,4
	JZ	@@ERRLEVEL_NotFwd
;@@ERRLEVEL_Fwd:
	MOV	AX,53
	JMP	ERROR
@@ERRLEVEL_NotFwd:
			; error # to change now in ax
	cmp	ax,NUM_ERRORS-1
	ja	@@ERRLEVEL_BAD
				; now ax = error #, cx = new setting(0,1,2 or 3)
	cmp	cx,3
	ja	@@ERRLEVEL_BAD2

	mov	bx,ax
	shl	bx,1
	mov	si,[ERROR_TABLE+bx]
	mov	[si],cl		; write new error level
	ret

@@ERRLEVEL_BAD:
	mov	ax,70
	jmp	Error

@@ERRLEVEL_BAD2:
	mov	ax,71
	jmp	Error
@@Not_ERRLEVEL:

	CMP	AX,'NC'			; CNOP
	jne	@@Not_CNOP
	cmp	BX,'PO'
	jne	@@Not_CNOP
					; to align: rem = pc%alignment
					; pc += alignment-rem
	call	Parse2Exp
	or	cx,cx
	jnz	@@NotZero
	mov	ax,73
	jmp	Error
@@NotZero:
	push	ax

	push	cx			; save alignment
	mov	ax,objptrl
	mov	dx,objptrh
	div	cx        		; pc/alignment
	pop	ax			; get aligment back
	or	dx,dx			; check for remainder
	jz	@@Aligned		; if remainder = 0, then already aligned
	sub	ax,dx			; aligment-rem
	call	SkipBytes
@@Aligned:
	pop	ax
	call	SkipBytes
	ret
@@Not_CNOP:

;==============================================================================

	CMP	AX,'SA'			; ASSERT
	jne	@@Not_ASSERT
	cmp	BX,'ES'
	jne	@@Not_ASSERT
	xor	si,si
	mov	di,param_last
	call	parse_exp
	TEST	SDB1,4		;NO HONKEY MO-FO-WARD REFERENCES ALLOWED HERE
	JZ	@@ASSOk
	MOV	AX,53
	JMP	ERROR
@@ASSOk:
	or	bx,cx
	jnz	@@ASSERT_Done
	mov	ax,DATA
	mov	es,ax
	lea	di,textBuffer
	lea	si,szAssert
	call	PrintToBuffer
	call	PrintFilenameHierarchy
	lea	si,szAssertSeperator
	call	PrintToBuffer
	lea	si,PARAM_FIELD
	call	PrintToBuffer
	lea	si,szFailed
	call	PrintToBuffer
	mov	bx,STDOUT
	lea	si,textBuffer
	call	PrintLine
	INC	ERROR_TOTAL			;ADD TO ERROR TOTAL
	mov	al,DOS_RETURNCODE_ERROR
	call	SetErrorLevel
	ret
@@ASSERT_Done:
	ret
@@Not_ASSERT:

;==============================================================================

	;***** INSERT NEW PSEUDO OPS HERE!

;==============================================================================

	jmp	ExecuteMacro		; will print error if no macro found

PARSE_PSEUDO	ENDP

;==============================================================================

	ifndef	GENESIS
parse_extension	PROC	NEAR
	push	di
	push	bp

	mov	SEYES,0			; default is byte
	mov	di,mne_len
	sub	di,2
	xor	bp,bp
	cmp	[mne_field+di],'.'
	jnz	@@end
	mov	bp,word ptr [mne_field+di]
	mov	seyes,0
	cmp	bp,'B.'
	jz	@@end
	mov	seyes,40h
	cmp	bp,'W.'
	jz	@@end
	mov	seyes,80h
	cmp	bp,'L.'
	jz	@@end
@@end:
	pop	bp
	pop	di
	ret
parse_extension	ENDP

;==============================================================================

	ifdef	SNES
PUSH_LONG	PROC	NEAR

PUSHLA:
	MOV	BX,LA_SP
	CMP	BX,64
	MOV	AX,58				; LONGA stack full
	JZ	ERRR
	MOV	AL,SDB1
	AND	AL,80H
	MOV	[LA_STACK+BX],AL
	INC	LA_SP
	RET

PUSHLI:
	MOV	BX,LI_SP
	CMP	BX,64
	MOV	AX,59				; LONGI stack full
	JZ	ERRR
	MOV	AL,SDB1
	AND	AL,40H
	MOV	[LI_STACK+BX],AL
	INC	LI_SP
	RET

POPLA:
	MOV	BX,LA_SP
	DEC	BX
	MOV	AX,60				; LONGA stack empty
	JS	ERRR
	MOV	AL,[LA_STACK+BX]
	AND	SDB1,NOT 80H
	OR	SDB1,AL
	DEC	LA_SP
	RET

POPLI:
	MOV	BX,LI_SP
	DEC	BX
	MOV	AX,61				; LONGI stack empty
	JS	ERRR
	MOV	AL,[LI_STACK+BX]
	AND	SDB1,NOT 40H
	OR	SDB1,AL
	DEC	LI_SP
	RET

ERRR:
	JMP	ERROR

PUSH_LONG	ENDP
	endif
	endif

;==============================================================================
