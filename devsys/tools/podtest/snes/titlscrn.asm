;============================================================================
;
; File:    titlscrn.asm
;
; Project: Game Genie (SNES)
;
; History:
;    8 Jun 92   WBNIV   Created
;
;============================================================================


;============================================================================

CODE    SEGMENT

TitleScreen_Display:
	;----- Invented by Codemasters...
	a8
	lda     #1
	sta     BG_MODE

	lda     #1
	sta     TM
	mode16

	jsr	|BgZero

	lda	#<CodeMastPal
	sta	lpPalette
	lda	#>CodeMastPal
	sta	lpPalette+1
	ldx	#16
	ldy	#0
	jsr	NewSetPalette

	LoadChr	CodeMast,#$4000

	;----- Load map
	lda     #$0800                  ;BG
	sta     scrBase

	lda     #CodeMastMap
	sta     mapPtrL
	lda     #>CodeMastMap
	sta     mapPtrL+1

	stz     CHARMASK

	jsr     ExtractMapToScreenFull

	lda     #224			;128
	jsr     |set_bg2_pos
	
	jsr     |FadeIn

.10 
	jsr     |set_bg2_pos
	sec
	sbc	#4
	bpl     .10

	rts
	

set_bg2_pos:
	waitvb
	a8
	sta     BG1VOFS
	xba
	sta     BG1VOFS
	xba
	a16

	rts


	END

