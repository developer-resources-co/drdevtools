;==============================================================================
;===	segalogo.asm: code to display the sega logo			    ===
;===	(c)1991,92 Developer Resources					    ===
;==============================================================================
;	Purpose:
;		display sega logo, with correct color cycling
;------------------------------------------------------------------------------

CODE	segment

;==============================================================================

SegaLogo:
				; initialize the VDP
	lea	VDPTable,a0		; this uses the default.asm vdp init
	jsr	InitVDP

	lea	blackPalette,a0
	clr.w	d0
	move.w	#16,d1
	jsr	SetPalette

	LoadChr	segaLogo,CHARSIZE
	PlotMap	A,14,12,1,segaLogo

; Fade in Sega Logo
					; fade in sega logo
	move.w	#0,d0
	move.w	#16,d1
	move.w	#0,d2
	lea	blackPalette,a0
	lea	segaLogoPal,a1
	jsr	CrossFade

; Color cycle logo

	move.w	#21-1,d2
	lea	ChangeTable+40,a2
.Loop
	move.l	a2,a0
	moveq	#2,d0
	moveq	#11,d1
	jsr	SetPalette
	WAITVB
	WAITVB
	WAITVB
	WAITVB
	subq.l	#2,a2
	jsr	GetJoy1
	and.b	#$f0,d0
	dbne	d2,.Loop

; Wait for 100 Cycles

	moveq	#100,d2
.Loop2
	WAITVB
	jsr	GetJoy1
	and.b	#$f0,d0
	dbne	d2,.Loop2

; Fade out & Exit
					; fade out sega logo
	move.w	#0,d0
	move.w	#16,d1
	move.w	#0,d2
	lea	segaLogoPal,a0
	lea	blackPalette,a1
	jsr	CrossFade
.Exit
	rts

;==============================================================================

; data for logo
segaLogoPal	dc.w	$000,$eee,$ec0,$ea0,$e80,$e60,$e40,$e20
		dc.w	$e00,$c00,$a00,$800,$600,$800,$a00,$c00
		dc.w	$000,$eee,$e22

ChangeTable	dc.w	$ec0,$ea0,$e80,$e60,$e40,$e20,$e00,$c00
		dc.w	$a00,$800,$600,$800,$a00,$c00,$e00,$e20
		dc.w	$e40,$e60,$e80,$ea0,$ec0,$ea0,$e80,$e60
		dc.w	$e40,$e20,$e00,$c00,$a00,$800,$600

	chrinc	segaLogo
	mapinc	segaLogo

;==============================================================================

	END

;==============================================================================
