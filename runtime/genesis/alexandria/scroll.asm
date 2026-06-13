;  scroll.asm -- functions for 4Way scrolling, and MetaScrolling

TWO_WAY_SCROLLING	EQU 1
FOUR_WAY_SCROLLING	EQU 0
META_SCROLLING	EQU 0


;
;
;
;
;
;
; Ver 0.9 Sep, and 3-Sep 1992: SLS
;	Fixed bugs in InitHorizScroll that had map width off by *8
;	Fixed bugs in ScrollHorizRight, ScrollHorizLeft to make
;		wrap the default state, and set a switch to disable.
;
; Ver 1.0 Oct 20, 1992 - Ray Ewan, Alexandria, Inc.
;
;	Added wrap around for 4 way scrolling
;	Fixed missing character in corner bug in 4 way scrolling
;
; Ver 1.01b1 Dec 1, 1992 - Ray Ewan, Alexandria, Inc.
;	got rid of divs opcode to get modulus in scrollMetaUp, scrollMetaDown,
;	scrollMetaLeft, and scrollMetaRight routines and replaced with and.w
;	opcodes
;
; Ver 1.01   Dec 2, 1992 - Ray Ewan, Alexandria, Inc.
;
;	re-wrote DownloadMetaRow and DownloadMetaColumn to work directly
;	with meta tiles and composite tiles resulting in an 380% speed
;	improvment.
;
; Ver 1.02b1 Dec 4, 1992 - Ray Ewan, Alexandria, Inc.
;
;	Changed InitMetaScroll to update the scroll register
;	variables to properly position the screen when specifying
;	a start position other than 0,0
;
; Ver 1.02   Dec 9, 1992 - Ray Ewan, Alexandria, Inc.
;
;	Changes ScrollMetaLeft, ScrollMetaRight, ScrollMetaUp, ScrollMetaDown
;	routines to accept an addition parameter specifying the number of
;	pixels to scroll resulting in a 42% speed improvement.
;
;==============================================================================
;===	scroll.asm: Genesis scrolling routines				    ===
;===	(c)1991,92 Developer Resources					    ===
;==============================================================================
;	Purpose:
;		Provide methods for scrolling playfields.
;		There are c intefaces in this library.
;
;		Direct Hardware Manipulation.
;			SetVScroll
;			SetHScroll
;			SetScrollRegs
;
;		Two-Way Horizontal Scrolling Inteface.
;			InitHorizScroll
;			RedrawHorizScroll
;			ScrollHorizLeft
;			ScrollHorizRight
;
;		Four-Way Scrolling Interaface.
;			Init4WayScroll
;			Redraw4WayScroll
;			Scroll4WayUp
;			Scroll4WayDown
;			Scroll4WayLeft
;			Scroll4WayRight
;
;		Four-Way Scrolling with Composite and Meta Tile Support
;			InitMetaScroll
;			RedrawMetaScroll
;			ScrollMetaUp
;			ScrollMetaDown
;			ScrollMetaLeft
;			ScrollMetaRight
;
;		Both the 2-way & 4-way scrolling interfaces use maps
;		as they come from CharGrab or Maped.
;
;		The meta scrolling routines use maps created by Tume
;		and it's related utilities
;
;------------------------------------------------------------------------------
;	Flow:
;		Once:
;			Download characters used by maps into VDP.
;			Init scroll interface
;		As desired, during VBLANK
;			scroll screen(ScrollHorizLeft,ScrollHorizRight,
;				Scroll4WayUp,Scroll4WayDown,Scroll4WayLeft,
;				Scroll4WayRight)
;------------------------------------------------------------------------------
;	(see sdemo, s2demo, s4demo for examples of how to use theses interfaces)
;------------------------------------------------------------------------------
; switches:
;	define SCROLLWRAP to get the scrolling to wrap at end of map
;	(currently only works on horiz scroll)
;==============================================================================
		opt	llbl
		include 'genesis.equ'
		include 'default.equ'
		include 'vdp.equ'
		include 'scroll.equ'
	IFNE SEGA_CD
	include	cd.inc
	include	cd.mac
	ENDC
	include genesis.mac

		XREF	_Crash
		XREF	VDPAddress
		XREF	InitFieldStruct
		XREF	VDPScreenAddrA
		XREF	VDPScreenAddrB

		XDEF	SetVScroll
		XDEF	SetHScroll
		XDEF	SetScrollRegs

		IFNE FOUR_WAY_SCROLLING
		XDEF	Init4WayScroll
		XDEF	Redraw4WayScroll
		XDEF	Scroll4WayUp
		XDEF	Scroll4WayDown
		XDEF	Scroll4WayLeft
		XDEF	Scroll4WayRight
		ENDC

		IFNE META_SCROLLING
		XDEF	InitMetaScroll
		XDEF	_RedrawMetaScroll
		XDEF	RedrawMetaScroll
		XDEF	ScrollMetaUp
		XDEF	ScrollMetaDown
		XDEF	ScrollMetaLeft
		XDEF	ScrollMetaRight


MTH_G_SH	EQU	5		; MT height (8x8) (product of 2) = 32
MTW_G_SH	EQU	6		; MT width (8x8) (product of 2) = 64

MTH_G		EQU	1<<MTH_G_SH	; meta tile height (Genesis 8x8 chars)
MTW_G		EQU	1<<MTW_G_SH	; meta tile width (Genesis 8x8 chars)

MTH_C		EQU	MTH_G/2		; meta tile height (composite 16x16 chars)
MTW_C		EQU	MTW_G/2		; meta tile width (composite 16x16 chars)

MTH_C_SH	EQU	MTH_G_SH-1	; MT height (16x16) (product of 2)
MTW_C_SH	EQU	MTW_G_SH-1	; MT width (16x16) (product of 2)

MT_SIZE_C_SH	EQU	MTH_C_SH+MTW_C_SH	; MT size (512) (product of 2)

BLOCKSHIFT      EQU     3               ; log2 width of block entry in bytes
		ENDC


	TSECTION	.text

;==============================================================================
; first, just the hardware scrolling interface

;==============================================================================
; 	SetVScroll: sets vert scroll assuming vert scroll in page mode
;	Inputs:
;		d0 = new vScroll value
;		d1 = which field to set(FIELD_A or FIELD_B)
;	Destroys:
;		d1.l
;------------------------------------------------------------------------------

	xdef	_SetVScroll
_SetVScroll:
	move.w	6(sp),d0
	move.w	10(sp),d1
SetVScroll:
	swap	d1
	clr.w	d1
	or.l	#VDP_MODE_VSW,d1
	WRITE_VDP_CMD.l d1
	WRITE_VDP_DATA d0
	rts


;==============================================================================
; 	SetHScroll: sets vert scroll assuming vert scroll in page mode
;	Inputs:
;		d0 = new vScroll value
;		d1 = which field to set(SCROLL_FIELDA or SCROLL_FIELDB)
;	Destroys:
;		d0.w
;------------------------------------------------------------------------------

	xdef	_SetHScroll
_SetHScroll:
	move.w	6(sp),d0
	move.w	10(sp),d1
SetHScroll:
	move.w	d2,-(sp)
	move.w	d0,d2
	move.w	#VDP_HSCROLLBASE,d0
	or.w	d1,d0
	jsr	VDPAddress
	WRITE_VDP_DATA d2		; move.w	d2,VDP_DATA
	move.w	(sp)+,d2
	rts


;==============================================================================
; 	SetScrollRegs
;
;		Outputs horizontal and vertical scroll values to the VDP
;		registers. Should be called during vblank
;
;	Inputs:	None
;
;	Destroys:
;		d0,d1,d2
;------------------------------------------------------------------------------

	XDEF	_SetScrollRegs
