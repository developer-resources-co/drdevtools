;==============================================================================
;===    Macros.asm: some useful macros                                     ===
;===	(c)1991,92 Developer Resources					   ===
;==============================================================================

;==============================================================================
; macros cause the assembler sux

string	macro
	dt	{1}
	dc.b	0
	endm

;==============================================================================
; gfx macros
;------------------------------------------------------------------------------

spriteinc	MACRO
{1}SPRCHR:
	incbin	{1}.chr
{1}SIZE		equ	pc-{1}SPRCHR
{1}spr:
	incbin	{1}.spr
	ENDM

;------------------------------------------------------------------------------

palinc	macro
{1}Pal
	incbin	{1}.pal
	even
{1}NumCols	equ	(pc-{1}Pal)/2
	endm

;------------------------------------------------------------------------------

chrinc	macro
{1}Chr
	incbin	{1}.chr
	even
{1}Size	equ	pc-{1}Chr
	endm

;------------------------------------------------------------------------------

rlechrinc	macro
{1}RLEChr
	incbin	{1}.rle
{1}Size	equ	@2({1}RLEChr)
	even
	endm


rleinc	macro
{1}RLE
	incbin	{1}.rle
	even
	endm

;------------------------------------------------------------------------------

mapinc	macro
{1}Map
	incbin	{1}.map
	even
	endm

;------------------------------------------------------------------------------
;	1 = name of palette
; 	2 = starting palette #(optional, default = 0)

SetPal	macro
	lea	{1}Pal,a0
	ifnot	{N}-2
	move.w	{2},d0
	endif
	if	{N}-2
	clr.w	d0
	endif
	move.w	#{1}NumCols,d1
	jsr	SetPalette
	endm

;==============================================================================
;	1 = name of gfx
; 	2 = vdp starting address
;------------------------------------------------------------------------------

LoadChr	macro
				; copy the graphics into VRAM
	lea	{1}Chr,a0		; address of chars in ROM
	move.l	#{2},d0			; location in VDP to place chars
	move.l	#{1}Size/2,d1		; # of chars
	jsr	DMACopy         	; dma the char definitions
	endm

;==============================================================================
;	1 = name of gfx
; 	2 = vdp starting address
;------------------------------------------------------------------------------

LoadRLEChr	macro
				; copy the graphics into VRAM
	move.w	{2},d0                 ; location in VDP to place chars
	jsr	VDPAddress
	lea	{1}RLEChr,a0		; address of chars in ROM
	jsr	DecompressToVDP
	endm

;==============================================================================
;	1 = name of gfx
; 	2 = vdp starting address
;------------------------------------------------------------------------------

LoadSprite	macro
				; copy the graphics into VRAM
	lea	{1}sprChr,a0		; address of chars in ROM
	move.l	#{2},d0			; location in VDP to place chars
	move.l	#{1}Size/2,d1		; # of chars
	jsr	DMACopy         	; dma the char definitions
	endm

;==============================================================================
; 1 = playfield to plot into
; 2 = x pos
; 3 = y pos
; 4 = starting char
; 5 = name of map
;------------------------------------------------------------------------------

PlotMap	macro
	move.w	#{2},d0
	move.w	#{3},d1
	move.w	#{4},d2
	lea	{5}Map,a0
	jsr	PlotMap{1}
	endm

;==============================================================================
; pseudo-ops

COLORCYCLE      MACRO
        move.l #VDP_MODE_CRW,VDP_CMD    ; self-explanatory
        move.w VDP_HVCOUNT,VDP_DATA
                ENDM

SETBACKCOLOR        MACRO                   ; set background color to input
        move.l #VDP_MODE_CRW,VDP_CMD
        move.w #{1},VDP_DATA
        ENDM

FREEZE  MACRO                           ; just lock up
.Loop{L}:
	COLORCYCLE
        BRA.S   .Loop{L}
        ENDM

;==============================================================================
;A COUPLE SENTIMENTAL INTEL-STYLE MACROS

PUSH	DEFM
	MOVEM{X} {1},-(SP)
	ENDM

POP	DEFM
	MOVEM{X} (SP)+,{1}
	ENDM

;==============================================================================

PUSHALL MACRO
        movem.l d0-d7/a0-a6,-(sp)
        ENDM

POPALL  MACRO
        movem.l (sp)+,d0-d7/a0-a6
        ENDM

;============================================================================

COLORVBI        macro
        move.l  #VDP_MODE_CRW,VDP_CMD
        move.w  RASTER,VDP_DATA
        endm

EI      macro
        andi    #%1111100011111111,SR
        endm

DI      macro
        ori     #%0000011100000000,SR
        endm

VIDEO_ON macro
        move.w  #$8174,VDP_CMD
        endm

VIDEO_OFF macro
        move.w  #$8134,VDP_CMD
        endm

;==============================================================================
; wait for vblank to start

WAITVB  macro
.Loop{L}
        cmpi.b  #VBRAST,VDP_RASTER
        bcc.s   .Loop{L}				; first insure we are not already there
.Loop2{L}
        cmpi.b  #VBRAST,VDP_RASTER
        bcs.s   .Loop2{L}
        endm

;==============================================================================

WAITVBSOUND	macro
	ifdef	SOUNDDRIVER
	PUSHALL
	jsr	SoundTick
	POPALL
	endif
	WAITVB
	endm

;------------------------------------------------------------------------------

	end

;==============================================================================


