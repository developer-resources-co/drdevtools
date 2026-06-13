;==============================================================================
; hdr.asm: pigskin genesis header
;==============================================================================

; genesis header area
	dt	'SEGA GENESIS    '
	dt	'(C) RSI 1992.JAN'
	dt	'PIGSKIN         '
	dt	'                '
	dt	'                '
	dt	'PIGSKIN         '
	dt	'                '
	dt	'                '
	dt	'GM T-56026 -00'
	dt	'00'								; checksum
	dt	'                '	; control data
	dc.l	0,$7ffff
	dc.l	$ff0000,$ffffff
	dt	'                '	;
	dt	'                '
	dt	'                '
	dt	'                '
	dt	'U               '


	END

;==============================================================================