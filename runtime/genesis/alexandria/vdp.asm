;==============================================================================
;===	vdp.asm: Genesis VDP core routines				    ===
;===	(c)1991,2 Developer Resources					    ===
;==============================================================================
	opt	llbl
	include	genesis.equ
	include	default.equ
	include	vdp.equ

	XDEF	InitVDP
	XDEF	SetPalette
	XDEF	PlotMapA
	XDEF	DMACopy
	XDEF	DeferedDMACopy
	XDEF	WaitBlank
	XDEF	SetBackColor
	XDEF	ClearFieldA
	XDEF	_VDPAddress
	XDEF	VDPAddress
	XDEF	VDPScreenAddrA
	XDEF	VDPScreenAddrB
	XDEF	InitFieldStruct
	XDEF	DMAVDPVDP
	XDEF	_InitFieldStruct

	IFNE SEGA_CD
	include	cd.inc
	include	cd.mac
	ENDC
	include genesis.mac

	IFNE GEMS
	XREF	_gemsdmastart
	XREF	_gemsdmaend
	ENDC


	TSECTION	.text

;==============================================================================

	xdef	_WaitVBlank
	xdef	_WaitBlank

_WaitVBlank:
_WaitBlank:
WaitBlank:
	IFNE SEGA_CD
	move.w   #CMD_WaitVBlank,SUB_1M_BASE
	WAIT_FOR_1M_SWAPREQ
	SWAP_1M
	rts
	ENDC

WB1:
	cmpi.b  #VBRAST,VDP_RASTER
	bcc.s   WB1                   ; first insure we are not already there
WB2:
	cmpi.b  #VBRAST,VDP_RASTER
	bcs.s   WB2
	rts


VDPSCREENADDRESS	MACRO
	move.w	Field_Width(a0),d2
	add.w	d2,d2
	MULU	d2,D1
	ADD.W	D0,D0
	ADD.W	D1,D0		; find screen location
	add.w	Field_Base(a0),d0
	move.w	d0,d1					; 4
	lsl.l	#2,d1					; 12
	move.w	d0,d1                                   ; 4
	swap	d1                                      ; 4
	bset	#30,d1                                  ; 12
	and.l	#%01111111111111110000000000000011,d1   ; 16
	WRITE_VDP_CMD.l d1
	ENDM


	xdef	_SetBackColor
_SetBackColor:
	move.w 6(sp),d0
SetBackColor:
	WRITE_VDP_CMD.l #VDP_MODE_CRW
	WRITE_VDP_DATA d0
	rts

;==============================================================================
;===	Inputs: 							    ===
;===		a0->vdp table						    ===
;==============================================================================

; setup vdp

	xdef	_InitVDP
_InitVDP:
	movea.l	4(sp),a0
InitVDP:
	IFNE SEGA_CD
	move.w   #CMD_InitVDP,SUB_1M_BASE      ;request service 01 - color cycle
	WAIT_FOR_1M_SWAPREQ                       ;wait for a request to swap
	SWAP_1M                                   ;do it!
	rts
	ENDC

;				; first check to see if the VDP is busy

	movem.l	d4-d7/a5,-(sp)
.WaitDMA
	move.w	VDP_CMD,d4
	btst	#VDP_STATB_DMA,d4		; Wait for DMA to Finished
	bne.s	.WaitDMA
;				; then set all vdp registers
	move.w	#VDP_REG0,D1
	moveq	#18,D0
.Loop
	move.b	(a0)+,d1
	move.w	d1,VDP_CMD
	add.w	#$100,d1 	; advance to next register
	dbra	d0,.Loop

;				; now clear vsram
	move.l	#VDP_MODE_VSW,VDP_CMD
	move.w	#($80/2)-1,d0
.Loop2
	move.w	#0,VDP_DATA
	dbra	d0,.Loop2

;				; then clear vdp memory
	move.l	#VDP_MODE_VRW,D7
	move.w	#$FFFF,D6
	moveq	#00,D5
	bsr	DMAFill
	movem.l	(sp)+,d4-d7/a5
	rts

;============================================================================

