;==============================================================================
; symbol.asm: symbol handlling for spasm/65816
;==============================================================================

DATA	SEGMENT	'DATA'

SYMBOL_FIELD	DB	20H DUP (0)
SYMBASE		DW	0
SYMCAP		DW	0
SYMPTRSEG	DW	0
SYMTOP		DW	0
SYMTOTAL	DW	0

;=============================================================================

DATA	SEGMENT	'DATA'

STXT		DB	'      Total Symbols: $'
caseSensitiveSymbolTranslationFirstChar LABEL BYTE
		;0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;0
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h ;1
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,'',00h ;2
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h ;3
	db	00h,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O' ;4
	db	'P','Q','R','S','T','U','V','W','X','Y','Z',00h,00h,00h,00h,'_' ;5
	db	00h,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o' ;6
	db	'p','q','r','s','t','u','v','w','x','y','z',00h,00h,00h,00h,'' ;7
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;8
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;9
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;A
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;B
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;C
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;D
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;E
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;F
caseSensitiveSymbolTranslation LABEL BYTE
		;0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;0
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h ;1
	db	00h,00h,00h,00h,'$',00h,00h,00h,00h,00h,00h,00h,00h,00h,'.',00h ;2
	db	'0','1','2','3','4','5','6','7','8','9',00h,00h,00h,00h,00h,00h ;3
	db	00h,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O' ;4
	db	'P','Q','R','S','T','U','V','W','X','Y','Z',00h,00h,00h,00h,'_' ;5
	db	00h,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o' ;6
	db	'p','q','r','s','t','u','v','w','x','y','z',00h,00h,00h,00h,'' ;7
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;8
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;9
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;A
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;B
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;C
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;D
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;E
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;F

caseInsensitiveSymbolTranslationFirstChar LABEL BYTE
		;0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;0
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h ;1
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,'',00h ;2
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h ;3
	db	00h,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O' ;4
	db	'P','Q','R','S','T','U','V','W','X','Y','Z',00h,00h,00h,00h,'_' ;5
	db	00h,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O' ;6
	db	'P','Q','R','S','T','U','V','W','X','Y','Z',00h,00h,00h,00h,'' ;7
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;8
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;9
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;A
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;B
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;C
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;D
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;E
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;F
caseInsensitiveSymbolTranslation LABEL BYTE
		;0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;0
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h ;1
	db	00h,00h,00h,00h,'$',00h,00h,00h,00h,00h,00h,00h,00h,00h,'.',00h ;2
	db	'0','1','2','3','4','5','6','7','8','9',00h,00h,00h,00h,00h,00h ;3
	db	00h,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O' ;4
	db	'P','Q','R','S','T','U','V','W','X','Y','Z',00h,00h,00h,00h,'_' ;5
	db	00h,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O' ;6
	db	'P','Q','R','S','T','U','V','W','X','Y','Z',00h,00h,00h,00h,'' ;7
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;8
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;9
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;A
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;B
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;C
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;D
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;E
	db	00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h,00h	;F

DATA	ENDS

;==============================================================================
.CODE
;------------------------------------------------------------------------------

ADD_SYMBOL	PROC	NEAR	;ADD A SYMBOL TO SYMBOL TABLE
				;SYMBOL TEXT IN [LINE+0]
	lea	bx,Uppercase
	mov	ax,WORD PTR [LINE]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah

	CMP	ax,'CP'
	JNZ	@@NotPredefinedPC
	CMP	[LINE+2],0
	JNZ	@@NotPredefinedPC
	MOV	AX,32
	JMP	ERROR
@@NotPredefinedPC:

	lea	bx,caseInsensitiveSymbolTranslation
	test	_bCaseSensitive,1
	jz	@@NotCaseSensitive
	lea	bx,caseSensitiveSymbolTranslation
@@NotCaseSensitive:

	XOR	SI,SI
	MOV	AL,[LINE]
	CMP	AL,LOCAL_SIG          	;DON'T FLUSH IF THIS IS LOCAL
	JZ	@@AS2

	push	bx
	CALL	FLUSH_LOCAL
	pop	bx
	AND	SDB2,NOT 1

	push	bx
	lea	bx,caseInsensitiveSymbolTranslationFirstChar
	test	_bCaseSensitive,1
	jz	@@NotCaseSensitiveFirstChar
	lea	bx,caseSensitiveSymbolTranslationFirstChar
