;=============================================================================
; parse.asm: input line parsing and conditioning for spasm
; and expression parser
;=============================================================================

DATA	SEGMENT	'DATA'

IF_LEVEL	DB	0
IF_FLAG		DB	10h DUP (1)
IF_STARTINGLINE	DW	10h DUP (?)

CAPITALS	DB	0

SPACE		DB	020H

; what are these?
SSKBL		DW	0FFFFH
SSKBH		DW	0FFFFH

SSKTL		DW	0
SSKTH		DW	0

SVL		DW	0
SVH		DW	0

STBL		DW	0FFFFH	;SYMBOL TOTAL BEFORE LOCAL

porgl		dw	0
porgh		dw	0

expr_in_quotes	dw	0
mneInLineOffset	dw	0	; offset in LINE MNE was pulled from(used for
				; non-upper cased version of MNE(macros)

Uppercase LABEL BYTE
		;0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	db	00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0Ah,0Bh,0Ch,0Dh,0Eh,0Fh	;0
	db	10h,11h,12h,13h,14h,15h,16h,17h,18h,19h,1Ah,1Bh,1Ch,1Dh,1Eh,1Fh ;1
	db	' ','!','"','#','$','%','&',27h,'(',')','*','+',',','-','.','/' ;2
	db	'0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?' ;3
	db	'@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O' ;4
	db	'P','Q','R','S','T','U','V','W','X','Y','Z','[','\',']','^','_' ;5
	db	'`','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O' ;6
	db	'P','Q','R','S','T','U','V','W','X','Y','Z','{','|','}','~','' ;7
	db	80h,81h,82h,83h,84h,85h,86h,87h,88h,89h,8Ah,8Bh,8Ch,8Dh,8Eh,8Fh ;8
	db	90h,91h,92h,93h,94h,95h,96h,97h,98h,99h,9Ah,9Bh,9Ch,9Dh,9Eh,9Fh ;9
	db	0A0h,0A1h,0A2h,0A3h,0A4h,0A5h,0A6h,0A7h,0A8h,0A9h,0AAh,0ABh,0ACh,0ADh,0AEh,0AFh ;A
	db	0B0h,0B1h,0B2h,0B3h,0B4h,0B5h,0B6h,0B7h,0B8h,0B9h,0BAh,0BBh,0BCh,0BDh,0BEh,0BFh ;B
	db	0C0h,0C1h,0C2h,0C3h,0C4h,0C5h,0C6h,0C7h,0C8h,0C9h,0CAh,0CBh,0CCh,0CDh,0CEh,0CFh ;C
	db	0D0h,0D1h,0D2h,0D3h,0D4h,0D5h,0D6h,0D7h,0D8h,0D9h,0DAh,0DBh,0DCh,0DDh,0DEh,0DFh ;D
	db	0E0h,0E1h,0E2h,0E3h,0E4h,0E5h,0E6h,0E7h,0E8h,0E9h,0EAh,0EBh,0ECh,0EDh,0EEh,0EFh ;E
	db	0F0h,0F1h,0F2h,0F3h,0F4h,0F5h,0F6h,0F7h,0F8h,0F9h,0FAh,0FBh,0FCh,0FDh,0FEh,0FFh ;F
DATA	ENDS

;=============================================================================
.CODE
;-----------------------------------------------------------------------------

BLIF	PROC	NEAR		;BREAK LINE INTO FIELDS.
				;OR SDB1,2 "MT", IF LABEL ONLY
				;SET CF IF NOT EVEN A LABEL

	lea	bx,Uppercase

	AND	SDB1,NOT 2	;CLEAR "MT" FLAG
	MOV	MNE_FIELD,0	;ERASE OLD DATA
	MOV	PARAM_FIELD,0
	MOV	PARAM_LAST,0
	MOV	AL,[LINE]
	OR	AL,AL		;IS FIRST CHR NUL, OR COMMENT CHR?
	JZ	@@BlankLine
	CMP	AL,';'
	JZ	@@BlankLine
	CMP	AL,'*'
	JNZ	@@NotEmpty
@@BlankLine:
	STC			;RETURN WITH CF SET
	RET

@@NotEmpty:
	XOR	SI,SI		;INDEX TO "LINE"
