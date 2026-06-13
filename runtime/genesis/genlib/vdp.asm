;==============================================================================
;===	vdp.asm: Genesis VDP core routines				    ===
;===	(c)1991,2 Developer Resources					    ===
;==============================================================================

code	segment

;==============================================================================
;===	Inputs: 							    ===
;===		a0->vdp table						    ===
;==============================================================================

InitVDP:				; setup vdp
				; first check to see if the VDP is busy

	move.w	VDP_CMD,d4
	btst	#VDP_STATB_DMA,d4		; Wait for DMA to Finished
	bne.s	InitVDP
				; then set all vdp registers
	move.w	#VDP_REG0,D1
	moveq	#18,D0
.Loop
	move.b	(a0)+,d1
	move.w	d1,VDP_CMD
	add.w	#$100,d1 	; advance to next register
	dbra	d0,.Loop

				; now clear vsram
	move.l	#VDP_MODE_VSW,VDP_CMD
	move.w	#($80/2)-1,d0
.Loop2
	move.w	#0,VDP_DATA
	dbra	d0,.Loop2

				; then clear vdp memory
	move.l	#VDP_MODE_VRW,D7
	move.w	#$FFFF,D6
	moveq	#00,D5
	bsr	DMAFill
	rts

;============================================================================

DMAFill:
	lea	VDP_CMD,a5		; VDP READ/WRITE
	move.w	#VDP_REG15+1,(a5)		; Set Auto Inc to 1 Byte
	move.l	#$00940000,D4		; Set # of bytes to fill
	move.w	D6,D4
	lsl.l	#8,D4
	move.w	#$9300,D4
	move.b	D6,D4
	move.l	D4,(A5)
	move.w	#$9780,(A5)             ; (23) Set VRAM Fill
	ori.l	#$40000080,D7		; Address to Fill
	move.l	D7,(A5)
	move.b	D5,VDP_DATA		; Set Fill Byte
.10	move.w	(A5),D4                 ; Wait DMA Finished
	btst	#VDP_STATB_DMA,D4
	bne.s	.10
	move.w	#VDP_REG15|2,(A5)             ; Set Auto Inc to 2 Bytes
	rts

;==============================================================================
;===	Inputs:								    ===
;===		d0.w = source in vdp					    ===
;===		d1.w = dest in vdp					    ===
;===		d2.w = length of copy					    ===
;===									    ===
;==============================================================================

VDPVDPDMATbl:
	dc.w	VDP_REG15+1
	dc.l	VDP_REG20<16|VDP_REG19
	dc.l	VDP_REG22<16|VDP_REG21
	dc.w	VDP_REG23|VDP_R23F_DMAVDPVDP
	dc.l	VDP_MODE_VRR|VDP_DMA_START|$40 		; set dma & vram write
	dc.l	%00111111111111110000000011000011	; kill unneeded bits

;------------------------------------------------------------------------------

DMAVDPVDP:
	lea	VDP_CMD,a6
	lea	VDPVDPDMATbl,a5

	move.w	(a5)+,(a6)		; Set Auto Inc to 1 Byte

					; do length
	move.l	(a5)+,d3
	move.b	d2,d3				; copy low byte of length
	swap	d3
	ror.w	#8,d2				; move hi byte down
	move.b	d2,d3				; copy hi byte
	move.l	d3,(a6)

					; set up source address
	move.l	(a5)+,d2
	move.b	d0,d2				; low byte
	swap	d2
	ror.w	#8,d0
	move.b	d0,d2				; bring in middle byte
	move.l	d2,(a6)                 	; write middle & low bytes to vdp

	move.w	(a5)+,(a6)			; write high byte(just clear it and set DMAVDPVDP)
					; set vdp destination and start the DMA
	move.w	d1,d0					; 4
	lsl.l	#2,d1					; 12
	move.w	d0,d1                                   ; 4
	swap	d1                                      ; 4
	or.l	(a5)+,d1				; 14 set dma & vram write
	and.l	(a5),d1   				; 14 kill unneeded bits
	lea	libTemp,a5
	move.l	d1,(a5)
	move.l	(a5),(a6)		   		; 12