@@NotCaseSensitiveFirstChar:

	XOR	SI,SI
	MOV	AL,[LINE]		;CHECK FIRST CHR FOR A-Z (not 0-9)
	xlatb
	pop	bx
	or	al,al
	jz	@@AS4

	;cmp	al,'_'		; kts added 5/18/92
	;jz	@@as1		; pass underline also
	;CMP	AL,'A'
	;JnC	@@AS1
	;CMP	AL,'Z'
	;Jbe	@@AS1
	;cmp	al,'a'
	;jnc	@@as1
	;cmp	al,'z'
	;ja	@@as4

@@AS1:
	MOV	AL,[LINE+SI]
	CMP	AL,20H
	JZ	@@AS1A
	CMP	AL,';'
	JZ	@@AS1A
	CMP	AL,':'
	JNZ	@@AS2
@@AS1A:
	XOR	AL,AL
@@AS2:
	MOV	[SYMBOL_FIELD+SI],AL			;STORE SYMBOL CHR
	INC	SI
	OR	AL,AL		;CHECK IF 'TWAS OK
	JZ	@@EndOfString
	CMP	SI,SYMBOL_LENGTH
	JA	@@AS3A

	xlatb
	mov	[symbol_field-1+si],al		; yuk! @@@ clean this up later
	or	al,al
	jnz	@@AS1
	;CMP	AL,'0'
	;JC	@@AS3
	;CMP	AL,'9'
	;JBE	@@AS1
	;CMP	AL,'A'
	;JC	@@AS3
	;CMP	AL,'Z'
	;JBE	@@AS1
	;cmp	al,'a'
	;jc	@@as3
	;cmp	al,'z'
	;jbe	@@as1
	;CMP	AL,'_'
	;JZ	@@AS1
	;CMP	AL,'.'
	;JZ	@@AS1
	;CMP	AL,LOCAL_SIG
	;JZ	@@AS1
;@@AS3:
	MOV	AX,15		;ILLEGAL CHR
	JMP	ERROR
@@AS3A:
	MOV	AX,14		;SYMBOL NAME TOO LONG
	JMP	ERROR
@@AS4:
	MOV	AX,1		;SYMBOL MUST BEGIN WITH ALPHA CHR
	JMP	ERROR

@@EndOfString:
	CALL	FIND_SYMBOL	;FIRST SEE IF SYMBOL ALREADY EXISTS
	JNC	@@AS8
;	jc	@@Nope
;	jmp	@@AS8
;@@Nope:
	MOV	AX,SYMTOTAL	;IF NOT, THEN ADD IT IN
	INC	SYMTOTAL
	MOV	CX,AX
	SUB	CX,DX
	MOV	ES,SYMPTRSEG
	JCXZ	@@AS5A
	MOV	DI,AX		;INSERT A PLACE IN THE SYMBOL TABLE
	SHL	DI,1
	MOV	SI,DI
	SUB	SI,2
	PUSH	DS
	MOV	DS,SYMPTRSEG
	STD
	REP	MOVSW
	CLD			; always have direction flag clear by default
	POP	DS
	JMP	@@AS6

@@AS5A:
	MOV	DI,DX
	SHL	DI,1

@@AS6:
	MOV	ES:[DI],AX	;STORE SYMBOL POINTER

	CMP	AX,SYMCAP	;OUT OF MEMORY?
	JB	@@AS6B
	MOV	AX,20		;IF SO THEN HALT
	CALL	ERROR
	mov	_ERRORLEVEL,DOS_RETURNCODE_SEVERE
	JMP	T1

@@AS6B:
	SHL	AX,1		;STORE SYMBOL TEXT
	ADD	AX,SYMBASE
	MOV	ES,AX
;	ifdef	GENESIS				; kts what the fuck?
	; does this have something to do with calls like "jsr <macroname>?"
	MOV	WORD PTR ES:[mac_sigoffset],'XX'	;(ENSURE DISTINCTNESS FROM MACRO)
;	endif
	XOR	BX,BX
