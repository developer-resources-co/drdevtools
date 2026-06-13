;==============================================================================
; error.asm: error handling for spasm
;==============================================================================

	PUBLIC	_ERRORLEVEL

DATA	SEGMENT	'DATA'
ETXT		DB	'      Total Errors:  ',0
arrowText	db	'---->',0
tabText	db	'	',0
blankText	db	0
textBuffer	DB	160h DUP (?)			; temp buffer

_ERRORLEVEL	db	0		; spaz error level(what gets returned to dos)
errorLevel	db	0		; temp error level inside of error handler
errorSeperator	db	': ',0

;------------------------------------------------------------------------------

ERRORLEVEL_NONE		EQU	0
ERRORLEVEL_WARNING	EQU	1
ERRORLEVEL_ERROR	EQU	2
ERRORLEVEL_SEVERE	EQU	3

;-----------------------------------------------------------------------------

DOS_RETURNCODE_WARNING	equ	0
DOS_RETURNCODE_ERROR	equ	10
DOS_RETURNCODE_SEVERE	equ	20

;------------------------------------------------------------------------------

NULLTEXT	DB	0
WARNINGTEXT	DB	'WARNING ',0
ERRORTEXT	DB	'ERROR ',0
SEVERETEXT	DB	'SEVERE ',0

;------------------------------------------------------------------------------

ERRORLEVEL_TABLE	label	word
	DW	OFFSET	NULLTEXT
	DW	OFFSET	WARNINGTEXT
	DW	OFFSET	ERRORTEXT
	DW	OFFSET	SEVERETEXT

;-----------------------------------------------------------------------------

RETURNCODE_TABLE	label	byte
	DB	0
	DB	DOS_RETURNCODE_WARNING
	DB	DOS_RETURNCODE_ERROR
	DB	DOS_RETURNCODE_SEVERE

;==============================================================================

