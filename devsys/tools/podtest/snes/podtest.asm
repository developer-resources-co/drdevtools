
;============================================================================
;
; File:    podtest.asm
;
; Project: 
;
; History:
;    
;
;============================================================================

FADE

	INCLUDE segments.asm

	INCDIR  l:/snes/lib
	INCLUDE snes.equ
	INCLUDE snes.mac
	INCDIR

;============================================================================

	LIST	ON

BANK0	SEGMENT

bootstrap:
	jml	main



CODE    SEGMENT

	INCLUDE	podtest.mac

	INCDIR	l:/snes/lib
	INCLUDE random.asm
	INCLUDE	decomp.asm
	INCLUDE	math.asm
	INCLUDE	map.asm
	INCLUDE	snes.asm
	INCLUDE bin.mac
	INCDIR

	CHRINC	codemast,chr,lbm/
	MAPINC	codemast,map,lbm/
	PALINC	codemast,lbm/

	INCLUDE	titlscrn.asm


CODE	SEGMENT

main:
	SEI                     ; disable interrupts

	CLC
	XCE                     ; leave emulation mode

	mode16
	LDA     #$01FF
	TCS                     ; initialize stack

TotalRecall:
	mode16
	LDA     #$01FF
	TCS                     ; initialize stack

	JSR     |InitSFX

	a8
	lda     #(2<2)|2
	sta     BG1SC
	lda     #(4<2)|1
	sta     BG2SC
	lda     #(6<2)|1
	sta     BG3SC

	lda     #$24            ; bg2 charbase / bg1 charbase
	sta     BG12NBA
	lda     #$06
	sta     BG34NBA
	a16

Restart:
	mode16
	jsr     |ClearVDPMem
	JSR     |TitleScreen_Display

	sei
	bra	TotalRecall
;.here	bra	.here

	INCLUDE	header.asm
	INCLUDE	vector.asm			; change!!! for testing

	END


;============================================================================