.Wait	move.w	(A6),D4                 ; Wait for DMA to Finished
	btst	#VDP_STATB_DMA,D4
	bne.s	.Wait

	move.w	#VDP_REG15|2,(A5)             ; Set Auto Inc to 2 Bytes
	rts

;==============================================================================
;===	DMACopy:  New improved version.                                     ===
;===	Inputs:                                                             ===
;===		d0.w = VDP destination address                              ===
;===		d1.w = VDP length (words)                                   ===
;===		a0   -> data                                                ===
;===		assumes	auto-increment is set to 2			    ===
;===	Outputs:                                                            ===
;===		none                                                        ===
;===	Destroys:                                                           ===
;===		d0,d1 temp regs                                             ===
;===		d2 - buffer to build long for VDP write                     ===
;===		a5 - pointer to offset table                                ===
;===		a6 - VDP pointer                                            ===
;===	Warning: DMA's on the Sega Genesis cannot cross a 64K boundry!      ===
;===                                                                        ===
;==============================================================================

VDPDMATbl:
	dc.l	VDP_REG20<16|VDP_REG19
	dc.l	VDP_REG22<16|VDP_REG21
	dc.w	VDP_REG23
	dc.l	VDP_MODE_VRW|VDP_DMA_START		; set dma & vram write
	dc.l	%01111111111111110000000010000011	; kill unneeded bits

DMACopy:
	lea	VDP_CMD,a6
	lea	VDPDMATbl,a5
					; do length
	move.l	(a5)+,d2
	move.b	d1,d2				; copy low byte of length
	swap	d2
	ror.w	#8,d1				; move hi byte down
	move.b	d1,d2				; copy hi byte
	move.l	d2,(a6)

					; set up source address
	move.l	a0,d1				; d1 is free now

	lsr.l	#1,d1				; vdp doesn't want low bit

	move.l	(a5)+,d2
	move.b	d1,d2				; low byte
	swap	d2
	ror.w	#8,d1
	move.b	d1,d2				; bring in middle byte
	move.l	d2,(a6)                 	; write middle & low bytes to vdp

	swap	d1
	move.w	(a5)+,d2
	move.b	d1,d2				; implied or with VDP_R23F_DMACOPY, which is zero
	move.w	d2,(a6)				; write hi byte

					; set vdp destination and start the DMA
	move.w	d0,d1					; 4
	lsl.l	#2,d1					; 12
	move.w	d0,d1                                   ; 4
	swap	d1                                      ; 4
	or.l	(a5)+,d1				; 14 set dma & vram write
	and.l	(a5),d1   				; 14 kill unneeded bits
	lea	libTemp,a5
	move.l	d1,(a5)
	move.l	(a5),(a6)		   		; 12
	rts

;==============================================================================
;===	FakeDMACopy:  New improved version.                                 ===
;===	Inputs:                                                             ===
;===		d0.w = VDP destination address                              ===
;===		d1.w = VDP length (words)                                   ===
;===		a0   -> data                                                ===
;===		a6   -> buffer to write vdp cmds to                         ===
;===		assumes	auto-increment is set to 2			    ===
;===	Outputs:                                                            ===
;===		none                                                        ===
;===	Destroys:                                                           ===
;===		d0.w,d1.l,d2.l,a5,l,a6.l                                    ===
;===	Internal		                                            ===
;===		d2 - buffer to build long for VDP write                     ===
;===		a5 - pointer to offset table                                ===
;===		a6 - VDP pointer                                            ===
;===                                                                        ===
;==============================================================================

