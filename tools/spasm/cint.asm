;==============================================================================
; cint.asm: C interface for spasm
;==============================================================================

DATA	SEGMENT	'DATA'

_SRC_FILE		DB	FILENAMELEN DUP(?)	;SOURCE CODE FILESPEC
_SLD_FILE		DB	FILENAMELEN DUP(?)
_SLDFLAG		DB	0	;IF TRUE THEN WRITE SLD_FILE


_LINES_ASSEMBLED	dd	0

cStackOff		dw	?
cStackSeg		dw	?	; c stack holders

_BCASESENSITIVE	DB	0
_SYSTEMTYPE	DB	0		;0 = devel system, 1 = nowhere, 2 = disk
_SYSTEM_FILE    	DB      FILENAMELEN DUP(?)
SYSTEMHAND	DW	0		; file handle
					; where to write output

_SYMFLAG		DB	0	;IF TRUE, THEN WRITE SYM_FILE
_SYM_FILE		DB	FILENAMELEN DUP(?)
SYM_HEADING	DB	13,10
		DB	'Symbol file produced by SPASM',13,10
		DB	'Copyright (C) Developer Resources 1991,1992.  All Rights Reserved.',13,10,26


SLD_HAND		DW	0
SLD_HEADING	DB	'Source level debugging file produced by SPASM',13,10
		DB	'Copyright (C) Developer Resources 1991,1992.  All Rights Reserved.',13,10,26

_SPASM_MAJOR	dw	?
_SPASM_MINOR	dw	?
_SPASM_REV	dw	?

_MAPFLAG		DB	0	;IF TRUE, THEN WRITE MAP_FILE
_MAP_FILE		DB	FILENAMELEN DUP(?)

DATA	ENDS

;==============================================================================
.CODE

;==============================================================================

	PUBLIC	_SRC_FILE
	PUBLIC	_SLDFLAG
	PUBLIC	_SLD_FILE
	PUBLIC	_MAP_FILE
	PUBLIC	_MAPFLAG
	PUBLIC	_SYMFLAG
	PUBLIC	_SYM_FILE

	PUBLIC	_SPASM_MAJOR,_SPASM_MINOR,_SPASM_REV

	PUBLIC	_BCASESENSITIVE
	PUBLIC	_SYSTEMTYPE
	PUBLIC	_SYSTEM_FILE

;	ifdef	SNES
;	PUBLIC	_SFXBASE
;	PUBLIC	_SFXMEMSEG
;	endif

	public	C	SPASM
	PUBLIC	_LINES_ASSEMBLED

;==============================================================================
; kevin's C interface

SPASM	proc	far
	push	ds
	push	es
	push	bp
	push	di
	push	si

	MOV	AX,SEG DATA		;POINT DS @ DATA SEGMENT
	MOV	DS,AX
	mov	es,ax
	mov	cStackSeg,ss
	mov	cStackOff,sp
	call	near ptr SPAZ
	ret
SPASM	ENDP

;==============================================================================

ReturnToC	proc	near
	MOV	AX,DATA		;POINT DS @ DATA SEGMENT
	MOV	DS,AX
	mov	ss,cStackSeg
	mov	sp,cStackOff
	pop	si
	pop	di
	pop	bp
	pop	es
	pop	ds
	ret
ReturnToC	endp

;==============================================================================

	extrn	_expBuffer:near
	extrn	_answer

	extrn	@DoExpr$qv:near

HELP_SCOTT	PROC	NEAR
	mov	si,expstart
	mov	cx,expend
	sub	cx,si
	inc	cx		; convert into length

	mov	ax,_DATA		; point at C data segment
	mov	es,ax
	mov	di,offset	DGROUP:_expBuffer
	cld

; Handled in expression parser now
;	lodsb
;	CMP	AL,LLCHR
;	jnz	@@No
;	mov	al,07Fh		; internal repersentation of llchar
;@@No:
;	stosb
;	dec	cx

	;jcxz	@@one_char_symbol	; length always at least 2 (symbol len + '\0')
