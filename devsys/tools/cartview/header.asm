
;============================================================================
;
; File:    header.asm
;
; Project: Game Genie (SNES)
;
; History:
;   19 Jun 92   WBNIV   Created
;
;============================================================================


HEADER	DEFS    ROM
HEADER	SEGMENT
	SIZE	$20
	ORG	$FFC0
	PORG	$7FC0

titleRegistrationArea:
;                123456789012345678901
;		 123456789012345678901
	dt	'Cartridge Viewer     '
	db	$20			; Map mode
	db	$00			; Cartride type: ROM
	db	$06       		; ROM Size: 8 x 2^n (4Mbit=9)
	db	$00			; RAM Size: Not used
	db	$01			; Destination Code: SNS
	db	$00			; Maker Code
	db	$00			; Mask ROM Version Number
	dw	$00			; Complement Check (lo)
	dw	$00			; Complement Check (hi)

	END

