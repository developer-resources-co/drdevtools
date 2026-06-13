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

	dw	VBI			; nmi  $ffea

	dw	bootstrap			; reserved $ffec

	IFDEF	IRQ_Code
	dw	IRQ_Code
	ENDIF

	IFNDEF	IRQ_Code
	dw	0
	ENDIF

	; emulation mode vectors
	dw	0			; fff0
	dw	0			; fff2
	dw	0			; cop $fff4
	dw	0			; reserved $fff6

	dw	$37c			; abort $fff8
	dw	0			; nmi  $fffa



	dw	bootstrap		; reset $fffc
	dw	MonBreak		; irq/break $fffe

;=============================================================================

	END