FakeDMACopy:
;	lea	VDP_CMD,a6
	lea	VDPDMATbl,a5
					; do length
	move.l	(a5)+,d2
	move.b	d1,d2				; copy low byte of length
	swap	d2
	ror.w	#8,d1				; move hi byte down
	move.b	d1,d2				; copy hi byte
	move.l	d2,(a6)+

					; set up source address
	move.l	a0,d1				; d1 is free now

	lsr.l	#1,d1				; vdp doesn't want low bit

	move.l	(a5)+,d2
	move.b	d1,d2				; low byte
	swap	d2
	ror.w	#8,d1
	move.b	d1,d2				; bring in middle byte
	move.l	d2,(a6)+                 	; write middle & low bytes to vdp

	swap	d1
	move.w	(a5)+,d2
	move.b	d1,d2				; implied or with VDP_R23F_DMACOPY, which is zero
	move.w	d2,(a6)+				; write hi byte

					; set vdp destination and start the DMA
	move.w	d0,d1					; 4
	lsl.l	#2,d1					; 12
	move.w	d0,d1                                   ; 4
	swap	d1                                      ; 4
	or.l	(a5)+,d1				; 14 set dma & vram write
	and.l	(a5),d1   				; 14 kill unneeded bits
	move.l	d1,(a6)+		   		; 12
	rts

;==============================================================================
;	Inputs:
;		a0->uninitialized field struct
;		d0.w = which field this will use(FIELD_A or FIELD_B)
;------------------------------------------------------------------------------

InitFieldStruct:
	move.w	d0,Field_Num(a0)
	move.w	#VDP_FIELDB_WIDTH,Field_Width(a0)
	move.w	#VDP_FIELDB_HEIGHT,Field_Height(a0)
	move.w	#VDP_SCREEN_B,Field_Base(a0)
	tst.w	d0
	bne.s	.FB
	move.w	#VDP_FIELDA_WIDTH,Field_Width(a0)
	move.w	#VDP_FIELDA_HEIGHT,Field_Height(a0)
	move.w	#VDP_SCREEN_A,Field_Base(a0)
.FB
	rts

;============================================================================
; Inputs:
;	d0 = x pos
;	d1 = y pos
;	d2 = character offset in VDP(plus palette selection)
;	a0-> field struct(this determines which field gets plotted to)
;	a1-> map
; Internal:
;	d0 = x coord
;	d1 = y coord
;	d2 = temp, x counter
;	d3 = x size
;	d4 = y counter
;	d5 = starting char storage
;	d6 = temp char storage
; Destroys:
;	d0-d1.w,d2.l,d3-d6.w,a1.l
; Assumes map is at least 1 wide by 1 tall
; Assumes vdp increment size is set to word(2)
;------------------------------------------------------------------------------

PlotMapField:
	move.w	d2,d5                   ; stash addend since VDPScreenAddr destroys it
	move.w	(a1)+,d3		; get x size from map
	move.w	(a1)+,d4		; get y size from map
	subq.w	#1,d3			; repair for dbra
	subq.w	#1,d4			; repair for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	jsr	VDPScreenAddress	; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	d3,d2      		; get x size back for dbra counter
.XLoop:
	move.w	(a1)+,d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset(and palette & flip bits)
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map
	addq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw top to bottom for height of map
	rts

;============================================================================
; Inputs:
;	d0 = x pos
;	d1 = y pos
;	d2 = character offset in VDP(plus palette selection)
;	(a0)-> map
; Internal:
;	d0 = x coord
;	d1 = y coord
;	d2 = temp, x counter
;	d3 = x size
;	d4 = y counter
;	d5 = starting char storage
;	d6 = temp char storage
; Destroys:
;	d0-d1.w,d2.l,d3-d6.w,a0.l
; Assumes map is at least 1 wide by 1 tall
; Assumes vdp increment size is set to word(2)
;------------------------------------------------------------------------------

PlotMapWindow:
	move.w	d2,d5                   ; stash addend since VDPScreenAddr destroys it
	move.w	(a0)+,d3		; get x size from map
	move.w	(a0)+,d4		; get y size from map
	subq.w	#1,d3			; repair for dbra
	subq.w	#1,d4			; repair for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	jsr	VDPScreenAddrWin	; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	d3,d2      		; get x size back for dbra counter
.XLoop:
	move.w	(a0)+,d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset(and palette & flip bits)
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map
	addq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw top to bottom for height of map
	rts



