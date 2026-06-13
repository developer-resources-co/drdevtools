;============================================================================
;
; File:    titlscrn.asm
;
; Project: SNES Graphics Downloader
;	 Copyright 1994 Developer Resources.  All Rights Reserved.
;
; History:
;
;============================================================================

ZPAGE	SEGMENT

bInitialized	ds	2
nLength		ds	2		; length of transferred data

;----- keep in this order and grouping!
nBg1Hpos		ds	2
nBg2Hpos		ds	2

nBg1Vpos		ds	2
nBg2Vpos		ds	2


CODE	SEGMENT

MoveBg:
	PUSH_ALL

	a16
	lda	nBg1Hpos
	a8
	sta	BG1HOFS
	xba
	sta	BG1HOFS

	a16
	lda	nBg1Vpos
	a8
	sta	BG1VOFS
	xba
	sta	BG1VOFS


	a16
	lda	nBg2Hpos
	a8
	sta	BG2HOFS
	xba
	sta	BG2HOFS

	a16
	lda	nBg2Vpos
	a8
	sta	BG2VOFS
	xba
	sta	BG2VOFS

	PULL_ALL
	rts


InitBgPos:
	PUSH_ALL
	mode16

	stz	nBg1Hpos
	stz	nBg2Hpos
	lda	# -(224-200)/2
	sta	nBg1Vpos
	sta	nBg2Vpos

	jsr	MoveBg

	PULL_ALL
	rts


TitleScreen_Display:
	PUSH_ALL
	mode16

	IFDEF TITLESCREEN
	LoadChr	drlogo,#$2000

	lda	#(1<10)
	sta	CHARMASK

	lda	#BG1_0
	sta	scrBase
	lda	#drLogoMap
	sta	mapPtrL
	lda	#>drLogoMap
	sta	mapPtrL+1

	lda	#0
	sta	xMapOffset

	sta	xScrColumn
	sta	yScrRow

	lda	#-1
	sta	xMapWidth
	sta	yMapHeight
	jsr	ExtractMapToRAM

	jsr	InitBgPos
	lda	#-73
	sta	nBg1Hpos
	lda	#-16
	sta	nBg1Vpos

	ENDIF

	PULL_ALL
	rts


	END