DMAFill:
	IFNE GEMS
	movem.l	d0/a0,-(sp)
	jsr	_gemsdmastart
	movem.l	(sp)+,d0/a0
	ENDC
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
	IFNE GEMS
	movem.l	d0/a0,-(sp)
	jsr	_gemsdmaend
	movem.l	(sp)+,d0/a0
	ENDC
	rts

;==============================================================================
;===	Inputs:								    ===
;===		d0.w = source in vdp					    ===
;===		d1.w = dest in vdp					    ===
;===		d2.w = length of copy (in words)			    ===
;===									    ===
;==============================================================================

VDPVDPDMATbl:
	dc.w	VDP_REG15+1
	dc.l	VDP_REG20<<16|VDP_REG19
	dc.l	VDP_REG22<<16|VDP_REG21
	dc.w	VDP_REG23|VDP_R23F_DMAVDPVDP
	dc.l	VDP_MODE_VRR|VDP_DMA_START|$40 		; set dma & vram write
	dc.l	%00111111111111110000000011000011	; kill unneeded bits

;------------------------------------------------------------------------------

DMAVDPVDP:
	movem.l	d3/d4/a5/a6,-(sp)
	lea	VDP_CMD,a6
	lea	VDPVDPDMATbl,a5

	move.w	(a5)+,(a6)		; Set Auto Inc to 1 Byte

;					; do length
	move.l	(a5)+,d3
	move.b	d2,d3				; copy low byte of length
	swap	d3
	ror.w	#8,d2				; move hi byte down
	move.b	d2,d3				; copy hi byte
	move.l	d3,(a6)

;					; set up source address
	move.l	(a5)+,d2
	move.b	d0,d2				; low byte
	swap	d2
	ror.w	#8,d0
	move.b	d0,d2				; bring in middle byte
	move.l	d2,(a6)                 	; write middle & low bytes to vdp
;
	move.w	(a5)+,(a6)			; write high byte(just clear it and set DMAVDPVDP)
;					; set vdp destination and start the DMA
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

	move.w	#VDP_REG15|2,(A6)             ; Set Auto Inc to 2 Bytes
	movem.l	(sp)+,d3/d4/a5/a6
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
	dc.l	VDP_REG20<<16|VDP_REG19
	dc.l	VDP_REG22<<16|VDP_REG21
	dc.w	VDP_REG23
	dc.l	VDP_MODE_VRW|VDP_DMA_START		; set dma & vram write
	dc.l	%01111111111111110000000010000011	; kill unneeded bits

	xdef	_DMACopy
_DMACopy:
	link	a6,#-16
	movem.l	d2/a4-a5,-12(a6)
	move.w	10(a6),d0
	move.w	14(a6),d1
	movea.l	16(a6),a0
	jsr	DMACopy
	movem.l	-12(a6),d2/a4-a5
	unlk	a6
	rts

DMACopy:
	IFNE SEGA_CD

	; Copy parameters to "parameter" RAM (after registers)
	; and adjust registers accordingly

	lea	SUB_1M_PARAMS,a1
	move.w	d1,-(sp)			; size of data
	bra.s	.entry
.next_word:
	move.w	(a0)+,(a1)+
.entry:	dbra	d1,.next_word

	move.w	(sp)+,d1
	lea	MAIN_1M_PARAMS,a0

	move.w   #CMD_DMACopy,SUB_1M_BASE	;request service 01 - color cycle
	XFER_REGS_TO_MAIN
	WAIT_FOR_1M_SWAPREQ			;wait for a request to swap
	SWAP_1M				;do it!
;;	WAIT_FOR_1M_SWAPREQ		;wait for a request to swap
;;	XFER_REGS_FROM_MAIN		; (stuck here until service is complete)

	rts
	ENDC

	IFNE GEMS
	movem.l	d0/a0,-(sp)
	jsr	_gemsdmastart
	movem.l	(sp)+,d0/a0
	ENDC

	lea	VDP_CMD,a4
	lea	VDPDMATbl,a5
;					; do length
	move.l	(a5)+,d2
	move.b	d1,d2				; copy low byte of length
	swap	d2
	ror.w	#8,d1				; move hi byte down
	move.b	d1,d2				; copy hi byte
	move.l	d2,(a4)