@@LabelLoop:
	MOV	AL,[LINE+SI]	;SCAN PAST LABEL IF THERE IS ONE
	CMP	AL,20H
	JZ	@@StartMNE
	CMP	AL,';'
	JZ	@@StartMNE
	OR	AL,AL
	JZ	@@LineEmpty
	INC	SI
	JMP	@@LabelLoop

@@LineEmpty:
	OR	SDB1,2		;MAKE LINE "MT"
	CLC
	RET

@@StartMNE:
	CALL	NEXT_TEXT	;SEARCH FOR MNEMONIC
	JC	@@LineEmpty		;IF NONE THEN DONE, LINE IS "MT"
	XOR	DI,DI
	mov	mneInLineOffset,si
@@NextMNEChar:
	MOV	AL,[LINE+SI]	;ELSE COPY MNEMONIC (OR PSEUDO OP)
	OR	AL,AL		;UNTIL NUL OR SPACE IS REACHED
	JZ	@@MNEDone
	CMP	AL,20H
	JZ	@@MNEDone
	; Make into capitals
	;CMP	AL,'a'      	;LOWER CASE?
	;JB	@@NotLowercase
	;CMP	AL,'z'
	;JA	@@NotLowercase
	;and	AL,NOT 20h	; Always uppercase MNE_FIELD
;@@NotLowercase:
	xlatb

	MOV	[MNE_FIELD+DI],AL			;INTO "MNE_FIELD"
	INC	SI		;ONE CHR AT A TIME
	INC	DI
	CMP	DI,MACRO_NAME_LEN+1		;FIELD TOO LONG?
	JC	@@NextMNEChar
	MOV	AX,3
	CALL	ERROR
@@MNEDone:
	MOV	[MNE_FIELD+DI],0;DELINEATE WITH 0
	MOV	[MNE_LEN],DI	;MARK END

	XOR	DI,DI
	CALL	NEXT_TEXT	;SEARCH FOR PARAMETERS
	JC	@@ParamDone		;JUMP IF NONE
	MOV	SPACE,020H
@@ParamLoop:
	MOV	AL,[LINE+SI]	;COPY PARAMATERS INTO "PARAM_FIELD"
	CMP	AL,''''		;SINGLE QUOTE
	JNZ	@@NotQuote
	XOR	SPACE,NOT 20H
@@NotQuote:
	CMP	AL,SPACE		;SPACES ARE IGNORED IF NOT IN QUOTES
	JZ	@@ParamSkipChar
	CMP	AL,';'          	;COMMENT FIELD?
	JZ	@@ParamDone
	OR	AL,AL		;OR END OF THE LINE?
	JZ	@@ParamDone	;THEN DONE

	if	0
	; kts 04-04-94 05:56pm for genesis version until case sense done
	; Make into capitals
	CMP	AL,'a'      	;LOWER CASE?
	JB	@@BLNotLowercase
	CMP	AL,'z'
	JA	@@BLNotLowercase
	and	AL,NOT 20h	; Always uppercase PARAM_FIELD in genesis version
@@BLNotLowercase:
	; end kts
	endif

	MOV	[PARAM_FIELD+DI],AL
	MOV	[PARAM_LAST],DI
	INC	DI
@@ParamSkipChar:
	INC	SI
	CMP	DI,MAX_PARAM_FIELD_LEN			;FIELD TO LONG?
	JC	@@ParamLoop
	MOV	AX,4
	CALL	ERROR
@@ParamDone:
	MOV	[PARAM_FIELD+DI],0			;DELINEATE WITH 0
	CLC
	RET

;-----------------------------------------------------------------------------

NEXT_TEXT:			;SEARCH [LINE+SI] FOR NON-SPACE CHR
	MOV	AL,[LINE+SI]	;GET CHR
	OR	AL,AL
	JZ	@@EndOfLine
	CMP	AL,20H		;NON-SPACE CHR?
	JNZ	@@NotWhiteSpace		;JUMP IF SO
	INC	SI
	JMP	NEXT_TEXT
@@EndOfLine:
	STC			;SET CF IF END OF THE LINE
	RET
@@NotWhiteSpace:
	CMP	AL,';'      	;COMMENT CHR?
	JZ	@@EndOfLine
	CMP	AL,'*'
	JZ	@@EndOfLine
	CLC			;CLEAR CF IF MORE TEXT FOUND
	RET

BLIF	ENDP

;=============================================================================
; Outputs:
;	di = linelen = # of chars in line
;-----------------------------------------------------------------------------

GET_LINE	PROC	NEAR	;READ INPUT FILE AND EXTRACT A LINE OF TEXT
				;AND CAPITALIZE IT.  [UNLESS WITHIN ' ']
				;CF SET IF EOF

	MOV	LINELEN,0

	TEST	SDB1,SDB1F_MACRO		;MACRO ENGAGED?
	JZ	@@GL
	JMP	GET_MAC

@@GL:
	inc	word ptr [_lines_assembled]
	MOV	AX,SRC_LEVEL	;FIND BUFSEG -- USE SI AS INDEX TO LEVEL
	MOV	SI,AX
	SHL	SI,1
	INC	[LINE_COUNT+SI]
	CMP	[LINE_COUNT+SI],0FFFFH
	JNZ	@@FileOk
	; file too long(more that 65535 lines)
	PUSH	AX
	MOV	AX,45
	CALL	ERROR
	POP	AX
@@FileOk:
	XCHG	AL,AH
	ADD	AX,BUFSEG
	MOV	ES,AX

	XOR	DI,DI		;USE DI AS INDEX TO LINE
	;MOV	CAPITALS,20h

@@Loop:
	MOV	AX,[SRC_PTR+SI]	;POINT AT WHERE WE LAST LEFT OFF
	INC	AX
	MOV	[SRC_PTR+SI],AX

@@TryAgain:
	CMP	AX,[SRC_ENDPTR+SI]
	JAE	@@GL5
	CMP	AX,1000H
	JC	@@SourceBufferOk
	push	ds
	call	ReadSourceChunk
	pop	ds
	JMP	@@TryAgain
@@SourceBufferOk:
	MOV	BP,AX
	MOV	AL,ES:[BP]
	CMP	AL,''''
	JZ	@@StoreIt
	CMP	AL,'"'
	JZ	@@StoreIt
	CMP	AL,10		;LF?
	JZ	@@Loop		;IGNORE
	CMP	AL,0		;NUL?
	JNZ	@@NotNull		;CHANGE TO SPACE
	MOV	AL,20H
	JMP	@@StoreIt
