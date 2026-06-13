;==============================================================================
; macro.asm: macro handling code
;==============================================================================

;==============================================================================
.code
;==============================================================================

ExecuteMacro:				;MAYBE IT'S A MACRO?
	cld
	ifdef	GENESIS
	MOV	BP,MACLEVEL
	INC	BP
	MOV	[MACEXT+BP],0
	endif
	XOR	BX,BX

	mov	si,offset MNE_FIELD
	test	_bCaseSensitive,1
	jz	@@NotCaseSensitive
	mov	si,offset LINE
 	add	si,mneInLineOffset		; get non-upper cased version of mnemonic
@@NotCaseSensitive:
	mov	al,[SI]
	MOV	[SYMBOL_FIELD],al
	or	al,al
	jz	@@pp98half
	CMP	AL,LLCHR
	JNZ	@@NotLocal
	MOV	[SYMBOL_FIELD],LOCAL_SIG
@@NotLocal:

@@Loop:
	lodsb
	MOV	[SYMBOL_FIELD+BX],AL
	INC	BX
	cmp	al,' '
	jz	@@Done
	OR	AL,AL

	ifndef	GENESIS
	JNZ	@@Loop
	else
	JZ	@@NoSize
	CMP	AL,'.'					; check if has size extension
	JNZ	@@Loop
	MOV	[SYMBOL_FIELD+BX-1],0
	MOV	AL,[MNE_FIELD+BX]
	MOV	MACEXT+BP,AL
@@NoSize:
	endif
@@Done:
@@pp98half:
	PUSH	DI
	CALL	FindSymbolOrMacro
	POP	DI
	jc	@@NotMacro				; kts 01-17-93 06:42pm if no symbol found, fail
	cmp	es:[mac_sigoffset],MACRO_SIG
	jnz	@@NotMacro
	JNC	@@DoMacro
@@NotMacro:
	MOV	AX,10			;DIDN'T RECOGNIZE AS MACRO
	JMP	ERROR


@@DoMacro:				; ok, we have a valid macro
	MOV	MACPA,0

	CMP	MACLEVEL,MACRO_NEST_MAX		;HOW DEEP ARE WE?
	JL	@@NotTooDeep
	MOV	AX,27			;macros nested toio deep error
	JMP	ERROR

@@NotTooDeep:
	MOV	BX,MACLEVEL
	INC	BX
	SHL	BX,1			; * 2 since MACREPTLEVEL is in words

	; kts 02-21-94 10:09pm
	mov	ax,REPT_LEVEL        	; remember rept level upon entry
	mov	[MACREPTLEVEL+BX],ax
	; end kts

	MOV	[MACLINE+BX],0	;INIT MACRO STUFF
	MOV	AX,ES:[mac_ptr]		; read ptr to begining of macro text
	MOV	[MACPTR+BX],AX
	MOV	[MACPC+BX],0

	XOR	BX,BX		;LOAD MACRO PARAMETERS
	MOV	SI,BX
	CMP	[PARAM_FIELD],0
	JZ	@@ParametersDone

@@ParameterLoop:
	MOV	DI,MACLEVEL
	INC	DI

	MOV	CL,MACRO_PARAM_SHIFTVAL		; di = di*MACRO_PARAM_LEN*MACRO_MAC_PARAMS
	SHL	DI,CL

	mov	ah,MACRO_PARAM_LEN	; keep track of how many chars are copied
					; so we can generate errors if params too long
@@CopyParmCharsLoop:
	MOV	AL,[PARAM_FIELD+SI]
	INC	SI
	CMP	AL,','
	JZ	@@NextParm
	MOV	[MACPARAM+BX+DI],AL
	OR	AL,AL
	JZ	@@BlankRest
	INC	DI
	dec	ah
	jz	@@ParamTooLong
	JMP	@@CopyParmCharsLoop

@@ParamTooLong:
	MOV	AX,79			;macros nested too deep error
	JMP	ERROR

@@NextParm:
	MOV	[MACPARAM+BX+DI],0
	INC	MACPA
	ADD	BX,MACRO_PARAM_LEN
	CMP	BX,MACRO_MAX_PARAMS*MACRO_PARAM_LEN
	JC	@@ParameterLoop
	MOV	AX,24		;TOO MANY PARAMETERS
	JMP	ERROR

@@BlankRest:
	INC	MACPA
	MOV	DI,MACLEVEL
	INC	DI

	MOV	CL,MACRO_PARAM_SHIFTVAL		; di = di*MACRO_PARAM_LEN*MACRO_MAC_PARAMS
	SHL	DI,CL


@@BlankLoop:
	ADD	BX,MACRO_PARAM_LEN	;BLANK OUT REST OF PARAMETERS
	CMP	BX,MACRO_MAX_PARAMS*MACRO_PARAM_LEN
	JZ	@@ParametersDone
	MOV	[MACPARAM+BX+DI],0
	JMP	@@BlankLoop

@@ParametersDone:
	INC	MACLEVEL
	MOV	BX,MACLEVEL
	SHL	BX,1
	MOV	AX,MACPA
	MOV	[MACPC+BX],AX
	OR	SDB1,SDB1F_MACRO			;ENGAGE MACRO
	RET

;==============================================================================
;
;IsMacro:
;	ret
;
;==============================================================================


