
;============================================================================
;
; File:    segments.asm
;
; Project: SNES Graphics Downloader
;
; History:
;	30 Mar 92	WBNIV	Created
;
;
;============================================================================

SRAM	DEFS	RAM
SRAM	SEGMENT
	SIZE	$2000-$200
	ORG	$200


CODE	DEFS	ROM
CODE	SEGMENT
	org	$008000
	porg	$000000
	size	$008000


;=============================================================================


ZPAGE	DEFS	RAM

ZPAGE	SEGMENT
	SIZE	$100
	org	0

;=============================================================================

;;WRAM64  DEFS    RAM
;;WRAM64  SEGMENT
;;	SIZE    $008000-$2000
;;	ORG     $7E0000+$2000


WRAM128	DEFS	RAM
WRAM128	SEGMENT
	SIZE	$010000
	ORG	$7F0000


	end