@@Loop:
	movsb
	loop	@@Loop
;	rep	movsb
	xor	al,al
	stosb			; zero terminate string
				; return to C

	mov	ax,_DATA		; point at C data segment
	mov	ds,ax

	call	@DoExpr$qv

	mov	bx,word ptr _answer+2
	mov	cx,word ptr _answer

	MOV	DX,DATA		;POINT DS @ DATA SEGMENT
	MOV	DS,DX

				; answer in bx:cx
				; ax = error #(0 =  no error)
	OR	AX,AX
	JZ	HS1
	cmp	ax,6
	jbe	@@FirstLocalLabel
	mov	ax,2		; internal error, something went wrong

@@FirstLocalLabel:
	ADD	AX,36		;CONVERT TO MY ERROR CODE

	CMP	AX,41		;IS ERROR "UNDEFINED SYMBOL?"
	JNZ	@@THERE		;JUMP IF NOT

	TEST	SDB1,11H	;ARE WE BACKPATCHING YET?
	JZ	HS0		;JUMP IF NOT AND MAKE HOLE

@@THERE:
	CALL	ERROR	;THIS SYMBOL IS REALLY UNDEFINED
	JMP	HS0A

HS0:
	CALL	MAKE_HOLE	;MAKE THE HOLE
	jmp	HS1		; kts added 03-09-94 02:11pm
HS0A:
	ifdef	GENESIS
	MOV	CX,04E71H	;RETURN NOP'S AS VALUE
	MOV	BX,CX
	endif
	ifdef	SNES
	MOV	CX,0EAEAh	;RETURN NOP'S AS VALUE
	mov	bx,cx
	endif
	ifdef	SPC700
	xor	cx,cx
	xor	bx,bx
	endif
HS1:
	RET
HELP_SCOTT	ENDP

;==============================================================================

	ifdef	GENESIS
Public C SwapWord
SwapWord    PROC NEAR
    Cprefix
    mov     ax,[bp]
    xchg    ah,al
    Cpostfix
SwapWord    ENDP

Public C SwapLongWords
SwapLongWords    PROC NEAR	; 1234 becomes 3412
    Cprefix
     mov dx,[bp]
     mov ax,[bp+2]
    Cpostfix
SwapLongWords    ENDP

Public C SwapLong
SwapLong    PROC NEAR		; 1234 becomes 4321
    Cprefix
     mov dx,[bp]
     mov ax,[bp+2]
     xchg    ah,al
     xchg    dh,dl
    Cpostfix
SwapLong    ENDP
	endif

;==============================================================================

_DATA	segment	'DATA'	; C-generated DATA SEGMENT
d@	label	byte
d@w	label	word
_DATA	ends


DATA	SEGMENT	'DATA'

__SPASM__		db	"__SPASM__",0	; assembler version #

	ifdef	GENESIS
__PROCESSOR__	db	"__68000__",0
__TARGET__	db	"__GENESIS__",0
	endif

	ifdef	SNES
__PROCESSOR__	db	"__65816__",0
__TARGET__	db	"__SNES__",0
	endif

	ifdef	SPC700
__PROCESSOR__	db	"__SPC700__",0
__TARGET__	db	"__SONY__",0	; ?? what should go here?
	endif
DATA	ENDS

;==============================================================================

init_predefined_symbols	PROC	NEAR
	push	es
	push	ds
	push	di
	push	si

	;----- Value = 1 (defined) for __PROCESSOR__ & __TARGET__
	mov	svl,1
	mov	svh,0

	mov	ax,SEG line
	mov	ds,ax
	mov	es,ax

	mov	di,OFFSET line
	mov	si,OFFSET __PROCESSOR__
@@strcpy_proc:
	movsb	; ds:si=>es:di
	or	al,al
	jnz	@@strcpy_proc
	call	ADD_SYMBOL

	mov	ax,SEG line
mov	ds,ax
	mov	es,ax
	mov	di,OFFSET line
	mov	si,OFFSET __TARGET__
