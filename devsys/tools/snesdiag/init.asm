VDPInit:
	php
	mode8
	LDA  #$8f
	STA  $2100
	LDA  #$01
	STA  $2101
	LDA  #$00
	STA  $2102
	STA  $2103
	LDA  #$02
	STA  $2105
	LDA  #$00
	STA  $2106
	LDA  #$01
	STA  $2107
	LDA  #$11
	STA  $2108
	LDA  #$7a
	STA  $2109
	LDA  #$79
	STA  $210a
	LDA  #$0
	STA  $210b
	LDA  #$77
	STA  $210c
	LDA  #$00
	STA  $210d
	STA  $210d
	STA  $210e
	STA  $210e
	STA  $210f
	STA  $210f
	STA  $2110
	STA  $2110
	STA  $2111
	STA  $2111
	STA  $2112
	STA  $2112
	STA  $2113
	STA  $2113
	STA  $2114
	STA  $2114
	LDA  #$80
	STA  $2115
	LDA  #$00
	STA  $2116
	STA  $2117
	STA  $211a
	STA  $211b
	LDA  #$01
	STA  $211b
	LDA  #$00
	STA  $211c
	STA  $211c
	STA  $211d
	STA  $211d
	STA  $211e
	LDA  #$01
	STA  $211e
	LDA  #$00
	STA  $211f
	STA  $211f
	STA  $2120
	STA  $2120
	STA  $2121
	STA  $2123
	STA  $2124
	STA  $2125
	STA  $2126
	STA  $2127
	STA  $2128
	STA  $2129
	STA  $212a
	STA  $212b
	LDA  #$1
	STA  $212c
	LDA  #$0	; #$17
	STA  $212d
	LDA  #$00
	STA  $212e
	STA  $212f
	LDA  #$30
	STA  $2130
	LDA  #$00
	STA  $2131
	LDA  #$e0
	STA  $2132
	LDA  #$00	; #$0
	STA  $2133
	lda  #0 ; #$80	; turn on NMI
	STA  $4200
	LDA  #$ff
	STA  $4201
	LDA  #$00
	STA  $4202
	STA  $4203
	STA  $4204
	STA  $4205
	STA  $4206
	STA  $4207
	STA  $4208
	STA  $4209
	STA  $420a
	STA  $420b
	STA  $420c

	plp
	rts


ClearVDPMem:
	php
	mode8
	blank
	mode16
	a8
	lda	#$80
	sta	VMAINC
	a16
	ldx	#$8000			; # of chars to wipe
	lda	#$0
	sta	vmaddl			; set VDP address
?wipe:
	sta	vmdatal
	dex
	bne	?wipe

	mode8
	unblank
	plp
	rts


;=============================================================================

InitSFX:
	mode8
	LDA  #$0
	STA  $4200		;
	LDA  #$80
	STA  $2100		; setup hardware?

	jsr	VDPInit
	jsr	ClearVDPMem
;	jsr	InitVDP
;	jsr	InitCPU

;	mode16
;	lda	#NMIVEC
;	sta	$ffea
;	mode8

	LDA  #$0f
	STA  $2100			; turn video on
	rts


;=============================================================================


VECTOR	DEFS	ROM

VECTOR	SEGMENT
;	org	$fffa
;	porg	$7ffa

	org	$ffe8
	porg	$7fe8

MonBreak816	equ	0	;
MonNMI		equ	0	;
MonBreak	equ	0	;

	; 8/16 mode vectors
	dw	$2ff			; abort $ffe8

	dw	NMIVEC			; nmi  $ffea
	dw	0			; reserved $ffec
	dw	MonBreak		; irq $ffee

	; emulation mode vectors
	dw	0			; fff0
	dw	0			; fff2
	dw	0			; cop $fff4
	dw	0			; reserved $fff6

	dw	$37c			; abort $fff8
	dw	NMIVEC			; nmi  $fffa



	dw	Top			; reset $fffc
	dw	MonBreak		; irq/break $fffe

;=============================================================================


	end
