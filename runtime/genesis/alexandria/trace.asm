;==============================================================================
; Code to assist in debugging.  Link this file to use exception tracing
;==============================================================================


	xdef	_TraceOn
	xdef	_TraceOff
	xdef	Trace

	xref	_Trace

_TraceOn:
	move.w	sr,-(a7)
	ori.w	#$8000,(a7)
	rte

_TraceOff:
	move.w	sr,-(a7)
	andi.w	#$7fff,(a7)
	rte


Trace:
	move.w	#0,-(sp)		; extend SR to a long
	movem.l	d0-d2/a0-a1,-(sp)
	jsr	_Trace
	movem.l	(sp)+,d0-d2/a0-a1
	addq	#2,sp
	rte



