;==============================================================================
; cd.asm
;==============================================================================

	include cd.mac
	include cd.inc

	XDEF	_SetBank
	XDEF	SetBank

	TSECTION        .text

_SetBank:
	move.b	7(sp),d0

SetBank:
; assert 0--3
	movem.w	d1,-(sp)
	move.b	$A12003,d1
	and.b	#%00111111,d1	; strip off bank bits
	lsl.b	#6,d0
	or.b	d0,d1
	move.b	d1,$A12003
	movem.w	(sp)+,d1

	rts


	xdef	_TestMainCpu
_TestMainCpu:
    move.w   #CMD_ColorCycle,SUB_1M_BASE      ;request service 01 - color cycle
    WAIT_FOR_1M_SWAPREQ                       ;wait for a request to swap
    SWAP_1M                                   ;do it!
.forever:	bra	.forever


	END

;==============================================================================
