VECTOR	DEFS	ROM

VECTOR	SEGMENT
;	org	$fffa
;	porg	$7ffa

	org	$ffe8
	porg	$7fe8

MonBreak816	equ	0	;
MonNMI		equ	0	;
MonBreak	equ	0	;

	; 65816 mode vectors
	dw	0			; abort $ffe8

	IFDEF	NMI_Code
	dw	NMI_Code		; nmi  $ffea
	ENDIF

	IFNDEF	NMI_Code
	dw	0
	ENDIF

	dw	0			; reserved $ffec
	dw	0			; irq $ffee

	; emulation mode vectors
	dw	0			; fff0
	dw	0			; fff2
	dw	0			; cop $fff4
	dw	0			; reserved $fff6

	dw	0			; abort $fff8
	dw	0			; nmi  $fffa



	dw	bootstrap		; reset $fffc
	dw	0			; irq/break $fffe

;=============================================================================

	END