@@AS7:
	MOV	AL,[SYMBOL_FIELD+BX]
	MOV	ES:[BX],AL
	INC	BX
	OR	AL,AL
	JNZ	@@AS7

	MOV	AX,SVL		;STORE SYMBOL VALUE
	MOV	ES:[symbol_val_l],AX
	MOV	AX,SVH
	MOV	ES:[symbol_val_h],AX

	mov	ah,0		; no flags set on add_symbol

	ifdef	SNES
	;----- Also store A/I flags for tracking (SNES)
	mov	al,sdb1
	and	al,80h OR 40h	; extract LONGA/LONGI flags

	and	ah,NOT (80h OR 40h)	; Zero out LONGA/LONGI flags
	or	ah,al		; Add in current LONGA/LONGI
	endif

	mov	es:[symbol_flag],ah ; Save in symbol structure


	CMP	BYTE PTR ES:[0],LOCAL_SIG ;BY THE WAY, WAS THIS ONE LOCAL?
	JNZ	@@AS7B

@@AS7A:
	CMP	STBL,0FFFFH	;IF IT'S LOCAL AND THE FIRST ONE
	JNZ	@@AS7B
	MOV	AX,SYMTOTAL	;THEN STORE "SYMBOL TOTAL B4 LOCAL"
	DEC	AX
	MOV	STBL,AX
@@AS7B:
	RET

@@AS8:
	OR	AL,AL		;SEGMENT SYMBOL?
	JZ	@@AS9
	RET

@@AS9:
	MOV	AX,13		;DUPLICATE DEFINITION!
	jmp	ERROR
ADD_SYMBOL	ENDP

;==============================================================================

PUBLIC	C	FindSymbol
FindSymbol	PROC	NEAR
	push	si
	push	di
	push	ds
	push	es
	push	bp
	mov	bp,sp
	add	bp,12
	lds	si,[bp]
	mov	ax,DATA
	mov	es,ax
	mov	di,offset SYMBOL_FIELD
	mov	cx,16
	rep	movsw
	mov	ds,ax

	lea	bx,Uppercase
	mov	ax,WORD PTR [SYMBOL_FIELD]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah

;	ifdef	SNES					; kts check this
	cmp	ax,'OP'
	JNZ	@@MaybePC

	mov	bx,ax
	mov	ax,WORD PTR [SYMBOL_FIELD+2]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah
	cmp	ax,'GR'
	mov	ax,bx				; get 1st 2 chars back
	jnz	@@MaybePC

	cmp	[SYMBOL_FIELD+4],0
	jnz	@@MaybePC
					; ok, read porg

	MOV	DI,OBJPTRL	;COMPUTE OBJPTR+DELTA
	ADD	DI,DELTA_L
	MOV	dx,OBJPTRH
	ADC	dx,DELTA_H

	mov	porgl,di		; store to porg
	mov	porgh,dx

	mov	ax,offset porgL		; point to porg temp
	mov	dx,ds
	jmp	@@Nope
@@MaybePC:
;	endif

	CMP	ax,'CP'    ;"PC", RESERVED WORD
	JNZ	@@NormalSymbol
	CMP	[SYMBOL_FIELD+2],0
	JNZ	@@NormalSymbol

;	ifdef	GENESIS				; kts made objptrh 16 bits
;	mov	ax,OBJPTRL
;	mov	fakePC,ax
;	mov	al,OBJPTRH
;	mov	fakePCH,al
;
;	mov	ax,offset fakePC
;	endif
;
;	ifdef	SNES
	mov	ax,offset OBJPTRL
;	endif

	mov	dx,ds		; and point to this segment
	jmp	@@Nope

@@NormalSymbol:
	call	FIND_SYMBOL
	mov	bh,0

	jnc	@@foundit
	mov	ax,0
	mov	dx,0
	jmp	@@Nope
@@foundit:
	mov	dx,es
	mov	ax,symbol_val_l		; skip label, get ptr to address
@@Nope:
	pop	bp
	pop	es
	pop	ds
	pop	di
	pop	si
	ret
FindSymbol	ENDP

;==============================================================================

FindSymbolParamField:
	push	si
	push	di
	mov	cx,32
	mov	di,0