ERROR0	DB	ERRORLEVEL_SEVERE,'INTERNAL -- Unused',0
ERROR1	DB	ERRORLEVEL_ERROR,'Non-local symbol must begin with alphabetical character',0
ERROR2	DB	ERRORLEVEL_WARNING,'Parameter not allowed; addressing mode is implied by mnemonic',0
ERROR3	DB	ERRORLEVEL_ERROR,'mnemonic or macro name must not exceed 27 characters',0
ERROR4	DB	ERRORLEVEL_ERROR,'Parameter field exceeds 255 characters',0
ERROR5	DB	ERRORLEVEL_ERROR,'Syntax Error',0
ERROR6	DB	ERRORLEVEL_ERROR,'Illegal operand size',0
ERROR7	DB	ERRORLEVEL_ERROR,'Relative address must be within 64k segment',0
ERROR8	DB	ERRORLEVEL_ERROR,'Relative address out of range by $???? bytes',0
ERROR9	DB	ERRORLEVEL_ERROR,'Illegal or invalid addressing mode',0
ERROR10	DB	ERRORLEVEL_ERROR,'Invalid instruction or undefined macro',0
ERROR11	DB	ERRORLEVEL_ERROR,'Must specify a parameter: "ON" or "OFF"',0
ERROR12	DB	ERRORLEVEL_ERROR,'File not found',0
ERROR13	DB	ERRORLEVEL_ERROR,'Duplicate definition:                              ',0
ERROR14	DB	ERRORLEVEL_ERROR,'Symbol name exceeds 26 characters',0
ERROR15	DB	ERRORLEVEL_ERROR,'Illegal character in symbol',0
ERROR16	DB	ERRORLEVEL_ERROR,'Reference to undefined symbol',0
ERROR17	DB	ERRORLEVEL_ERROR,'Nested INCLUDE directives cannot exceed 4 levels',0
ERROR18	DB	ERRORLEVEL_ERROR,'INTERNAL -- Obselete (DT)',0
ERROR19	DB	ERRORLEVEL_SEVERE,'Too many forward references,',0
ERROR20	DB	ERRORLEVEL_SEVERE,'Out of symbol space',0
ERROR21	DB	ERRORLEVEL_ERROR,'Nested IF and IFDEF statements cannot exceed 15 levels',0
ERROR22	DB	ERRORLEVEL_WARNING,'ENDIF encountered without IF or IFDEF',0
ERROR23	DB	ERRORLEVEL_SEVERE,'Internal error, should not occur',0
ERROR24	DB	ERRORLEVEL_ERROR,'Macro can have no more than 16 parameters',0
ERROR25	DB	ERRORLEVEL_ERROR,'Line length exceeded 250 characters',0
ERROR26	DB	ERRORLEVEL_ERROR,'No operand(s) found when expected',0
ERROR27	DB	ERRORLEVEL_ERROR,'Nested macros cannot exceed 4 levels',0
ERROR28	DB	ERRORLEVEL_SEVERE,'Out of macro space',0
ERROR29	DB	ERRORLEVEL_ERROR,'INTERNAL -- Too many forward references with local labels',0
ERROR30	DB	ERRORLEVEL_ERROR,'Invalid segment type, valid types are "RAM" or "ROM"',0
ERROR31	DB	ERRORLEVEL_ERROR,'Symbol was not defined as a segment',0
ERROR32	DB	ERRORLEVEL_ERROR,'Symbol "PC" is reserved by the assembler',0
ERROR33	DB	ERRORLEVEL_ERROR,'Object code address conflicts with "SFX MAP"',0
ERROR34	DB	ERRORLEVEL_ERROR,'Segment name must not exceed 15 characters',0
ERROR35	DB	ERRORLEVEL_ERROR,'Object code address transgresses defined segment size',0
ERROR36	DB	ERRORLEVEL_ERROR,'Code or data cannot be assembled into a "RAM" segment',0
ERROR37	DB	ERRORLEVEL_ERROR,'Syntax error within expression',0
ERROR38	DB	ERRORLEVEL_ERROR,'INTERNAL -- expression parser failed',0
ERROR39	DB	ERRORLEVEL_ERROR,'Divide by zero within expression',0
ERROR40	DB	ERRORLEVEL_ERROR,'Misbalanced parentheses within expression',0
ERROR41	DB	ERRORLEVEL_ERROR,'Undefined symbol used within expression',0
ERROR42	DB	ERRORLEVEL_ERROR,'Invalid symbol name used within expression',0
ERROR43	DB	ERRORLEVEL_ERROR,'Missing parameter in expression',0
ERROR44	DB	ERRORLEVEL_ERROR,'Filespec cannot exceed 95 characters in length',0
ERROR45	DB	ERRORLEVEL_WARNING,'More than 65535 lines in file',0
ERROR46	DB	ERRORLEVEL_ERROR,'Must specify a parameter: "ON" / "OFF" / "PUSH" / "POP" ',0
ERROR47	DB	ERRORLEVEL_ERROR,'Illegal or invalid register',0
ERROR48	DB	ERRORLEVEL_ERROR,'Memory shifts are word-sized only',0
ERROR49	DB	ERRORLEVEL_ERROR,'Illegal size override',0
ERROR50	DB	ERRORLEVEL_ERROR,'CCR can not be used here',0
ERROR51	DB	ERRORLEVEL_ERROR,'SR can not be used here',0
ERROR52	DB	ERRORLEVEL_ERROR,'USP can not be used here',0
ERROR53	DB	ERRORLEVEL_ERROR,'Forward reference can not be used here',0
;ERROR54	DB	ERRORLEVEL_WARNING,'Unbalanced IF/ENDIF at end of file',0
ERROR54	DB	ERRORLEVEL_ERROR,'If started at line '
ERROR54a db	'      has no matching endif',0
ERROR55	DB	ERRORLEVEL_ERROR,'Code assembled at an odd address',0
ERROR56	DB	ERRORLEVEL_WARNING,'Too many operands',0
ERROR57	DB	ERRORLEVEL_ERROR,'No destination operand found when expected',0
ERROR58	DB	ERRORLEVEL_ERROR,'LONGA stack full',0
ERROR59	DB	ERRORLEVEL_ERROR,'LONGI stack full',0
ERROR60	DB	ERRORLEVEL_ERROR,'LONGA stack empty',0
ERROR61	DB	ERRORLEVEL_ERROR,'LONGI stack empty',0
ERROR62	DB	ERRORLEVEL_ERROR,'Invalid direct page; must be $0 or $100',0		; SPC700 only
ERROR63	DB	ERRORLEVEL_ERROR,'Nested REPT directives cannot exceed 15 levels',0
ERROR64	DB	ERRORLEVEL_WARNING,'ENDR encountered without REPT',0
ERROR65	DB	ERRORLEVEL_WARNING,'Spooling backpatch entries to disk',0
ERROR66	DB	ERRORLEVEL_SEVERE,'File I/O Error',0
ERROR67	DB	ERRORLEVEL_ERROR,'User Error',0
ERROR68	DB	ERRORLEVEL_WARNING,'User Warning',0
ERROR69	DB	ERRORLEVEL_SEVERE,'User Severe',0
ERROR70	DB	ERRORLEVEL_ERROR,'Error # out of range',0
ERROR71	DB	ERRORLEVEL_ERROR,'Error level must be 0Ä3',0
ERROR72	DB	ERRORLEVEL_ERROR,'Macro used as symbol',0
ERROR73	DB	ERRORLEVEL_ERROR,'Cannot have an alignment of zero',0
ERROR74	db	ERRORLEVEL_WARNING,'Structure crosses page boundary',0
ERROR75	db	ERRORLEVEL_WARNING,'ENDM encountered outside of macro',0
ERROR76	db	ERRORLEVEL_ERROR,'Rept/Macro nesting problem',0
ERROR77	DB	ERRORLEVEL_WARNING,'ELSE encountered without IF or IFDEF',0
ERROR78	DB	ERRORLEVEL_ERROR,'Rept count of zero invalid',0
ERROR79	DB	ERRORLEVEL_ERROR,'Macro parameter too long',0