@@NotNull:
	CMP	AL,13		;CR?
	JNZ	@@NotCR
	XOR	AL,AL		;CHANGE TO NUL
	JMP	@@StoreIt
@@NotCR:
	CMP	AL,9		;TAB?
	JNZ	@@NotTab
	MOV	AL,20H		;CHANGE TO SPACE
	JMP	@@StoreIt
@@NotTab:
	CMP	AL,'a'      	;LOWER CASE?
	JB	@@GL3A
	CMP	AL,'z'
	JA	@@GL3A
	XOR	AL,CAPITALS	;CHANGE TO UPPER CASE IF "CAPITALS" IS ON
	JMP	@@StoreIt
@@GL3A:
	CMP	CAPITALS,0
	JZ	@@StoreIt

	ifdef	PIGSKIN
	CMP	AL,'['		;ALSO CHANGE [] TO ()
	JNZ	@@NotLeftBrocket
	MOV	AL,'('
	JMP	@@NotRightBrocket
@@NotLeftBrocket:
	CMP	AL,']'
	JNZ	@@NotRightBrocket
	MOV	AL,')'
@@NotRightBrocket:
	endif

@@StoreIt:
	MOV	[LINE+DI],AL
	CMP	DI,MAX_LINE_LENGTH		;IF LINE EXCEEDS 250 BYTES THEN COMPLAIN
	JAE	@@LineTooLong
	INC	DI
	OR	AL,AL		;IS THIS THE END OF THE LINE?
	JNZ	@@Loop
	DEC	DI
	MOV	LINELEN,DI
	CLC			;RETURN WITH CF CLEAR -- DI = LENGTH
	RET

@@GL5:
	cmp	di,0
	je	@@ReallyEOF
	mov	[LINE+1+DI],0		; Zero-terminate the input line
	mov	linelen,di
	clc
	ret

@@ReallyEOF:
	JMP	POP_FILE	;EOF

@@LineTooLing:
	MOV	AX,25
	CALL	ERROR
	STC
	RET

;-----------------------------------------------------------------------------

