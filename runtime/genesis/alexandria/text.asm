;============================================================================
;===	text.asm: genesis core routines					  ===
;===	(c) 1991,92 Developer Resources					  ===
;============================================================================
	opt	llbl
	include 'genesis.equ'
	include 'default.equ'
	IFNE SEGA_CD
	include 'cd.inc'
	include 'cd.mac'
	ENDC
	include 'genesis.mac'

	xref	DMACopy
	xref	InputRelease
	xref	GetDigitalInput
	xref	ButtonDispatch
	xref	VDPAddress
	xref	DMAVDPVDP
	xref	ClearFieldA
	xref	_LoadCharsAtAddress

	xdef	maxChars
	xdef	cursorX
	xdef	oldCursorX
	xdef	cursorY
	xdef	oldCursorY
	xdef	textAttr
	xdef	_textAttr

	XDEF	PrintTextCount

	TSECTION	.text

TEXT_SIZE	equ	96*32

; note: you must reserve TEXT_SIZE bytes in the vdp starting at
; whatever address you initialize in d0 for the text routines to work


	xdef	_SaveCursor
_SaveCursor:
	move.w	cursorY,d0
	swap	d0
	move.w	cursorX,d0
	rts

	xdef	_RestoreCursor
_RestoreCursor:
	move.w	6(sp),cursorX
	move.w	4(sp),cursorY
	rts