;-----------------------------------------------------------------------------

ERROREOFINMACRO	DB	'EOF encountered within macro definition$'

;------------------------------------------------------------------------------

ERROR_TABLE LABEL WORD

	if 0
nError = 0
	REPT 74
		DW	%nError
nError = nError + 1
	ENDM

	else

	DW	OFFSET	ERROR0
	DW	OFFSET	ERROR1
	DW	OFFSET	ERROR2
	DW	OFFSET	ERROR3
	DW	OFFSET	ERROR4
	DW	OFFSET	ERROR5
	DW	OFFSET	ERROR6
	DW	OFFSET	ERROR7
	DW	OFFSET	ERROR8
	DW	OFFSET	ERROR9
	DW	OFFSET	ERROR10
	DW	OFFSET	ERROR11
	DW	OFFSET	ERROR12
	DW	OFFSET	ERROR13
	DW	OFFSET	ERROR14
	DW	OFFSET	ERROR15
	DW	OFFSET	ERROR16
	DW	OFFSET	ERROR17
	DW	OFFSET	ERROR18
	DW	OFFSET	ERROR19
	DW	OFFSET	ERROR20
	DW	OFFSET	ERROR21
	DW	OFFSET	ERROR22
	DW	OFFSET	ERROR23
	DW	OFFSET	ERROR24
	DW	OFFSET	ERROR25
	DW	OFFSET	ERROR26
	DW	OFFSET	ERROR27
	DW	OFFSET	ERROR28
	DW	OFFSET	ERROR29
	DW	OFFSET	ERROR30
	DW	OFFSET	ERROR31
	DW	OFFSET	ERROR32
	DW	OFFSET	ERROR33
	DW	OFFSET	ERROR34
	DW	OFFSET	ERROR35
	DW	OFFSET	ERROR36
	DW	OFFSET	ERROR37
	DW	OFFSET	ERROR38
	DW	OFFSET	ERROR39
	DW	OFFSET	ERROR40
	DW	OFFSET	ERROR41
	DW	OFFSET	ERROR42
	DW	OFFSET	ERROR43
	DW	OFFSET	ERROR44
	DW	OFFSET	ERROR45
	DW	OFFSET	ERROR46
	DW	OFFSET	ERROR47
	DW	OFFSET	ERROR48
	DW	OFFSET	ERROR49
	DW	OFFSET	ERROR50
	DW	OFFSET	ERROR51
	DW	OFFSET	ERROR52
	DW	OFFSET	ERROR53
	DW	OFFSET	ERROR54
	DW	OFFSET	ERROR55
	DW	OFFSET	ERROR56
	DW	OFFSET	ERROR57
	DW	OFFSET	ERROR58
	DW	OFFSET	ERROR59
	DW	OFFSET	ERROR60
	DW	OFFSET	ERROR61
	DW	OFFSET	ERROR62
	DW	OFFSET	ERROR63
	DW	OFFSET	ERROR64
	DW	OFFSET	ERROR65
	DW	OFFSET	ERROR66
	DW	OFFSET	ERROR67
	DW	OFFSET	ERROR68
	DW	OFFSET	ERROR69
	DW	OFFSET	ERROR70
	DW	OFFSET	ERROR71
	DW	OFFSET	ERROR72
	DW	OFFSET	ERROR73
	dw	offset	ERROR74
	dw	offset	ERROR75
	dw	offset	ERROR76
	dw	offset	ERROR77
	dw	offset	ERROR78
	dw	offset	ERROR79
	endif
	NUM_ERRORS	EQU	( $ - ERROR_TABLE ) / 2
ERROR_TOTAL	DW	0

errorOnOff	DB	NUM_ERRORS DUP (1)	; All errors on by default

DATA	ENDS

;==============================================================================
.CODE

