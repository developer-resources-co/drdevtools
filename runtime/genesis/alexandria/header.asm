;==============================================================================
; header.asm: generic genesis header
;==============================================================================

	TSECTION  .header
;header	segment

	dc.b	'SEGA GENESIS    '	; Platform name.
	dc.b	'(C) ALEXANDRIA  '	; Your message here.
	dc.b	'GAMESHELL       '	; Title of game.
	dc.b	'                '
	dc.b	'                '
	dc.b	'GAMESHELL       '	; Title again.
	dc.b	'                '
	dc.b	'                '
	dc.b	'GM T-00000 -00'	; mystery stuff.  Assigned by SOA

	dc.b	'00'								; checksum
	dc.b	'                '	; control data
	dc.l	0,$7ffff		; has to do with size.
	dc.l	$ff0000,$ffffff		; RAM you use.
	dc.b	'                '	;
	dc.b	'                '
	dc.b	'                '
	dc.b	'                '
	dc.b	'U               '	; what markets (U = USA, J=JAPAN)


	END

;==============================================================================