;					; set up source address
	move.l	a0,d1				; d1 is free now

	lsr.l	#1,d1				; vdp doesn't want low bit

	move.l	(a5)+,d2
	move.b	d1,d2				; low byte
	swap	d2
	ror.w	#8,d1
	move.b	d1,d2				; bring in middle byte
	move.l	d2,(a4)                 	; write middle & low bytes to vdp

	swap	d1
	move.w	(a5)+,d2
	move.b	d1,d2				; implied or with VDP_R23F_DMACOPY, which is zero
	move.w	d2,(a4)				; write hi byte

;					; set vdp destination and start the DMA
	move.w	d0,d1					; 4
	lsl.l	#2,d1					; 12
	move.w	d0,d1                                   ; 4
	swap	d1                                      ; 4
	or.l	(a5)+,d1				; 14 set dma & vram write
	and.l	(a5),d1   				; 14 kill unneeded bits
	lea	libTemp,a5
	move.l	d1,(a5)
	move.l	(a5),(a4)		   		; 12

.Wait	move.w	(A4),D2                 ; Wait for DMA to Finished
	btst	#VDP_STATB_DMA,D2
	bne.s	.Wait

	IFNE GEMS
	movem.l	d0/a0,-(sp)
	jsr	_gemsdmaend
	movem.l	(sp)+,d0/a0
	ENDC
	rts

;==============================================================================
;===	DeferedDMACopy:  Compute DMA parameters for subsequent execution.   ===
;===	Inputs:                                                             ===
;===		d0.w = VDP destination address                              ===
;===		d1.w = VDP length (words)                                   ===
;===		a0   -> data                                                ===
;===		a1   -> buffer to write vdp cmds to                         ===
;===		assumes	auto-increment is set to 2			    ===
;===	Outputs:                                                            ===
;===		A0 -> next position in buffer                               ===
;===	Internal		                                            ===
;===		d2 - buffer to build long for VDP write                     ===
;===		a2 - pointer to offset table                                ===
;===		a1 - VDP pointer                                            ===
;===                                                                        ===
;==============================================================================


	xdef	_DeferedDMACopy
_DeferedDMACopy:
	move.w	6(sp),d0
	move.w	10(sp),d1
	movea.l	12(sp),a0
	movea.l	16(sp),a1

DeferedDMACopy:
	IFNE	SEGA_CD
	bra	DMACopy
	ENDC

	move.l	a2,-(sp)
	lea	VDPDMATbl,a2
;					; do length
	move.l	(a2)+,d2
	move.b	d1,d2				; copy low byte of length
	swap	d2
	ror.w	#8,d1				; move hi byte down
	move.b	d1,d2				; copy hi byte
	move.l	d2,(a1)+

;					; set up source address
	move.l	a0,d1				; d1 is free now

	lsr.l	#1,d1				; vdp doesn't want low bit

	move.l	(a2)+,d2
	move.b	d1,d2				; low byte
	swap	d2
	ror.w	#8,d1
	move.b	d1,d2				; bring in middle byte
	move.l	d2,(a1)+                 	; write middle & low bytes to vdp

	swap	d1
	move.w	(a2)+,d2
	move.b	d1,d2				; implied or with VDP_R23F_DMACOPY, which is zero
	move.w	d2,(a1)+				; write hi byte

;					; set vdp destination and start the DMA
	move.w	d0,d1					; 4
	lsl.l	#2,d1					; 12
	move.w	d0,d1                                   ; 4
	swap	d1                                      ; 4
	or.l	(a2)+,d1				; 14 set dma & vram write
	and.l	(a2),d1   				; 14 kill unneeded bits
	move.l	d1,(a1)+		   		; 12
	move.l	(sp)+,a2
	move.l	a1,a0
	rts

;==============================================================================
;  ExecuteDeferedDMA:  Activate the DMAs in stasis.
;  Inputs:
;	A0 -> table of DMA commands
;	D0.W = number of DMAs to execute
;==============================================================================
; modified to wait for DMA completion.
;
	xdef	_ExecuteDeferedDMA
