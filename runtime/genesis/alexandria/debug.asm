;==============================================================================
;===    debug.asm: Genesis debugging code                                   ===
;===    (c) 1991,92 Developer Resources                                     ===
;==============================================================================
;       Purpose: provide developer with some simple debugging tools
;
;       PrintRegs: dumps all registers on the screen, must be called as a
;       sub-routine to print pc properly.
;       (mostly useful to attach to unused vectors when game is being
;       playtested from a cart)
;
;       VDPView: very handy for checking vdp character useage, shows first
;               half of the vdp character memory on the screen.
;       (usually never call this from a game, just from the monitor)
;
;==============================================================================
	opt llbl
	include genesis.equ
	include default.equ
	include macros.mac
	IFNE SEGA_CD
	include	cd.inc
	include	cd.mac
	ENDC
	include genesis.mac
	include vdp.equ

	XREF	cursorX
	XREF	cursorY
	XREF	PrintText
	XREF	PrintHex32
	XREF	ClearFieldA
	XREF	ClearFieldB
	XREF	ClearWindow
	XREF	SetHScroll
	XREF	SetVScroll
	XREF	ClearSpriteTable
	XREF	CopySprites
	XREF	VDPScreenAddrB

	xdef	Buserr
	xdef	Adderr
	xdef	Illegal
	xdef	Diverr
	xdef	Trace
	xdef	LineA
	xdef	LineF
	xdef	Blank
	xdef	IRQ01
	xdef	IRQ02
	xdef	IRQ03
	xdef	IRQ04
	xdef	IRQ05
	xdef	IRQ06
	xdef	NMI
	xdef	_LastObject


	TSECTION        .text

;==============================================================================
; print all registers on the screen

	ifne	0
	xdef	_PrintRegs
_PrintRegs:

PrintRegs:
	movem.l d0-d7/a0-a6,-(sp)               ; preserve all registers

;                ; kts bug: 64(sp) doesn't work

	move.l  16*4(a7),-(sp)                  ; push return address onto stack
	movem.l d0-d7/a0-a7,-(sp)
	clr.w   cursorY
	move.w  #(16+1)-1,d7
	lea     .regStrings,a0
.Next:
	move.w  #20,cursorX
	addq.w  #1,cursorY
	jsr     PrintText
	move.l  (sp)+,d0
	jsr     PrintHex32
	dbra    d7,.Next
	movem.l (sp)+,d0-d7/a0-a6
	rts

;------------------------------------------------------------------------------

.regStrings:
	dc.b      'd0: '
	dc.b    0
	dc.b      'd1: '
	dc.b    0
	dc.b      'd2: '
	dc.b    0
	dc.b      'd3: '
	dc.b    0
	dc.b      'd4: '
	dc.b    0
	dc.b      'd5: '
	dc.b    0
	dc.b      'd6: '
	dc.b    0
	dc.b      'd7: '
	dc.b    0

	dc.b      'a0: '
	dc.b    0
	dc.b      'a1: '
	dc.b    0
	dc.b      'a2: '
	dc.b    0
	dc.b      'a3: '
	dc.b    0
	dc.b      'a4: '
	dc.b    0
	dc.b      'a5: '
	dc.b    0
	dc.b      'a6: '
	dc.b    0
	dc.b      'a7: '
	dc.b    0
	dc.b      'pc: '
	dc.b    0

;	even
	ds.w	1	;even

	endc

;============================================================================
; destroys:
;       most data registers, but who cares, this is a debugging routine

	xdef	_VdpView
_VdpView:

VdpView:
	clr.w   d4
;	push.w  d4
	movem.w d4,-(sp)

	jsr     ClearFieldA
	jsr     ClearFieldB
	jsr     ClearWindow

	clr.w	d0
	moveq	#FIELD_A,d1
	jsr	SetHScroll

	clr.w   d0
	moveq	#FIELD_B,d1
	jsr     SetHScroll

	jsr     ClearSpriteTable
	move.w  #VDP_SPRITEBASE,d0
	jsr     CopySprites

	clr.w   d7