PrintFilenameHierarchy	PROC
	cmp	SRC_LEVEL,0ffffh			; if at top, no info
	jz	@@Ret
				; display filename heiarchy
	xor	bx,bx
	jmp	short @@Entry
@@HLoop:
	mov	al,'|'
	stosb
@@Entry:
	push	bx
	ERRIF	FILESPEC_LEN NE 80H "FILESPEC_LEN has changed, code must be updated"
	MOV	CL,7
	SHL	BX,CL
	ADD	BX,OFFSET FILESPEC
	MOV	si,BX

	mov	ah,'$'
	call	PrintToBufferTerm
	pop	bx
	inc	bx
	cmp	bx,SRC_LEVEL
	jbe	@@HLoop

				; display line # in parens
	mov	al,'('
	stosb
	mov	bx,SRC_LEVEL
	shl	bx,1
	test	SDB1,1
	JZ	@@NoFill
	XOR	BX,BX
@@NoFill:
	MOV	AX,[LINE_COUNT+BX]
	CALL	PrintDec

	TEST	SDB1,SDB1F_MACRO		;PRINT MACRO LINE IF MACRO ENGAGED
	JZ	@@NotInMacro

	MOV	CX,MACLEVEL
	INC	CX
	XOR	BP,BP
@@Loop:
	MOV	al,'.'
	stosb
	MOV	AX,[MACLINE+BP]
	CALL	PrintDec
	ADD	BP,2
	LOOP	@@Loop

@@NotInMacro:
	mov	al,')'
	stosb
@@Ret:
	ret
PrintFilenameHierarchy	ENDP


;------------------------------------------------------------------------------
; Inputs:
;	AX = ERROR #
;	DX = AUX INFO (dependent upon error number)
; Ouputs:
;	None
; Destroys:
;	None
;------------------------------------------------------------------------------


ERROR	PROC	NEAR		;NOTE THAT AN ERROR OCCURRED AND ADD IT TO LIST

	TEST	SDB2,1		;ALREADY AN ERROR?
	JZ	@@Doit
	RET
@@Doit:
	PUSH_ALL
	ifdef	_68000
	CMP	AX,6		;ILLEGAL OPERAND SIZE?
	JNZ	@@NotIll
	TEST	SDB1,4		;IF FWD REF THEN DON'T COMPLAIN
	JNZ	@@Ret
;	TEST	SDB2,4
;	JNZ	@@Ret
@@NotIll:
	endif

	push	ax				; popped into bx
	push	dx				; save aux info

	mov	bx,ax
			; prepare to print to buffer
	mov	ax,DATA
	mov	es,ax
	mov	di,offset textBuffer

			; first find out error level
	add	bx,bx
	mov	si,[ERROR_TABLE+bx]
	lodsb					; read error level

	mov	errorLevel,al

	cmp	al,ERRORLEVEL_ERROR
	jnz	@@NotError
	INC	ERROR_TOTAL			;ADD TO ERROR TOTAL
	OR	SDB2,1				; set errors flag
@@NotError:
					; print error level
	xor	bh,bh
	mov	bl,al
	add	bx,bx
	mov	si,[ERRORLEVEL_TABLE+bx]
	call	PrintToBuffer
	call	PrintFilenameHierarchy

	pop	dx		; "DATA"
	POP	BX		;LOOK UP AND PRINT ERROR TEXT MESSAGE
	SHL	BX,1

	cmp	bx,8*2		;RELATIVE ADDRESS OUT OF RANGE ERROR?
	jne	@@NotRel

	push	di
	mov	di,OFFSET ERROR8+35
	mov	ax,dx
	call	PrintHex16
	pop	di
@@NotRel:
	cmp	bx,54*2		;if without matching endif
	jne	@@NotIf

	push	di
	mov	di,OFFSET ERROR54a
	mov	ax,dx
	call	PrintDec
	pop	di

@@NotIf:
	CMP	BX,13*2		;DUPLICATE DEFINITION ERROR?
	JNZ	@@NotDup
	XOR	SI,SI
@@DupSymLoop:
	MOV	AL,[SYMBOL_FIELD+SI]
	CMP	AL,7FH
	JNZ	@@NotLocal
	MOV	AL,LLCHR
@@NotLocal:
	MOV	[ERROR13+24+SI],AL
	INC	SI
	CMP	AL,3
	JA	@@DupSymLoop
	MOV	[ERROR13+24+SI],0