_SetScrollRegs:

SetScrollRegs:
	move.w	hScrollA,d0
	move.w	#FIELD_A,d1
	jsr	SetHScroll
	move.w	vScrollA,d0
	move.w	#FIELD_A,d1
	jsr	SetVScroll

	move.w	hScrollB,d0
	move.w	#FIELD_B,d1
	jsr	SetHScroll
	move.w	vScrollB,d0
	move.w	#FIELD_B,d1
	jsr	SetVScroll

	rts
;==============================================================================
;	Get Scroll Regs
;
;		Returns value of one of four internal varaibles.
;		Takes a single parameter, formed from a field constant
;		and a variable constant.  SCROLL_FieldA | SCROLL_VScroll
;		for example.
;
;	Inputs:
;		C-stack parameter;
;
;	Returns:
;		Scroll value.
;
;	Destroys:
;
;-----------------------------------------------------------------------------

	XDEF	_GetScrollRegs
_GetScrollRegs:
GetScrollRegs:
	moveq	#0,d0
	move.w	6(a7),d0
	lea	hScrollA,a0
	add.w	d0,d0
	move.w	0(a0,d0.w),d0
	rts

;==============================================================================
	IFNE TWO_WAY_SCROLLING
;==============================================================================
; now a horizontal 2-way scroll interface
;------------------------------------------------------------------------------
;	Inputs:
;		a0->unitialized scroll struct
;		a1->map
;		d0.w = starting x coordinate
;		d1.w = y offset in chars(for plotting map lower on screen)
;		d2.w = character offset in VDP(plus palette selection)
;		d3.w = which field this will use(SCROLL_FIELDA or SCROLL_FIELDB)
;	Destroys:
;		VDP,d0.l,d1.l,d2.w-d6.w,a1.l
;	Restrictions:
;		Map must be at least the width of the screen
;------------------------------------------------------------------------------

	xref	_fgAztec
	xref	_aztecMap
	xref	_chAztecBase


	xdef _InitHorizScroll
_InitHorizScroll:
	link	a6,#0
	movem.l	d3-d7/a2-a6,-(sp)
	move.w	10(a6),d0
	move.w	14(a6),d1
	move.w	18(a6),d2
	move.w	20(a6),d3
	movea.l	24(a6),a0
	movea.l	28(a6),a1
	jsr	InitHorizScroll
	movem.l	(sp)+,d3-d7/a2-a6
	unlk	a6
	rts

InitHorizScroll:
	move.w	d0,Scroll_XPos(a0)		; starting x coordinate
	move.w	d2,Scroll_CharBase(a0)

	move.w	d3,d0
	jsr	InitFieldStruct
;						; move down screen
	mulu.w	Field_Width(a0),d1
	add.l	d1,Field_Base(a0)
;						; read map width & height
	move.w	(a1)+,d0
	move.w	d0,Scroll_MapHeight(a0)
	add.w	d0,d0
	add.w	d0,d0
	add.w	d0,d0
	move.w	d0,Scroll_MapHeightPixel(a0)

	sub.w	#VDP_SCREEN_HEIGHT*8,d0		; fixed kts bug 3-sep-1992
	move.w	d0,Scroll_BotLimit(a0)

	move.w	(a1)+,d0
	move.w	d0,Scroll_MapWidth(a0)
	add.w	d0,d0
	add.w	d0,d0
	add.w	d0,d0
	move.w	d0,Scroll_MapWidthPixel(a0)
	sub.w	#VDP_SCREEN_WIDTH*8,d0		; fixed kts bug 2-sep-1992
;	;lsl.w	#3,d0				; fixed kts bug 2-sep 1992
	move.w	d0,Scroll_RightLimit(a0)

	move.l	a1,Scroll_MapPtr(a0)
	move.w	#SCROLL_PREVD_RIGHT,Scroll_PrevDirection(a0)

	jmp	RedrawHorizScroll			; fall through


;==============================================================================
;	Inputs:
;		a0->scroll structure
;	Destroys:
;		VDP,d0.l,d1.l,d2.w-d5.w,a1.l
;------------------------------------------------------------------------------

	xdef _RedrawHorizScroll
_RedrawHorizScroll:
	movea.l	4(sp),a0
	movem.l	d3-d7/a2-a6,-(sp)
	jsr	RedrawHorizScroll
	movem.l	(sp)+,d3-d7/a2-a6
	rts

RedrawHorizScroll:
	move.w	Scroll_XPos(a0),d0
	neg.w	d0
	move.w	Field_Num(a0),d1
	bsr	SetHScroll
	move.w	#VDP_SCREEN_WIDTH,d6		; # of actual columns on the screen + 1 column
.Loop:
	move.w	Scroll_XPos(a0),d0		; get x offset so we start in right place
	lsr.w	#3,d0				; convert x & y into char offsets
	add.w	d6,d0				; add in current column to draw
	move.w	d0,d2
	move.w	Field_Width(a0),d3
	subq.w	#1,d3
	and.w	d3,d2				; and with # of comlunms
	move.w	Scroll_YPos(a0),d3
	bsr	DownloadColumn
	dbra	d6,.Loop			; now let's do the column
;						; to our left
;
	move.w	Scroll_MapWidth(a0),d0
	subq.w	#1,d0				; point to last column

	move.w	Field_Width(a0),d2
	subq.w	#1,d2

	move.w	Scroll_YPos(a0),d3
	bsr	DownloadColumn

	rts

;==============================================================================
;	Inputs:
;		a0-> scroll structure
;		d0.w = x position in map to start from(char)
;		d2.w = column in field(in vdp) to plot to(char)
;		d3.w = y character position down the screen to start plotting map
;	Destroys:
;		VDP,d0.l,d1.l,d2.w-d5.w,a1.l
;------------------------------------------------------------------------------

DownloadColumn:
	move.w	#VDP_REG_AUTOINCSIZE+(VDP_FIELDA_WIDTH*2),VDP_CMD		; set up auto-increment
;	WRITE_VDP_CMD.w #VDP_REG_AUTOINCSIZE+(VDP_FIELDA_WIDTH*2)	; set up auto-increment
;				; first find correct address in map
	move.l	Scroll_MapPtr(a0),a1
	add.w	d0,d0				; double since each entry is 2 bytes long
	mulu.w	Scroll_MapHeight(a0),d0
	add.w	d0,a1				; point to correct column in map
;				; now set correct address on field(screen)

	add.w	d2,d2
	add.w	Field_Base(a0),d2		; add in base, d2 now contains starting position in field

	move.w	Scroll_CharBase(a0),d5
	move.w	Scroll_MapHeight(a0),d4		; since map could be shorter than screen
	move.w	Field_Width(a0),d3	; remember field width
	add.w	d3,d3
	subq.w	#1,d4
.Loop:
	move.w	d2,d0
	jsr	VDPAddress		; point vdp there
	add.w	d3,d2			; go to next row in screen memory

	move.w	(a1)+,d0				; read from map
	add.w	d5,d0					; add in character offset
	move.w	d0,VDP_DATA				; write to vdp
;;;	WRITE_VDP_DATA d0
	dbra	d4,.Loop				; next

	move.w	#VDP_REG_AUTOINCSIZE+2,VDP_CMD
;	WRITE_VDP_CMD.w #VDP_REG_AUTOINCSIZE+2
	rts

;==============================================================================
	xdef _ScrollHorizLeft
