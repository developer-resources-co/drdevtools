;==============================================================================
; patch.asm: backpatching code
;==============================================================================
; backpatch entry(?):
;	[filename ('$'-terminated)] (global only--not local)
;	WORD containing line number
;	(SNES) BYTE LONGA/LONGI flags
;	WORD GVL, WORD GVH
;	WORD OBJPTRL, WORD OBJPTRH
;	WORD DELTA_L, WORD DELTA_H
;	[MNE FIELD (NUL-terminated)]
;	[PARAM_FIELD (NUL-terminated)]
;	(local) ???
;------------------------------------------------------------------------------

DATA	SEGMENT	'DATA'

	ifdef	_SPOOL_BACKPATCH
SPOOL_FLAG      DW      1       ;IF = 1, THEN SPOOL FWD REF DATA TO DISK
SPOOL_HANDLE    DW      ?
_SPOOL_FILE	DB      'FWDREF.SPZ',0
	endif

GVFLAG	DB	0
GVL	DW	0		; grab val low
GVH	DW	0		; grab val high

HOLEEND	DW	0
HOLEPTR	DW	0
HOLESEG	DW	0

;patch_listPos	dq	0	; position in listing file to patch

DATA	ENDS

;==============================================================================
.CODE
;------------------------------------------------------------------------------

FILL_HOLES	PROC	NEAR	;FILL IN ANY FORWARD REFERENCES

	ifdef	_SPOOL_BACKPATCH
	CMP	SPOOL_FLAG,1	;IF SPOOLING THEN CLOSE FILE
	JNA	@@NotSpooled
	CALL	PUSH_SPOOL
	MOV	AH,3EH
	MOV	BX,SPOOL_HANDLE
	INT	21H
	MOV	AX,INT21_OPENFILE	;AND REOPEN IT FOR READ ONLY
	MOV	DX,OFFSET _SPOOL_FILE
	INT	21H
	JC	@@FUCKED
	MOV	SPOOL_HANDLE,AX
	CALL	PULL_SPOOL
	DEC	SPOOL_FLAG
@@NotSpooled:
	endif

	MOV	BX,CSPTR	;FIX SSK THINGS
	OR	BX,BX
	JZ	@@FH

	MOV	ES,BX
	MOV	AX,SSKBL	;SAVE SSKB INTO LAST SEG
	MOV	ES:[symbol_segment_bottom_l],AX
	MOV	AX,SSKBH
	MOV	ES:[symbol_segment_bottom_h],AL

	MOV	AX,SSKTL	;SAVE SSKT INTO LAST SEG
	MOV	ES:[symbol_segment_top_l],AX
	MOV	AX,SSKTH
	MOV	ES:[symbol_segment_top_h],AL

@@FH:
	ifdef	_SPOOL_BACKPATCH
	CMP	SPOOL_FLAG,1	;NO FORWARD REFERENCES?
	JA	@@FH0
	endif
	CMP	HOLEPTR,0
	JNZ	@@FH0
	RET

@@FUCKED:
	JMP	SEVERE

@@FH0:
	MOV	SRC_LEVEL,0	;SET UP ENVIRONMENT

	push	objptrl
	push	objptrh		;INIT "SPOOL" FRAME
	or	sdb1,10h

	mov	cstype,SEGTYPE_ROM
@@fh0x:
	mov	ax,holeptr	;INIT "SPOOL" FRAME
	mov	holeend,ax
	mov	holeptr,0

@@FH1:
	MOV	ES,HOLESEG	;INIT POINTERS
	MOV	DI,HOLEPTR

	XOR	BX,BX
@@FH2:
	MOV	AL,ES:[DI]	;EXTRACT FILESPEC
	INC	DI
	MOV	[FILESPEC+BX],AL
	INC	BX
	CMP	AL,'$'
	JNZ	@@FH2

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
@@FH3:
	MOV	AL,ES:[DI]	;EXTRACT MNE_FIELD
	INC	DI
	MOV	[MNE_FIELD+BX],AL
	MOV	[MNE_LEN],BX
	INC	BX
	OR	AL,AL
	JNZ	@@FH3

	XOR	BX,BX
