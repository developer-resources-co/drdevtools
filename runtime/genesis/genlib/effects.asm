;==============================================================================
;=== effects.asm: video effects code					    ===
;=== (c)1991,92 Developer Resources					    ===
;==============================================================================
;	Purpose: provide routines to do special effects on the screen.
;
;	CrossFade:
;		Transition between two palettes, fully automated.
;	SetCrossFade:
;		Manual version of CrossFade, can be called repeatedly
;			while game does other this(like animations)
;			(recomend copying CrossFade and modifying)
;------------------------------------------------------------------------------


;==============================================================================
;	Inputs:
;		d0 = starting color #
;		d1 = # of colors to cross fade
;		d2 = # of frames to wait between steps-1
;		a0-> source colors
;		a1-> dest colors
;		a3-> code to run each frame
;	Destroys:
;		d0.w-d5.w,d7.w,a2.l
;------------------------------------------------------------------------------

CrossFadeCallBack:
	move.w	#17-1,d7
	move.w	d0,d6
	clr.w	d0
.Loop:
	lea	crossFadeBuffer,a2
	movem.l	d0-d2/d6/a0/a1,-(sp)
	bsr	SetCrossFade
	movem.l	(sp)+,d0-d2/d6/a0/a1
	move.w	d2,d3
.WaitLoop:
	jsr	(a3)
;	WAITVB
	dbra	d3,.WaitLoop

	movem.l	d0-d1/a0/a1,-(sp)
	lea	crossFadeBuffer,a0
	move.w	d6,d0
	bsr	SetPalette
	movem.l	(sp)+,d0-d1/a0/a1
	addq.w	#1,d0
	dbra	d7,.Loop
	rts


;==============================================================================
;	Inputs:
;		d0 = starting color #
;		d1 = # of colors to cross fade
;		d2 = # of frames to wait between steps-1
;		a0-> source colors
;		a1-> dest colors
;	Destroys:
;		d0.w-d5.w,d7.w,a2.l
;------------------------------------------------------------------------------

CrossFade:
	move.w	#17-1,d7
	move.w	d0,d6
	clr.w	d0
PartialCrossFade:
;		d0 = starting step
;		d6 = starting color #
;		d7 = # of steps to execute
.Loop:
	lea	crossFadeBuffer,a2
	movem.l	d0-d2/d6/a0/a1,-(sp)
	bsr.s	SetCrossFade
	movem.l	(sp)+,d0-d2/d6/a0/a1
	move.w	d2,d3
.WaitLoop:
	WAITVB
	dbra	d3,.WaitLoop

	movem.l	d0-d1/a0/a1,-(sp)
	lea	crossFadeBuffer,a0
	move.w	d6,d0
	bsr	SetPalette
	movem.l	(sp)+,d0-d1/a0/a1
	addq.w	#1,d0
	dbra	d7,.Loop
	rts

;==============================================================================
;	Inputs:
;		d0 = current cross fade value(0 = all source, $f = all destination)
;		d1 = # of colors to cross fade
;		a0-> source colors
;		a1-> dest colors
;		a2-> buffer to place result
;	Destroys:
;		d0.w-d5.w,a0.l-a2.l
;------------------------------------------------------------------------------

SetCrossFade:
	move.w	#$10,d2
	sub.w	d0,d2 			; create inverse

	bra.s	.Entry
.Loop:
	move.w	(a0)+,d4
	move.w	(a1)+,d5

				; first do source
					; first do blue
	move.w	d4,d3
	and.w	#$00f,d3
	mulu	d2,d3
	lsr.w	#4,d3
	move.w	d4,d6			; now do green
	lsr.w	#4,d6
	and.w	#$f,d6
	mulu	d2,d6
	and.w	#$f0,d6
	or.w	d6,d3
	lsr.w	#8,d4			; now do blue
	and.w	#$f,d4
	mulu	d2,d4
	and.w	#$f0,d4
	lsl.w	#4,d4
	or.w	d3,d4			; d4 now contains new value

				; now do destination
					; first do blue
	move.w	d5,d3
	and.w	#$00f,d3
	mulu	d0,d3
	lsr.w	#4,d3
	move.w	d5,d6			; now do green
	lsr.w	#4,d6
	and.w	#$f,d6
	mulu	d0,d6
	and.w	#$f0,d6
	or.w	d6,d3
	lsr.w	#8,d5			; now do blue
	and.w	#$f,d5
	mulu	d0,d5
	and.w	#$f0,d5
	lsl.w	#4,d5
	or.w	d3,d5			; d3 now contains new value

	add.w	d5,d4		; merge source and destination
	move.w	d4,(a2)+	; and finaly write it out
.Entry:
	dbra	d1,.Loop
	rts

;==============================================================================
; handy for fadeing to or from black

blackPalette:
	dc.w	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.w	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.w	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.w	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

;==============================================================================

ram	segment

	even
crossFadeBuffer
	ds.b	64*4			; temporary buffer for storing cross-fade results

;==============================================================================

	end

;==============================================================================
