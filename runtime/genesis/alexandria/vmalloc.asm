;==============================================================================
; vmalloc.asm: VDP memory allocation routines
; By Scott L. Statton
;
;==============================================================================

	opt	llbl
	include object.equ

;==============================================================================

	xdef	_vmalloc
	xref	_wBlock
	xref	_Crash
;	xref	_wObjID
	xref	_LastObject
;	xref	_wBlockBase             ; base index into array
;	xref	_wBlockEnd              ; end address of array
; _vmalloc:  Allocated VDP memory.  Converted to assembly, by Scott 03-08-93
; C callable.
; TJ 05-02-93 made necessary changes since wBlock is dynamically
;   allocated

;==============================================================================
;	Note:
;		I use many word operations on addresses, knowing that
;		Genesis RAM is only 64K.  If there is ever a version
;		that has more than 64K of work RAM, these will need
;		to be changed.

_vmalloc:
	move.w	6(a7),d1		; how many chars to allocate
	beq.s	.Crash			; error condition.

	lea	_wBlock,a1
	moveq	#0,d0

.CheckBlock:
	move.w	(a1),d0			; look at a character
	beq.s	.Avail
	cmp.w	#$ffff,d0		; consumed space?
	beq	.Consume
	add.w	d0,a1			; nonzero -- move ahead.
	add.w	d0,a1
	cmp.l	#_wBlock+$1000,a1
	blo.s	.CheckBlock
	bra.s	.Fail

.Avail:			; here, we have a block of unknown size of data.
	lea	(a1),a0			; temp ptr.
	move.w	d1,d0			; loop counter

	subq	#1,d0			; zero length disallowed

.Loop
	tst.w	(a0)+			; wish I had an 010
	dbne	d0,.Loop

	beq	.GotIt
;
; block wasn't large enough
;
	subq.l	#2,a0			 ; back up
	move.l	a0,a1
	cmp.l	#_wBlock+$1000,a1
	blo.s	.CheckBlock
	bra.s	.Fail

.GotIt:					; found enough space for the block!
	move.l	a1,d0			; going to compute charnum
	sub.l	#_wBlock,d0		; this is really stupid
	lsr.l	#1,d0
	move.w	d1,(a1)+		; mark the block

	subq.w	#1,d1			; first word is already done
	bra.s	.1			; fill remaining words
.Loop2:
	move.w	#$ffff,(a1)+
.1:
	dbra	d1,.Loop2

; D0 contains char # of first block

	move.w	d0,d1
	add.w	d1,d1			; word offset
;	lea	_wObjID,a0
;	movea.l	_LastObject,a1
;	move	TObject_nType(a1),0(a0,d1.w)

	bra.s	.Exit			; happy exit!

.Crash:
	pea	.vMallocFuckedText
	jsr	_Crash

.Consume:
.Fail:
	moveq	#0,d0
.Exit
	rts

;-----------------------------------------------------------------------------

.vMallocFuckedText:
	dc.b	'vmalloc() input parameter',10
	dc.b	'    fails validation',10
	dc.b	'    d1.w = # of chars asked for',0
	ds.w	1

;==============================================================================