@@strcpy_target:
	movsb
	or	al,al
	jnz	@@strcpy_target
	call	ADD_SYMBOL


	mov	ah,BYTE PTR [_SPASM_MAJOR]
	mov	al,BYTE PTR [_SPASM_MINOR]
	mov	svh,ax
	mov	ax,[_SPASM_REV]
	mov	svl,ax

	mov	ax,SEG line
	mov	ds,ax
	mov	es,ax

	mov	di,OFFSET line
	mov	si,OFFSET __SPASM__
@@strcpy_spasm:
	movsb
	or	al,al
	jnz	@@strcpy_spasm
	call	ADD_SYMBOL


	pop	si
	pop	di
	pop	ds
	pop	es

	ret
init_predefined_symbols	ENDP

;==============================================================================

	extrn	_memChunk
	extrn	_memSizeSeg

;------------------------------------------------------------------------------

INIT_XTRA	PROC	NEAR	;SET UP EXTRA SEGMENTS
	push	ds
	mov	ax,_DATA
	ASSUME	DS:_DATA
	mov	ds,ax
	mov	ax,WORD PTR _DATA:_memChunk+2
	inc	ax		; skip first segment, since not aligned
	ASSUME	DS:DATA
	pop	ds

	ADD	AX,21H		;+210H BYTES
	MOV	BUFSEG,AX	;20K SOURCE FILE BUFFERS
;	ADD	AX,500H		;+20K BYTES
	ADD	AX,(SOURCE_BUFFER_SIZE*INCLUDEMAX)/16		;+20K BYTES
	MOV	BUFSEG2,AX	;16K FILE BUFFER  (INCBIN, INCSYM, ETC.)
;	ADD	AX,400H		;+16K BYTES
	ADD	AX,FILE_BUFFER_SIZE/16
	CMP	_SLDFLAG,0	;DOING SLD?
	JZ	IX
	MOV	SLDBUF,AX	;MAKE 16K BUFFER IF SO.
;	ADD	AX,400H
	ADD	AX,SLD_BUFFER_SIZE/16

IX:
	MOV	HOLESEG,AX	;56K "HOLES" TABLE
;	ADD	AX,0F00H	;+56K BYTES
	ADD	AX,HOLE_BUFFER_SIZE/16

	MOV	LFRSEG,AX	;8K LOCAL FWD REF TABLE
	ADD	AX,LOCAL_FWDREF_BUFFER_SIZE/16
;	ADD	AX,200H		;+8K BYTES
	MOV	MACSEG,AX	;12K MACRO TABLE
;	ADD	AX,300H		;+12K BYTES
	ADD	AX,MACRO_BUFFER_SIZE/16		;+12K BYTES
	MOV	SYMPTRSEG,AX	;24K "SYMPTR" TABLE
;	ADD	AX,600H		;+24K BYTES
	ADD	AX,SYMPTR_BUFFER_SIZE/16

	MOV	SYMBASE,AX	;SYMBOL TABLE WILL BE STORED STARTING HERE

; kts changed 7-7-91
				; 1K <<6 = * 64 = convert into segment count
	push	ds
	mov	ax,_DATA
	ASSUME	DS:_DATA
	mov	ds,ax
	mov	ax,WORD PTR _DATA:_memChunk+2
	inc	ax
	add	ax,WORD PTR _DATA:_memSizeSeg			; read actual size
	ASSUME	DS:DATA
	pop	ds
	MOV	SYMTOP,AX
	SUB	AX,SYMBASE
	JNC	@@Ok
	MOV	AH,9
	MOV	DX,OFFSET AMNESIA
	INT	21H
	mov	_ERRORLEVEL,DOS_RETURNCODE_SEVERE
	JMP	T1
@@Ok:
	SHR	AX,1
	MOV	SYMCAP,AX
	CMP	AX,3000H
	JC	@@NoCap
	MOV	SYMCAP,3000H
@@NoCap:
	call	InitOutput
	RET
INIT_XTRA	ENDP

;==============================================================================