_ExecuteDeferedDMA:
	movea.l	4(sp),a0
	move.w	10(sp),d0
ExecuteDeferedDMA:

	IFNE SEGA_CD
	rts
	ENDC

	IFNE GEMS
	movem.l	d0/a0,-(sp)
	jsr	_gemsdmastart
	movem.l	(sp)+,d0/a0
	ENDC
	lea	VDP_CMD,a1
	dbra	d0,.Loop
	IFNE GEMS
	movem.l	d0/a0,-(sp)
	jsr	_gemsdmaend
	movem.l	(sp)+,d0/a0
	ENDC
	rts
.Loop:
	move.l	(a0)+,(a1)
	move.l	(a0)+,(a1)
	move.w	(a0)+,(a1)
	move.l	(a0)+,(a1)	    	; do the dma
.Wait	move.w 	(a1),D1                 ; Wait for DMA to Finished
	btst	#VDP_STATB_DMA,D1
	bne.s	.Wait
	dbra	d0,.Loop

	IFNE GEMS
	movem.l	d0/a0,-(sp)
	jsr	_gemsdmaend
	movem.l	(sp)+,d0/a0
	ENDC
	rts

;==============================================================================
;	Inputs:
;		a0->uninitialized field struct
;		d0.w = which field this will use(FIELD_A or FIELD_B)
;------------------------------------------------------------------------------
;
; prototype:
;	InitFieldStruct(TScrollField *,unsigned short );
;

_InitFieldStruct:
	movea.l	4(a7),a0
	move.w	10(a7),d0

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



;==============================================================================
;	PlotSubMap -- Given an X,Y position, and X,Y size, plot a fraction
;	of a graphic wrapping at the right/bottom edges of the ScrollField.
;
;	Assumes map is at least 1 x 1, and submap is at least 1 x 1.
;	x/y size of submap must be less than or equal to the size of the
;	graphic pointed to by a1.
;
;	VDP increment must be word.
;
;  Destroys:
;	C Compliant, destroys d0,d1,d2, a0, and a1.
;
;  Within the tight loop, d0,d1 are the X & Y positions on screen,
;    a2 is xModulo
;    a3 is # of chars off the edge
;    d2 is the x Size DBRA counter.
;    d3 reloads d2 at the beginning of each line.
;    d4 is the y Size DBRA counter
;    d5 carries the ATTRIBUTE word
;    d6 is scratch
;    d7 is field_height minus one
;
;  Prototype:
;	void PlotSubMap(short x, short y,short x1,short y1,
;	 short x2,short y2,short attrib,TPlayField *,TMapData *);
;
;------------------------------------------------------------------------------
;	STACK FRAME
;	longwords
;-------------------------------------
;	8	|     0000|      xPos|
;-------------------------------------
;	12	|     0000|      yPos|
;-------------------------------------
;	16	|     0000|    xStart|
;-------------------------------------
;       20      |     0000|    yStart|
;-------------------------------------
;	24	|     0000|     xStop|
;-------------------------------------
;	28	|     0000|     yStop|
;-------------------------------------
;	32	|     0000|  wAttribs|
;-------------------------------------
;	36	|      TScrollField *|
;-------------------------------------
;	40	|          TMapData *|
;-------------------------------------

	xdef _PlotSubMap
_PlotSubMap:


.SF_xPos	equ	10
.SF_yPos	equ	14
.SF_xStart	equ	18
.SF_yStart	equ	22
.SF_xStop	equ	26
.SF_yStop	equ	30
.SF_wAttribs	equ	34
.SF_pField	equ	36
.SF_pMap	equ	40

	link	a6,#-32
	movem.l	d3-d7/a2-a3,-28(a6)

	move.w	.SF_wAttribs(a6),d5	; attributes
	movea.l .SF_pField(a6),a0	; field structure
	movea.l	.SF_pMap(a6),a1		; map pointer

	move.w	(a1),d3			; get x size from map
	moveq	#0,d1
	move.w	d3,d1
	mulu	.SF_yStart(a6),d1	; # of chars to skip going down