@@FSPFLoop:
	mov	al,[param_field+si]
	mov	[symbol_field+di],al
	inc	si
	inc	di
	loop	@@FSPFLoop
	pop	di
	pop	si


	if 0
;
; Input: ?X:?X  Value of symbol to locate
;
FIND_SYMBOL_BY_VAL	PROC	NEAR
FIND_SYMBOL_BY_VAL	ENDP
	endif


;==============================================================================
FIND_SYMBOL	PROC	NEAR
; input: ascii 0 term in symbol_field
; output: carry flag is set upon failure
;	  carry clear on sucess
;	 value in bl:cx
;	es:0 -> symbol entry
;------------------------------------------------------------------------------

					; kts 04-30-94 00:08am
	mov	al,[SYMBOL_FIELD]
	CMP	AL,LLCHR
	JNZ	@@NotLocal
	MOV	[SYMBOL_FIELD],LOCAL_SIG
@@NotLocal:
					; end kts 04-30-94 00:08am

	call	FindSymbolOrMacro
	jc	@@Err
					; kts added 01-17-93 07:25pm
	cmp	WORD PTR es:[mac_sigoffset],MACRO_SIG		; if symbol is a macro, error
	jnz	@@NotMacro
	MOV	AX,72
	CALL	ERROR
@@NotMacro:
	CLC
@@Err:
	RET
FIND_SYMBOL	endp

;==============================================================================
FindSymbolOrMacro	PROC	NEAR
; input: ascii 0 term in symbol_field
; output: carry flag is set upon failure
;	  carry clear on sucess
;	 value in bl:cx
;	es:0 -> symbol entry
;------------------------------------------------------------------------------

	lea	bx,Uppercase

	mov	ax,WORD PTR [SYMBOL_FIELD+2]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah
	mov	cx,ax

	mov	ax,WORD PTR [SYMBOL_FIELD]
	xlatb
	xchg	al,ah
	xlatb
	xchg	al,ah

;	ifdef	SNES
	cmp	ax,'OP'
	JNZ	@@NotPORG
	cmp	cx,'GR'
	jnz	@@NotPORG
	cmp	[SYMBOL_FIELD+4],0
	jnz	@@NotPORG
					; ok, read porg

	MOV	cx,OBJPTRL	;COMPUTE OBJPTR+DELTA
	ADD	cx,DELTA_L
	MOV	bx,OBJPTRH
	ADC	bx,DELTA_H
	clc
	ret
@@NotPORG:
;	endif

	CMP	ax,'CP'    ;"PC", RESERVED WORD
	JNZ	@@NotPC
	CMP	cl,0
	JNZ	@@NotPC
	MOV	CX,[OBJPTRL]
	MOV	BX,[OBJPTRH]
	CLC
	RET
@@NotPC:

	ifdef	GENESIS
	CMP	ax,'CC'	;IF "CCR" THEN SOMETHING AMISS
	JNZ	@@RW1
	CMP	cx,'R'
	JNZ	@@RW1
	MOV	AX,50
	CALL	ERROR
	CLC
	RET
@@RW1:

	CMP	ax,'RS'	;IF "SR" THEN IT'S NOT BLISS
	JNZ	@@RW2
	CMP	cl,0
	JNZ     @@RW2
	MOV	AX,51
	CALL	ERROR
	CLC
	RET
@@RW2:

	CMP	ax,'SU'			;IF "USP" THEN "OH PISS!"
	JNZ	@@NotUSP
	CMP	cx,'P'
	JNZ	@@NotUSP
	MOV	AX,52
	CALL	ERROR
	CLC
	RET
@@NotUSP:
	endif

	XOR	CX,CX		;CX=LOWEST GUESS				AL SET TO TYPE
	MOV	DX,SYMTOTAL	;DX=HIGHEST GUESS
	OR	DX,DX
	JZ	@@FS4
@@FS1:
	MOV	BP,CX		;BP = AVG(CX,DX)
	ADD	BP,DX
	SHR	BP,1
	CALL	CMP_SYMBOL
	JZ	@@SymbolFound
	JC	@@FS2
	MOV	AX,DX		;HIGHER
	SUB	AX,CX
	CMP	AX,1
	JZ	@@FS4
	MOV	CX,BP
	JMP	@@FS1