@@NotDup:

	lea	si,errorSeperator
	call	PrintToBuffer

	mov	si,[ERROR_TABLE+bx]

	lodsb
	or	al,al				; first check if this error counts
	jz	@@Ret
	call	PrintToBuffer

	xor	al,al
	stosb

	lea	si,textBuffer
	MOV	BX,STDOUT
	call	PrintLine
	lea	si,arrowText
	call	PrintPartialLine
				; write source line to display also
	test	SDB1,SDB1F_FILLHOLES OR SDB1F_LOCALFILLHOLES
	jnz	@@Backpatching
	mov	si,offset LINE
	call	PrintLine
	jmp	short @@NotBackpatching
@@Backpatching:
					; just print what we have
	mov	si,offset MNE_FIELD
	mov	cx,MNE_LEN
	call	PrintData
	mov	si,offset tabText
	call	PrintPartialLine
	mov	si,offset PARAM_FIELD
	mov	cx,PARAM_LAST
	inc	cx			; since last points to last valid char
	call	PrintData
	mov	si,offset BlankText
	call	PrintLine
@@NotBackpatching:
	lea	si,textBuffer
	call	ListPrintLine

	mov	bl,errorLevel
	mov	bh,0
	mov	al,[RETURNCODE_TABLE+bx]

	call	SetErrorLevel

	cmp	errorLevel,ERRORLEVEL_SEVERE
	je	SEVERE_ENTRY					; severe does not come back

@@Ret:
	POP_ALL
	RET
ERROR	ENDP

;=============================================================================
; Inputs:
;	al = new error level, will only set if new level is larger than old

SetErrorLevel	proc	NEAR
	cmp	_ERRORLEVEL,al
	ja	@@ErrorCodeLarger
	mov	_ERRORLEVEL,al
@@ErrorCodeLarger:
	ret
	ENDP
;==============================================================================

SEVERE	PROC	NEAR
	if	1
	mov	ax,66
	call	Error
	else
	mov	ax,DATA
	mov	ds,ax
	MOV	AH,9			;PRINT FILE I/O ERROR MESSAGE
	MOV	DX,OFFSET IOERR
	INT	21H
	endif
SEVERE_ENTRY:
	mov	_ERRORLEVEL,DOS_RETURNCODE_SEVERE
	JMP	T1
SEVERE	ENDP

;==============================================================================
; print total errors
;------------------------------------------------------------------------------

DISPLAY_ERRORS	PROC	NEAR
	MOV	ax,DATA		;POINT DS @ DATA SEGMENT
	MOV	es,ax
	mov	di,offset textBuffer

	CMP	ERROR_TOTAL,0
	JZ	@@NoError
	MOV	AH,9				; print cr/lf
	MOV	DX,OFFSET CRLF
	INT	21H
@@NoError:
	mov	si,offset ETXT
	call	PrintToBuffer

	mov	ax,ERROR_TOTAL
	call	PrintDec

	xor	al,al			; zero terminate string
	stosb
					; send to screen and to file
	lea	si,textBuffer
	call	ListPrintLine		; if listing on, print line there
	lea	si,textBuffer
	MOV	BX,STDOUT
	call	PrintLine
	RET
DISPLAY_ERRORS	ENDP

;==============================================================================

TERMINATE	PROC	NEAR
	CALL	DISPLAY_ERRORS		;DISPLAY ERROR TOTAL
	CALL	DISPLAY_SYMSTATS	;DISPLAY SYMBOL STATISTICS

	CMP	_MAPFLAG,0
	JZ	@@NoMap
	CALL	WRITE_MAP

@@NoMap:
	MOV	BX,LISTHAND		;IF A LIST FILE WAS OPEN
	CMP	BX,0
	JZ	@@NoList
	MOV	AH,3EH			;THEN CLOSE IT
	INT	21H

@@NoList:
	CMP	_SLDFLAG,0	;IF SLD FILE WAS OPEN
	JZ	T1
	CALL	SLDWriteBuffer		;THEN FLUSH BUFFER
	MOV	AH,3EH		;AND CLOSE
	MOV	BX,SLD_HAND
	INT	21H
T1:
	ifdef	_SPOOL_BACKPATCH
	MOV	AH,41H			;DELETE "FWDREF.SPZ"
	MOV	DX,OFFSET _SPOOL_FILE
	INT	21H
	endif
			; kts added 2/5/92
	cmp	_SYSTEMTYPE,2
	jnz	@@SpNope
	MOV	AH,3EH		; CLOSE FILE
	MOV	BX,SYSTEMHAND
	INT	21H
@@SpNope:
	cmp	_SYSTEMTYPE,0
	jnz	@@No
	mov	al,CMD_RESTOREVECT
	push	ax
	call	SendCmd
	pop	ax
@@No:
	jmp	ReturnToC
TERMINATE	ENDP

;==============================================================================