@@FH4:
	MOV	AL,ES:[DI]	;EXTRACT PARAM_FIELD
	INC	DI
	MOV	[PARAM_FIELD+BX],AL
	INC	BX
	OR	AL,AL
	JNZ	@@FH4
	SUB	BX,2
	MOV	[PARAM_LAST],BX

	MOV	HOLEPTR,DI			; kts 01-17-93 02:56pm was above list code
	CALL	PARSE_MNE	;PARSE AND ASSEMBLE THE FIELDS

	MOV	AX,HOLEPTR
	CMP	AX,HOLEEND
	JNC	@@FH6X
	JMP	@@FH1

@@FH6X:
	ifdef	_SPOOL_BACKPATCH
	DEC	SPOOL_FLAG
	JS	@@FH7
	JZ	@@FH7
	CALL	PULL_SPOOL
	JMP	@@FH0X
@@FH7:
	endif
	POP	OBJPTRH
	POP	OBJPTRL
	RET

FILL_HOLES	ENDP

;==============================================================================

MAKE_HOLE	PROC	NEAR	;HANDLE FORWARD REFERENCE

	CLD
	CMP	[SYMBOL_FIELD],LOCAL_SIG  ;LOCAL LABELS GO INTO SEPERATE BUFFER
	JZ	@@MLH

	TEST	SDB1,4		;ALREADY AN F.R. ON THIS LINE?
	JZ	@@MH		;IF NOT...

;@@AlreadyForwardRef:
	JMP	@@MH3B

@@MH:	MOV	ES,HOLESEG	;THEN MAKE A HOLE TABLE ENTRY
	MOV	DI,HOLEPTR
	OR	SDB1,4
	JMP	@@MH0

@@MLH:
;	TEST	SDB2,4		;MAKE LOCAL HOLE
;	JNZ	@@AlreadyForwardRef
	MOV	ES,LFRSEG
	MOV	DI,LFRPTR
	OR	SDB1,4
	OR	SDB2,4
	JMP	@@MH1A

		; ES:DI points to hole table (local or global)
@@MH0:
	MOV	BX,SRC_LEVEL	;STORE FILESPEC
	ERRIF	FILESPEC_LEN NE 80H "FILESPEC_LEN has changed, code must be updated"
	MOV	CL,7
	SHL	BX,CL
	ADD	BX,OFFSET FILESPEC
@@MH1:
	MOV	AL,[BX]
	INC	BX
	STOSB
	CMP	AL,'$'
	JNZ	@@MH1

@@MH1A:
	MOV	BX,SRC_LEVEL	;STORE LINE_COUNT
	SHL	BX,1
	MOV	AX,[LINE_COUNT+BX]
	STOSW

	ifdef	SNES
	MOV	AL,SDB1		;STORE LONGA/LONGI STATUS
	AND	AL,0C0H
	STOSB
	;---- Store size of hole
	endif

	CMP	GVFLAG,0	;THIS KLUDGE MAKES FWD REF WITH DC.x WORK
	JZ	@@GVK1
	MOV	AX,GVL
	STOSW
	MOV	AX,GVH
	STOSW
	JMP	@@GVK2

@@GVK1:
	MOV	AX,OBJPTRL	;STORE OBJECTPTRS
	STOSW
	MOV	AX,OBJPTRH
	STOSW
@@GVK2:
	MOV	AX,DELTA_L
	STOSW
	MOV	AX,DELTA_H
	STOSW

	XOR	BX,BX		;STORE MNE_FIELD
@@MH2:	MOV	AL,[MNE_FIELD+BX]
	INC	BX
	STOSB
	OR	AL,AL
	JNZ	@@MH2

	XOR	BX,BX		;STORE PARAM_FIELD
