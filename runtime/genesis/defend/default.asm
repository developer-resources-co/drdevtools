;==============================================================================
;===	default.asm: default library setup				    ===
;===	(c)1991,92 Developer Resources					    ===
;==============================================================================

;==============================================================================
; equates and structure definition for the object handler

ObjSeg	DEFS	RAM
ObjSeg	SEGMENT
	org	$000
; these first entries are required by the object handler
				; note: the object handler assumes the first 2
				; words are flags and delay, it wil break if you change this
Obj_Flags	ds.w	1		; execution flags
Obj_Delay	ds.w	1		; # of frames to wait before object runs again
Obj_Routine	ds.l	1		; pointer to code to run if object alive
Obj_Type	ds.w	1		; convient for counting objects of a particular type

; the rest is simply for object usage

Obj_Counter	ds.w	1		; for timing things
Obj_XPos	ds.l	1		; generaly, most objects are visible,
Obj_YPos	ds.l	1		; so they have coordinates
Obj_XSize	ds.w	1
Obj_YSize	ds.w	1
Obj_DataPtr	ds.l	1		; used for pointing to sprite struct, etc
; (user additions go here)
Obj_SprNumber	ds.w	1		; sprite # being displayed(used by missiles)
Obj_Direction	ds.w	1		; sprite direction
Obj_xVector	ds.l	1		; sprite speed, int & fraction
Obj_yVector	ds.l	1		; sprite speed

Obj_Size:				; end of object structure defintion

;-----------------------------------------------------------------------------
					; note: alive must be within the top 8 bits for the library to function
OBJFLAGB_ALIVE	EQU	15		; upper-most bit in word
OBJFLAGF_ALIVE	EQU	1<OBJFLAGB_ALIVE

MAX_OBJECTS	EQU	50

;-----------------------------------------------------------------------------
; by using a segment for the structure definition, you can safely
; add additional object types elsewhere in your code, just switch
; to this segment, and define them
; Note: the object handler always sets the object type to generic
; your add code must set the object to its correct type if you wish
; to make use of the typing

ObjType	DEFS	RAM
ObjType	SEGMENT
	org	$000

	; example object types
ObjType_Generic	ds.b	1		; this is the only type used by the handler
	; these are exaples, you don't have to use them
ObjType_Player  ds.b	1
ObjType_Friend  ds.b	1
ObjType_Ship    ds.b	1
ObjType_Missle  ds.b	1
ObjType_Enemy  ds.b	1

	; user types go here, or anywhere else in your code

;=============================================================================
; this is the default vdp setup(most of the library code will work
; with most settings, but no guarantees!)
;-----------------------------------------------------------------------------

code	segment

VBRAST	EQU	$e4    		; raster line on which vblank starts

MAXSPRITES	EQU	80		; if in 320 pixel mode
;MAXSPRITES	EQU	64		; if in 256 pixel mode


VDP_SCREEN_A	EQU	$C000
VDP_SCREEN_B	EQU	$E000
VDP_SPRITEBASE	EQU	$A800
VDP_WINDOWBASE	EQU	$B000
VDP_HSCROLLBASE EQU	$AC00

VDP_TOPFREE	EQU	VDP_SPRITEBASE

VDPTable:
	DC.B	VDP_R0F_REQ             ; 0
	DC.B	VDP_R1F_REQ|VDP_R1F_DISP|VDP_R1F_DMAEN		;Display/Vblank/Dma/Set (1)
	DC.B	VDP_SCREEN_A>10 ;$30	; Scroll A base Address  $C000 (2)
	DC.B	VDP_WINDOWBASE>10 ;$2C	; Window Address  $B000 (3)
	DC.B	VDP_SCREEN_B>13 ;$07	; Scroll B Address  $E000 (4)
	DC.B	VDP_SPRITEBASE>9 ;$54	; Sprite Base Address  $A800 (5)
	DC.B	$00			; Unused (6)
	DC.B	$00			; Background color selection (7)
	DC.B	$00			; Unused(8)
	DC.B	$00			; Unused(9)
	DC.B	$00			; HBI Skip Count (10)
	DC.B	$00			; Scroll size settings, & ext int enable(11)
	DC.B	VDP_R12F_H40		; Shadow trans/Set & display width(12)
	DC.B	VDP_HSCROLLBASE>10	; Horzontal Scroll Base(13)
	DC.B	$00			; Unused(14)
	DC.B	$02			; Auto Increment Size(15)(set to word)
	DC.B	VDP_R16F_SCRY32|VDP_R16F_SCRX64	; Vert Size/Horz Size(16)
	DC.B	$00			; window H position(17)
	DC.B	$00    			; window V position(18)
					; the rest of the registers are
					; only used by dma's, and get set
					; up when the dma occurs


; change these if you change the field sizes
VDP_FIELDA_WIDTH	EQU	64
VDP_FIELDB_WIDTH	EQU	64

VDP_FIELDA_HEIGHT	EQU	32
VDP_FIELDB_HEIGHT	EQU	32

VDP_WINDOW_WIDTH	EQU	32
VDP_WINDOW_HEIGHT	EQU	32

; change these if you turn on interlace, or reduce to 32 column mode
VDP_SCREEN_WIDTH	EQU	40
VDP_SCREEN_HEIGHT	EQU	28

	even

;============================================================================
; other stuff
;------------------------------------------------------------------------------

SPRITECOUNT	EQU	100		; kts temp until dsprites example
dynamicSpriteBase			; kts temp
	END

;============================================================================