_ScrollHorizLeft:
	movea.l	4(sp),a0
	lea	_fgAztec,a0
	movem.l	d3-d7/a2-a6,-(sp)
	jsr	ScrollHorizLeft
	movem.l	(sp)+,d3-d7/a2-a6
	rts

ScrollHorizLeft:
	move.w	Scroll_XPos(a0),d0
	subq.w	#1,d0
	move.w	d0,Scroll_XPos(a0)

	neg.w	d0
	move.w	Field_Num(a0),d1
	bsr	SetHScroll

	move.w	Scroll_XPos(a0),d0
	move.w	d0,d1
	tst.w	Scroll_PrevDirection(a0)	; was the previous scroll to the right?
	bne.s	.DoItAnyway
	and.w	#$7,d1
	bne.s	.Nope
.DoItAnyway
	move.w	d0,d2				; place xpos in d2

	asr.w	#3,d2				; convert it into column address
	subq.w	#1,d2				; offset to just left of screen
	move.w	d2,d3

;	; constrain to map

.constrain
	tst.w	d2
	bpl.s	.constrain2
	add.w	Scroll_MapWidth(a0),d2		; move back into the map
	bra.s	.constrain
.constrain2
	cmp.w	Scroll_MapWidth(a0),d2
	bcs.s	.Ok
	sub.w	Scroll_MapWidth(a0),d2
	bra.s	.constrain
.Ok:
	move.w	d2,d0				; make map offset

	move.w	d3,d2
	move.w	Field_Width(a0),d1
	subq.w	#1,d1
	and.w	d1,d2				; and with # of comlunms
;
	move.w	Scroll_YPos(a0),d3
;	ifdef	DEBUGSCROLLCODE
;	clr.l	d4
;	move.w	Scroll_XPos(a0),d4
;	asr.w	#3,d4
;	jsr	PrintRegs
;	endif
	bsr	DownloadColumn
.Nope:
	move.w	#SCROLL_PREVD_LEFT,Scroll_PrevDirection(a0)
	rts

;==============================================================================

	xdef _ScrollHorizRight
_ScrollHorizRight:
	movea.l	4(sp),a0
	lea	_fgAztec,a0
	movem.l	d3-d7/a2-a6,-(sp)
	jsr	ScrollHorizRight
	movem.l	(sp)+,d3-d7/a2-a6
	rts

ScrollHorizRight:
	move.w	Scroll_XPos(a0),d0
	addq.w	#1,d0
	move.w	d0,Scroll_XPos(a0)		; in the 64k universe
	neg.w	d0				; hw scroll is backwards
	move.w	Field_Num(a0),d1
	bsr	SetHScroll
	move.w	Scroll_XPos(a0),d0		; get new position back
	move.w	d0,d1
	tst.w	Scroll_PrevDirection(a0)
	beq.s	.DoItAnyway
	and.w	#$7,d1				; are we on char boundry?
	bne.s	.Nope				; if not, we are done
.DoItAnyway

	move.w	d0,d2				; place xpos in d2
	asr.w	#3,d2				; convert xpos into column address
	add.w	#VDP_SCREEN_WIDTH,d2		; offset to right of screen
	move.w	d2,d3				; save for later

;	; constrain to MAP!   Fixed KTS bug 3-SEP-1992
.constrain
	tst.w	d2
	bpl.s	.constrain2
	add.w	Scroll_MapWidth(a0),d2		; move back into the map
	bra.s	.constrain
.constrain2
	cmp.w	Scroll_MapWidth(a0),d2
	bcs.s	.Ok
	sub.w	Scroll_MapWidth(a0),d2
	bra.s	.constrain
.Ok:

;	; d2 contains char column in map
	move.w	d2,d0                     	; set up for call to D.L.Col.


;	; now we want to get the XPos back

	move.w	d3,d2

	move.w	Field_Width(a0),d1		; constrain ..
	subq.w	#1,d1				; .. to within ..
	and.w	d1,d2				; .... field.

	move.w	Scroll_YPos(a0),d3		; get x offset so we start in right place
;	ifdef	DEBUGSCROLLCODE
;	clr.l	d4
;
;	move.w	Scroll_XPos(a0),d4
;	asr.w	#3,d4
;	jsr	PrintRegs
;	endif
	bsr	DownloadColumn
.Nope:
	move.w	#SCROLL_PREVD_RIGHT,Scroll_PrevDirection(a0)
	rts

	ENDC



;==============================================================================
	IFNE FOUR_WAY_SCROLLING
;==============================================================================
; now the 4-way scrolling code
;------------------------------------------------------------------------------
;	Inputs:
;		a0->scroll struct(this allows the same code to work on both playfields)
;		a1->map
;		d0.w = starting x coordinate
;		d1.w = starting y coordinate
;		d2.w = char base in vdp
;		d3.w = which field this will use(SCROLL_FIELDA or SCROLL_FIELDB)
;
;	Destroys:
;		??(lots of registers)
;	Restrictions:
;		Map must be at least the size of the screen
;------------------------------------------------------------------------------

Init4WayScroll:
	movem.l	d3-d7/a2-a6,-(sp)

	move.w	d0,Scroll_XPos(a0)
	move.w	d1,Scroll_YPos(a0)
	move.w	d2,Scroll_CharBase(a0)

	move.w	d3,d0
	jsr	InitFieldStruct
;						; who cares, only happens during init
	tst.w	d3
	bne.s	.FB
	move.l	#VDPScreenAddrA,Scroll_ScreenAddrRout(a0)
	clr.w	vScrollA
	clr.w	hScrollA
	bra.s	.ok
.FB
	move.l	#VDPScreenAddrB,Scroll_ScreenAddrRout(a0)
	clr.w	vScrollB
	clr.w	hScrollB
.ok:
	move.w	(a1)+,d0
	move.w	d0,Scroll_MapWidth(a0)
	sub.w	#VDP_SCREEN_WIDTH,d0
	lsl.w	#3,d0
	move.w	d0,Scroll_RightLimit(a0)

	move.w	(a1)+,d0
	move.w	d0,Scroll_MapHeight(a0)
	sub.w	#VDP_SCREEN_HEIGHT,d0
	lsl.w	#3,d0
	move.w	d0,Scroll_BotLimit(a0)

	move.l	a1,Scroll_MapPtr(a0)
	bsr	Redraw4WayScroll

	movem.l	(sp)+,d3-d7/a2-a6
	rts


;==============================================================================

Redraw4WayScroll:
	move.w	#VDP_SCREEN_HEIGHT,d5
.Loop:
	move.w	Scroll_YPos(a0),d3		; get x offset so we start in right place
	lsr.w	#3,d3				; convert ypos into column address
	add.w	d5,d3 				; current line offset
	move.w	d3,d1
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of comlunms

	move.w	Scroll_XPos(a0),d0		; get x offset so we start in right place
	lsr.w	#3,d0				; convert x & y into char offsets
	move.w	d0,d2
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of comlunms
	bsr	Download4WayRow
	dbra	d5,.Loop
	rts

;==============================================================================
; Inputs:
;	a0-> scroll struct
;------------------------------------------------------------------------------