GET_MAC:			;FILL LINE WITH MACRO DATA

	MOV	BX,MACLEVEL	;BX IS INDEX TO MACRO LEVEL
	SHL	BX,1

	INC	[MACLINE+BX]
	MOV	ES,MACSEG
	XOR	DI,DI
	MOV	SI,[MACPTR+BX]
@@GetMacLoop:
	MOV	AL,ES:[SI]
	INC	SI
	CMP	AL,'{'			;IF PARAMETER THEN INSERT
	JZ	@@HandleParam
@@NotParam:
	MOV	[LINE+DI],AL
	INC	DI
	CMP	DI,MAX_LINE_LENGTH
	JA	@@LineTooLongTemp
	OR	AL,AL
	JNZ	@@GetMacLoop
	CLC
	DEC	DI
	MOV	LINELEN,DI
	MOV	[MACPTR+BX],SI
	RET
@@LineTooLongTemp:
	JMP	@@LineTooLong

;-----------------------------------------------------------------------------

@@HandleParam:
	CMP	BYTE PTR ES:[SI+1],'}'
	JNZ	@@NotParam
	MOV	AL,ES:[SI]
	lea	bx,Uppercase			; kts 04-19-94 05:53pm
	xlatb
	MOV	BX,MACLEVEL	;BX IS INDEX TO MACRO LEVEL
	SHL	BX,1


	ADD	SI,2
	ifdef	GENESIS
	CMP	AL,'X'
	JZ	@@Extension
	endif
	CMP	AL,'U'
	JZ	@@Unique

	CMP	AL,'L'
	JZ	@@LineNo

	CMP	AL,'F'
	JZ	@@FileName

	CMP	AL,'N'				;# OF PARAMETERS
	JNZ	@@Param

;-----------------------------------------------------------------------------
; do # of params
	MOV	AL,BYTE PTR [MACPC+BX]
	CALL	HEXOUT
	MOV	[LINE+DI],AL
	INC	DI
	JMP	@@GetMacLoop

;-----------------------------------------------------------------------------

	ifdef	GENESIS
@@Extension:
	PUSH	BX				;SIZE EXTENSION
	SHR	BX,1
	MOV	AL,[MACEXT+BX]
	POP	BX
	OR	AL,AL
	JZ	@@GetMacLoop
	MOV	[LINE+DI],'.'
	INC	DI
	MOV	[LINE+DI],AL
	INC	DI
	JMP	@@GetMacLoop
	endif

;-----------------------------------------------------------------------------

@@Unique:
	mov	al,byte ptr [_lines_assembled]
	CALL	HEXOUT
	MOV	[LINE+DI],AH
	INC	DI
	MOV	[LINE+DI],AL
	INC	DI
	mov	al,byte ptr [_lines_assembled+1]
	CALL	HEXOUT
	MOV	[LINE+DI],AH
	INC	DI
	MOV	[LINE+DI],AL
	INC	DI
@@Invalid:
	JMP	@@GetMacLoop

;-----------------------------------------------------------------------------

@@FileName:
	push	si
	push	bx
	push	es
	push	ds
	pop	es

	MOV	BX,SRC_LEVEL
	ERRIF	FILESPEC_LEN NE 80H "FILESPEC_LEN has changed, code must be updated"
	MOV	CL,7
	SHL	BX,CL
	ADD	BX,OFFSET FILESPEC
	MOV	si,BX

	add	di,OFFSET LINE
	mov	ah,'$'
	call	PrintToBufferTerm

	sub	di,OFFSET LINE
	pop	es
	pop	bx
	pop	si
	JMP	@@GetMacLoop

;-----------------------------------------------------------------------------

@@LineNo:
	PUSH	BP
	MOV	BP,SRC_LEVEL			;SOURCE LINE
	SHL	BP,1

;	ax = # to print in decimal
;       es:di -> buffer to print into

	if	1
	push	es
	push	ds
	pop	es
	add	di,OFFSET LINE

	MOV	AX,WORD PTR [LINE_COUNT+BP]
	cld
	call	PrintDec
	sub	di,OFFSET LINE

	pop	es

	else
	MOV	AL,BYTE PTR [LINE_COUNT+BP+1]
	CALL	HEXOUT
	MOV	[LINE+DI],AH
	INC	DI
	MOV	[LINE+DI],AL
	INC	DI
	MOV	AL,BYTE PTR [LINE_COUNT+BP]
	CALL	HEXOUT
	MOV	[LINE+DI],AH
	INC	DI
	MOV	[LINE+DI],AL
	INC	DI
	endif
	POP	BP
	JMP	@@GetMacLoop