PlotMapA:
	move.w	d2,d5                   ; stash addend since VDPScreenAddr destroys it
	move.w	(a0)+,d3		; get x size from map
	move.w	(a0)+,d4		; get y size from map
	subq.w	#1,d3			; repair for dbra
	subq.w	#1,d4			; repair for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	jsr	VDPScreenAddrA		; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	d3,d2      		; get x size back for dbra counter
.XLoop:
	move.w	(a0)+,d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset(and palette & flip bits)
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map
	addq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw top to bottom for height of map
	rts

; kts: just for fun

PlotMapXFlipA:
	or.w	#CHARF_XFLIP,d2
	move.w	d2,d5                   ; stash addend since VDPScreenAddr destroys it
	move.w	(a0)+,d3		; get x size from map
	move.w	(a0)+,d4		; get y size from map
	subq.w	#1,d4			; repair for dbra

	move.w	d3,d7
	add.w	d7,d7
	add.w	d7,a0			; skip to end of line
	add.w	d7,d7
	subq.w	#1,d3			; repair for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	jsr	VDPScreenAddrA		; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	d3,d2      		; get x size back for dbra counter
.XLoop:
	move.w	-(a0),d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset(and palette & flip bits)
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map
	add.w	d7,a0			; skip to end of line
	addq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw top to bottom for height of map
	rts



PlotMapYFlipA:
	or.w	#CHARF_YFLIP,d2		; set x flip attribute
	move.w	d2,d5
	move.w	(a0)+,d3		; get x size from map
	move.w	(a0)+,d4		; get y size from map

	add.w	d4,d1			; skip to bottom character line of graphic
	subq.w	#1,d1			; back up one since line 0 is included in count

	subq.w	#1,d3			; repair for dbra
	subq.w	#1,d4			; repair for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	jsr	VDPScreenAddrA		; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	d3,d2      		; get x size back for dbra counter
.XLoop:
	move.w	(a0)+,d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset(and palette & flp bits)
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map
	subq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw bottom to top for height of map
	rts


PlotMapXYFlipA:
	or.w	#CHARF_XFLIP|CHARF_YFLIP,d2
	move.w	d2,d5                   ; stash addend since VDPScreenAddr destroys it
	move.w	(a0)+,d3		; get x size from map
	move.w	(a0)+,d4		; get y size from map

	add.w	d4,d1			; skip to bottom character line of graphic
	subq.w	#1,d1			; back up one since line 0 is included in count

	move.w	d3,d7
	add.w	d7,d7
	add.w	d7,a0			; skip to end of line
	add.w	d7,d7

	subq.w	#1,d3			; repair for dbra
	subq.w	#1,d4			; repair for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	jsr	VDPScreenAddrA		; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	d3,d2      		; get x size back for dbra counter
.XLoop:
	move.w	-(a0),d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset(and palette & flip bits)
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map
	add.w	d7,a0			; skip to end of line
	subq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw top to bottom for height of map
	rts

;============================================================================
; Inputs:
;	d0 = x pos
;	d1 = y pos
;	d2 = character offset in VDP(plus palette selection)
;	(a0)-> map
; Internal:
;	d0 = x coord
;	d1 = y coord
;	d2 = temp, x counter
;	d3 = x size
;	d4 = y counter
;	d5 = starting char storage
;	d6 = temp char storage
; Destroys:
;	d0-d1.w,d2.l,d3-d6.w,a0.l
; Assumes map is at least 1 wide by 1 tall
; Assumes vdp increment size is set to word(2)

PlotMapB:
	move.w	d2,d5                   ; stash addend since VDPScreenAddr destroys it
	move.w	(a0)+,d3		; get x size from map
	move.w	(a0)+,d4		; get y size from map
	subq.w	#1,d3			; repair for dbra
	subq.w	#1,d4			; repair for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	jsr	VDPScreenAddrB		; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	d3,d2      		; get x size back for dbra counter
.XLoop:
	move.w	(a0)+,d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset(and palette & flip bits)
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map
	addq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw top to bottom for height of map
	rts