;	pop.w   d4
	movem.w (sp)+,d4
	move.w  #28-1,d6
.yLoop:
	move.w  #40-1,d5
	clr.w   d0
	move.w  d7,d1
	bsr     VDPScreenAddrB
.xLoop:
	WRITE_VDP_DATA d4		;move.w  d4,VDP_DATA
	addq.w  #1,d4
	dbra    d5,.xLoop
	addq.w  #1,d7
	dbra    d6,.yLoop
.forever:	bra	.forever

;==============================================================================

	xref	cursorX,cursorY
	xref	InitText
	xref	SetPalette
	xref	_TextSetLeftMargin
	xref	textAttr
	xref	VDPTable
	xref	InitVDP

;------------------------------------------------------------------------------

_DebugTextInit:
;	lea	VDPTable,a0
;	jsr	InitVDP

	jsr	ClearFieldA
	clr.w   d0
	moveq	#FIELD_A,d1
	jsr     SetHScroll
	clr.w   d0
	moveq	#FIELD_A,d1
	jsr     SetVScroll

	lea	.pal1,a0			; background color
	clr.w	d0
	moveq	#1,d1
	jsr	SetPalette
	lea	.pal2,a0			; text color
	move.w	#8,d0
	moveq	#1,d1
	bsr	SetPalette

	move.w	#$d400,d0			; hide charset
	bsr	InitText
	move.w	#CHARF_PRI,textAttr

	move.l	#4,-(sp)
	jsr	_TextSetLeftMargin
	addq	#4,sp
	move.w	#4,cursorX
	move.w	#4,cursorY
	rts

;------------------------------------------------------------------------------

.pal1:
	dc.w	0
.pal2:
	dc.w	$fff

;==============================================================================

EXCEPT_BUSERR	EQU	2
EXCEPT_ADDR	EQU	3
EXCEPT_ILLEGAL	EQU	4
EXCEPT_DIVERR	EQU	5
EXCEPT_CHK	EQU	6
EXCEPT_TRAPV	EQU	7
EXCEPT_PRIV	EQU	8
EXCEPT_TRACE	EQU	9
EXCEPT_LINEA	EQU	$A
EXCEPT_LINEF	EQU	$B
EXCEPT_OTHER	EQU	$C		; and others
EXCEPT_IRQ1	EQU	$11
EXCEPT_IRQ2	EQU	$12
EXCEPT_IRQ3	EQU	$13
EXCEPT_IRQ4	EQU	$14
EXCEPT_IRQ5	EQU	$15
EXCEPT_IRQ6	EQU	$16
EXCEPT_NMI	EQU	$17

;------------------------------------------------------------------------------

