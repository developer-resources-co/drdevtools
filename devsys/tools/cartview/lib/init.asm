

CODE	SEGMENT
VDPInit:
	PUSH_ALL
	mode8

	ldx	#0			; X always zero
	ldy	#1			; Y always one

	LDA	#$8f
	STA	|$2100
	STx  |$2101
	STx  |$2102
	STx  |$2103
;;	STx  |$2104
	STx  |$2105
	STx  |$2106
	STx  |$2107
	STx  |$2108
	STx  |$2109
	STx  |$210a
	STx  |$210b
	STx  |$210c

	STx  |$210d
	STx  |$210d
	STx  |$210e
	STx  |$210e
	STx  |$210f
	STx  |$210f
	STx  |$2110
	STx  |$2110
	STx  |$2111
	STx  |$2111
	STx  |$2112
	STx  |$2112
	STx  |$2113
	STx  |$2113
	STx  |$2114
	STx  |$2114
	LDA  #$80
	STA  |$2115
	STx  |$2116
	STx  |$2117
;;	stx	$2118	;?
;;	stx	$2119	;?
	STx  |$211a
	STx  |$211b
	STy  |$211b
	STx  |$211c
	STx  |$211c
	STx  |$211d
	STx  |$211d
	STx  |$211e
	STy  |$211e
	STx  |$211f
	STx  |$211f
	STx  |$2120
	STx  |$2120
	STx  |$2121
;?	$2122
	STx  |$2123
	STx  |$2124
	STx  |$2125
	STx  |$2126
	STx  |$2127
	STx  |$2128
	STx  |$2129
	STx  |$212a
	STx  |$212b
	STy  |$212c
	STx  |$212d
	STx  |$212e
	STx  |$212f
	LDA  #$30
	STA  |$2130
	STx  |$2131
	LDA  #$e0
	STA  |$2132
	STx  |$2133

	STx  |$4200
	LDA  #$ff
	STA  |$4201
	STx  |$4202
	STx  |$4203
	STx  |$4204
	STx  |$4205
	STx  |$4206
	STx  |$4207
	STx  |$4208
	STx  |$4209
	STx  |$420a
	STx  |$420b
	STx  |$420c
	stx	|$420d

	PULL_ALL
	rts


ClearVDPMem:
	php
	mode16
	a8
	lda	#$80
	sta	|VMAINC
	a16
	ldx	#$8000			; # of chars to wipe
	lda	#$0
	sta	|vmaddl			; set VDP address
.wipe:
	sta	|vmdatal
	dex
	bne	.wipe

	plp
	rts


;=============================================================================

InitSFX:
	php
	mode8

	IFDEF	DEBUG
	stz	NMI_Shadow
	stz	HDMA_Shadow
	ENDIF

	LDA	#$0
	STA	|NMITIMEN
	jsr	|VDPInit
;?	jsr	|ClearVDPMem

	plp
	rts


;=============================================================================

	end