; kts: just for fun

PlotMapXFlipB:
       	or.w	#CHARF_XFLIP,d2
       	move.w	d2,d5         		; stash addend since VDPScreenAddr destroys it
       	move.w	(a0)+,d3		; get x size from map
       	move.w	(a0)+,d4		; get y size from map
       	subq.w	#1,d4			; repair for dbra

       	move.w	d3,d7
       	add.w	d7,d7
       	add.w	d7,a0			; skip to end of line
       	add.w	d7,d7
       	subq.w	#1,d3			; repair for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	jsr	VDPScreenAddrB		; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	d3,d2      	       	; get x size back for dbra counter
.XLoop:
	move.w	-(a0),d6	       	; read next word from map
	add.w	d5,d6		       	; add in vdp char offset(and palette & flip bits)
	move.w	d6,VDP_DATA	       	; write it to vdp
	dbra	d2,.XLoop	        ; draw left to right for width of map
	add.w	d7,a0		       	; skip to end of line
	addq.w	#1,d1		       	; move down screen one character line(increment y pos)
	dbra	d4,.YLoop	       	; draw top to bottom for height of map
	rts



PlotMapYFlipB:
	or.w	#CHARF_YFLIP,d2		; set x flip attribute
	move.w	d2,d5
	move.w	(a0)+,d3		; get x size from map
	move.w	(a0)+,d4		; get y size from map

	add.w	d4,d1			; skip to bottom character line of graphic
	subq.w	#1,d1			; back up one since line 0 is included in count

	subq.w	#1,d3			; repair for dbra
	subq.w	#1,d4			; repair for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	jsr	VDPScreenAddrB		; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	d3,d2      		; get x size back for dbra counter
.XLoop:
	move.w	(a0)+,d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset(and palette & flp bits)
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map
	subq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw bottom to top for height of map
	rts


PlotMapXYFlipB:
	or.w	#CHARF_XFLIP|CHARF_YFLIP,d2
	move.w	d2,d5                   ; stash addend since VDPScreenAddr destroys it
	move.w	(a0)+,d3		; get x size from map
	move.w	(a0)+,d4		; get y size from map

	add.w	d4,d1			; skip to bottom character line of graphic
	subq.w	#1,d1			; back up one since line 0 is included in count

	move.w	d3,d7
	add.w	d7,d7
	add.w	d7,a0			; skip to end of line
	add.w	d7,d7

	subq.w	#1,d3			; repair for dbra
	subq.w	#1,d4			; repair for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	jsr	VDPScreenAddrB		; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	d3,d2      		; get x size back for dbra counter
.XLoop:
	move.w	-(a0),d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset(and palette & flip bits)
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map
	add.w	d7,a0			; skip to end of line
	subq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw top to bottom for height of map
	rts

;============================================================================
;	Inputs:
;		d0 = x pos
;		d1 = y pos
; 	Destroys: d0.w,d1.w,d2.l
;	Function:
;		Set the vdp into write mode, pointing at screen a at coordinates
;============================================================================

VDPScreenAddrWin:
	move.l	#(VDP_MODE_VRW>16)|((VDP_WINDOWBASE<2)&$30000)|(VDP_WINDOWBASE&$3FFF),d2			; do it like this so it will be correct when swapped
	MULU	#VDP_FIELDA_WIDTH*2,D1
	ADD.W	D0,D0
	ADD.W	D1,D0		; find screen location

	OR.W	D0,D2
	SWAP	D2
	MOVE.L	D2,VDP_CMD
	rts

VDPScreenAddrA:
	move.l	#(VDP_MODE_VRW>16)|((VDP_SCREEN_A<2)&$30000)|(VDP_SCREEN_A&$3FFF),d2			; do it like this so it will be correct when swapped
	MULU	#VDP_FIELDA_WIDTH*2,D1
	ADD.W	D0,D0
	ADD.W	D1,D0		; find screen location

	OR.W	D0,D2
	SWAP	D2
	MOVE.L	D2,VDP_CMD
	rts