;	; (sizex - lastcolumn) - 1 + firstcolumn

	sub.w	.SF_xStop(a6),d3	; how many do I skip at the end
	subq.w	#1,d3			;
	move.w	.SF_xStart(a6),d0	; how many do I skip at the beginning
	add.w	d0,d3			; total skip value

	add.w	d0,d1			; Add X offset to Y offset
	add.w	d1,d1			;  compute # bytes

	lea	4(a1,d1.w),a1		; get new map pointer

	add.w	d3,d3			; premultiply by two to add in to map
	move.w	d3,a2

	move.w	.SF_xStop(a6),d3	; compute X size
	sub.w	.SF_xStart(a6),d3
	addq.w	#1,d3			; store in D3

	move.w	.SF_yStop(a6),d4	; get y size (minus one, for DBRA)
	sub.w	.SF_yStart(a6),d4

	move.w	.SF_xPos(a6),d0		; x position
	move.w	.SF_yPos(a6),d1		; y position

	move.w	d0,d2
	add.w	d3,d2			; d2 = left edge of original graphic

	sub.w	Field_Width(a0),d2	; # of chars over edge
	bgt.s	.1                      ; if it fits in the scroll...
	moveq	#0,d2			; .. don't bother doing second pass
.1:
	sub.w	d2,d3			; only go up to edge
	move.w	d2,a3			; next time, do this many

	subq.w	#1,d3			; repair x size	for dbra

	move.w	Field_Height(a0),d7
	subq.w	#1,d7			; mask height

.YLoop:
	movem.w	d0-d1,-(sp)
	and.w	d7,d1
	VDPSCREENADDRESS		; destroys d0/d1/d2
	movem.w	(sp)+,d0-d1
	move.w	d3,d2			; get x size back for dbra counter
.XLoop:
	move.w	(a1)+,d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset/palette/flip
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map

	movem.w	d0-d1,-(sp)		; reset VDP address for overflow
	moveq	#0,d0
	VDPSCREENADDRESS
	movem.w	(sp)+,d0-d1

	move.w	a3,d2			; get # of overflow chars
	bra.s	.2
.X2Loop:
	move.w	(a1)+,d6
	add.w	d5,d6
	move.w	d6,VDP_DATA
.2:
	dbra	d2,.X2Loop
	addq.w	#1,d1			; move down screen one character line(increment y pos)
	lea	0(a1,a2.w),a1		; skip the chars I don't want
	dbra	d4,.YLoop		; draw top to bottom for height of map

.Exit
	movem.l	-28(a6),d3-d7/a2/a3
	unlk	a6
	rts



;------------------------------------------------------------------------------

;==============================================================================










;============================================================================
;	PlotWrapA -- Correctly handle wraparound at right/bottom edge
;
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

	xdef	_PlotWrapA

_PlotWrapA:
.Sf_xOver	equ	-26
.Sf_xSize	equ	-28
.Sf_SIZEOF	equ	-32
	link	a6,#.Sf_SIZEOF
	movem.l	d2-d6,-20(a6)

	move.w	10(a6),d0		; x position
	move.w	14(a6),d1		; y position
	move.w	18(a6),d5		; attributes
	movea.l	20(a6),a0		; map pointer
	move.w	(a0)+,d3		; get x size from map
	move.w	(a0)+,d4		; get y size from map

	move.w	d0,d2
	add.w	d3,d2

	sub.w	#VDP_FIELDA_WIDTH,d2	; # of chars over edge
	bgt.s	.1
	moveq	#0,d2
.1:
	sub.w	d2,d3

	move.w	d2,.Sf_xOver(a6)
	subq.w	#1,d3			; repair x size	for dbra
	move.w	d3,.Sf_xSize(a6)
	subq.w	#1,d4			; repair y size	for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	and.w	#VDP_FIELDA_HEIGHT-1,d1
	jsr	VDPScreenAddrA		; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	.Sf_xSize(a6),d2	; get x size back for dbra counter
.XLoop:
	move.w	(a0)+,d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset/palette/flip
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map

	movem.w	d0-d1,-(sp)		; reset VDP address for overflow
	moveq	#0,d0
	and.w	#VDP_FIELDA_HEIGHT-1,d1
	jsr	VDPScreenAddrA
	movem.w	(sp)+,d0-d1

	move.w	.Sf_xOver(a6),d2	; get # of overflow chars
	bra.s	.2