;-----------------------------------------------------------------------------
				; if here, must be a parameter
@@Param:
	SUB	AL,'1'				;PARAMETER 1->9
	CMP	AL,BYTE PTR [MACPC+BX]
	JAE	@@Invalid

	XOR	AH,AH		;POINT TO PARAMETER FIELD
	MOV	CL,MACRO_PARAMLEN_SHIFTVAL
	SHL	AX,CL
	MOV	BP,AX
	MOV	AX,MACLEVEL	;BX IS INDEX TO MACRO LEVEL
	MOV	CL,MACRO_PARAM_SHIFTVAL
	SHL	AX,CL
	ADD	BP,AX

@@CopyParamLoop:
	MOV	AL,[MACPARAM+BP]
	INC	BP
	OR	AL,AL
	JNZ	@@KeepGoing
	JMP	@@GetMacLoop 			; done
@@KeepGoing:
	MOV	[LINE+DI],AL
	INC	DI
	CMP	DI,MAX_LINE_LENGTH
	JBE	@@CopyParamLoop
@@LineTooLong:
	MOV	MACPTR,SI		;LINE TOO LONG
	MOV	AX,25
	CALL	ERROR
	STC
	RET
GET_LINE	ENDP

;=============================================================================
; Destroys:
;	?,ax,bx,bp,si
; Returns:
;	answer = bx:cx

PARSE_EXP	PROC	NEAR	;PARSES "EXPRESSION" ([PARAM_FIELD+SI] TO
				;[PARAM_FIELD+DI]) AND RETURNS 32-BIT VALUE
				;IN BX:CX

	MOV	OVERRIDE,0	;SET OVERIDE FLAG TO NONE
	mov	expr_in_quotes,0

	CMP	PARAM_FIELD,0	;IS PARAM_FIELD EMPTY?
	JNZ	@@PE
@@Nope:
	MOV	AX,26
	JMP	ERROR

@@PE:
	MOV	AX,OFFSET PARAM_FIELD			;SAVE START AND END OF PARAMETER
	ADD	AX,SI		;IN CASE WE HAVE TO CALL HELP_SCOTT
	MOV	EXPSTART,AX
	; check for null expression
	cmp	[PARAM_FIELD+si],0
	jz	@@Nope

	MOV	AX,OFFSET PARAM_FIELD
	ADD	AX,DI
	MOV	EXPEND,AX
	XOR	BX,BX		;CLEAR BX:CX
	MOV	CX,BX

	MOV	AL,[PARAM_FIELD+SI]			;GET FIRST CHR IN PARAM FIELD

	CMP	[PARAM_FIELD+SI-1],'#'  ;IMMEDIATE MODE?
	JZ	@@PE0A		;THEN SKIP OVERRIDE CHECK

	CMP	AL,'<'          ;CHECK FOR OVERRIDE CHARACTERS
	JZ	@@OR1
	CMP	AL,'|'
	JZ	@@OR2
	CMP	AL,'!'
	JZ	@@OR2
	CMP	AL,'>'
	JZ	@@OR3
	JMP	@@PE0A

@@OR1:
	MOV	OVERRIDE,1
	INC	SI
	JMP	@@PE
@@OR2:
	MOV	OVERRIDE,2
	INC	SI
	JMP	@@PE
@@OR3:
	MOV	OVERRIDE,3
	INC	SI
	JMP	@@PE

@@PE0A:
	CMP	AL,'$'          ;HEXADECIMAL EXPRESSION?
	JZ	@@HEXIN
	CMP	AL,'%'          ;BINARY EXPRESSION?
	JZ	@@BININ
	CMP	AL,'0'          ;DECIMAL EXPRESSION?
	JC	@@PE1
	CMP	AL,'9'
	JA	@@PE1
	JMP	@@DI1
@@PE1:
	CMP	AL,LLCHR
	JZ	@@PE2
	or	al,al
	jz	@@barf
	jmp	@@symbolin

@@PE2:
	MOV	AL,LOCAL_SIG          ;LOCAL LABEL?
	MOV	[PARAM_FIELD+SI],AL
	JMP	@@SYMBOLIN