VDPScreenAddrB:
	move.l	#(VDP_MODE_VRW>16)|((VDP_SCREEN_B<2)&$30000)|(VDP_SCREEN_B&$3FFF),d2			; do it like this so it will be correct when swapped
	MULU	#VDP_FIELDB_WIDTH*2,D1
	ADD.W	D0,D0
	ADD.W	D1,D0		; find screen location

	OR.W	D0,D2
	SWAP	D2
	MOVE.L	D2,VDP_CMD
	rts

;============================================================================
;	Inputs:
;		d0 = x pos
;		d1 = y pos
;		a0-> field struct
; 	Destroys: d0.w,d1.w,d2.l
;	Function:
;		Set the vdp into write mode, pointing at screen a at coordinates
;============================================================================

VDPScreenAddress:
	move.w	Field_Width(a0),d2
	add.w	d2,d2
	MULU	d2,D1
	ADD.W	D0,D0
	ADD.W	D1,D0		; find screen location
	add.w	Field_Base(a0),d0
				; fall through

;============================================================================
; NOTE: Kevin, make a macro version
;	VDPAddress: sets vdp into vram write mode at d0
;	Inputs:
;		d0 = 16-bit vdp address
;	Destroys:
;		d0.l,d1.l
;------------------------------------------------------------------------------

VDPAddress:
	move.w	d0,d1					; 4
	lsl.l	#2,d1					; 12
	move.w	d0,d1                                   ; 4
	swap	d1                                      ; 4
	bset	#30,d1                                  ; 12
	and.l	#%01111111111111110000000000000011,d1   ; 16
	move.l	d1,VDP_CMD				; 16
	rts

;==============================================================================
;===	SetColor: set a single vdp color
;===	Inputs:
;===		d0.w = color # to set(0-63)
;===		d1.w = new color value
;==============================================================================

SetColor:
	add.w	d0,d0
	swap	d0
	clr.w	d0
	or.l	#VDP_MODE_CRW,d0
	move.l	d0,VDP_CMD
	move.w	d1,VDP_DATA
	rts

;============================================================================
; SetPalette: download color palette into vdp
;	Inputs:
;		d0 = color # to start at(0-63)
;		d1 = # of entries to set
;		a0-> palette
;	Destroys:
;		d0.l,d1.w,a0.l
;------------------------------------------------------------------------------

SetPalette:
	add.w	d0,d0
	swap	d0
	clr.w	d0
	or.l	#VDP_MODE_CRW,d0
	move.l	d0,VDP_CMD
	bra.s	.Entry
.Loop
	move.w	(a0)+,VDP_DATA
.Entry:
	dbra	d1,.Loop
	rts

;============================================================================
; KTS check, would a dma fill be faster?

ClearFieldA:
	move.w	#VDP_SCREEN_A,d0
	bsr	VDPAddress
	move.w	#(VDP_FIELDA_WIDTH*VDP_FIELDA_HEIGHT)-1,d0			; note, make this work with
						; any screen size
	clr.w	d1
.CFLoop:
	move.w	d1,VDP_DATA
	dbra	d0,.CFLoop
	rts

ClearWindow:
	move.w	#VDP_WINDOWBASE,d0
	bsr	VDPAddress
	move.w	#(VDP_WINDOW_WIDTH*VDP_WINDOW_HEIGHT)-1,d0			; note, make this work with
						; any screen size
	clr.w	d1
.CFLoop:
	move.w	d1,VDP_DATA
	dbra	d0,.CFLoop
	rts

;============================================================================

ClearFieldB:
	move.w	#VDP_SCREEN_B,d0
	bsr	VDPAddress
	move.w	#(VDP_FIELDB_WIDTH*VDP_FIELDB_HEIGHT)-1,d0
	clr.w	d1
.CFLoop:
	move.w	d1,VDP_DATA
	dbra	d0,.CFLoop
	rts

;============================================================================

ram	segment

	even
libTemp	ds.w	2

;============================================================================

	END

;============================================================================
