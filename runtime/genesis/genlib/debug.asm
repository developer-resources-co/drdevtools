;==============================================================================
;===	debug.asm: Genesis debugging code				    ===
;===	(c) 1991,92 Developer Resources					    ===
;==============================================================================
;	Purpose: provide developer with some simple debugging tools
;
;	PrintRegs: dumps all registers on the screen, must be called as a
;	sub-routine to print pc properly.
;	(mostly useful to attach to unused vectors when game is being
;	playtested from a cart)
;
;	VDPView: very handy for checking vdp character useage, shows first
;		half of the vdp character memory on the screen.
;	(usually never call this from a game, just from the monitor)
;
;==============================================================================

code	segment

;==============================================================================
; print all registers on the screen

PrintRegs:
	movem.l	d0-d7/a0-a6,-(sp)		; preserve all registers

		; kts bug: 64(sp) doesn't work

	move.l	16*4(a7),-(sp)			; push return address onto stack
	movem.l	d0-d7/a0-a7,-(sp)
	clr.w	cursorY
	move.w	#(16+1)-1,d7
	lea	textStrings,a0
.Next:
	move.w	#20,cursorX
	addq.w	#1,cursorY
	jsr	PrintText
	move.l	(sp)+,d0
	jsr	PrintHex32
	dbra	d7,.Next
	movem.l	(sp)+,d0-d7/a0-a6
	rts

;------------------------------------------------------------------------------

textStrings:
	dt	'd0: '
	dc.b	0
	dt	'd1: '
	dc.b	0
	dt	'd2: '
	dc.b	0
	dt	'd3: '
	dc.b	0
	dt	'd4: '
	dc.b	0
	dt	'd5: '
	dc.b	0
	dt	'd6: '
	dc.b	0
	dt	'd7: '
	dc.b	0

	dt	'a0: '
	dc.b	0
	dt	'a1: '
	dc.b	0
	dt	'a2: '
	dc.b	0
	dt	'a3: '
	dc.b	0
	dt	'a4: '
	dc.b	0
	dt	'a5: '
	dc.b	0
	dt	'a6: '
	dc.b	0
	dt	'a7: '
	dc.b	0
	dt	'pc: '
	dc.b	0

	even

;============================================================================
; destroys:
;	most data registers, but who cares, this is a debugging routine

VdpView:
	clr.w	d4
	push.w	d4
	jsr	ClearFieldA
	jsr	ClearFieldB
	jsr	ClearWindow

	clr.w	d0
	jsr	SetHScrollA

	clr.w	d0
	jsr	SetHScrollB

	jsr	ClearSpriteTable
	move.w	#VDP_SPRITEBASE,d0
	jsr	CopySprites

	clr.w	d7
	pop.w	d4
	move.w	#28-1,d6
.yLoop:
	move.w	#40-1,d5
	clr.w	d0
	move.w	d7,d1
	bsr	VDPScreenAddrB
.xLoop:
	move.w	d4,VDP_DATA
	addq.w	#1,d4
	dbra	d5,.xLoop
	addq.w	#1,d7
	dbra	d6,.yLoop
	illegal					; since usually run from the monitor
	rts

;==============================================================================

	end

;==============================================================================