Scroll4WayUp:
	movem.l	d3-d7/a2-a6,-(sp)

	move.w	Scroll_YPos(a0),d0
	subq.w	#1,d0
	bmi.s	.Nope
	move.w	d0,Scroll_YPos(a0)
	move.w	Field_Num(a0),d1
	bsr	SetVScroll

	add.w	#1,d0				; fixed missing character bug
	move.w	d0,d1
	and.w	#$7,d1
	bne.s	.Nope

	move.w	d0,d3				; place xpos in d3
	lsr.w	#3,d3				; convert it into column address
	subq.w	#1,d3				; offset to above top of screen
	move.w	d3,d1				; make map offset
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of comlunms

	clr.l	d0
	move.w	Scroll_XPos(a0),d0		; get x offset so we start in right place


	asr.w	#3,d0				; convert x & y into char offsets
	move.w	d0,d2
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of comlunms

	move.w	Scroll_MapWidth(a0),d5		; constrain to map by taking
	sub.w	#1,d5				; modulus of map width
	and.w	d5,d0

	bsr	Download4WayRow
.Nope:

	movem.l	(sp)+,d3-d7/a2-a6
	rts

;==============================================================================

Scroll4WayDown:
	movem.l	d3-d7/a2-a6,-(sp)

	move.w	Scroll_YPos(a0),d0
	addq.w	#1,d0
	cmp.w	Scroll_BotLimit(a0),d0
	bcc.s	.Nope
	move.w	d0,Scroll_YPos(a0)
	move.w	Field_Num(a0),d1
	bsr	SetVScroll
	move.w	d0,d1
	and.w	#$7,d1				; are we on char boundry?
	bne.s	.Nope				; if not, we are done

	move.w	d0,d3				; place xpos in d3
	lsr.w	#3,d3				; convert it into column address
	add.w	#VDP_SCREEN_HEIGHT,d3		; offset to bottom of screen
	move.w	d3,d1
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of comlunms

	clr.l	d0
	move.w	Scroll_XPos(a0),d0		; get x offset so we start in right place
	asr.w	#3,d0				; convert x & y into char offsets
	move.w	d0,d2
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of comlunms

	move.w	Scroll_MapWidth(a0),d5		; constrain to map by taking
	sub.w	#1,d5				; modulus of map width
	and.w	d5,d0

	bsr	Download4WayRow

.Nope:

	movem.l	(sp)+,d3-d7/a2-a6
	rts

;==============================================================================

Scroll4WayLeft:
	movem.l	d3-d7/a2-a6,-(sp)

	move.w	Scroll_XPos(a0),d0		; decrement x position
	subq.w	#1,d0
	move.w	d0,Scroll_XPos(a0)

	neg.w	d0				; output horz offset
	move.w	Field_Num(a0),d1
	bsr	SetHScroll

	move.w	Scroll_XPos(a0),d0
	add.w	#1,d0				; fixes missing character bug

	move.w	d0,d1
	and.w	#$7,d1
	bne.s	.Nope

	move.w	d0,d2				; place xpos in d2
	asr.w	#3,d2				; convert it into column address
	subq.w	#1,d2				; offset to left of screen

	clr.l	d0
	move.w	d2,d0				; make map offset
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of rows (constrain to field)

	move.w	Scroll_MapWidth(a0),d5		; constrain to map by taking
	sub.w	#1,d5				; modulus of map width
	and.w	d5,d0

	move.w	Scroll_YPos(a0),d1
	lsr.w	#3,d1
	move.w	d1,d3
	and.w	#VDP_FIELDA_HEIGHT-1,d3
	bsr	Download4WayColumn
.Nope:

	movem.l	(sp)+,d3-d7/a2-a6
	rts

;==============================================================================

Scroll4WayRight:
	movem.l	d3-d7/a2-a6,-(sp)

	move.w	Scroll_XPos(a0),d0		; increment x position
	addq.w	#1,d0
	move.w	d0,Scroll_XPos(a0)

	neg.w	d0				; output horz offset
	move.w	Field_Num(a0),d1
	bsr	SetHScroll

	move.w	Scroll_XPos(a0),d0
	move.w	d0,d1
	and.w	#$7,d1				; are we on x char boundry?
	bne.s	.Nope				; if not, we are done

	move.w	d0,d2				; place xpos in d2
	asr.w	#3,d2				; convert it into column address
	add.w	#VDP_SCREEN_WIDTH,d2		; offset to right of screen
	clr.l	d0
	move.w	d2,d0				; save x map address
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of columns (constrain to field space)

; constrain to map

	move.w	Scroll_MapWidth(a0),d5		; constrain to map by taking
	sub.w	#1,d5				; modulus of map width
	and.w	d5,d0

	move.w	Scroll_YPos(a0),d1		; get x offset so we start in right place
	lsr.w	#3,d1				; convert x & y into char offsets
	move.w	d1,d3
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of rows
	jsr	Download4WayColumn
.Nope:

	movem.l	(sp)+,d3-d7/a2-a6
	rts

;==============================================================================
; Inputs:
;	a0-> scroll structure
;	d0.w = x position in map to start from(char)
;	d1.w = y positon in map to start from(char)
;	d2.w = column in field(in vdp) to draw to(char)
;	d3.w = row in field(in vdp) to draw to(char)
;------------------------------------------------------------------------------

Download4WayRow:
;					; first find correct address in map
	move.w	d5,-(sp)

	move.l	Scroll_MapPtr(a0),a1
	mulu.w	Scroll_MapWidth(a0),d1
	add.w	d0,d1				; add in x offset
	add.w	d1,d1				; double since each entry is 2 bytes long
	andi.l	#$ffff,d1
	add.l	d1,a1				; point to correct spot in map

	move.w	Scroll_MapWidth(a0),d5		; get width
	sub.w	d0,d5				; calc # of entries till end of map

	move.w	d3,d1
	move.w	d2,d0

	move.w	#VDP_SCREEN_WIDTH,d7
	move.w	d7,d6
	add.w	d2,d7				; get ending point
	sub.w	Field_Width(a0),d7
	blt.s	.Safe
;						; d7 now contains # of columns past end of line to draw
	sub.w	d7,d6
	exg	d6,d7

	move.l	Scroll_ScreenAddrRout(a0),a2
	jsr	(a2)				; set screen address
;					; now actually download new row
	move.w	Scroll_CharBase(a0),d1
	subq.w	#1,d7
.Loop:
	move.w	(a1)+,d0
	add.w	d1,d0
	WRITE_VDP_DATA d0

	sub.w	#1,d5
	bne	.loopa
	clr.l	d0
	move.w	Scroll_MapWidth(a0),d0
	sub.l	d0,a1
	sub.l	d0,a1

.loopa
	dbra	d7,.Loop

	clr.w	d0
.Safe:
;						; download second half
	move.w	d3,d1
	move.l	Scroll_ScreenAddrRout(a0),a2
	jsr	(a2)				; set field address
;					; now actually download new row
	move.w	Scroll_CharBase(a0),d1
;	subq.w	#1,d6
.Loop2:
	move.w	(a1)+,d0
	add.w	d1,d0
	WRITE_VDP_DATA d0

	sub.w	#1,d5
	bne	.loopb
	clr.l	d0
	move.w	Scroll_MapWidth(a0),d0
	sub.l	d0,a1
	sub.l	d0,a1

.loopb
	dbra	d6,.Loop2

	move.w	(sp)+,d5
	rts

;------------------------------------------------------------------------------
;	Inputs:
;	a0-> scroll structure
;	d0.w = x position in map to start from(char)
;	d1.w = y positon in map to start from(char)
;	d2.w = column in field(in vdp) to draw to(char)
;	d3.w = row in field(in vdp) to draw to(char)


Download4WayColumn:
;	move.w	#VDP_REG_AUTOINCSIZE+(VDP_FIELDA_WIDTH*2),VDP_CMD		; set up auto-increment
	WRITE_VDP_CMD.w #VDP_REG_AUTOINCSIZE+(VDP_FIELDA_WIDTH*2)	; set up auto-increment
