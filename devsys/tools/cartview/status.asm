;=============================================================================
;
; File:    status.asm
;
; Project: Developer Resources Development System ROM
;	 Copyright 1993,1994 Developer Resources.  All Rights Reserved.
;
; History:
;
;=============================================================================

;===============================================================================

	INCLUDE	lib/bin.mac

animFrame		DEFS	ROM
animFrame		SEGMENT
animFrameOffset	ds	1
animFrameMap	ds	3
animFrameTick	ds	1
animFrame_SIZEOF


FRAME	MACRO
	db	{1}
	dl	{2}
	db	{3}
	ENDM



ZPAGE	SEGMENT

ind0l		ds	3
copyrightPos	ds	2

ppStatusOutShadow	ds	2
ppDataShadow	ds	2

xOutput		ds	2
yOutput		ds	2


CODE	SEGMENT
Display_StatusScreen:
	db	%00001001
	db	%111

	dl	drlogoPal		; BG1
	db	16
	db	16

	dl	0		; BG2
	db	0
	db	0

	dl	font8Pal		; BG3
	db	16
	db	0

	dl	0		; BG4
	db	0
	db	0


	END