@@DECIN:
	INC	SI		;GET NEXT ASCII CHR
	CMP	DI,SI
	JC	@@DI2
	MOV	AL,[PARAM_FIELD+SI]
@@DI1:
	XOR	AH,AH		;(ENTRY POINT)
	SUB	AL,30H
	CMP	AL,9
	JA	@@BARF
	PUSH	AX		;MULTIPLY BX:CX BY 10
	SHL	CX,1
	RCL	BX,1
	MOV	BP,BX
	MOV	AX,CX
	SHL	CX,1
	RCL	BX,1
	SHL	CX,1
	RCL	BX,1
	ADD	CX,AX
	ADC	BX,BP
	POP	AX		;ADD IN DIGIT
	ADD	CX,AX
	ADC	BX,0
	JMP	@@DECIN
@@DI2:
	RET

@@BININ:
	INC	SI		;GET NEXT DIGIT
	CMP	DI,SI
	JC	@@BI2
	MOV	AL,[PARAM_FIELD+SI]
	cmp	al,'_'
	je	@@binin
	SUB	AL,30H		;CONVERT INTO VALUE
	SHR	AL,1		;WHAT A HACK!
	JNZ	@@BARF
	RCL	CX,1
	RCL	BX,1
	JMP	@@BININ
@@BI2:
	RET

@@BARF:
	JMP	@@PUKE

@@HEXIN:
	INC	SI		;POINT TO NEXT ASCII CHR
	CMP	DI,SI		;PAST END?
	JC	@@HI2		;DONE IF SO

	cmp	al,'_'
	je	@@hexin

	MOV	AL,[PARAM_FIELD+SI]			;GET THE CHR
	MOV	AH,'0'
	CMP	AL,AH		;INVALID CHR?
	JC	@@BARF
	CMP	AL,'9'      ;BETWEEN '0' & '9'
	JBE	@@HI1		;OKAY
	CMP	AL,'A'      ;OTHERWISE MAKE SURE IT'S BETWEEN A & F
	JC	@@BARF
	CMP	AL,'F'
	JA	@@BARF
	MOV	AH,'A'-0AH
@@HI1:
	SUB	AL,AH		;HERE IS THE NIBBLE

	SHL	CX,1		;PREPARE BX:CX FOR 4 NEW BITS
	RCL	BX,1
	SHL	CX,1
	RCL	BX,1
	SHL	CX,1
	RCL	BX,1
	SHL	CX,1
	RCL	BX,1

	OR	CL,AL		;MASK IT IN
	JMP	@@HEXIN
@@HI2:
	RET

;-----------------------------------------------------------------------------

@@SYMBOLIN:
	XOR	BX,BX
@@SI0:
	MOV	AL,[PARAM_FIELD+SI]	     ;MOVE SECTION OF [PARAM_FIELD]
	INC	SI