;				; first find correct address in map

	move.l	Scroll_MapPtr(a0),a1
	mulu.w	Scroll_MapWidth(a0),d1
	add.w	d0,d1			; add in x offset
	add.w	d1,d1			; double since each entry is 2 bytes long
	andi.l	#$FFFF,d1		; clear top word
	add.l	d1,a1			; point to correct spot in map
;		     			; now set correct address on field
	move.w	d3,d1			; Field Y (leave these here ...
	move.w	d2,d0			; Field X   ...for ScreenAddr )

	move.w	#VDP_SCREEN_HEIGHT,d7
	move.w	d7,d6

	add.w	d3,d7		     ; d7 = one screen below current Y
	move.w	d2,d3				; Field X
	sub.w	Field_Height(a0),d7  ; d7 =
	blt.s	.Safe

;; d7 now contains # of rows past end of line to draw

	sub.w	d7,d6
	exg	d6,d7

;;; Now, D7 contains the # of chars to do in the first phase, while
;;; D6 contains the # of chars to do in the second phase.

; Set screen address to (D0,D1)

	move.l	Scroll_ScreenAddrRout(a0),a2
	jsr	(a2)

	move.w	Scroll_CharBase(a0),d1
	clr.l	d2
	move.w	Scroll_MapWidth(a0),d2
	add.w	d2,d2
;	subq.w	#1,d6
	bra.s	.LoopEnd
.Loop:
	move.w	(a1),d0
	add.l	d2,a1				; skip to next row in map
	add.w	d1,d0
	WRITE_VDP_DATA d0
.LoopEnd
	dbra	d7,.Loop

	clr.w	d1
.Safe:
;						; download second half
	move.w	d3,d0				; Field X Pos
	move.l	Scroll_ScreenAddrRout(a0),a2
	jsr	(a2)				; set field address
;					; now actually download new row
	move.w	Scroll_CharBase(a0),d1
	clr.l	d2
	move.w	Scroll_MapWidth(a0),d2
	add.l	d2,d2
;	subq.w	#1,d6
;	bra.s	.Loop2End
.Loop2:
	move.w	(a1),d0
	add.l	d2,a1
	add.w	d1,d0
	WRITE_VDP_DATA d0
.Loop2End
	dbra	d6,.Loop2

;	move.w	#VDP_REG_AUTOINCSIZE+2,VDP_CMD
	WRITE_VDP_CMD.w #VDP_REG_AUTOINCSIZE+2
	rts

	ENDC


	IFNE META_SCROLLING
;==============================================================================
; now the meta scrolling code
;------------------------------------------------------------------------------
;	Inputs:
;		a0->scroll struct(this allows the same code to work on both playfields)
;		a1->map pointer
;		a2->metamap pointer
;		a3->composite tile translation table pointer
;		d0.w = starting x coordinate
;		d1.w = starting y coordinate
;		d2.w = char base in vdp
;		d3.w = which field this will use(SCROLL_FIELDA or SCROLL_FIELDB)
;
;	Destroys:
;		??(lots of registers)
;	Restrictions:
;		Map must be at least the size of the screen
;	Prototype:
;		void InitMetaScroll(short xCoordinate, short yCoordinate, short nFirstChar,
;			short nField, void *pScrollStruct, void *pMap, void *pMetaMap,
;			void *Xlation);
;
;------------------------------------------------------------------------------

	XDEF	_InitMetaScroll

_InitMetaScroll:
	link	a6,#-40			; #bytes for local var + 4
	movem.l	d3-d7/a2-a5,-36(a6)

	move.w	10(a6),d0
	move.w	14(a6),d1
	move.w	18(a6),d2
	move.w	22(a6),d3
	move.l	24(a6),a0
	move.l	28(a6),a1
	move.l	32(a6),a2
	move.l	36(a6),a3

	bsr	InitMetaScroll

	movem.l	-36(a6),d3-d7/a2-a5
	unlk	a6
	rts

;==============================================================================
; destroys d0

VALIDATEPTR	MACRO
	IFEQ SEGA_CD
	move.l	\1,d0
	beq.s	\@Fucked
	btst	#0,d0
	bne.s	\@Fucked
	cmp.l	#$1000000,d0			; validate only 24 bit ptr
	bcc.s	\@Fucked
	swap	d0
	cmp.w	#$00ff,d0			; validate if points to ram
	beq.s	\@NotRam
	cmp.w	#$002f,d0			; or points to 1st 2 meg of rom
	bcc.s	\@Fucked
\@NotRam:
	bra.s	\@Ok
\@Fucked:
	pea	\2
	jsr	_Crash
\@Ok:
	ENDC
		ENDM


;==============================================================================

MetaFuckedText:
	dc.b	'InitMetaScroll() parameters fail validation',10
	dc.b	' d0.w -> character base',10
	dc.b	' a0   -> scroll struct',10
	dc.b	' a1   -> level map',10
	dc.b	' a2   -> metamap',10
	dc.b	' a3   -> xlate tbl',0
	ds.w	1

	IFNE SEGA_CD
InitMetaScrollDifferentBanks
	dc.b	'ROM-based parameters of',10
	dc.b	'InitMetaScroll() located in',10
	dc.b	'different banks'
	ds.w	1
	ENDC

InitMetaScroll:
; kts input validation added 05-01-93 05:48pm
	move.l	d0,-(sp)
	VALIDATEPTR	a0,MetaFuckedText
	VALIDATEPTR	a1,MetaFuckedText
	VALIDATEPTR	a2,MetaFuckedText
	VALIDATEPTR	a3,MetaFuckedText
	move.l	(sp)+,d0

	IFNE SEGA_CD
; a1/a2/a3 must be in the same bank
;;;	UNTESTED CODE
	movem.l	d0-d2,-(sp)

	move.l	a1,d1
	and.l	#$FF000000,d1	; d1 = bank required
	move.l	a1,d0		; a0 -> d0
	move.l	a2,d2
	and.l	d2,d0		; a2 -> d0
	move.l	a3,d2
	and.l	d2,d0		; a3 -> d0
	and.l	#$FF000000,d0
	cmp.l	d0,d1
	beq	.sameBankOk
	pea	InitMetaScrollDifferentBanks
	jsr	_Crash
.sameBankOk:
	movem.l	(sp)+,d0-d2
	ENDC

	cmp.w	#2048,d0		; less than 2048
	bcs.s	.ChrPtrOk
	cmp.w	#0,d0		; and not 0
	bne.s	.ChrPtrOk
	pea	MetaFuckedText
	jsr	_Crash
.ChrPtrOk:

	cmp.w	#0,d3					;SCROLL_FIELDA,d3
	beq.s	.d3Ok
	cmp.w	#2,d3 					;SCROLL_FIELDB,d3
	beq.s	.d3Ok
	pea	MetaFuckedText
	jsr	_Crash
.d3Ok:
; end kts addition

	move.w	d0,Scroll_XPos(a0)
	move.w	d1,Scroll_YPos(a0)
	move.w	d2,Scroll_CharBase(a0)

	move.w	d3,d0
	jsr	InitFieldStruct
;						; who cares, only happens during init
	move.l	#VDPScreenAddrB,Scroll_ScreenAddrRout(a0)
	tst.w	d3
	bne.s	.FB
	move.l	#VDPScreenAddrA,Scroll_ScreenAddrRout(a0)