@@FS2:
	CMP	CX,DX		;LOWER
	JZ	@@FS4
	MOV	AX,DX
	MOV	DX,BP
	JMP	@@FS1
@@SymbolFound:
	MOV	CX,ES:[symbol_val_l]	;SYMBOL FOUND -- EXTRACT VALUE
	MOV	BX,ES:[symbol_val_h]
	CLC
	RET
@@FS4:
	STC			;SYMBOL NOT FOUND--DX POINTS TO WHERE
	RET			;IT WOULD BE INSERTED FOR ADD_SYMBOL

;==============================================================================

CMP_SYMBOL:			;SOURCE STRING IN "SYMBOL"
	lea	bx,caseInsensitiveSymbolTranslation
	test	_bCaseSensitive,1
	jz	@@NotCaseSensitive
	lea	bx,caseSensitiveSymbolTranslation
@@NotCaseSensitive:

	MOV	DI,BP		;BP POINTS TO TABLE ENTRY
	SHL	DI,1		;SET ZF IF NOT EQUAL
	MOV	ES,SYMPTRSEG	;SET CF IF "LESS"
	MOV	AX,ES:[DI]
	SHL	AX,1
	ADD	AX,SYMBASE
	MOV	ES,AX
	XOR	DI,DI
@@CS1:
	MOV	AL,ES:[DI]
	MOV	AH,[SYMBOL_FIELD+DI]
	xchg	ah,al				;@@@
	xlatb
	xchg	ah,al				;@@@
	CMP	AH,AL
	JNZ	@@CS1A
	OR	AH,AL
	JZ	@@CS3
	INC	DI
	JMP	@@CS1

@@CS1A:
	CMP	AL,3
	JA	@@CS2
	OR	AH,AH
	JZ	@@CS3

@@CS2:
	CMP	AH,AL		;SET OR CLEAR CARRY BASED ON COMPARE
	RET

@@CS3:
	XOR	AH,AH		;CLZ
	RET
FindSymbolOrMacro	ENDP

;==============================================================================

FLUSH_LOCAL	PROC	NEAR

	ifdef	SNES
	mov	al,sdb1			; memorize current longas & i settings
	push	ax
	endif

;FL1:
	CMP	LFRPTR,0	;ANY LOCAL FORWARD REFERENCES?
	JNZ	@@FL2
	JMP	@@FL8

@@FL2:
	MOV	AX,LFRPTR	;INIT POINTERS AND FILL LOCAL HOLES
	MOV	LFREND,AX
	MOV	LFRPTR,0
	OR	SDB1,1

	PUSH	OBJPTRL
	PUSH	OBJPTRH
	PUSH	LINE_COUNT

@@FL3:
	MOV	ES,LFRSEG	;INIT/RESTORE LFRSEG
	MOV	DI,LFRPTR

	MOV	AX,ES:[DI]	;EXTRACT LINE_COUNT
	ADD	DI,2
	MOV	LINE_COUNT,AX

	ifdef	SNES
	MOV	AL,ES:[DI]	;EXTRACT LONGA/LONGI
	INC	DI
	AND	SDB1,NOT 0C0H
	OR	SDB1,AL
	endif

	MOV	AX,ES:[DI]	;EXTRACT OBJPTR'S
	ADD	DI,2
	MOV	OBJPTRL,AX
	MOV	AX,ES:[DI]
	ADD	DI,2
	MOV	OBJPTRH,AX

	MOV	AX,ES:[DI]
	ADD	DI,2
	MOV	DELTA_L,AX
	MOV	AX,ES:[DI]
	ADD	DI,2
	MOV	DELTA_H,AX

	XOR	BX,BX
@@FL4:
	MOV	AL,ES:[DI]	;EXTRACT MNE_FIELD
	INC	DI
	MOV	[MNE_FIELD+BX],AL
	MOV	[MNE_LEN],BX
	INC	BX
	OR	AL,AL
	JNZ	@@FL4

	XOR	BX,BX
@@FL5:
	MOV	AL,ES:[DI]	;EXTRACT PARAM_FIELD
	INC	DI
	MOV	[PARAM_FIELD+BX],AL
	INC	BX
	OR	AL,AL
	JNZ	@@FL5

	SUB	BX,2
	MOV	[PARAM_LAST],BX
	MOV	LFRPTR,DI

	CALL	PARSE_MNE	;PARSE AND ASSEMBLE THE FIELDS

	MOV	AX,LFRPTR	;LOOP UNTIL FINISHED
	CMP	AX,LFREND
	JNC	@@FL7
	JMP	@@FL3

