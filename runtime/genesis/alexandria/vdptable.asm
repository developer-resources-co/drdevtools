;==============================================================================
;===	vdptable.asm: Genesis VDP core routines				    ===
;===	(c)1991,2 Developer Resources					    ===
;===    split from vdp.asm by Craig
;==============================================================================

        opt	llbl
	include	genesis.equ
	include	default.equ
	include	vdp.equ

	TSECTION	.text

        xdef	_VDPTable
	xdef	VDPTable
_VDPTable:
VDPTable:
	DC.B	VDP_R0F_REQ             ; 0
	DC.B	VDP_R1F_REQ|VDP_R1F_DISP|VDP_R1F_DMAEN		;Display/Vblank/Dma/Set (1)
	DC.B	VDP_SCREEN_A>>10 ;$30	; Scroll A base Address  $C000 (2)
	DC.B	VDP_WINDOWBASE>>10 ;$2C	; Window Address  $B000 (3)
	DC.B	VDP_SCREEN_B>>13 ;$07	; Scroll B Address  $E000 (4)
	DC.B	VDP_SPRITEBASE>>9 ;$54	; Sprite Base Address  $A800 (5)
	DC.B	$00			; Unused (6)
	DC.B	$00			; Background color selection (7)
	DC.B	$00			; Unused(8)
	DC.B	$00			; Unused(9)
	DC.B	$00			; HBI Skip Count (10)
	DC.B	VDP_R11F_EXINT		; Scroll size settings, & ext int enable(11)
	DC.B	VDP_R12F_H40		; Shadow trans/Set & display width(12)
	DC.B	VDP_HSCROLLBASE>>10	; Horzontal Scroll Base(13)
	DC.B	$00			; Unused(14)
	DC.B	$02			; Auto Increment Size(15)(set to word)
	DC.B	VDP_R16F_SCRY32|VDP_R16F_SCRX64	; Vert Size/Horz Size(16)
	DC.B	$00			; window H position(17)
	DC.B	$00    			; window V position(18)
;					; the rest of the registers are
;					; only used by dma's, and get set
;					; up when the dma occurs


	xdef	_VDP256Table
	xdef	VDP256Table
_VDP256Table:
VDP256Table:
	DC.B	VDP_R0F_REQ             ; 0
	DC.B	VDP_R1F_REQ|VDP_R1F_DISP|VDP_R1F_DMAEN		;Display/Vblank/Dma/Set (1)
	DC.B	VDP_SCREEN_A>>10 ;$30	; Scroll A base Address  $C000 (2)
	DC.B	VDP_WINDOWBASE>>10 ;$2C	; Window Address  $B000 (3)
	DC.B	VDP_SCREEN_B>>13 ;$07	; Scroll B Address  $E000 (4)
	DC.B	VDP_SPRITEBASE>>9 ;$54	; Sprite Base Address  $A800 (5)
	DC.B	$00			; Unused (6)
	DC.B	$00			; Background color selection (7)
	DC.B	$00			; Unused(8)
	DC.B	$00			; Unused(9)
	DC.B	$00			; HBI Skip Count (10)
	DC.B	VDP_R11F_EXINT		; Scroll size settings, & ext int enable(11)
	DC.B	VDP_R12F_H32		; Shadow trans/Set & display width(12)
	DC.B	VDP_HSCROLLBASE>>10	; Horzontal Scroll Base(13)
	DC.B	$00			; Unused(14)
	DC.B	$02			; Auto Increment Size(15)(set to word)
	DC.B	VDP_R16F_SCRY32|VDP_R16F_SCRX64	; Vert Size/Horz Size(16)
	DC.B	$00			; window H position(17)
	DC.B	$00    			; window V position(18)


