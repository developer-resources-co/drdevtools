;==============================================================================
; print.asm: text printing routines for spasm
;==============================================================================

DATA	SEGMENT	'DATA'

ACD		DB	'0123456789ABCDEF'
CRLF		DB	13,10,0,'$'

DATA	ENDS

;==============================================================================
.CODE
;------------------------------------------------------------------------------

DISP1	PROC	NEAR		;DISPLAY ONE NUMERICAL CHR IN DL UNLESS
				;DL="0" AND (SPAZFLAGS AND 20H)=0
	CMP	DL,30H
	JNZ	@@Nope
	TEST	SDB1,20H		; what the fuck?
	JZ	@@Ret
@@Nope:
	OR	SDB1,20H
	PUSH	AX
	MOV	AH,2
	INT	21H
	POP	AX
@@Ret:
	RET
DISP1	ENDP

;==============================================================================
; inputs:
;	ax = # to print in decimal
;
;------------------------------------------------------------------------------

DISP_DEC	PROC	NEAR	;DISPLAY DECIMAL NUMBER

	PUSH	AX		;CONVERT 16-BIT VALUE IN AX TO BCD IN BX:CX
	PUSH	BX		;(MAGIC XECOM THING)
	PUSH	CX
	PUSH	DX
	PUSH	si
	MOV	si,AX
	XOR	AX,AX
	MOV	BX,AX
	MOV	DX,AX
	MOV	CX,10H
@@Loop:
	SHL	si,1
	XCHG	DX,AX
	ADC	AL,AL
	DAA
	XCHG	AH,AL
	ADC	AL,AL
	DAA
	XCHG	AH,AL
	XCHG	DX,AX
	XCHG	BX,AX
	ADC	AL,AL
	DAA
	XCHG	AH,AL
	ADC	AL,AL
	DAA
	XCHG	AH,AL
	XCHG	BX,AX
	ADC	AL,0
	LOOP	@@Loop
	MOV	CX,DX

	AND	SDB1,NOT 20H

	MOV	AL,BL		;DISPLAY 10,000TH PLACE
	CALL	HEXOUT
	MOV	DL,AL
	CALL	DISP1

	MOV	AL,CH		;DISPLAY 1,000TH AND 100TH PLACE
	CALL	HEXOUT
	MOV	DL,AH
	CALL	DISP1
	MOV	DL,AL
	CALL	DISP1

	MOV	AL,CL		;DISPLAY 10TH AND 1TH PLACE
	CALL	HEXOUT
	MOV	DL,AH
	CALL	DISP1
	MOV	DL,AL
	OR	SDB1,20H
	CALL	DISP1

	POP	si
	POP	DX
	POP	CX
	POP	BX
	POP	AX
	RET

DISP_DEC	ENDP

;==============================================================================

PrintDigit	PROC	NEAR		;DISPLAY ONE NUMERICAL CHR IN DL UNLESS
				;DL="0" AND (SPAZFLAGS AND 20H)=0
	CMP	al,30H
	JNZ	@@Nope
	TEST	SDB1,20H		; have we printed anything but zero yet?
	JZ	@@Ret			; if not, return
@@Nope:
	OR	SDB1,20H		; flag we have printed and actual digit
	stosb				; write it out
@@Ret:
	RET
PrintDigit	ENDP


;==============================================================================
; Inputs:
;	ax -> 16-bit value
;	es:di -> buffer to print it into
;------------------------------------------------------------------------------
PrintHex16	PROC	NEAR

	push	bx
	mov	bl,al
	xchg	al,ah
	call	HEXOUT
	xchg	al,ah
	call	PrintDigit
	xchg	al,ah
	call	PrintDigit

	mov	al,bl
	pop	bx
	call	HEXOUT
	xchg	al,ah
	call	PrintDigit
	xchg	al,ah
	jmp	PrintDigit

PrintHex16	ENDP

;==============================================================================
; inputs:
;	ax = # to print in decimal
;       es:di -> buffer to print into
;------------------------------------------------------------------------------


PrintDec	PROC	NEAR	;DISPLAY DECIMAL NUMBER

	PUSH	AX		;CONVERT 16-BIT VALUE IN AX TO BCD IN BX:CX
	PUSH	BX		;(MAGIC XECOM THING)
	PUSH	CX
	PUSH	DX
	PUSH	si
	MOV	si,AX
	XOR	AX,AX
	MOV	BX,AX
	MOV	DX,AX
	MOV	CX,10H