@@MH3:	MOV	AL,[PARAM_FIELD+BX]
	INC	BX
	STOSB
	OR	AL,AL
	JNZ	@@MH3

	;----- store listing file position
	if 0
	mov	bx,listhand
	mov	ax,04201h			; LSEEK, offset from current pos.
	xor	cx,cx
	xor	dx,dx
	int	21h				; actually, just read file position
	jnc	@@Rept_LSEEK_Ok
	jmp	severe

@@Rept_LSEEK_Ok:
	; remember where in the file the line after the rept is
	stosw
	mov	ax,dx
	stosw
	endif

	CMP	[SYMBOL_FIELD],LOCAL_SIG
	JNZ	@@MH3A

	MOV	LFRPTR,DI	;STORE LOC FWD REF POINTER
	CMP	DI,LOCAL_FWDREF_BUFFER_SIZE
	JNC	@@MH5
	JMP	@@MH3B

@@MH3A:
	MOV	HOLEPTR,DI	;STORE GLOBAL HOLEPTR
	CMP	DI,HOLE_BUFFER_SIZE
	JNC	@@MH4

@@MH3B:
	ifdef	GENESIS
	MOV	CX,04E71H		; 68000 NOP
	mov	bx,cx
	endif
	ifdef	SNES
	MOV	CX,0EAEAh		; 65816 NOP (x3)
	mov	bx,00EAh
	endif
	ifdef	SPC700
	xor	cx,cx			; SPC700 NOP (x4)
	mov	bx,cx
	endif
	RET

@@MH4:
	ifdef	_SPOOL_BACKPATCH
	CMP	SPOOL_FLAG,0
	JZ	@@MH4X
	CALL	PUSH_SPOOL
	JMP	@@MH3B
@@MH4X:
	endif

	MOV	AX,19		;TOO MANY HOLES!
	CALL	ERROR
	mov	_ERRORLEVEL,DOS_RETURNCODE_SEVERE
	JMP	T1
@@MH5:
	MOV	AX,29
	CALL	ERROR
	mov	_ERRORLEVEL,DOS_RETURNCODE_SEVERE
	JMP	T1
MAKE_HOLE	ENDP

;==============================================================================

	ifdef	_SPOOL_BACKPATCH
PUSH_SPOOL PROC	NEAR

	CMP	SPOOL_FLAG,1		;FIRST PUSH?
	JNZ	PS1
	MOV	AH,3CH			;THEN OPEN SPOOL FILE
	XOR	Cx,Cx				; kts 12-25-92 05:32am
	MOV	DX,OFFSET _SPOOL_FILE
	INT	21H
	JC	PS9
	MOV	SPOOL_HANDLE,AX
	;----- Print message stating spooling backpatch file
	if	1
	mov	ax,65
	call	Error
	else
	lea	si,spoolingBackpatchText
	push	si
	mov	bx,STDOUT
	call	PrintLine
	pop	si
	call	ListPrintLine
	endif

PS1:
	MOV	AH,40H			;WRITE HOLEPTR
	MOV	BX,SPOOL_HANDLE
	MOV	CX,2
	MOV	DX,OFFSET HOLEPTR
	INT	21H
	JC	PS9
	MOV	AH,40H			;WRITE HOLESEG DATA
	MOV	BX,SPOOL_HANDLE
	MOV	CX,HOLEPTR
	XOR	DX,DX
	PUSH	DS
	MOV	DS,HOLESEG
	INT	21H
	POP	DS
        JC      PS9
	MOV	HOLEPTR,0
	INC	SPOOL_FLAG
	RET

PULL_SPOOL:
	MOV	AH,3FH			;READ HOLEPTR
	MOV	BX,SPOOL_HANDLE
	MOV	CX,2
	MOV	DX,OFFSET HOLEPTR
	INT	21H
	JC	PS9
	MOV	AH,3FH			;READ HOLESEG DATA
	MOV	BX,SPOOL_HANDLE
	MOV	CX,HOLEPTR
	XOR	DX,DX
	PUSH	DS
	MOV	DS,HOLESEG
	INT	21H
	POP	DS
        JC      PS9
	RET

PS9:	JMP	SEVERE

PUSH_SPOOL ENDP

	endif