.FB
	move.w	(a2)+,d0			; get meta width
	move.w	d0,Scroll_MetaWidth(a0)		; store meta map width
	jsr	GetBitNumber
	move.w	d0,Scroll_MetaWidthSh(a0)	; meta map width (product of 2)
	move.w	Scroll_MetaWidth(a0),d0
	lsl.w	#6,d0
	move.w	d0,Scroll_MapWidth(a0)		; store map width in chars
	jsr	GetBitNumber
	move.w	d0,Scroll_MapWidthSh(a0)	; map width as product of 2
	move.w	Scroll_MapWidth(a0),d0
	sub.w	#1,d0
	move.w	d0,Scroll_MapWidthMask(a0)	; store (map width - 1)

	move.w	(a2)+,d0
	move.w	d0,Scroll_MetaHeight(a0)	; store meta map height
	lsl.w	#5,d0
	move.w	d0,Scroll_MapHeight(a0)		; store map height
	sub.w	#1,d0
	move.w	d0,Scroll_MapHeightMask(a0)	; store map height mask
	move.w	Scroll_MapHeight(a0),d0
	sub.w	#VDP_SCREEN_HEIGHT,d0
	lsl.w	#3,d0
	move.w	d0,Scroll_BotLimit(a0)		; store bottom limit (pixels)

	move.l	a1,Scroll_MapPtr(a0)		; store map pointer
	move.l	a2,Scroll_MetaMap(a0)		; store metamap pointer
	move.l	a3,Scroll_CompTileTbl(a0)	; store xlate table pointer

	bsr	RedrawMetaScroll

	move.w	Scroll_YPos(a0),d0		; set scroll registers
	move.w	Field_Num(a0),d1
	beq.s	.setA1
	move.w	d0,vScrollB
	bra.s	.ok1
.setA1:
	move.w	d0,vScrollA
.ok1:
	move.w	Scroll_XPos(a0),d0
	neg.w	d0
	move.w	Field_Num(a0),d1
	beq.s	.setA2
	move.w	d0,hScrollB
	bra.s	.ok2
.setA2:
	move.w	d0,hScrollA
.ok2:
	rts


;==============================================================================
;GetBitNumber
;
;Input:	d0 contains a set bit
;
;Output:	d0 contains number of first bit found (0-15)
;
;Destroys:	d1
;------------------------------------------------------------------------------

GetBitNumber:
	clr.w	d1
.loop
	btst	#0,d0
	bne	.end
	lsr.w	#1,d0
	add.w	#1,d1
	bra	.loop
.end
	move.w	d1,d0
	rts

;==============================================================================

_RedrawMetaScroll:
	movea.l	4(sp),a0 		; read scroll struct ptr
	movem.l	d3-d7/a2-a5,-(sp)
	jsr	RedrawMetaScroll
	movem.l	(sp)+,d3-d7/a2-a5
	rts

;------------------------------------------------------------------------------

RedrawMetaScroll:
	move.w	#VDP_SCREEN_HEIGHT,d5
.Loop:
	move.w	Scroll_YPos(a0),d3		; get x offset so we start in right place
	lsr.w	#3,d3				; convert ypos into column address
	add.w	d5,d3 				; current line offset
	move.w	d3,d1
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of comlunms

	move.w	Scroll_XPos(a0),d0		; get x offset so we start in right place
	lsr.w	#3,d0				; convert x & y into char offsets
	move.w	d0,d2
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of comlunms
	move.w	d5,-(sp)
	bsr	DownloadMetaRow
	move.w	(sp)+,d5
	dbra	d5,.Loop
	rts

;==============================================================================
; Inputs:
;	a0-> scroll struct
;------------------------------------------------------------------------------

	xdef	_ScrollMetaUp
_ScrollMetaUp:
	link	a6,#-40
	movem.l	d3-d7/a2-a5,-36(a6)
	move.l	8(a6),a0
	move.w	14(a6),d7
	bra.s	.upEnd
.upLoop:
	move.w	d7,-(sp)
	jsr	ScrollMetaUp
	move.w	(sp)+,d7
.upEnd:
	dbra	d7,.upLoop

	movem.l	-36(a6),d3-d7/a2-a5
	unlk	a6
	rts

	xdef	_ScrollMetaDown
_ScrollMetaDown:
	link	a6,#-40
	movem.l	d3-d7/a2-a5,-36(a6)
	move.l	8(a6),a0
	move.w	14(a6),d7
	bra.s	.downEnd
.downLoop:
	move.w	d7,-(sp)
	jsr	ScrollMetaDown
	move.w	(sp)+,d7
.downEnd:
	dbra	d7,.downLoop

	movem.l	-36(a6),d3-d7/a2-a5
	unlk	a6
	rts

	xdef	_ScrollMetaLeft
_ScrollMetaLeft:
	link	a6,#-40
	movem.l	d3-d7/a2-a5,-36(a6)
	move.l	8(a6),a0
	move.w	14(a6),d7
	bra.s	.leftEnd
.leftLoop:
	move.w	d7,-(sp)
	jsr	ScrollMetaLeft
	move.w	(sp)+,d7
.leftEnd:
	dbra	d7,.leftLoop

	movem.l	-36(a6),d3-d7/a2-a5
	unlk	a6
	rts

	xdef	_ScrollMetaRight
_ScrollMetaRight:
	link	a6,#-40
	movem.l	d3-d7/a2-a5,-36(a6)
	move.l	8(a6),a0
	move.w	14(a6),d7
	bra.s	.rightEnd
.rightLoop:
	move.w	d7,-(sp)
	jsr	ScrollMetaRight
	move.w	(sp)+,d7
.rightEnd:
	dbra	d7,.rightLoop

	movem.l	-36(a6),d3-d7/a2-a5
	unlk	a6
	rts


ScrollMetaUp:

	move.w	Scroll_YPos(a0),d0
	subq.w	#1,d0
;	bmi.s	.Nope
	move.w	d0,Scroll_YPos(a0)
	move.w	Field_Num(a0),d1
	beq.s	.setA
	move.w	d0,vScrollB
	bra.s	.ok
.setA:
	move.w	d0,vScrollA
.ok:

	add.w	#1,d0				; fixed missing character bug
	move.w	d0,d1
	and.w	#$7,d1
	bne.s	.Nope

	move.w	d0,d3				; place xpos in d3
	lsr.w	#3,d3				; convert it into column address
	subq.w	#1,d3				; offset to above top of screen
	move.w	d3,d1				; make map offset
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of comlunms

	clr.l	d0
	move.w	Scroll_XPos(a0),d0		; get x offset so we start in right place


	asr.w	#3,d0				; convert x & y into char offsets
	move.w	d0,d2
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of comlunms

	move.w	Scroll_MapWidth(a0),d5
	sub.w	#1,d5
	and.w	d5,d0			; constrain to map by taking modulus of map width

	bsr	DownloadMetaRow
.Nope:

	rts

;==============================================================================

ScrollMetaDown:

	move.w	Scroll_YPos(a0),d0
	addq.w	#1,d0
	cmp.w	Scroll_BotLimit(a0),d0
;	bcc.s	.Nope
	move.w	d0,Scroll_YPos(a0)
	move.w	Field_Num(a0),d1
	beq.s	.setA
	move.w	d0,vScrollB
	bra.s	.ok
.setA:
	move.w	d0,vScrollA
.ok:
	move.w	d0,d1
	and.w	#$7,d1				; are we on char boundry?
	bne.s	.Nope				; if not, we are done

	move.w	d0,d3				; place xpos in d3
	lsr.w	#3,d3				; convert it into column address
	add.w	#VDP_SCREEN_HEIGHT,d3		; offset to bottom of screen
	move.w	d3,d1
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of comlunms

	clr.l	d0
	move.w	Scroll_XPos(a0),d0		; get x offset so we start in right place
	asr.w	#3,d0				; convert x & y into char offsets
	move.w	d0,d2
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of comlunms

	move.w	Scroll_MapWidth(a0),d5		; constrain to map by taking
	sub.w	#1,d5				; modulus of map width
	and.w	d5,d0

	bsr	DownloadMetaRow

