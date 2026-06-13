
;============================================================================
;
; File:    map.asm
;
; Project: Where's Waldo? (SNES)
;
; History:
;       30 Mar 92       WBNIV   Created
;       10 May 92       WBNIV   Fixed bug in CopyMap (CHARMASK shouldn't
;                                affect the 2-word header of the map!)
;
;============================================================================


;
; Forced blank (or V Blank) are assumed
;

;============================================================================

MapHdr	DEFS	RAM
MapHdr	SEGMENT
mapHdrWidth	ds	2
mapHdrHeight	ds	2
mapHdrData
MapHdr_SIZEOF


;============================================================================

SRAM    SEGMENT
BG1_0	DS	32*32*2
BG2_0	DS	32*32*2
BG3_0	DS	32*32*2


;============================================================================

ZPAGE   SEGMENT
mapWidth        DS      2
dstPtr          ds      2

CHARMASK        DS      2
xMapOffset      DS      2
scrBase         DS      2
xScrColumn      DS      2
yScrRow         DS      2

srcPtr          DS      2
ptr             DS      2

MAPPTR  ds      2               	; pointer to MAP data
	ORG     PC-2
mapPtrL ds      3
mapPtrL_d       ds      3               ; destination map pointer
modulus         ds      2

yMapHeight      ds      2
xMapWidth	ds	2

nMapTemp	ds	2

nMapModulus	ds	2
nScrModulus	ds	2


;============================================================================

CODE    SEGMENT

;
; xMapOffset (subtract this from calculated width)
;
; xMapWidth
; yMapHeight
;
; xScrColumn (not totally ok, doesn't clip)
; yScrRow
;
; CHARMASK
;
ExtractMapToRam:
	PUSH_ALL

	mode16
	lda	scrBase
	pha
	lda	yMapHeight
	pha
	lda	xMapWidth
	pha

;^	lda	xMapWidth
	cmp	#-1
	bne	.05
	yLDA	[mapPtrL],#mapHdrWidth
	cmp	#32
	bcc	.05
	lda	#32
.05
	sta	nMapTemp
	dec	a
	asl	a
	sta	xMapWidth

	yLDA	[mapPtrL],#mapHdrWidth
	sec
	sbc	nMapTemp
	asl	a
	sta	nMapModulus

	lda	#32
	sec
	sbc	nMapTemp
	asl	a
	sta	nScrModulus


	lda	yMapHeight
	cmp	#-1
	bne	.06
	yLDA	[mapPtrL],#mapHdrHeight
.06
	dec	a
	asl	a
	sta	yMapHeight

	;----- VMADDL
	lda	#32			; width of physical screen
	ldx	yScrRow
	jsr	Multiply
	clc
	adc	scrBase
	adc	xScrColumn
	adc	xScrColumn
	sta	scrBase

	lda	#mapHdrData
	clc
	adc	xMapOffset
	adc	xMapOffset
	tay

.07
	ldx	xMapWidth
.10	lda	[mapPtrL],y
	clc
	adc	CHARMASK
	sta	(scrBase)		;;
	inc	scrBase			;;
	inc	scrBase			;;
	iny
	iny
	dex
	dex
	bpl	.10

	lda	scrBase
	clc
	adc	nScrModulus
	sta	scrBase

	tya
	clc
	adc	nMapModulus
	tay

	dec	yMapHeight
	dec	yMapHeight
	bpl	.07

	pla
	sta	xMapWidth
	pla
	sta	yMapHeight
	pla
	sta	scrBase

	PULL_ALL
	rts


; ExtractColumnFromMapToRam
;
; Inputs:
;	mapPtrL
;	xMapOffset
;	xScrColumn
;	yScrRow
;
; Destroys:
;	(requires mode16 to call) ?
;	All registers
;

	LONGA	ON
	LONGI	ON

ExtractColumnFromMapToRam:
	PUSH_ALL
	mode16

	;----- Calculate srcPtr ( srcPtr = &map.data + (map.width*yMapOffset) + x )
	clc
	lda     #2
	adc     xMapOffset
	asl	a
	tay

	;----- Calculate dstPtr ( dstPtr = scrBase + (yScrOffset*32) + x )
	lda     scrBase
	clc
	adc     xScrColumn
	adc     xScrColumn

	ldx     yScrRow
.3      cpx     #0
	beq     .4

	clc
;;	adc     #32*2
	adc	#32

	dex
	bra     .3

.4
	sta     dstPtr

	lda     [mapPtrL]
	asl     a
	sta     mapWidth                ; width of map data (in bytes)

	phy
	ldx     #31
	ldy     #2
	lda     [mapPtrL],y
	cmp     #32
	bcs     .5
	dec     a
	bra     .6
.5:     lda     #31
.6:     tax
	ply
.10
	lda     [mapPtrL],y
;	ora     CHARMASK
	clc
	adc	CHARMASK
	sta     (dstPtr)

	lda     dstPtr
	clc
	adc     #32*2
;;	adc	#32
	sta     dstPtr

	;----- Add width of map
	tya
	clc
	adc     mapWidth
	tay

	dex
	bpl     .10

	PULL_ALL
	rts


	END