;============================================================================
;===	Inputs: 							  ===
;===		d0 = base in vdp(in bytes)				  ===
;===	Destroys:							  ===
;===		d0.w,d1.l,d2.b,a0					  ===
;	Prototype:
;		void InitText(unsigned short wVDPCharSetAddress
;============================================================================

	xdef	_InitText
	xdef	InitText


_InitText:
	move.w	6(sp),d0
InitText:
	lea	alphaCharset,a0
InitTextCustomFont:				; a0 -> charset
;						; d0.w = vdp address
	move.w	d3,-(sp)
	move.w	d0,d3
	lsr.w	#5,d3
	move.w	d3,textBase
	ext.l	d3
	move.l	d3,-(a7)
	move.l	a0,-(a7)
	jsr	_LoadCharsAtAddress
	lea	8(a7),a7			; normalize stack

	clr.w	d0
	move.w	d0,cursorX
	move.w	d0,cursorY
	move.w	d0,textAttr
	move.w	d0,textLeftMargin
	sf	lZeroSupr				; default to no
	sf	leftJustify
	sf	cursorOn
	move.w	#'_',cursorDef
	move.w	d0,tScreenScroll
	move.w	#VDP_SCREEN_WIDTH-1,tScreenWidth
	move.w	#VDP_SCREEN_HEIGHT-1,tScreenHeight
	move.w	#VDP_SCREEN_A,textScreenBase
	move.w	(sp)+,d3
	rts

;==============================================================================

	xdef	_TextSetLeftMargin
_TextSetLeftMargin:
	move.w	6(sp),textLeftMargin
	rts

;==============================================================================
;=== FillString:			 				    ===
;=== 	Inputs:								    ===
;===		a0-> string to fill	 				    ===
;===		d0.b = char to fill with 				    ===
;===		d1.w = # of bytes to fill 				    ===
;===					 				    ===
;==============================================================================

FSLoop:
	move.b	d0,(a0,d1.w)
FillString:
	dbra	d1,FSLoop
	rts

;============================================================================
;===	GetTextString: Get text string from player			  ===
;===	Inputs: 							  ===
;=== 		a0-> buffer to place string				  ===
;=== 		a1-> input structure					  ===
;===		d0 = # of characters to enter				  ===
;===		cursorX & cursorY = coordinate to begin entering string	  ===
;===	 								  ===
;===	Internal:							  ===
;===		maxchars = max # of chars				  ===
;===		d3.b = current letter being entered			  ===
;===		d4.w = current position in string			  ===
;===		d5.w = continue flag					  ===
;=== 		a4-> buffer to place string				  ===
;=== 		a3-> input structure					  ===
;===	 								  ===
;----------------------------------------------------------------------------
        ifne 0
GetTextString:
	move.w	d0,maxChars
	move.w	cursorX,oldCursorX
	move.w	cursorY,oldCursorY
	move.l	a0,a4
	move.l	a1,a3
	move.w	#'A',d3
	clr.w	d4
	st	d5


	move.w	d3,-(sp)
	jsr	PrintTextCount
	move.w	(sp)+,d3
.Loop:
	move.l	a3,a0
	jsr	InputRelease
	move.l	a3,a0
	jsr	GetDigitalInput			; waits for keypress, and returns it
	lea	.buttonTable(pc),a0
	jsr	ButtonDispatch

	move.b	d3,(a4,d4.w)			; update string
	move.l	a4,a0
	move.w	maxChars,d0
	move.w	d3,-(sp)
	move.w	oldCursorX,cursorX
	move.w	oldCursorY,cursorY
	jsr	PrintTextCount
	move.w	(sp)+,d3

	tst.w	d5
	bne.s	.Loop
	rts
        endc
;------------------------------------------------------------------------------

.buttonTable:
	dc.l	.Up
	dc.l	.Down
	dc.l	.Left
	dc.l	.Right
	dc.l	.B
	dc.l	.C
	dc.l	.A
	dc.l	.Start

;------------------------------------------------------------------------------

.Left
	subq.b	#1,d3
	cmp.b	#'A',d3
	bcc.s	.UpOk
	move.b	#'z',d3
.UpOk:
	rts

.Right
	addq.b	#1,d3
	cmp.b	#'z'+1,d3
	bcs.s	.DownOk
	move.b	#'A',d3
.DownOk:
	rts

.B
	subq.w	#1,d4
	bcc.s	.BOk
	addq.w	#1,d4

.BOk:
	move.b	(a4,d4.w),d3
.Down
.Up:
	rts

.C
.A
	addq.w	#1,d4
	cmp.w	maxChars,d4
	bne.s	.NotYet
	clr.w	d5				; last digit entered, all done
.NotYet:
	move.b	(a4,d4.w),d3
	rts

; fuck it, we are done
.Start
	clr.w	d5			; clear continue flag
	rts

;============================================================================
;===	PrintTextCount: Print text for d0.w chars			  ===
;===	Inputs: 							  ===
;===		a0-> text string to print				  ===
;===		d0.w = # of chars to print				  ===
;===	Outputs: 							  ===
;===		a0-> just beyond end of string				  ===
;===	Destroys:							  ===
;===		d0.w,d1.l,d2.b,d3.w,a0					  ===
;============================================================================

PrintTextCount:
	move.w	d0,d3
	bra.s	.Entry
.Loop:
	move.b	(a0)+,d0
	bsr	PrintChar
.Entry:
	dbra	d3,.Loop
	rts

;============================================================================
;===	Inputs: 							  ===
;===		a0-> text string to print, zero terminated		  ===
;===	Outputs: 							  ===
;===		a0-> just beyond end of string				  ===
;===	Destroys:							  ===
;===		d0.w,d1.l,d2.b,a0					  ===
;	Prototype:
;		char *PrintText(char *s)
;============================================================================

	xdef	_PrintText
	xdef	PrintText
_PrintText:
	movea.l	4(sp),a0
PrintText:
	clr.w	d0
	move.b	(a0)+,d0
	beq.s	.Done			; handle null, terminate
	cmp.b	#13,d0
	bne.s	.Nope    		; handle all other chars
	move.b	#10,d0
	bsr	PrintChar		; handle cr, go down and terminate
	rts
.Nope
	bsr	PrintChar
	bra.s	PrintText
.Done:
	rts

;============================================================================
;===	Inputs: 							  ===
;===		d0.l = value to print					  ===
;			works with scores up to 99,999
;===		d1.w = # of digits to print				  ===
;===		lZeroSupr						  ===
;===	Destroys:							  ===
;===		d0.w,d1.l,d2.b,a0					  ===
;===	Internal:							  ===
;===		d3 = value to print	 				  ===
;===		d4 = # of digits to print				  ===
;===		d5 = current digit					  ===
;===									  ===
;===									  ===
;============================================================================

PrintDecimalCount:
	sf	lZeroTemp
	move.l	d0,d3
	move.w	d1,d4
	lea	.decimalTab,a0
	move.w	#5-1,d5
.Loop:
	bsr.s	.GetDigit
	cmp.w	d4,d5
	bcc.s	.DontPrint
	tst.b	lZeroSupr
	beq.s	.DontCheck			; if supresion is disabled, dont check
	tst.b	lZeroTemp
	bne.s	.DontCheck			; if we have already printed a non-zero, dont check
	tst.b	d0
	seq	lZeroTemp			; if still zero, set, if not, clear
	bne.s	.DontCheck			; if non-zero, go ahead and print it
	tst.b	leftJustify
	bne.s	.DontPrint
	move.b	#' ',d0				; convert leading zeros to spaces
.DontCheck:
	add.b	#'0',d0
	jsr	PrintChar
.DontPrint:
	dbra	d5,.Loop
	rts

.GetDigit:
	divu	(a0)+,d3
	move.w	d3,d0			; get answer
	clr.w	d3			; clear upper word of new dividend
	swap	d3			; place remainder in lower word
	rts

;------------------------------------------------------------------------------

.decimalTab
	dc.w	10000
	dc.w	1000
	dc.w	100
	dc.w	10
	dc.w	1

;============================================================================
;===	Inputs: 							  ===
;===		d0.l = value to print					  ===
;===	Destroys:							  ===
;===		d0.w,d1.b,d2.b,d3.b,d4.b				  ===
;============================================================================

	xdef	PrintHex32
	xdef	PrintHex16
	xdef	PrintHex8
PrintHex32:
	move.w	d0,d4
	swap	d0
	bsr.s	PrintHex16
	move.w	d4,d0
	bsr.s	PrintHex16
	rts

;============================================================================
;===	Inputs: 							  ===
;===		d0.w = value to print					  ===
;===	Destroys:							  ===
;===		d0.w,d1.b,d2.b,d3.b					  ===
;============================================================================

PrintHex16:
	move.b	d0,d3
	lsr.w	#8,d0
	bsr.s	PrintHex8
	move.b	d3,d0
	bsr.s	PrintHex8
	rts

;============================================================================
;===	Inputs: 							  ===
;===		d0.b = value to print					  ===
;===	Destroys:							  ===
;===		d0.w,d1.w,d2.b						  ===
;============================================================================

PrintHex8:
	move.w	d0,-(sp)		; stash lower nibble
	lsr.b	#4,d0			; get upper nibble where we can use it
	add.b	#'0',d0                 ; convert to ascii
	cmp.b	#'9'+1,d0               ; check if larger than 9
	bcs.s	.NotHex
	add	#'A'-('9'+1),d0
.NotHex:
	bsr.s	PrintChar
	move.w	(sp)+,d0
	and.b	#$f,d0			; remove upper nibble
	add.b	#'0',d0                 ; convert to ascii
	cmp.b	#'9'+1,d0               ; check if larger than 9
	bcs.s	.NotHex1
	add	#'A'-('9'+1),d0
.NotHex1:
	bsr.s	PrintChar
	rts

;==============================================================================
;===	Destroys:        						    ===
;===		d0.l,d1.l						    ===
;==============================================================================

SetVDPCursorAddress:
	move.w	cursorY,d0
	add.w	d0,d0			; * 2
	add.w	d0,d0			; * 4
	add.w	d0,d0			; * 8
	add.w	d0,d0			; * 16
	add.w	d0,d0			; * 32
	add.w	d0,d0			; * 64
	add.w	cursorX,d0
	add.w	d0,d0			; * 128(+cursorX * 2)
	add.w	textScreenBase,d0		; kts 7/8/92
	jsr	VDPAddress
	rts

;============================================================================
;===	PrintChar							  ===
;===	Inputs: 							  ===
;===		d0.b = character to print				  ===
;===	Destroys:							  ===
;===		d0.w,d1.l,d2.b						  ===
;	Prototype:
;		void PrintChar(char c)
;============================================================================

	xdef	_putchx
	xdef	_PrintChar
_putchx:
_PrintChar:
	clr.w	d0
	move.b	7(sp),d0
PrintChar:
	move.l	a0,-(sp)

	cmp.w	#8,d0
	beq	.DoBS
	cmp.w	#10,d0
	beq.s	.DoLF
	cmp.w	#12,d0
	beq	.DoFF

	sub.b	#' ',d0                 ; offset into font

	move.b	d0,d2

	jsr	SetVDPCursorAddress

	move.w	textAttr,d0
	move.b	d2,d0			; assemble complete word
	add.w	textBase,d0
	WRITE_VDP_DATA d0

	move.w	cursorX,d0
	addq.w	#1,d0
	cmp.w	tScreenWidth,d0
	bcs.s	.Ok

; here is where line-feeds occur
.DoLF:
	move.w	cursorY,d0
	addq.w	#1,d0
	cmp.w	tScreenHeight,d0
	bcs.s	.Ok2
;					; if at bottom, wrap around
;					; first try at scrolling the text upwards
;	move.w	#VDP_SCREEN_A,d1
	move.w	textScreenBase,d1
;	move.w	#VDP_SCREEN_A+(VDP_FIELDA_WIDTH*2),d0
	move.w	d1,d0
	add.w	#VDP_FIELDA_WIDTH*2,d0

	move.w	#$fc0,d2
	jsr	DMAVDPVDP
	move.w	cursorY,d0
.Ok2:
	move.w	d0,cursorY
	move.w	textLeftMargin,d0					; kts 03-12-93 01:47pm (orig)10-28-92 01:32am
;	clr.w	d0
.Ok:
	move.w	d0,cursorX
.Done:
	move.l	(sp)+,a0
	tst.b	 cursorOn
	beq.s	.Nope
	jsr	SetVDPCursorAddress
	move.w	cursorDef,d0
	sub.b	#' ',d0                 ; offset into font
	add.w	textBase,d0
	WRITE_VDP_DATA d0
.Nope:
	rts

.DoBS:
	move.w	cursorX,d0
	subq.w	#1,d0
	bcc.s	.DoBSOk
	move.w	cursorY,d0
	subq.w	#1,d0
	bcc.s	.DoBSTop
	clr.w	d0
.DoBSTop:
	move.w	d0,cursorY
	clr.w	d0
.DoBSOk:
	move.w	d0,cursorX
	bra.s	.Done

.DoFF:
	move.w	#0,cursorX
	move.w	#0,cursorY
	jmp	ClearFieldA
;	rts

;============================================================================

	include 	charset.dat

;============================================================================

;ram	segment
	BSECTION	.bss

;	even
	ds.w	1	;even

maxChars	ds.w	1

textBase	ds.w	1		; vdp text base
        xdef _textScreenBase
_textScreenBase
textScreenBase	ds.w	1		; which screen in vdp to plot text
cursorX 	ds.w	1
cursorY 	ds.w	1
oldCursorX 	ds.w	1
oldCursorY 	ds.w	1
tScreenHeight	ds.w	1
tScreenWidth	ds.w	1
tScreenScroll	ds.w	1
_textAttr
textAttr	ds.w	1
textLeftMargin	ds.w	1
cursorDef	ds.w	1
cursorOn	ds.b	1
lZeroSupr	ds.b	1
leftJustify	ds.b	1
lZeroTemp	ds.b	1

;	even
	ds.w	1	;even

;============================================================================

	END

;============================================================================