.Nope:

	rts

;==============================================================================

ScrollMetaLeft:

	move.w	Scroll_XPos(a0),d0		; decrement x position
	subq.w	#1,d0
	move.w	d0,Scroll_XPos(a0)

	neg.w	d0				; output horz offset
	move.w	Field_Num(a0),d1
	beq.s	.setA
	move.w	d0,hScrollB
	bra.s	.ok
.setA:
	move.w	d0,hScrollA
.ok:

	move.w	Scroll_XPos(a0),d0
	add.w	#1,d0				; fixes missing character bug

	move.w	d0,d1
	and.w	#$7,d1
	bne.s	.Nope

	move.w	d0,d2				; place xpos in d2
	asr.w	#3,d2				; convert it into column address
	subq.w	#1,d2				; offset to left of screen

	clr.l	d0
	move.w	d2,d0				; make map offset
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of rows (constrain to field)

	move.w	Scroll_MapWidth(a0),d5		; constrain to map by taking
	sub.w	#1,d5				; modulus of map width
	and.w	d5,d0

	move.w	Scroll_YPos(a0),d1
	lsr.w	#3,d1
	move.w	d1,d3
	and.w	#VDP_FIELDA_HEIGHT-1,d3
	bsr	DownloadMetaColumn
.Nope:

	rts

;==============================================================================

ScrollMetaRight:

	move.w	Scroll_XPos(a0),d0		; increment x position
	addq.w	#1,d0
	move.w	d0,Scroll_XPos(a0)

	neg.w	d0				; output horz offset
	move.w	Field_Num(a0),d1
	beq.s	.setA
	move.w	d0,hScrollB
	bra.s	.ok
.setA:
	move.w	d0,hScrollA
.ok:

	move.w	Scroll_XPos(a0),d0
	move.w	d0,d1
	and.w	#$7,d1				; are we on x char boundry?
	bne.s	.Nope				; if not, we are done

	move.w	d0,d2				; place xpos in d2
	asr.w	#3,d2				; convert it into column address
	add.w	#VDP_SCREEN_WIDTH,d2		; offset to right of screen
	clr.l	d0
	move.w	d2,d0				; save x map address
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of columns (constrain to field space)

; constrain to map

	move.w	Scroll_MapWidth(a0),d5		; constrain to map by taking
	sub.w	#1,d5				; modulus of map width
	and.w	d5,d0				; if negetive number

	move.w	Scroll_YPos(a0),d1		; get x offset so we start in right place
	lsr.w	#3,d1				; convert x & y into char offsets
	move.w	d1,d3
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of rows
	jsr	DownloadMetaColumn
.Nope:

	rts

;==============================================================================
; Inputs:
;	a0-> scroll structure
;	d0.w = x position in map to start from(char)
;	d1.w = y positon in map to start from(char)
;	d2.w = column in field(in vdp) to draw to(char)
;	d3.w = row in field(in vdp) to draw to(char)
;------------------------------------------------------------------------------

DownloadMetaRow:

	jsr	CalcMapAddr			; return map addr in a1

	movem.w	d0-d2,-(sp)
	move.w	d2,d0				; get field coordinates
	move.w	d3,d1
	move.l	Scroll_ScreenAddrRout(a0),a3	; set VDP start address
	jsr	(a3)
	movem.w	(sp)+,d0-d2

	move.w	#VDP_SCREEN_WIDTH+1,d7		; number of chars to xfer
	move.w	Scroll_CharBase(a0),d6		; character base addr in VDP
	move.l	Scroll_CompTileTbl(a0),a2	; address of blk table

	btst	#0,d1				; test if even row
	bne	.DLRO				; jump if not to odd row routines

;-----------------------------------------------
;	Download routines for rows on even
;	character rows

.DLRE:
	btst	#0,d0				; test if starting at odd column
	beq	.DLREoddLoop			; if not, start at end of odd column routine
	bra	.DLREevenLoop			; is so, start at end of even loop routine

.DLREeven:
	move.w	d0,d2
	and.w	#MTW_G-1,d2			; check if on new meta tile
	bne.s	.DLREeven2			; jump if not
	jsr	CalcMapAddr			; calc new map address
	movem.w	d0-d1,-(sp)
	clr.w	d0
	and.w	#VDP_FIELDA_HEIGHT-1,d1
	move.l	Scroll_ScreenAddrRout(a0),a3	; set VDP start address
	jsr	(a3)
	movem.w	(sp)+,d0-d1

.DLREeven2:
	clr.l	d5
	move.w	(a1),d5				; get blk table index
        asl.l   #BLOCKSHIFT,d5                  ; index => byte offset
	move.w	0(a2,d5.l),d2			; get character map element
	add.w	d6,d2				; add character base offset
	WRITE_VDP_DATA d2				; write character out to VDP
	add.w	#1,d0				; increment map x position

.DLREevenLoop:
	dbra	d7,.DLREodd			; branch if not done
	bra	.DLRend				; done

.DLREodd:
	clr.l	d5
	move.w	(a1)+,d5			; get blk table pointer
        asl.l   #BLOCKSHIFT,d5                  ; index => byte offset
	move.w	2(a2,d5.l),d2			; get character map element
	add.w	d6,d2				; add character base offset
	WRITE_VDP_DATA d2				; write character out to VDP
	add.w	#1,d0				; increment map x position

.DLREoddLoop:
	dbra	d7,.DLREeven			; branch if not done
	bra	.DLRend


;-----------------------------------------------
;	Download routines for rows on odd
;	character rows

.DLRO:
	btst	#0,d0				; test if starting at odd column
	beq	.DLROoddLoop			; if not, start at end of even column routine
	bra.s	.DLROevenLoop			; if so, start at end of odd column routine

.DLROeven:
	move.w	d0,d2
	and.w	#MTW_G-1,d2			; check if on new meta tile
	bne.s	.DLROeven2			; jump if not
	jsr	CalcMapAddr			; calc new map address
	movem.w	d0-d1,-(sp)
	clr.w	d0
	and.w	#VDP_FIELDA_HEIGHT-1,d1
	move.l	Scroll_ScreenAddrRout(a0),a3	; set VDP start address
	jsr	(a3)
	movem.w	(sp)+,d0-d1

.DLROeven2:
	clr.l	d5
	move.w	(a1),d5				; get blk table pointer
        asl.l   #BLOCKSHIFT,d5                  ; index => byte offset
	move.w	4(a2,d5.l),d2			; get character map element
	add.w	d6,d2				; add character base offset
	WRITE_VDP_DATA d2				; write character out to VDP
	add.w	#1,d0				; increment map x position

.DLROevenLoop:
	dbra	d7,.DLROodd			; branch if not done
	bra	.DLRend				; done

.DLROodd:
	clr.l	d5
	move.w	(a1)+,d5			; get blk table pointer
        asl.l   #BLOCKSHIFT,d5                  ; index => byte offset
	move.w	6(a2,d5.l),d2			; get character map element
	add.w	d6,d2				; add character base offset
	WRITE_VDP_DATA d2				; write character out to VDP
	add.w	#1,d0				; increment map x position

.DLROoddLoop:
	dbra	d7,.DLROeven			; branch if not done

;-----------------------------------------------

.DLRend:

	rts

