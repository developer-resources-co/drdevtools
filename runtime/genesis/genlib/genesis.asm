;============================================================================
;===	genesis.asm: Genesis Library Routines				  ===
;===	(c)1991,92 Developer Resources					  ===
;============================================================================
;	purpose: include all of Developer Resources libraries
;	If user only wants a sub-set of the libraries, simply copy this
;	file, or the includes in this file, to your own code, and modify.
;	Note: NEVER modify any of the files in the library directory,
;	if you want to change something, copy it to your directory first.
;	This way, when we update your libraries, you won't lose anything.
;------------------------------------------------------------------------------

	include		genesis.inc		; genesis equates
	include		macros.asm		; handy macros

	include		general.asm		; misc
	include 	vdp.asm			; video interface
	include 	text.asm		; text interface
	include		sprite.asm		; sprite interface
	ifdef	game_PIGSKIN
	include		dsprite.asm		; dynamic sprite interface
	endif
	include		scroll.asm		; hardware, 2-way & 4-way scrolling
	include		joy.asm			; joystick io and handling
	include		input.asm		; high level input handling
	include		menu.asm		; list selection library
	include		object.asm		; dynamicaly allocated object handler
	include		math.asm		; simple number handlers(rand)
	include		sound.asm		; simple sound interface
	include		effects.asm		; VDP effects code
	include		segalogo.asm            ; simple segalogo display
	include		animplay.asm		; generic animation player
	include		compress.asm		; decompressors
	include		hiscore.asm		; high score table handling
	ifdef	DEBUG
	include		debug.asm		; debugging routines
	endif

;============================================================================

	END

;============================================================================