.X2Loop:
	move.w	(a0)+,d6
	add.w	d5,d6
	move.w	d6,VDP_DATA
.2:
	dbra	d2,.X2Loop
	addq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw top to bottom for height of map
.Exit
	movem.l	-20(a6),d2-d6
	unlk	a6
	rts


;
; MapB version
;

	xdef	_PlotWrapB

_PlotWrapB:
PlotWrapB:
.SF_xOver	equ	-26
.SF_xSize	equ	-28
.SF_SIZEOF	equ	-32
	link	a6,#.SF_SIZEOF
	movem.l	d2-d6,-20(a6)

	move.w	10(a6),d0		; x position
	move.w	14(a6),d1		; y position
	move.w	18(a6),d5		; attributes
	movea.l	20(a6),a0		; map pointer
	move.w	(a0)+,d3		; get x size from map
	move.w	(a0)+,d4		; get y size from map

	move.w	d0,d2
	add.w	d3,d2

	sub.w	#VDP_FIELDB_WIDTH,d2	; # of chars over edge
	bgt.s	.1
	moveq	#0,d2
.1:
	sub.w	d2,d3

	move.w	d2,.SF_xOver(a6)
	subq.w	#1,d3			; repair x size	for dbra
	move.w	d3,.SF_xSize(a6)
	subq.w	#1,d4			; repair y size	for dbra
.YLoop:
	movem.w	d0-d1,-(sp)
	and.w	#VDP_FIELDB_HEIGHT-1,d1
	jsr	VDPScreenAddrB		; point vdp to correct place on screen
	movem.w	(sp)+,d0-d1
	move.w	.SF_xSize(a6),d2	; get x size back for dbra counter
.XLoop:
	move.w	(a0)+,d6		; read next word from map
	add.w	d5,d6			; add in vdp char offset/palette/flip
	move.w	d6,VDP_DATA		; write it to vdp
	dbra	d2,.XLoop		; draw left to right for width of map

	movem.w	d0-d1,-(sp)		; reset VDP address for overflow
	moveq	#0,d0
	and.w	#VDP_FIELDB_HEIGHT-1,d1
	jsr	VDPScreenAddrB
	movem.w	(sp)+,d0-d1

	move.w	.SF_xOver(a6),d2	; get # of overflow chars
	bra.s	.2
.X2Loop:
	move.w	(a0)+,d6
	add.w	d5,d6
	move.w	d6,VDP_DATA
.2:
	dbra	d2,.X2Loop
	addq.w	#1,d1			; move down screen one character line(increment y pos)
	dbra	d4,.YLoop		; draw top to bottom for height of map
.Exit
	movem.l	-20(a6),d2-d6
	unlk	a6
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

	xdef	_PlotMapA
_PlotMapA:
	link	a6,#-24
	movem.l	d2-d6,-20(a6)

	move.w	10(a6),d0
	move.w	14(a6),d1
	move.w	18(a6),d2
	movea.l	20(a6),a0
	jsr	PlotMapA
	movem.l	-20(a6),d2-d6
	unlk	a6
	rts

PlotMapA:
	IFNE SEGA_CD

;	d0 = x pos
;	d1 = y pos
;	d2 = character offset in VDP(plus palette selection)
;	(a0)-> map

	; Copy parameters to "parameter" RAM (after registers)
	; and adjust registers accordingly

	lea	SUB_1M_PARAMS,a1
	move.w	d1,-(sp)			; save y-pos

	; calculate size (in words) of map (x * y + 2)
	move.w	(a0),d1
	mulu.w	2(a0),d1
	addq.w	#2,d1		; + width + height data elements
	bra.s	.entry
.next_word:
	move.w	(a0)+,(a1)+
.entry:	dbra	d1,.next_word

	move.w	(sp)+,d1
	lea	MAIN_1M_PARAMS,a0

	move.w   #CMD_PlotMapA,SUB_1M_BASE	;request service 01 - color cycle
	XFER_REGS_TO_MAIN
	WAIT_FOR_1M_SWAPREQ			;wait for a request to swap
	SWAP_1M				;do it!