@@FL7:
	POP	LINE_COUNT	;RESTORE THINGS TO NORMAL
	POP	OBJPTRH
	POP	OBJPTRL
	AND	SDB1,NOT 1
	CALL	BLIF		;REBLIF, BECAUSE FIELDS ARE NOW BOGUS!

@@FL8:
	MOV	AX,STBL		;WHERE THERE ACTUALLY ANY LOCAL LABELS?
	CMP	AX,0FFFFH
	JNZ	@@FL9
	jmp	short @@FL_EXIT

@@FL9:
	MOV	SYMTOTAL,AX
	MOV	STBL,0FFFFH
	MOV	LFRPTR,0

@@FL_EXIT:
	ifdef	SNES
	pop	ax
	mov	sdb1,al
	endif
	RET
FLUSH_LOCAL	ENDP

;==============================================================================

WRITE_SYM	PROC	NEAR

	CMP	SYMTOTAL,0
	JNZ	@@WS
	RET

@@WS:
	MOV	AH,3CH		;DUMP SYMBOL DATA TO SYM_FILE
	XOR	CX,CX			; kts was cl 12-25-92 05:32am
	MOV	DX,OFFSET _SYM_FILE
	INT	21H
	JNC	@@WS0
	JMP	SEVERE

@@WS0:
	MOV	INCHAND,AX

	XOR	BX,BX		;STORE HEADING
	MOV	ES,BUFSEG2
@@WS0A:
	MOV	AL,[SYM_HEADING+BX]
	MOV	ES:[BX],AL
	INC	BX
	CMP	AL,1AH
	JNZ	@@WS0A

	MOV	DI,SYMTOTAL	;MOVE SYMBOL DATA INTO BUFSEG2
	MOV	BP,SYMBASE
	MOV	BX,80H

@@WS1:
	XOR	SI,SI		;STORE SYMBOL NAME
	MOV	CX,BX
	MOV	ES,BP
	CMP	WORD PTR ES:[mac_sigoffset],MACRO_SIG	; IF MACRO THEN DON'T STORE!
	JZ	@@WS1B1

@@WS1A:
	MOV	ES,BP
	MOV	AL,ES:[SI]
	INC	SI
	MOV	ES,BUFSEG2
	MOV	ES:[BX],AL
	INC	BX
	CMP	AL,SEGTYPE_COUNT		;IF SEGMENT THEN DON'T STORE!
	JAE	@@WS1A
	OR	AL,AL
	JZ	@@WS1B
	MOV	BX,CX
	JMP	@@WS1B1

@@WS1B:
	MOV	ES,BP		;STORE SYMBOL VALUE
	MOV	AX,ES:[symbol_val_l]
	MOV	DX,ES:[symbol_val_h]

	MOV	ES,BUFSEG2
	MOV	ES:[BX],AX
	ADD	BX,2
	MOV	ES:[BX],DX
	ADD	BX,2

@@WS1B1:
	ADD	BP,2		;LOOP UNTIL DONE
	DEC	DI
	JZ	@@WS1C
	CMP	BX,3F00H
	JC	@@WS1
	CALL	WBUF2
	JMP	@@WS1
@@WS1C:
	CALL	WBUF2

	MOV	AH,3EH		;CLOSE
	MOV	BX,INCHAND
	INT	21H
	RET
WRITE_SYM	ENDP

;==============================================================================

DISPLAY_SYMSTATS	PROC	NEAR			;DISPLAY SYMBOL STATISTICS

	MOV	AH,9
	MOV	DX,OFFSET STXT
	INT	21H
	MOV	AX,SYMTOTAL
	CALL	DISP_DEC
	MOV	AH,2
	MOV	DL,'/'
	INT	21H
	MOV	AX,SYMCAP
	CALL	DISP_DEC
	MOV	AH,9
	MOV	DX,OFFSET CRLF
	INT	21H
	RET

DISPLAY_SYMSTATS	ENDP

;==============================================================================
