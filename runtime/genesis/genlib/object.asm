;============================================================================
;===	object.asm: 68000 generic object handler 			  ===
;===	(c)1991,92 Developer Resources					  ===
;============================================================================
;	Purpose: provide a simple method of creating, using and deleteing
;		 code objects
;	The object library mantains an array of objects, and runs the code
;	pointed to by the routine pointer each time UpdateObjects is called.
;	Objects that have not been created will not have their OBJFLAGB_ALIVE
;	bit set, and therefore will not run, and are free to be re-used.
;	Initialzing the object library:
;		Simply call InitObjects. adjust the constant MAX_OBJECTS
;		to reflect the maximun # of objects needed(note: don't make
;		this # to big, as it will use all of your working ram)
;	Createing an object:
;		Simply call AddObject with a0 pointing to new objects routine
;		(code to run each frame), and if one is available, the Z flag
;		will be set.
;	Removeing an object:
;		If you have the object pointer in a6, the macro
;			KillObject will do it for you. Or, if you prefer to
;			do it yourself, just clear the OBJFLAGB_ALIVE bit
;			in Obj_Flags, and the object will no longer execute.
;	Updateing an object:
;		All alive objects will be run each time UpdateObjects is called.
;	Writing object code:
;		All objects run as sub-routines to UpdateObjects.
;			The only restrictions are a6 must be preserved.
;			(which you want to do anyway, since it points to your
;			object structure.
;		For most games, you will wish to add variables to the object
;			structure(objects should not use global variables to
;			remember anything internal to the object, otherwise
;			you could not have more than one of that object type).
;			For this reason, the object structure is defined in
;			default.asm, so simply add variables to the end.
;
;	(see game.asm for an example of how use this library)
;------------------------------------------------------------------------------

code	segment

;==============================================================================

InitObjects:
	move.w	#MAX_OBJECTS-1,d0
	lea	objectBase,a0
.Loop:
	clr.l	(a0)				; clear flags and delay
	lea	Obj_Size(a0),a0
	dbra	d0,.Loop
	rts

;==============================================================================
; add new object to list
; Note: if calling this from another object, you MUST save a6 first!
; Inputs:
;	a0-> object routine to run
; Outputs:
;	a6-> newly created object(this is in a6 so all object manipulation
;		routines will work on new objects)
;		Note: this means if you create an object from inside another,
;		you MUST save your current object pointer, and restore it
;		before exiting the current object.
;	Note: object will have alive, routine and type initilized, all other
;	initializations must be done by the caller
;	Z flag	(if Z is set(zero, beq), then object was created,
;		if Z is clear(not zero, bne), then no objects are available
; Destroys:
;	d0.w,a6.l
;-----------------------------------------------------------------------------

AddObject:
	move.w	#MAX_OBJECTS-1,d0
	lea	objectBase-Obj_Size,a6
.Loop:
	lea	Obj_Size(a6),a6
	btst	#OBJFLAGB_ALIVE,(a6)		; is this one alive?(assumes flags are first word)
						; note: btst on memory is byte wide, this only works
						; because the bit is in the upper byte of the word
						; and btst tests modulo 8
	dbeq	d0,.Loop
	bne.s	.Nope
	move.l	a0,Obj_Routine(a6)		; set routine pointer
	move.w	#ObjType_Generic,Obj_Type(a6)	; set default type
	bset	#OBJFLAGB_ALIVE,(a6)		; last so the Z flag will be cleared
.Nope:
	rts

;==============================================================================
; Inputs:
;	a6-> object to kill
;-----------------------------------------------------------------------------

KillObject	macro
	bclr	#OBJFLAGB_ALIVE,(a6)
	endm

;==============================================================================
; call all alive objects in list(if delays are at zero)
; inputs: none
; outputs: none
; destroys: all
;==============================================================================

UpdateObjects:	   		; this is where are the objects get called
	move.w	#MAX_OBJECTS-1,d0
	lea	objectBase,a6
.Loop:						; loop through object list
	btst	#OBJFLAGB_ALIVE,(a6)		; is this one alive?
	beq.s	.Nope				; if not, go around
	move.w	Obj_Delay(a6),d1		; is this objects delay counter at 0?
	bne.s	.Wait				; if so, decrement and go around
	move.w	d0,-(sp)			; save object delay counter
	move.l	Obj_Routine(a6),a0		; get object's routine address
	jsr	(a0)		                ; call object
	move.w	(sp)+,d0			; retrieve object delay counter
.Nope:
	lea	Obj_Size(a6),a6			; point to next object in list
	dbra	d0,.Loop			; loop to next object
	rts

;-----------------------------------------------------------------------------

.Wait:
	subq.w	#1,Obj_Delay(a6)		; since not going to run object
	bra.s	.Nope				; this frame, ok to waste a little time

;==============================================================================
; ObjectCoutinue: pauses current object for one frame
; Note!: there can be nothing on the stack when your object calls this,
; or BOOM!
; also, since this call destroys Obj_Routine, once you call it,
; you may never return from your object(it you do, next frame will begin
; execution at the same spot as the last frame)
;------------------------------------------------------------------------------

ObjectContinue:
	move.l	(sp)+,Obj_Routine(a6)		; point to new entry point
	rts					; return back to object update loop

;==============================================================================
; FindObject:
;	Inputs:
;		d0.w = ObjType to find
;	Outputs:
;		Z = found, NZ = none
;		a0-> found object, if any
; FindNextObject:
;		d0.w = ObjType to find
;		d1.w = previous output of FindObject
;		a0.l = previous output of FindObject
;==============================================================================

FindObject:
	move.w	#MAX_OBJECTS-1,d1
	lea	objectBase,a0
FindObjectLoop:						; loop through object list
	btst	#OBJFLAGB_ALIVE,(a0)		; is this one alive?
	beq.s	.Nope				; if not, go around
	cmp.w	Obj_Type(a0),d0
	bne.s	.Nope
					; object found
	rts
.Nope:
FindNextObject:
	lea	Obj_Size(a0),a0			; point to next object in list
	dbra	d1,FindObjectLoop			; loop to next object
	moveq	#1,d1				; clear z flag
	rts

;==============================================================================

MacFindNxtObj	macro
	bra.s	.Entry{L}
.Top{L}:
	btst	#OBJFLAGB_ALIVE,(a0)		; is this one alive?
	beq.s	.Nope{L}				; if not, go around
	cmp.w	Obj_Type(a0),d0
	beq.s	.Done{L}				; object found
.Nope{L}:
.Entry{L}:
	lea	Obj_Size(a0),a0			; point to next object in list
	dbra	d1,.Top{L}			; loop to next object
	moveq	#1,d1				; clear z flag
.Done{L}:
	endm

;==============================================================================
; rules for writing objects:
;
;	a6->your object structure, must preserve
;	all other registers are nukeable
;	when processing finished, just return
;==============================================================================


;==============================================================================

ram	segment

	even
objectBase	ds.b	Obj_Size*MAX_OBJECTS

;==============================================================================

	END

;==============================================================================