;;	WAIT_FOR_1M_SWAPREQ		;wait for a request to swap
;;	XFER_REGS_FROM_MAIN		; (stuck here until service is complete)

	rts

	ENDC


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

	xdef	_PlotMapB
_PlotMapB:
	link	a6,#-24
	movem.l	d2-d6,-20(a6)

	move.w	10(a6),d0
	move.w	14(a6),d1
	move.w	18(a6),d2
	movea.l	20(a6),a0
	jsr	PlotMapB
	movem.l	-20(a6),d2-d6
	unlk	a6
	rts

PlotMapB:
	IFNE SEGA_CD

;	d0 = x pos
;	d1 = y pos
;	d2 = character offset in VDP(plus palette selection)
;	(a0)-> map

	; Copy parameters to "parameter" RAM (after registers)
	; and adjust registers accordingly

	lea	SUB_1M_PARAMS,a1
	move.w	d1,-(sp)			; save y-pos

	; calculate size (in words) of map (x * y + 2)
	move.w	(a0),d1
	mulu.w	2(a0),d1
	addq.w	#2,d1		; + width + height data elements
	bra.s	.entry
.next_word:
	move.w	(a0)+,(a1)+
.entry:	dbra	d1,.next_word

	move.w	(sp)+,d1
	lea	MAIN_1M_PARAMS,a0

	move.w   #CMD_PlotMapB,SUB_1M_BASE	;request service 01 - color cycle
	XFER_REGS_TO_MAIN
	WAIT_FOR_1M_SWAPREQ			;wait for a request to swap
	SWAP_1M				;do it!
;;	WAIT_FOR_1M_SWAPREQ		;wait for a request to swap
;;	XFER_REGS_FROM_MAIN		; (stuck here until service is complete)

	rts

	ENDC


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


;============================================================================
;	Inputs:
;		d0 = x pos
;		d1 = y pos
; 	Destroys: d0.w,d1.w,d2.l
;	Function:
;		Set the vdp into write mode, pointing at screen a at coordinates
;============================================================================

VDPScreenAddrWin:
	move.l	#(VDP_MODE_VRW>>16)|((VDP_WINDOWBASE<<2)&$30000)|(VDP_WINDOWBASE&$3FFF),d2			; do it like this so it will be correct when swapped
	MULU	#VDP_FIELDA_WIDTH*2,D1
	ADD.W	D0,D0
	ADD.W	D1,D0		; find screen location

	OR.W	D0,D2
	SWAP	D2
	MOVE.L	D2,VDP_CMD
	rts

VDPScreenAddrA:
	move.l	#(VDP_MODE_VRW>>16)|((VDP_SCREEN_A<<2)&$30000)|(VDP_SCREEN_A&$3FFF),d2			; do it like this so it will be correct when swapped
	MULU	#VDP_FIELDA_WIDTH*2,D1
	ADD.W	D0,D0
	ADD.W	D1,D0		; find screen location

	OR.W	D0,D2
	SWAP	D2
	MOVE.L	D2,VDP_CMD
	rts

VDPScreenAddrB:
	move.l	#(VDP_MODE_VRW>>16)|((VDP_SCREEN_B<<2)&$30000)|(VDP_SCREEN_B&$3FFF),d2			; do it like this so it will be correct when swapped
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
	bra.s	VDPAddress
;				; fall through

;============================================================================
; NOTE: Kevin, make a macro version
;	VDPAddress: sets vdp into vram write mode at d0
;	Inputs:
;		d0 = 16-bit vdp address
;	Destroys:
;		d0.l,d1.l
;------------------------------------------------------------------------------

_VDPAddress:
	move.w	6(sp),d0
VDPAddress:
	move.w	d0,d1					; 4
	lsl.l	#2,d1					; 12
	move.w	d0,d1                                   ; 4
	swap	d1                                      ; 4
	bset	#30,d1                                  ; 12
	and.l	#%01111111111111110000000000000011,d1   ; 16
	WRITE_VDP_CMD.l d1
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
	WRITE_VDP_CMD.l d0
	WRITE_VDP_DATA d1
	rts