Buserr
	move.w	8(sp),rgSR+2		; save status register
	move.l	10(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_BUSERR,exType
	bra	Exception
Adderr
	move.w	8(sp),rgSR+2		; save status register
	move.l	10(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_ADDR,exType
	bra	Exception
Illegal
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_ILLEGAL,exType
	bra	Exception
Diverr
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_DIVERR,exType
	bra	Exception
Trace
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_TRACE,exType
	bra	Exception
LineA
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_LINEA,exType
	bra	Exception
LineF
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_LINEF,exType
	bra	Exception
Blank
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_OTHER,exType
	bra	Exception
IRQ01
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_IRQ1,exType
	bra	Exception
IRQ02
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_IRQ2,exType
	bra	Exception
IRQ03
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_IRQ3,exType
	bra	Exception
IRQ04
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_IRQ4,exType
	bra	Exception
IRQ05
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_IRQ5,exType
	bra	Exception
IRQ06
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_IRQ6,exType
	bra	Exception
NMI
	move.w	(sp),rgSR+2		; save status register
	move.l	2(sp),rgPC
	clr.w	rgSR
	move.w	#EXCEPT_NMI,exType
	bra	Exception

Exception:
	movem.l	d0-d7/a0-a7,regs		; store registers
	move.l	usp,a0
	move.l	a0,rgUSP

	jsr	_DebugTextInit
	move.l	crashCallbackPointer,a0
	move.l	a0,d0				; test for null ptr.
	beq	.1
	jsr	(a0)
.1:

	lea	.crashText,a0
	bsr	PrintText

	lea	.exceptTextArray,a0
	move.w	exType,d0
	add.w	d0,d0
	add.w	d0,d0
	move.l	(a0,d0.w),a0
	bsr	PrintText

	bsr	PrintRegs
	bra	CrashWait

;------------------------------------------------------------------------------

.crashText
	dc.b	'Exception',10,10,0

.exceptTextArray
	dc.l	.OTHERText		; never  happens
	dc.l	.OTHERText              ; never  happens
	dc.l	.BUSERRText
	dc.l	.ADDRText
	dc.l	.ILLEGALText
	dc.l	.DIVERRText
	dc.l	.CHKText
	dc.l	.TRAPVText
	dc.l	.PRIVText
	dc.l	.TRACEText
	dc.l	.LINEAText
	dc.l	.LINEFText
	dc.l	.OTHERText
	dc.l	.OTHERText
	dc.l	.OTHERText
	dc.l	.OTHERText
	dc.l	.OTHERText
	dc.l	.IRQ1Text
	dc.l	.IRQ2Text
	dc.l	.IRQ3Text
	dc.l	.IRQ4Text
	dc.l	.IRQ5Text
	dc.l	.IRQ6Text
	dc.l	.NMIText

.BUSERRText
	dc.b	'Bus Error ',0
.ADDRText
	dc.b	'Address Error ',0
.ILLEGALText
	dc.b	'Illegal Instruction',0
.DIVERRText
	dc.b	'Divide By Zero',0
.CHKText
	dc.b	'CHK out of bounds',0
.TRAPVText
	dc.b	'TrapV',0
.PRIVText
	dc.b	'Privilege Violation',0
.TRACEText
	dc.b	'Trace bit set',0
.LINEAText
	dc.b	'Line A emulation',0
.LINEFText
	dc.b	'Line F emulation',0
.OTHERText
	dc.b	'Unknown exception',0
.IRQ1Text
	dc.b	'IRQ1 occured',0
.IRQ2Text
	dc.b	'IRQ2 occured',0
.IRQ3Text
	dc.b	'IRQ3 occured',0
.IRQ4Text
	dc.b	'IRQ4 occured',0
.IRQ5Text
	dc.b	'IRQ5 occured',0
.IRQ6Text
	dc.b	'IRQ6 occured',0
.NMIText
	dc.b	'NMI occured',0

	ds.w	1	;even

;==============================================================================
; assembly version of crash
;	a0-> text string to print
;------------------------------------------------------------------------------

	xref	whitePalette
	xref	ReadJoy1
	xref	_ButtonRelease
	xref	_ColorCycle
	xdef	_CrashCallback			; call this function, please...

_CrashCallback:
	move.l	4(sp),crashCallbackPointer
	rts

	xdef	_Crash
;	xdef	Crash
_Crash:
	IFNE SEGA_CD
	moveq	#0,d0
	move.w	#64,d1
	lea	whitePalette,a0
	jsr	SetPalette
.forever:	bra	.forever
	ENDC

	move.w	sr,rgSR+2			; put in the low word
	clr.w	rgSR 			; so you can printLong
	movem.l	d0-d7/a0-a7,regs		; store registers
	move.l	usp,a0
	move.l	a0,rgUSP

	move.l	4(sp),a0
;Crash:
	move.l	a0,-(sp)
	jsr	_DebugTextInit
	move.l	crashCallbackPointer,a0
	move.l	a0,d0				; test for null ptr.
	beq	.1
	jsr	(a0)
.1:

	lea	.crashText,a0
	bsr	PrintText
	move.l	4(sp),rgPC  		; read final pc
	bsr	PrintRegs

	move.l	#0,-(sp)
	jsr	_TextSetLeftMargin
	addq	#4,sp

	move.l	(sp)+,a0
	bsr	PrintText
CrashWait:
.Loop:
	jsr	ReadJoy1
	tst.b	d0
	beq.s	.Loop

	btst	#JOYB_START,d0
	bne.s	.Recover
	btst	#JOYB_C,d0
	bne.s	.Clear

	move.l	#8,-(sp)
	jsr	_ColorCycle
	add.l	#4,sp
	WAITVB
	bra.s	.Loop

.Clear:
	jsr	ClearFieldB
	jsr     ClearSpriteTable
	move.w  #VDP_SPRITEBASE,d0
	jsr     CopySprites
	WAITVB
	bra.s	.Loop

.Recover:
	jsr	_ButtonRelease
	movem.l	d0-d7/a0-a7,regs		; store registers
	rts

.crashText
	dc.b	'User Exception:',0

;==============================================================================
; note: registers must be placed into the register storage area for this to
; function
;------------------------------------------------------------------------------

PrintRegs:
	move.w	#4,cursorX
	move.w	#.rgY,cursorY

	lea	.regText,a0
	bsr	PrintText			; print grid to print into

	lea	regs,a6
	lea	.regTable,a5
	move.w	#19-1,d7
.Loop:
	move.w	(a5)+,cursorX			; read position to print
	move.w	(a5)+,cursorY			; next register
	move.l	(a6)+,d0			; read next register
	bsr	PrintHex32			; actually print it
	dbra	d7,.Loop
	move.w	(a5)+,cursorX           	; set cursor for printing
	move.w	(a5)+,cursorY			; custom message
	rts

;------------------------------------------------------------------------------

.rgX	equ	9
.rgX2	equ	24
.rgY	equ	8

.regTable:
	dc.w	.rgX,.rgY+0		; d0
	dc.w	.rgX,.rgY+1		; d1
	dc.w	.rgX,.rgY+2		; d2
	dc.w	.rgX,.rgY+3		; d3
	dc.w	.rgX,.rgY+4		; d4
	dc.w	.rgX,.rgY+5		; d5
	dc.w	.rgX,.rgY+6		; d6
	dc.w	.rgX,.rgY+7		; d7

	dc.w	.rgX2,.rgY+0		; a0
	dc.w	.rgX2,.rgY+1		; a1
	dc.w	.rgX2,.rgY+2		; a2
	dc.w	.rgX2,.rgY+3		; a3
	dc.w	.rgX2,.rgY+4		; a4
	dc.w	.rgX2,.rgY+5		; a5
	dc.w	.rgX2,.rgY+6		; a6
	dc.w	.rgX2,.rgY+7		; a7
	dc.w	.rgX,.rgY+8		; pc
	dc.w	.rgX2,.rgY+8		; usp
	dc.w	.rgX,.rgY+9		; sr

	dc.w	4,.rgY+11		; user text

;------------------------------------------------------------------------------

.regText
	dc.b	'd0 =           a0 = ',10
	dc.b	'd1 =           a1 = ',10
	dc.b	'd2 =           a2 = ',10
	dc.b	'd3 =           a3 = ',10
	dc.b	'd4 =           a4 = ',10
	dc.b	'd5 =           a5 = ',10
	dc.b	'd6 =           a6 = ',10
	dc.b	'd7 =          ssp = ',10
	dc.b    'pc =          usp = ',10
	dc.b	'sr =',10,10,0

;==============================================================================

	BSECTION	.bss

;	even
	ds.w	1	;even

exType	ds.w	1
regs	ds.l	16
rgPC	ds.l	1
rgUSP	ds.l	1
rgSR	ds.l	1
crashCallbackPointer	ds.l	1
_LastObject	ds.l	1

;==============================================================================

	end

;==============================================================================
