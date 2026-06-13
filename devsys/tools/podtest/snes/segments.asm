
;============================================================================
;
; File:    ________.asm 
;
; Project: Where's Waldo? (SNES)
;
; History: 
;   30 Mar 92   WBNIV   Created
;
;
;============================================================================

MUSICS	defs    rom
MUSICS	segment
	size    $8000
	org $18000
	porg    $8000

SRAM    DEFS    RAM
SRAM    SEGMENT
	SIZE    $1E00-$200
	ORG $200

	IFDEF	DEBUG
NMI_Shadow	ds	1
HDMA_Shadow	ds	1
	ENDIF


BANK0	DEFS	ROM
BANK0	SEGMENT
	size    $7F00			; - $100 for vectors, header, etc.
	lorg	$008000
	porg    $000000


CODE    DEFS    ROM
CODE    SEGMENT
	size    $7F00			; - $100 for vectors, header, etc.
	lorg	$018000
	porg    $008000

;=============================================================================


ZPAGE   DEFS    RAM

ZPAGE   SEGMENT
	SIZE    $100
	org 0

;=============================================================================



	end
