;==============================================================================
; list.asm: listing support for spasm
;==============================================================================

	PUBLIC	_LIST_FILE
	PUBLIC	_bBackpatchListingFile

;==============================================================================
DATA	SEGMENT	'DATA'
;------------------------------------------------------------------------------

	ifdef	GENESIS
LISTHEXSTART	EQU	14
LISTNUMSTART	EQU	LISTHEXSTART+20
LISTSOURCESTART	EQU	LISTHEXSTART+26
	endif

	ifdef	SNES
LISTHEXSTART	EQU	25+2
LISTNUMSTART	EQU	LISTHEXSTART+12
LISTSOURCESTART	EQU	LISTHEXSTART+18
	endif

	ifdef	SPC700			; !!!!! CHANGE
LISTHEXSTART	EQU	25
LISTNUMSTART	EQU	LISTHEXSTART+12
LISTSOURCESTART	EQU	LISTHEXSTART+18
	endif

list_TITL	db	132 DUP (0)
LIST_HEADER	db	'Listing file created by SPASM',13,10
	ifdef	SNES
		DB	'LORG   PORG   DB DP   fl t HEX         Line#  Source',13,10
		DB	'--------------------------------------------------------------------------------'
	endif
	ifdef	SPC700
		DB	'LORG   PORG   DB DP   fl HEX         Line#  Source',13,10
		DB	'--------------------------------------------------------------------------------'
	endif
	ifdef	GENESIS
		DB	'LORG   PORG   HEX                 Line#  Source',13,10
		DB	'--------------------------------------------------------------------------------'
	endif
		db	0				; NUL terminator
;LIST_HEADER_SIZE	EQU	$-LIST_HEADER

_LIST_FILE	DB	FILENAMELEN DUP (?)
_bBackpatchListingFile	DB	0

LISTHAND	DW	0
LIST_BYTES      DB      8       DUP(0)
LBPTR           DW      0
LIST_PREFIX     DB      132	DUP(0)

list_lineNum	dw	0	; line number (in page)
list_linesPerPage	dw	0
list_pageWidth	dw	132
list_FF		db	12,0

;------------------------------------------------------------------------------
DATA	ENDS
;==============================================================================
.CODE
;------------------------------------------------------------------------------

; print begining of the listing line

list_checkHeader	PROC	NEAR

	;----- Check to see if we've filled the page
	mov	cx,list_linesPerPage
	or	cx,0
	jz	@@Cont			; == 0 means page breaking disabled

	mov	cx,list_lineNum
	cmp	cx,1
	jne	@@Not_Header
	lea	si,list_TITL
	call	ListPrintLine
@@Not_Header:

	mov	ax,list_lineNum
	cmp	ax,list_linesPerPage
	jb	@@Cont

	;----- Page full: eject page, do header
	mov	bx,LISTHAND
	push	si
	mov	si,offset list_FF
	call	PrintPartialLine
	pop	si

	mov	list_lineNum,0

@@Cont:

	ret

list_checkHeader	ENDP


LIST_START      PROC    NEAR            ;START A LIST FILE ENTRY

	CMP	LISTHAND,0		;IF LISTFILE NOT OPEN THEN OPEN IT
	JNZ	LS2
	MOV	AH,3CH
	XOR	Cx,Cx				; kst 12-25-92 05:33am
	MOV	DX,OFFSET _LIST_FILE
	INT	21H

	JNC	LS1
	JMP	SEVERE

LS1:
	MOV	LISTHAND,AX

	; print banner into listing file, kts 11-24-92 02:25pm

;	MOV	AH,40H
	MOV	BX,LISTHAND
;	MOV	CX,LIST_HEADER_SIZE
;	MOV	DX,OFFSET LIST_HEADER
;	INT	21H

	lea	si,list_header
	call	ListPrintLinePartial
				; end kts
LS2:
	call	list_checkHeader

	MOV     AX,DS			;CLEAR OUT LIST_PREFIX
        MOV     ES,AX
        MOV     AX,2020H
        MOV     CX,132/2 			;size of output buffer
        MOV     DI,OFFSET LIST_PREFIX
        CLD
        REP     STOSW
					; print org address
	MOV	AX,[OBJPTRH]		;STORE OBJPTRS
	CALL	HEXOUT
	MOV	[LIST_PREFIX],AH
	MOV	[LIST_PREFIX+1],AL
	MOV	AL,BYTE PTR [OBJPTRL+1]
	CALL	HEXOUT
	MOV	[LIST_PREFIX+2],AH
	MOV	[LIST_PREFIX+3],AL
	MOV	AL,BYTE PTR [OBJPTRL]
	CALL	HEXOUT
	MOV	[LIST_PREFIX+4],AH
	MOV	[LIST_PREFIX+5],AL
				; kts added 4/24/92
					; print porg address

	MOV	cX,OBJPTRL	;COMPUTE OBJPTR+DELTA
	ADD	cX,DELTA_L
	MOV	dx,OBJPTRH
	ADC	dx,DELTA_H
					; dl:cx contains porg
	mov	ah,':'
	MOV	[LIST_PREFIX+6],AH

	MOV	AL,dl
	CALL	HEXOUT
	MOV	[LIST_PREFIX+7],AH
	MOV	[LIST_PREFIX+8],AL
	MOV	AL,ch
	CALL	HEXOUT
	MOV	[LIST_PREFIX+9],AH
	MOV	[LIST_PREFIX+10],AL

	MOV	AL,cl
	CALL	HEXOUT
	MOV	[LIST_PREFIX+11],AH
	MOV	[LIST_PREFIX+12],AL
				; end kts addition
	ifdef	SNES
					; kts added to print data assume
	mov	al,dataAssume		; DB
	call	HEXOUT
	MOV	[LIST_PREFIX+14],AH
	MOV	[LIST_PREFIX+15],AL

	mov	cx,directAssume		; DP
	mov	al,ch
	call	HEXOUT
	MOV	[LIST_PREFIX+17],AH
	MOV	[LIST_PREFIX+18],AL
	mov	al,cl
	call	HEXOUT
	MOV	[LIST_PREFIX+19],AH
	MOV	[LIST_PREFIX+20],AL

					; end kts

	;----- print size flags
	mov	ax,'aA'
	test	sdb1,80h
	jnz	LS3
	xchg	ah,al