;============================================================================
; SetPalette: download color palette into vdp
;	Inputs:
;		d0 = color # to start at(0-63)
;		d1 = # of entries to set
;		a0-> palette
;	Destroys:
;		d0.l,d1.w,a0.l + a1.l
;------------------------------------------------------------------------------

	xdef	_SetPalette
_SetPalette:
	move.w	6(sp),d0
	move.w	10(sp),d1
	movea.l	12(sp),a0
SetPalette:

	IFNE SEGA_CD

	; Copy parameters to "parameter" RAM (after registers)
	; and adjust registers accordingly

	lea	SUB_1M_PARAMS,a1
	move.w	d1,-(sp)			; save number of entries
	bra.s	.entry
.next_color:
	move.w	(a0)+,(a1)+
.entry:	dbra	d1,.next_color

	move.w	(sp)+,d1
	lea	MAIN_1M_PARAMS,a0

	move.w   #CMD_SetPalette,SUB_1M_BASE	;request service 01 - color cycle
	XFER_REGS_TO_MAIN
	WAIT_FOR_1M_SWAPREQ			;wait for a request to swap
	SWAP_1M				;do it!
;;	WAIT_FOR_1M_SWAPREQ		;wait for a request to swap
;;	XFER_REGS_FROM_MAIN		; (stuck here until service is complete)

	rts
	ENDC

	add.w	d0,d0
	swap	d0
	clr.w	d0
	or.l	#VDP_MODE_CRW,d0
	WRITE_VDP_CMD.l d0

	bra.s	.Entry
.Loop
	move.w	(a0)+,VDP_DATA
.Entry:
	dbra	d1,.Loop
	rts

;============================================================================
; GetPalette: download color palette from vdp
;	Inputs:
;		d0 = color # to start at(0-63)
;		d1 = # of entries to set
;		a0-> palette
;	Destroys:
;		d0.l,d1.w,a0.l
;------------------------------------------------------------------------------

	xdef	_GetPalette
_GetPalette:
	move.w	6(sp),d0
	move.w	10(sp),d1
	movea.l	12(sp),a0
GetPalette:
	add.w	d0,d0
	swap	d0
	clr.w	d0
	or.l	#VDP_MODE_CRR,d0
	WRITE_VDP_CMD.l d0
	bra.s	.Entry
.Loop
	move.w	VDP_DATA,(a0)+
.Entry:
	dbra	d1,.Loop
	rts

;============================================================================
; KTS check, would a dma fill be faster?

	xdef	_ClearFieldA
_ClearFieldA:
ClearFieldA:
	move.w	#VDP_SCREEN_A,d0
	bsr	VDPAddress
	move.w	#(VDP_FIELDA_WIDTH*VDP_FIELDA_HEIGHT)-1,d0			; note, make this work with
;						; any screen size
	clr.w	d1
.CFLoop:
	WRITE_VDP_DATA d1
	dbra	d0,.CFLoop
	rts


	xdef	ClearWindow
ClearWindow:
	move.w	#VDP_WINDOWBASE,d0
	bsr	VDPAddress
	move.w	#(VDP_WINDOW_WIDTH*VDP_WINDOW_HEIGHT)-1,d0			; note, make this work with
;						; any screen size
	clr.w	d1
.CFLoop:
	WRITE_VDP_DATA d1
	dbra	d0,.CFLoop
	rts

;============================================================================

	xdef	_ClearFieldB
	xdef	ClearFieldB
_ClearFieldB:
ClearFieldB:
	move.w	#VDP_SCREEN_B,d0
	bsr	VDPAddress
	move.w	#(VDP_FIELDB_WIDTH*VDP_FIELDB_HEIGHT)-1,d0
	clr.w	d1
.CFLoop:
	WRITE_VDP_DATA d1
	dbra	d0,.CFLoop
	rts

	xref	_VDPTable
	xref	VDPTable
	xref	_VDP256Table
	xref	VDP256Table

;============================================================================

;ram	segment
	BSECTION	.bss

;	even
	ds.w	1	; even
libTemp	ds.w	2

;============================================================================

	END

;============================================================================

