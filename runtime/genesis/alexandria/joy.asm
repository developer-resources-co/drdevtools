;============================================================================
;===    joy.asm: Genesis joystick routines                                ===
;===    (c)1991,92 Developer Resources                                    ===
;============================================================================
;       Purpose:
;               provide an interface to the user joysticks.
;------------------------------------------------------------------------------
	opt llbl
	include genesis.equ
	include default.equ
	include macros.mac

	XDEF    ButtonDispatch
	XDEF	ReadJoy1
	XDEF	ReadJoy2

	IFNE SEGA_CD
	include	cd.inc
	include	cd.mac
	ENDC
	include genesis.mac

	IFNE GEMS
	xref	_gemsholdz80
	xref	_gemsreleasez80
	ENDC

	TSECTION        .text           ;e changed from code segment

;==============================================================================
;       Inputs:
;               none
;       Outputs:
;               d0.b = joystick bits
;       Destroys:
;               d0.b,d1.b,a0.l
;------------------------------------------------------------------------------

;============================================================================
;       See Genesis.inc for joystick equates
;=== Bit 0 = up / Bit 1 = Down / Bit 2 = Left / Bit 3 = Right
;=== Bit 4 = B  / Bit 5 = C    / Bit 6 = A    / Bit 7 = START
;------------------------------------------------------------------------------

		XDEF _ReadJoy1
ReadJoy1
_ReadJoy1:
	IFNE SEGA_CD
;;	move.w   #CMD_ReadJoy1,SUB_1M_BASE
	move.w   #CMD_NOP,SUB_1M_BASE
	WAIT_FOR_1M_SWAPREQ
	SWAP_1M

	; wait for completion
;;	WAIT_FOR_1M_SWAPREQ

	; transfer registers back (return value)
;;	XFER_REGS_FROM_MAIN

	rts
	ENDC
		MOVE.B  #$40,$A10009            ; DDR Port 1 Joystick
		lea     $A10003,A0
		bra.s   ReadJoy

		XDEF	_ReadJoy2
ReadJoy2
_ReadJoy2:
	IFNE SEGA_CD
;;	move.w   #CMD_ReadJoy2,SUB_1M_BASE
	move.w   #CMD_NOP,SUB_1M_BASE
	WAIT_FOR_1M_SWAPREQ
	SWAP_1M

	; wait for completion
;;	WAIT_FOR_1M_SWAPREQ

	; transfer registers back (return value)
;;	XFER_REGS_FROM_MAIN
	rts
	ENDC
		lea     $A10005,A0
		MOVE.B  #$40,$A1000B            ; DDR Port 2 Joystick

ReadJoy:
		IFNE GEMS
		movem.l	d0/a0,-(sp)
		jsr	_gemsholdz80
		movem.l	(sp)+,d0/a0
		ENDC

	        bclr    #6,(A0)                 ; Set Bank 0
		nop
		nop
		move.b  (A0),D1
		and.b   #%00110000,D1
		asl.b   #2,D1
		bset    #6,(A0)                 ; Set Bank 1
		nop
		nop
		move.b  (A0),D0
		and.b   #%00111111,D0
		or.b    D1,D0
		not.b   D0

		IFNE GEMS
		movem.l	d0/a0,-(sp)
		jsr	_gemsreleasez80
		movem.l	(sp)+,d0/a0
		ENDC

		rts

;==============================================================================

ButtonReleaseBoth
;        IFEQ    SOUNDDRIVER
;        WAITVBSOUND
;        ENDC
	clr.l   d0
	bsr.s   ReadJoyBoth
	tst.l   d0
	bne.s   ButtonReleaseBoth
	rts

;==============================================================================

	xdef	_ButtonRelease
_ButtonRelease:
ButtonRelease:

	clr.l   d0
	bsr     ReadJoy1
	tst.l   d0
	bne.s   ButtonRelease
	rts

;==============================================================================
;       Destroys:
;               d0.b,d1.b,d2.b,a0.l
;------------------------------------------------------------------------------

ReadJoyBoth:
	bsr     ReadJoy1
	move.b  d0,d2
	bsr     ReadJoy2
	or.b    d2,d0
	rts

;==============================================================================

WaitForButton1
	clr.l   d0
	bsr     ReadJoy1
	tst.l   d0
	beq.s   WaitForButton1
	rts

;==============================================================================
;===    WaitJoy: Abortable wait                                             ===
;===    Wait for # of frames, unless user presses button                    ===
;===    Inputs:                                                             ===
;===            d0 = # of frames to wait                                    ===
;==============================================================================

WaitJoy:
	move.w  d0,d3
	bra.s   .Entry

.Loop:
	WAITVBSOUND
.Entry:
	jsr     ReadJoyBoth
	tst.b   d0
	dbne    d3,.Loop
.Done:
	rts

;============================================================================
;       ButtonDispatch: Automated button handling
;       (see gameobj.asm for example of use)
;       Inputs:
;               a3-> list of ptrs to routines
;               d3 = joystick input
; Bit 0 = up / Bit 1 = Down / Bit 2 = Left / Bit 3 = Right
; Bit 4 = B  / Bit 5 = C    / Bit 6 = A    / Bit 7 = START
;------------------------------------------------------------------------------

; speed is not that important here
	xdef	_ButtonDispatch
_ButtonDispatch:
	link	a6,#-20
	movem.l	d3-d4/a2/a3,-16(a6)
	movea.l	8(a6),a3
	movea.l	12(a6),a2	; pass this through unmolested
	move.w	18(a6),d3
	jsr	ButtonDispatch
	movem.l	-16(a6),d3-d4/a2/a3
	unlk	a6
	rts

ButtonDispatch:
	move.w  #8-1,d4
.Loop2:
	move.l  (a3)+,a1
	roxr.b  #1,d3
	bcc.s   .Nope
	move.l	a2,-(sp)	; pass this parameter
	jsr     (a1)		; must be C compliant (restore D3-D7, A2-A6)
	addq.l	#4,sp		; and get rid of it
.Nope:
	dbra    d4,.Loop2
	rts

;============================================================================

	END

;============================================================================

