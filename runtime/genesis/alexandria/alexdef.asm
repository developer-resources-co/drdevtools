;=====================================================
;
; Fixed point routines
;
; (c) 1992, Scott Statton / Alexandria, Inc.
;
; This library uses a 16.16 format.
;-----------------------------------------------------
;

    opt llbl
    xdef	_fpadd
    xdef	_fpsub
    xdef	_fpmul
    xdef	_fpabs
    xdef	_fpsgn

_fpadd:
    move.l	4(sp),d0
    add.l	8(sp),d0
    rts

_fpsub:
    move.l	4(sp),d0
    sub.l	8(sp),d0
    rts

_fpmul:
    link	a6,#-8
    move.l	d3,-4(a6)
    move.l	8(a6),d0
    move.l	12(a6),d1
    move.l	d0,d2
    eor.l	d1,d2
    btst.l	#31,d2
    sne		-5(a6)
    tst.l	d0
    bpl.s	.1
    neg.l	d0
.1:
    tst.l	d1
    bpl.s	.2
    neg.l	d1
.2:
;	now, d0 and d1 contain their own absolute values
;
    move.w	d1,d3
    mulu.w	d0,d3		; first product

    move.w	d1,d2		; low d1 * hi d0
    swap	d0
    mulu.w	d0,d2
    move.l	d2,-(sp)        ; second product

    swap	d0		; d0 normal
    swap	d1		; d1 swapped
    move.w	d0,d2		; low d0 * hi d1
    mulu.w	d1,d2		; third product
    add.l	(sp)+,d2	; GOD, I love the 68K!

    clr.w	d3		; nuke the bs part
    swap	d3		; now get the high word of d3
    add.l	d3,d2           ; getting close!

    swap	d0		; d0 swapped, d1 swapped
    move.w	d0,d3
    mulu.w	d1,d3		; fourth product
    swap	d3
    clr.w	d3		; bottom part is caca
    add.l	d3,d2		; Got it!  d2 = |d0| * |d1|
    move.l	d2,d0
    tst.b	-5(a6)
    beq.s	.3
    neg.l	d0
.3:
    move.l	-4(a6),d3
    unlk	a6
    rts

_fpabs:
    move.l	4(sp),d0
    tst.l	d0
    bpl		.1
    neg.l	d0
.1:
    rts

_fpsgn:
    tst.l	4(sp)
    beq.s	.1
    bmi.s	.2
    moveq.l	#1,d0
    rts
.1:
    moveq.l	#0,d0
    rts
.2:
    moveq.l	#-1,d0
    rts