LS3:
	mov	[LIST_PREFIX+22],al

	mov	ax,'iI'
	test	sdb1,40h
	jnz	LS4
	xchg	ah,al
LS4:
	mov	[LIST_PREFIX+23],al
	endif

	MOV     LBPTR,0                 ;CLEAR LBPTR

	RET

LIST_START      ENDP

;==============================================================================

LIST_END        PROC    NEAR            ;FINISH A LIST FILE ENTRY

	XOR	BX,BX			;STORE BYTES
	MOV	DI,LISTHEXSTART			; offset into list string to begin displaying hex
@@Loop:
	CMP	BX,LBPTR
	JAE	@@Done
	MOV	AL,[LIST_BYTES+BX]
	CALL	HEXOUT
	MOV	[LIST_PREFIX+DI],AH
	MOV	[LIST_PREFIX+DI+1],AL
	INC	BX
	ADD	DI,3
	JMP	@@Loop

@@Done:
	MOV	BX,SRC_LEVEL
	CMP	BX,0FFFFH
	JNZ	@@Cont
	RET

@@Cont:
	SHL	BX,1
	MOV	AX,[LINE_COUNT+BX]

	MOV	DI,AX		;MAGIC XECOM THING
	XOR	AX,AX
	MOV	BX,AX
	MOV	DX,AX
	MOV	CX,10H
@@PrintLoop:
	SHL	DI,1
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
	LOOP	@@PrintLoop

	MOV	CX,DX

	AND	SDB1,NOT 20H
	MOV	DI,LISTNUMSTART			;22H

	MOV	AL,BL		;DISPLAY 10,000TH PLACE
	CALL	HEXOUT
	MOV	DL,AL
	CALL	LE4

	MOV	AL,CH		;DISPLAY 1,000TH AND 100TH PLACE
	CALL	HEXOUT
	MOV	DL,AH
	CALL	LE4
	MOV	DL,AL
	CALL	LE4

	MOV	AL,CL		;DISPLAY 10TH AND 1TH PLACE
	CALL	HEXOUT
	MOV	DL,AH
	CALL	LE4
	MOV	DL,AL
	OR	SDB1,20H
	CALL	LE4

	ifdef SNES
	test	SDB2,10h
	jz	@@NoOpcode
	;----- Print opcode cycle timings
	mov	al,[LIST_BYTES]		; fetch opcode @@@ need a better way!
	call	CalculateOpcodeTiming
	call	HEXOUT
	mov	[LIST_PREFIX+25],al
@@NoOpcode:
	endif

			; write listing to listing file
	MOV	BX,LISTHAND
	mov	cx,LISTSOURCESTART
	mov	si,offset LIST_PREFIX
	call	near ptr PrintData
	JC	@@Severe

				; write source file to listing file
	mov	si,offset LINE
	call	ListPrintLine
	JC	@@Severe
	RET

@@Severe:
	JMP	SEVERE

;************************************

LE4:	CMP	DL,30H
	JNZ	LE5
	TEST	SDB1,20H
	JZ	LE6
LE5:	OR	SDB1,20H
	MOV	[LIST_PREFIX+DI],DL
	INC	DI
	RET
LE6:	MOV	[LIST_PREFIX+DI],20H
	RET

LIST_END        ENDP

;==============================================================================
; inputs:
;	ds:si-> buffer to print (0 terminated)
; Outputs:
;	carry set = write error
; Prints line, and prints cr/lf
;------------------------------------------------------------------------------

ListPrintLinePartial	PROC
	TEST	SDB2,8
	JZ	@@Nope

	mov	bx,LISTHAND
	call	PrintLine
@@Nope:
	ret
ListPrintLinePartial	ENDP

ListPrintLine	PROC

	call	ListPrintLinePartial
	inc	list_lineNum
;;	lea	si,crlf
;;	call	ListPrintLinePartial
	ret

ListPrintLine	ENDP

;==============================================================================

