;---------------------------------------------------------
;
; DLTA processor, should work on both amiga, and pc format
; anim brushes.
;
; Who wrote this?  (I donna know!)  "AND REMEMBER THAT!!!"
; Who owns it?  (I donna know!)  "AND REMEMBER THAT!!!"
;
; What you need, the ability to read in the first PBM portion of
; the file and stick it in ImageSeg:0  (For the IBM this is 256 color
; mode only.)  Then call ProcessDelta on each DLTA record (See directions.)
;
; Showimage will display it to the screen assuming 256 1bpp format.
;  (Even if its an amiga format planar image it still assume 256 1bpp format.)
;
; Thats it!
;
;---------------------------------------------------------


NOJUMPS
LOCALS

.286

DEBUGLBM=0

Code segment para public 'code'
code ends

StackSeg segment para stack 'stack'
StackSeg ends

Data segment para public 'data'

DpaintHeader	label	byte
HEADER		db	'FORM'
FILELength	dd		0		;size of file less 4 bytes
UnknownCode	db	'PBM '
		db	'BMHD'		;BMHD 20 bytes length
		db	0,0,0,14h
xsize		dw	0
Ysize		dw	0
		db	0,0,0,0
bitplanes	db	8
whoknows	db	2
PackFlag	db	1
WhoKnows2	db	0
		db	0
TransMask	db	255
		db	5,6
ACOPY		db	1,40,0,0c8h

data ends

Zseg	segment word public 'data'
Zseg	ends


Code segment para public 'code'

ASSUME CS:CODE, DS:data, ES:data

;----------------------------------------------
;
; ProcessDelta
;		After reading DLTA and length,
;		point DS:SI to the DLTA data, and
;		point ES:DI to the desintation buffer.
;		This will perform the uncompression.
;
;----------------------------------------------

ProcessDelta:
		mov	bh,SS:BitPlanes
		cmp	bh,8
		jnz	ProcessPlaneDelta

		add	si,2		;Skip over 0,0,0,4
		mov	ax,[si]
		xchg	al,ah
		or	ax,ax
		jz	@@DoneDelta
		mov	si,ax

		xor	ah,ah
		mov	ch,ah

@@NextScan:	lodsb
		mov	dh,al
		or	al,al
		jz	@@DoneDelta

@@NextToken:	lodsb
		test	al,80h
		js	@@Skip

		mov	cl,al
		jcxz	@@RUN
	    rep movsb	
		jmp	short @@DoneToken

@@RUN:		lodsb
		mov	cl,al
		lodsb
	    rep stosb
		jmp	short @@DoneToken	

@@Skip:		and	al,7fh
		add	di,ax
@@DoneToken:
		dec	dh
		jnz	@@NextToken
		jmp	@@NextScan

@@DoneDelta:
		mov	ax,data
		mov	ds,ax
		mov	es,ax
		ret
		

ProcessPlaneDelta:
		push	si
		inc	si
		inc	si
		mov	ax,[si]
		xchg	al,ah
		mov	si,ax		;Get plane ptr.
		or	ax,ax
		jz	@@PlaneDone
		mov	bl,SS:BitPlanes
		mov	bp,SS:XSIZE
		shr	bp,3
		xor	dx,dx

@@ANP:		add	dx,bp
		dec	bl
		jnz	@@ANP


@@NExtCol:
if debugLBM
		pusha
		push	ds
		push	es

		mov	ax,data
		mov	ds,ax
		mov	es,ax

		call	ShowTheImage

		pop	es
		pop	ds
		popa
endif
		lodsb			;Get column token count.
		mov	cl,al
		xor	ch,ch
		jcxz	@@NoColChange

		push	di

@@NextToken:	lodsb
		or	al,al
		jz	@@RunToken
		js	@@CopyBytes

@@NextBSkip:	add	di,dx
		dec	al
		jnz	@@NextBSkip
		jmp	short @@TokenDone

@@RunToken:	lodsb
		mov	ah,al
		lodsb
@@NextBSt:	xor	ES:[di],al			;was MOV
		add	di,dx
		dec	ah
		jnz	@@NextBst
		jmp	short @@TokenDone

@@CopyBytes:	and	al,7fh
		mov	ah,al
@@CopyLoop:	lodsb
		xor	ES:[di],al			;was MOV
		add	di,dx
		dec	ah
		jnz	@@CopyLoop

@@TokenDone:	loop	@@NextToken

		pop	di

@@NoColChange:	inc	di
		dec	bp
		jnz	@@NextCol
@@PlaneDone:	pop	si
		add	si,4
		dec	bh
		jz	@@Done
		jmp	ProcessPlaneDelta
@@Done:

		mov	ax,data
		mov	es,ax
		mov	ds,ax
		ret


;----------------------------------------------

ShowTheImage:	cmp	BitPlanes,8
		jnz	ShowThePlaneImage
		mov	ax,0a000h
		mov	es,ax
		mov	bp,XSIZE
		mov	dx,YSIZE
		mov	ah,TransMask
		mov	ds,ImageSeg
		
		xor	si,si
		xor	di,di

@@NextScan:	mov	cx,bp
@@NextPXL:	lodsb
		or	al,al
		jnz	@@NotTrans
		mov	al,ah
		jmp	short @@GotClr
@@NotTrans:	cmp	al,ah
		jnz	@@GotClr
		xor	al,al
@@GotClr:	stosb
		loop	@@NextPXL
		add	di,320
		sub	di,bp
		dec	dx
		jnz	@@NextScan

		mov	ax,data
		mov	ds,ax
		mov	es,ax
		ret

ShowThePlaneImage:
				mov	ax,0a000h
		mov	es,ax

		mov	dx,YSIZE
		shr	bp,3
		mov	ds,ImageSeg
		
		xor	si,si
		xor	di,di

@@NextScan:	mov	bl,SS:BitPlanes
		mov	ah,1
		mov	bh,0feh

@@NotherPlane:	push	di
		mov	bp,SS:XSIZE
		shr	bp,3
@@NextByte:	lodsb
		mov	cx,8
@@NextPix:	and	byte ptr ES:[di],bh
		shl	al,1
		jnc	@@NoSet
		or	byte ptr ES:[di],ah
@@NoSet:	inc	di
		loop	@@NextPix
		dec	bp
		jnz	@@NextByte
		pop	di
		shl	ah,1
		rol	bh,1
		dec	bl
		jnz	@@NotherPlane

		add	di,320
		dec	dx
		jnz	@@NextScan

		mov	ax,data
		mov	ds,ax
		mov	es,ax

		ret

Code ends
end

