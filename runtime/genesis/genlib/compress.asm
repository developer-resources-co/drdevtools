;==============================================================================
;===	compress.asm: Genesis decompression routines			    ===
;===	(c)1992 Developer Resources					    ===
;==============================================================================
;	Purpose:
;		Decompress data from ROM to RAM or VDP.
;------------------------------------------------------------------------------

comprStruct	defs	ram
comprStruct	segment
	org	0
comprLen	ds.w	1
comprType	ds.b	1
comprTypeData	ds.b	1			; type specific data
compr_Size

;------------------------------------------------------------------------------

code	segment

;==============================================================================
;	Inputs:
;		a0->	compressed data(input)
;		a1->	buffer to decompress into(output)
;------------------------------------------------------------------------------

DecompressToVDP:
	add.w	#compr_Size,a0
	bsr.s	DecompressRLEToVDP
	rts

;==============================================================================
;	Inputs:
;		a0->	compressed data(input)
;		a1->	buffer to decompress into(output)
;------------------------------------------------------------------------------

Decompress:
	add.w	#compr_Size,a0
	bsr.s	DecompressRLE
	rts

;==============================================================================
;	Inputs:
;		a0->	rle data(input)
;		a1->	buffer to decompress into(output)
;	Internal
;		d0 = cmd byte
;------------------------------------------------------------------------------

DecompressRLE:
.Loop:
	clr.w	d0
	move.b	(a0)+,d0			; read cmd byte
	beq.s	.Done
	bclr	#7,d0
	bne.s	.Run
	bra.s	.Unique
.ULoop:
	move.b	(a0)+,(a1)+
.Unique
	dbra	d0,.ULoop
	bra.s	.Loop
.RLoop:
	move.b	(a0),(a1)+
.Run:
	dbra	d0,.RLoop
	addq.l	#1,a0
	bra.s	.Loop
.Done:
	rts

;==============================================================================
;	Inputs:
;		a0->	rle data(input)
;		a1->	buffer to decompress into(output)
;		d0 = modulo used to compress data
;	Internal
;		d0 = cmd byte
;		(this routine has not yet been tested)
;------------------------------------------------------------------------------

DecompressModuloRLE:
	move.l	a1,a2
	move.w	d0,d2
	subq.w	#1,d2
.OuterLoop:

.Loop:
	clr.w	d1
	move.b	(a0)+,d1			; read cmd byte
	beq.s	.Done
	bclr	#7,d1
	bne.s	.Run
	bra.s	.Unique
.ULoop:
	move.b	(a0)+,(a1)
	add.w	d0,a1
.Unique
	dbra	d1,.ULoop
	bra.s	.Loop
.RLoop:
	move.b	(a0),(a1)
	add.w	d0,a1
.Run:
	dbra	d1,.RLoop
	addq.l	#1,a0
	bra.s	.Loop
.Done:
	addq.w	#1,a2
	move.l	a2,a1
	dbra	d2,.OuterLoop
	rts

;==============================================================================
;	Inputs:
;		a0->	rle data(input)
;		VDPDATA already pointing to correct address(output)
;	Internal
;		d0 = cmd byte
;------------------------------------------------------------------------------

DecompressRLEToVDP:
					; setup
	lea	VDP_DATA,a1
	lea	libtemp,a2
	lea	1(a2),a3
.Loop:
					; get next command
	clr.w	d0
	move.b	(a0)+,d0			; read cmd byte
	beq.s	.Done
	bclr	#7,d0
	beq.s	.Unique
	move.b	(a0)+,d1			; pre-read the byte to repeat
	bra.s	.Run

.LoopPhase:
					; get next command
	clr.w	d0
	move.b	(a0)+,d0			; read cmd byte
	beq.s	.Done
	bclr	#7,d0
	beq.s	.UEn2
	move.b	(a0)+,d1			; pre-read the byte to repeat
	bra.s	.Run2


;------------------------------------------------------------------------------

					; unique run
.ULoop:
	move.b	(a0)+,(a2)
.UEn2:
	dbra	d0,.WriteIt
	bra.s	.LoopPhase

.WriteIt:
	move.b	(a0)+,(a3)
	move.w	(a2),(a1)			; write it to VDP
.Unique
	dbra	d0,.ULoop
	bra.s	.Loop

;------------------------------------------------------------------------------
;					; repeated run
.RLoop:
	move.b	d1,(a2)
.Run2
	dbra	d0,.RLoop2
	bra.s	.LoopPhase

.RLoop2:
	move.b	d1,(a3)
	move.w	(a2),(a1)			; write it to VDP
.Run:
	dbra	d0,.RLoop
	bra.s	.Loop
.Done:
	rts

;==============================================================================

ram	segment

;==============================================================================

	END

;==============================================================================