;------------------------------------------------------------------------------
;	Inputs:
;	a0-> scroll structure
;	d0.w = x position in map to start from(char)
;	d1.w = y positon in map to start from(char)
;	d2.w = column in field(in vdp) to draw to(char)
;	d3.w = row in field(in vdp) to draw to(char)


DownloadMetaColumn:
;	move.w	#VDP_REG_AUTOINCSIZE+(VDP_FIELDA_WIDTH*2),VDP_CMD		; set up auto-increment
	WRITE_VDP_CMD.w #VDP_REG_AUTOINCSIZE+(VDP_FIELDA_WIDTH*2)
;                        ; set up auto-increment
;				; first find correct address in map

	jsr	CalcMapAddr			; return map addr in a1

	movem.w	d0-d2,-(sp)
	move.w	d2,d0				; get field coordinates
	move.w	d3,d1
	move.l	Scroll_ScreenAddrRout(a0),a3	; set VDP start address
	jsr	(a3)
	movem.w	(sp)+,d0-d2

	move.w	#VDP_SCREEN_HEIGHT+1,d7		; number of chars to xfer
	move.w	Scroll_CharBase(a0),d6		; character base addr in VDP
	move.l	Scroll_CompTileTbl(a0),a2	; address of blk table

	btst	#0,d0				; test if even column
	bne	.DLCO				; jump if not to odd column routines

;-----------------------------------------------
;	Download routines for even columns

.DLCE:
	btst	#0,d1				; test if starting at odd row
	beq	.DLCEoddLoop			; if not, start at end of odd column routine
	bra	.DLCEevenLoop			; is so, start at end of even loop routine

.DLCEeven:
	move.w	d1,d2
	and.w	#MTH_G-1,d2			; check if on new meta tile
	bne.s	.DLCEeven2			; jump if not
	jsr	CalcMapAddr			; calc new map address
	movem.w	d0-d1,-(sp)
	clr.w	d1				; constrain to field
	and.w	#VDP_FIELDA_WIDTH-1,d0
	move.l	Scroll_ScreenAddrRout(a0),a3	; set VDP start address
	jsr	(a3)
	movem.w	(sp)+,d0-d1

.DLCEeven2:
	clr.l	d5
	move.w	(a1),d5				; get blk table pointer
        asl.l   #BLOCKSHIFT,d5                  ; index => byte offset
	move.w	0(a2,d5.l),d2			; get character map element
	add.w	d6,d2				; add character base offset
	WRITE_VDP_DATA d2				; write character out to VDP
	add.w	#1,d1				; increment map x position

.DLCEevenLoop:
	dbra	d7,.DLCEodd			; branch if not done
	bra	.DLCend				; done

.DLCEodd:
	clr.l	d5
	move.w	(a1),d5				; get blk table pointer
        asl.l   #BLOCKSHIFT,d5                  ; index => byte offset
	move.w	4(a2,d5.l),d2			; get character map element
	add.w	d6,d2				; add character base offset
	WRITE_VDP_DATA d2				; write character out to VDP
	add.w	#1,d1				; increment map x position
	add.l	#MTW_C*2,a1

.DLCEoddLoop:
	dbra	d7,.DLCEeven			; branch if not done
	bra	.DLCend


;-----------------------------------------------
;	Download routines odd columns

.DLCO:
	btst	#0,d1				; test if starting at odd column
	beq	.DLCOoddLoop			; if not, start at end of odd row routine
	bra.s	.DLCOevenLoop			; if so, start at end of even row routine

.DLCOeven:
	move.w	d1,d2
	and.w	#MTH_G-1,d2			; check if on new meta tile
	bne.s	.DLCOeven2			; jump if not
	jsr	CalcMapAddr			; calc new map address
	movem.w	d0-d1,-(sp)
	clr.w	d1				; constrain to map
	and.w	#VDP_FIELDA_WIDTH-1,d0
	move.l	Scroll_ScreenAddrRout(a0),a3	; set VDP start address
	jsr	(a3)
	movem.w	(sp)+,d0-d1

.DLCOeven2:
	clr.l	d5
	move.w	(a1),d5				; get blk table pointer
        asl.l   #BLOCKSHIFT,d5                  ; index => byte offset
	move.w	2(a2,d5.l),d2			; get character map element
	add.w	d6,d2				; add character base offset
	WRITE_VDP_DATA d2				; write character out to VDP
	add.w	#1,d1				; increment map x position

.DLCOevenLoop:
	dbra	d7,.DLCOodd			; branch if not done
	bra	.DLCend				; done

.DLCOodd:
	clr.l	d5
	move.w	(a1),d5				; get blk table pointer
        asl.l   #BLOCKSHIFT,d5                  ; index => byte offset
	move.w	6(a2,d5.l),d2			; get character map element
	add.w	d6,d2				; add character base offset
	WRITE_VDP_DATA d2				; write character out to VDP
	add.w	#1,d1				; increment map x position
	add.l	#MTW_C*2,a1

.DLCOoddLoop:
	dbra	d7,.DLCOeven			; branch if not done

;-----------------------------------------------

.DLCend:

;	move.w	#VDP_REG_AUTOINCSIZE+2,VDP_CMD
	WRITE_VDP_CMD.w #VDP_REG_AUTOINCSIZE+2
	rts


;==============================================================================
;
;	ClacMapAddr
;
;	Given virtual map column,row coordinates in d0,d1, a1
; 	will be set to the address of the map element
;
;	returns:    a1 = map address
;
;	destroys:  d4,d5,a3

CalcMapAddr:

	movem.w	d0-d3,-(sp)


; calculate meta tile number

	move.w	Scroll_MapWidthMask(a0),d4
	and.w	d4,d0				; constrain to map and divide by width of
	move.w	d0,d2
	lsr.w	#MTW_G_SH,d0			; metatile in characters to get meta tile x pos

	move.w	Scroll_MapHeightMask(a0),d4
	and.w	d4,d1				; constrain to map and divide by width of
	move.w	d1,d3
	lsr.w	#MTH_G_SH,d1			; metatile in characters to get meta tile y pos

	move.w	Scroll_MetaWidthSh(a0),d5
	lsl.w	d5,d1				; multiply meta map y pos by meta map width
	add.w	d0,d1				; add meta map x position
	add.w	d1,d1				; d1 contains offset into meta map
	move.l	Scroll_MetaMap(a0),a3		; get meta map address
	clr.l	d0
	move.w	(a3,d1.w),d0			; d0.l contains meta map number
	lsl.l	#MTW_C_SH,d0			; multiply by # of tiles in meta tile
	lsl.l	#MTH_C_SH+1,d0			; d0 contains actual addr offset into meta tile

; calculate offset into meta tile

	lsr.w	#1,d2				; convert to tile coordinates
	lsr.w	#1,d3

	and.w	#MTH_C-1,d3
	lsl.w	#MTW_C_SH,d3
	and.w	#MTW_C-1,d2
	add.w	d3,d2
	add.w	d2,d2				; d2 contains offset into meta tile

; add meta tile offset to within tile offset

	and.l	#$ffff,d2
	add.l	d2,d0
	move.l	Scroll_MapPtr(a0),a1		; add map base address
	add.l	d0,a1				; a1 contains map address

	movem.w	(sp)+,d0-d3
	rts

	ENDC

;====================================================================


	BSECTION	.bss

; DO NOT CHANGE THE ORDER OF THESE VARIABLES WITHOUT CHANGING "GetScrollRegs"

hScrollA	ds.w	1
vScrollA	ds.w	1		; value to write to reg during VBLANK
hScrollB	ds.w	1
vScrollB	ds.w	1



;==============================================================================

	END

;==============================================================================