@@Loop:
	SHL	si,1
	XCHG	DX,AX
	ADC	AL,AL
	DAA
	XCHG	AH,AL
	ADC	AL,AL
	DAA
	XCHG	AH,AL
	XCHG	DX,AX
	XCHG	BX,AX
	ADC	AL,AL
	DAA
	XCHG	AH,AL
	ADC	AL,AL
	DAA
	XCHG	AH,AL
	XCHG	BX,AX
	ADC	AL,0
	LOOP	@@Loop
	MOV	CX,DX

	AND	SDB1,NOT 20H		; clear zero truncate flag

	MOV	AL,BL		;DISPLAY 10,000TH PLACE
	CALL	HEXOUT
	CALL	PrintDigit

	MOV	AL,CH		;DISPLAY 1,000TH AND 100TH PLACE
	CALL	HEXOUT
	MOV	dl,al
	mov	al,ah
	CALL	PrintDigit
	MOV	al,dl
	CALL	PrintDigit

	MOV	AL,CL		;DISPLAY 10TH AND 1TH PLACE
	CALL	HEXOUT
	MOV	dl,al
	mov	al,ah
	CALL	PrintDigit
	MOV	al,dl
	OR	SDB1,20H
	CALL	PrintDigit

	POP	si
	POP	DX
	POP	CX
	POP	BX
	POP	AX
	RET

PrintDec	ENDP

;==============================================================================
; Inputs:
;	ds:si -> zero terminated source string
;	es:di -> buffer to print it into
;------------------------------------------------------------------------------

PrintToBuffer	proc
	mov	ah,0

;==============================================================================
; Inputs:
;	ds:si -> source string
;	es:di -> buffer to print it into
;	ah = termination character
;------------------------------------------------------------------------------

PrintToBufferTerm:
	cld
@@Loop:
	lodsb
	stosb
	cmp	al,ah
	jnz	@@Loop
	dec	di			; point back to zero
	ret
PrintToBuffer	endp

;==============================================================================
;	Inputs:
;		al = # to print
;	Outputs:
;		ax = 2 char ascii
;	Destroys:
;		ax
;------------------------------------------------------------------------------

HEXOUT	PROC	NEAR		;BYTE IN AL, ASCII IN AX.
	PUSH	BX
	MOV	AH,AL
	SHR	AH,4
	AND	AL,0FH
	XOR	BH,BH
	MOV	BL,AL
	MOV	AL,[ACD+BX]
	MOV	BL,AH
	MOV	AH,[ACD+BX]
	POP	BX
	RET
HEXOUT	ENDP

;==============================================================================
; Inputs:
;	ds:si-> zero terminated string
; Outputs:
;	ax = length of string in bytes
; Destroys:
;	ax,cx,si
;------------------------------------------------------------------------------

StrLen	proc	near
	mov	al,0  				; search for zero

;------------------------------------------------------------------------------
; Inputs:
;	al = terminator character
;	es:di-> al terminated string
;------------------------------------------------------------------------------

StrLenChar:					; al = char to search for
	cld
	mov	cx,0ffffh
	repne	scasb

	mov	ax,0ffffh
	sub	ax,cx
	ret
StrLen	endp

;==============================================================================
; inputs:
;	ds:si-> buffer to print(0 terminated)
;	bx = output device handle
; Outputs:
;	carry set = write error
;------------------------------------------------------------------------------

PrintPartialLine	proc	near
	push	es
	mov	di,si
	push	ds
	pop	es
	call	StrLen
	pop	es
	mov	cx,ax

;------------------------------------------------------------------------------
; inputs:
;	ds:si-> buffer to print
;	bx = output device handle
;	cx = # of bytes to print
;------------------------------------------------------------------------------

PrintData:
	mov	dx,si
	MOV	AH,40H
	INT	21H
	ret
PrintPartialLine	endp

;==============================================================================
; inputs:
;	ds:si-> buffer to print (0 terminated)
;	bx = output device handle
; Outputs:
;	carry set = write error
; Prints line, and prints cr/lf
;------------------------------------------------------------------------------

PrintLine	proc
	call	PrintPartialLine
	jc	@@Oops
	mov	si,offset crlf
	mov	cx,2
	call	PrintData
@@Oops:
	ret
PrintLine	endp

;==============================================================================