;	cmp	al,''''
;	jne	@@not_single_quote
;	test	expr_in_quotes,1
;@@not_single_quote:
;	test	expr_in_quotes,1
;	jnz	...

	; kts moved up from below 03-15-94 01:04am
	push	bx
	lea	bx,caseInsensitiveSymbolTranslation
	test	_bCaseSensitive,1
	jz	@@NotCaseSensitive
	lea	bx,caseSensitiveSymbolTranslation
@@NotCaseSensitive:
	xlatb
	pop	bx

@@in_quotes:
	MOV	[SYMBOL_FIELD+BX],AL	     ;INTO [SYMBOL_FIELD]
	INC	BX
	CMP	SI,DI
	JA	@@SI2

	or	al,al
	jz	@@puke
	;CMP	AL,'0'
	;JC	@@PUKE
	;CMP	AL,'9'
	;JBE	@@SI1
	;CMP	AL,'A'
	;JC	@@PUKE
	;CMP	AL,'Z'
	;JBE	@@SI1
	;CMP	AL,'_'
	;JZ	@@SI1
	;CMP	AL,'.'
	;JZ	@@SI1
	;CMP	AL,LOCAL_SIG
	;JNZ	@@PUKE
@@SI1:
	CMP	BX,SYMBOL_LENGTH
	JBE	@@SI0
	MOV	AX,14		;SYMBOL SIZE TOO LONG!
	JMP	ERROR
@@SI2:
	MOV	[SYMBOL_FIELD+BX],0

	CALL	FIND_SYMBOL	;FIND SYMBOL AND IT'S VALUE
	JC	@@SI3
	RET
@@SI3:
	TEST	SDB1,11H	;ARE WE FILLING HOLES YET?
	JNZ	@@SI4
	JMP	MAKE_HOLE	;IF NOT, THEN MAKE NEW HOLE

@@SI4:
	MOV	AX,16		;SYMBOL UNDEFINED
	CALL	ERROR
	XOR	BX,BX
	MOV	CX,BX
	RET
;-----------------------------------------------------------------------------

@@PUKE:
	JMP	HELP_SCOTT	;YIKES!  THIS COULD BE A COMPLEX EXPRESSION!

PARSE_EXP	ENDP

;=============================================================================

PUSH_IF	PROC	NEAR		;HANDLE NESTED "IF" STATEMENTS
				;AL=1 -> OKAY TO ASSEMBLE
				;AL=0 -> BYPASS ASSEMBLY

	TEST	SDB1,4		;IF FWD REF THEN BITCH
;	JNZ	@@PI
;	TEST	SDB2,4
	JZ	@@NotForwardRef
;@@PI:
	MOV	AX,53
	CALL	ERROR
	XOR	AL,AL
@@NotForwardRef:
	XOR	BH,BH
	MOV	BL,IF_LEVEL
	CMP	BL,0FH
	JC	@@NotTooDeep
	MOV	AX,21
	JMP	ERROR
@@NotTooDeep:
	; kts added 01-17-93 00:31am
	push	ax
	mov	si,src_level
	shl	si,1
	mov	ax,[line_count+si]
	mov	cx,bx
	add	bx,bx
	mov	[IF_STARTINGLINE+BX],ax
	pop	ax
	; end kts addition
	mov	bx,cx
	INC	BL
	MOV	[IF_FLAG+BX],AL
	MOV	IF_LEVEL,BL
	RET
PUSH_IF	ENDP

;=============================================================================

POP_IF	proc	near
	XOR	BH,BH
	MOV	BL,IF_LEVEL
	OR	BL,BL
	JNZ	@@Ok
	MOV	AX,22
	JMP	ERROR
@@Ok:
	DEC	IF_LEVEL
	RET

POP_IF	ENDP

;=============================================================================

	ifndef	SNES

POS_DST	PROC	NEAR		;POSITION SI,DI TO DESTINATION FIELD
	XOR	AH,AH
	MOV	DI,PARAM_LAST
	MOV	SI,DI
	OR	SI,SI
	JZ	@@PDST3
@@PDST1:
	MOV	AL,[PARAM_FIELD+SI]
	DEC	SI
	JS	@@PDST3
	CMP	AL,')'
	JNZ	@@PDST2
	INC	AH
@@PDST2:
	CMP	AL,'('
	JNZ	@@PDST2A
	DEC	AH
@@PDST2A:
	OR	AH,AH
	JNZ	@@PDST1
	CMP	AL,','
	JNZ	@@PDST1
	ADD	SI,2
	CMP	SI,PARAM_LAST
	JA	@@PDST3
	CLC
	RET
@@PDST3:
	STC
	RET
POS_DST	ENDP

;-----------------------------------------------------------------------------

POS_SRC	PROC	NEAR		;POSITION SI,DI TO SOURCE FIELD
	XOR	SI,SI
	XOR	AH,AH
	MOV	DI,PARAM_LAST
	OR	DI,DI
	JNZ	@@PSRC1
	MOV	AX,26
	CALL	ERROR
	ADD	SP,2
	RET
@@PSRC1:
	MOV	AL,[PARAM_FIELD+DI]
	DEC	DI
	CMP	DI,SI
	JL	@@PSRC3
	CMP	AL,')'
	JNZ	@@PSRC2
	INC	AH
@@PSRC2:
	CMP	AL,'('
	JNZ	@@PSRC2A
	DEC	AH
@@PSRC2A:
	OR	AH,AH
	JNZ	@@PSRC1
	CMP	AL,','
	JNZ	@@PSRC1
	RET
@@PSRC3:
	MOV	DI,PARAM_LAST
	RET
POS_SRC	ENDP

	endif

;=============================================================================
