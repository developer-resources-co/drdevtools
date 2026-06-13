; spanim.asm -- RenderSprite, aka AddMagicSprite
;       void RenderSprite (TObject *p)   -- draws a sprite on the screen
;       3/23/93 craig : don't fail if no characters to render, skip


; kts this sux, if the base class bit fields move, this will have to be updated
; (base class is in base.h)
; Craig 9/14/93 : Guard the vmalloc in RenderLSprite: don't try if #chars==0



; in Render Class
;
fActive		equ	0
fOnScreen	equ	4


; in Anim class
;
fPermSprite	equ	0
fDisplaySprite	equ	3

        opt	llbl
	include object.equ


	xref	_vfree
	xref	_vmalloc
	xref	_MakeMapSpriteChars
	xref	_DeferedDMACopy
	xref	_AddLSpriteBase
	xref	_IntersectCamera
	xref	_pSpriteCamera


	xref	_vlargestblock
	xref	_vtotalfree


	xref	_Crash


;	GLOBAL TObject *pCamera;

;	GLOBAL TMapSpriteHeader  oMSH[];

	xref	_oMSH

;
;	PUBLIC void
;	AddMagicSprite(TObject *p)

	xdef	_RenderSprite
_RenderSprite:

	link	a6,#-40
	movem.l	d3-d7/a2-a5,-36(a6)

;	{

; pass in xPos, yPos instead of pointer to Render
; Todd 7/27/93
;	move.l	8(a6),a2		; a2 = pRender
	move.l	16(a6),a3		; a3 = pAnim


	btst.b	#fDisplaySprite,Anim_wAnimFlags+1(a3)
	bne	.DoRender

;	if (p->fOnScreen)

; on screen test moved to RenderRoutine (render.c)
; Todd 7/27/93
;	btst.b	#fOnScreen,Render_wObjectFlags+1(a2)
;	bne.s	.DoRender

;			(pRender->mRender)(p);
;		else
;			if (!pAnim->fPermSprite && pAnim->wCharBase)
.DontRender
	btst	#fPermSprite,Anim_wAnimFlags+1(a3)
	bne.s	.DontFree

	move.w	Anim_wMap(a3),d0
	and.w	#$7FF,d0
	tst.w	d0

	beq.s	.DontFree
;			{
;				vfree(pAnim->wCharBase);
	clr.l	d0
	move.w	Anim_wMap(a3),d0
	and.w	#$7FF,d0
	move.l	d0,(sp)				; pseudo-push wCharBase
	jsr	_vfree
;				pAnim->wCharBase = 0;
	and.w 	#$f800,Anim_wMap(a3)
;				pAnim->nPrevAnimNumber = ~0;
	move.b	#$ff,d0
	move.b	d0,Anim_nPrevAnimNumber(a3)
;				pAnim->nPrevFrameNumber = ~0;
	move.b	d0,Anim_nPrevFrameNumber(a3)
;			}
.DontFree:
	bra	.leave

.DoRender:
; end kts

;  	short            nFrame      = a->nFrameNumber;
	moveq	#0,d4
	move.b	Anim_nFrameNumber(a3),d4	; d4 = nFrame

;  	x = p->xPos - pCamera->xPos - p->xSize/2;

	move.l	_pSpriteCamera,a1
;	move.w	Render_xPos(a2),d0
	move.w	10(a6),d0			; 10(a6) = xPos
	add.w	Anim_xOffset(a3),d0
	sub.w	Render_xLoc(a1),d0
	move.w	Anim_xAnimSize(a3),d1
	lsr.w	#1,d1
	sub.w	d1,d0
	move.w	d0,d5			; d5 = x

;  	y = p->yPos - pCamera->yPos - p->ySize;

;	move.w	Render_yPos(a2),d0
	move.w	14(a6),d0			; 14(a6) = yPos
	add.w	Anim_yOffset(a3),d0
	sub.w	Render_yLoc(a1),d0
	sub.w	Anim_yAnimSize(a3),d0
	swap	d5
	move.w	d0,d5			; d5 = XXXXYYYY

;  	pSequence = pAnim->oAnims[nAnimToPlay];

	move.l	Anim_oAnims(a3),a1
	moveq	#0,d0
	move.b	Anim_nAnimNumber(a3),d0
	lsl.w	#2,d0
	move.l	(a1,d0.w),a4		; a4 = pSequence


;  	nNumFrames = pSequence->nNumFrames;

	clr.w	d1
	move.w	TSpriteAnim_nNumFrames(a4),d1
	bne.s	.100
	pea	noFramesText
	jsr	_Crash

.100

;  	while ( nFrame >= nNumFrames )
;    		nFrame -= nNumFrames;

.1:
	cmp.w	d1,d4
	blt.s	.2
	sub.w	d1,d4
	bra.s	.1
.2:

;  	pFrame = pSequence->frame+nFrame;

;	lsl.w	#3,d4
; Frame is no longer 8 bytes, but 14
; Should we pad two bytes to get back a shift (power of 2)?
; No, but you SHOULD optimize the multiply SLS 7/29/93
; Todd 7/27/93

;	mulu	#TSpriteAnimFrame__SIZEOF__,d4
; 	optimized 14n
	add.w	d4,d4				; * 2
	move.w	d4,d0				; save 2n
	lsl.w	#3,d4				; * 16
	sub.w	d0,d4				; 16n - 2n = 14n

	move.l	TSpriteAnim_frame(a4),a0
	lea	(a0,d4),a2			; a2 = pFrame

;  	if ( pAnim->nAnimNumber != pAnim->nPrevAnimNumber || pAnim->nFrameNumber != pAnim->nPrevFrameNumber )
;  	{

;;;; Note -- these depend on Anim having the following four variables in
;;;; the following order:
;;;;		nPhase
;;;;		nAnimToPlay
;;;;		nPrevPhase
;;;;		nPrevAnimToPlay

	move.w	Anim_nPrevFrameNumber(a3),d0
	cmp.w	Anim_nFrameNumber(a3),d0
	beq	.10

;    	pAnim->nPrevFrameNumber = pAnim->nFrameNumber;
;    	pAnim->nPrevAnimNumber = pAnim->nAnimNumber;

	move.w	Anim_nFrameNumber(a3),Anim_nPrevFrameNumber(a3)

;    	if (pAnim->wCharBase)
;      		vfree(pAnim->wCharBase);

	move.w	Anim_wMap(a3),d0
	andi.w	#$7FF,d0
	beq	.3

	ext.l	d0
	move.l	d0,(a7)
	jsr	_vfree
	and.w	#$f800,Anim_wMap(a3)
.3:


	lea	_oMSH,a5
;	move.l	TMapSpriteHeader_pCharBuffer(a5),d7	; temp

;    	oMSH.pCharBuffer = MakeMapSpriteChars
;      	( pFrame->map, pSequence->pCharSet, oMSH.pCharBuffer,
;          	pFrame->ls );

;	move.l	TSpriteAnimFrame_ls(a2),(a7)
;	move.l	TMapSpriteHeader_pCharBuffer(a5),-(a7)

;	move.l	TSpriteAnim_pCharSet(a4),a0
;	lea	TCharSetData_wData(a0),a0
;	move.l	a0,-(a7)
;	move.l	TSpriteAnim_pCharSet(a4),-(a7)
;	move.l	TSpriteAnimFrame_index(a2),-(a7)

; inlining a subroutine
	movem.l	a2/a5,-(sp)
	move.l	TMapSpriteHeader_pCharBuffer(a5),a1	; destination
	move.l	a1,d6					; used to point to source...
;							; ...for defered dma
	move.l	TSpriteAnim_pCharSet(a4),a0
	lea	TCharSetData_wData(a0),a0		; ptr into chrset
	move.l	TSpriteAnimFrame_index(a2),a5
	move.w	(a5)+,d2				; # of chars needed
	move.w	d2,d7					; numChars
	bra.s	.99
.98:
	move.w	(a5)+,d1				; get char index
	ext.l	d1
	lsl.l	#5,d1					; make into offset
	lea	(a0,d1.l),a2				; source
	move.l	(a2)+,(a1)+				; copy forward char
	move.l	(a2)+,(a1)+				; takes 96 cycles
	move.l	(a2)+,(a1)+				; that's 3 cycles
	move.l	(a2)+,(a1)+				; per byte
	move.l	(a2)+,(a1)+
	move.l	(a2)+,(a1)+
	move.l	(a2)+,(a1)+
	move.l	(a2)+,(a1)+
.99:
	dbra	d2,.98
	movem.l	(sp)+,a2/a5

	move.l	a1,TMapSpriteHeader_pCharBuffer(a5)

;    	numChars = (oMSH.pCharBuffer - temp) >> 5;

	andi.w	#$f800,Anim_wMap(a3)		; in case we decide not to allocate mem

	tst.w	d7
        beq     .leave                  ; if #chars is zero, do no more. Craig

;    	pAnim->wCharBase = vmalloc(numChars);

	move.l	d7,(a7)
	jsr	_vmalloc
	tst.w	d0
	beq	.Fail

	andi.w	#$7FF,d0
	andi.w	#$f800,Anim_wMap(a3)
	or.w	d0,Anim_wMap(a3)

.Ok:

;	oMSH.nNumChars += numChars;

	add.w	d7,TMapSpriteHeader_nNumChars(a5)

;	if ( oMSH.nNumChars > oMSH[a-...]nBufferSize
;		Crash("RenderSprite:  Exceeded DMA Buffer Size");

	move.w	TMapSpriteHeader_nBufferSize(a5),d2
	cmp.w	TMapSpriteHeader_nNumChars(a5),d2
	bpl.s	.Ok2

	pea	noBufferText
	jsr	_Crash

.Ok2:
;    	oMSH.pDeferDMA = DeferedDMACopy(a->wCharBase<<5,numChars<<4,temp,oMSH.pDeferDMA);

	move.l	TMapSpriteHeader_pDeferDMA(a5),(a7)
	move.l	d6,-(sp)
	moveq	#0,d1
	move.w	d7,d1
	lsl.l	#4,d1
	move.l	d1,-(sp)
	lsl.l	#5,d0
	move.l	d0,-(sp)
	jsr	_DeferedDMACopy
	move.l	a0,TMapSpriteHeader_pDeferDMA(a5)
	lea	12(sp),sp


;    	oMSH.nNumDMAs++;

	addq.w	#1,TMapSpriteHeader_nNumDMAs(a5)

;  	}

.10:
;  	desc = a->wPalette | a->wSpriteStat;
;  	AddLSpriteBase(pFrame->ls,x,y,desc,a->wCharBase);

	moveq	#0,d0
	move.w	Anim_wMap(a3),d0
	and.w	#$7FF,d0
	move.l	d0,(sp)               		; a->wCharBase
	move.w	Anim_wMap(a3),d0
	and.w	#$f800,d0
	move.l	d0,-(sp)			; a->wSpriteStat | a->wPalette
	move.w	d5,d0
	move.l	d0,-(sp)			; y
	swap	d5
	move.w	d5,d0
	move.l	d0,-(sp)			; x
	move.l	TSpriteAnimFrame_ls(a2),-(sp)	; pFrame->ls
	jsr	_AddLSpriteBase
	bra.s	.leave
;  	return;

.Fail:
	movea.l	_RenderSpritePanic,a0
	move.l	a0,d0
	beq.s	.Fail2
	jsr	(a0)				; if exists, call Panic Trap
.Fail2
	andi.w	#$f800,Anim_wMap(a3)
	move.w	#-1,Anim_nPrevFrameNumber(a3)	; force reload next frame
.leave
	movem.l	-36(a6),d3-d7/a2-a5
	unlk	a6
	rts

;	}

	xdef	_RenderLSprite
_RenderLSprite:			; good oldfashioned Logical Sprites

 	link	a6,#-40
	movem.l	d3-d7/a2-a5,-36(a6)

;	{

; pass in xPos, yPos instead of pointer to Render
; Todd 7/27/93
;	move.l	8(a6),a2		; a2 = pRender
	move.l	16(a6),a3		; a3 = pAnim

	btst.b	#fDisplaySprite,Anim_wAnimFlags+1(a3)
	bne	.DoRender

;	if (p->fOnScreen)

; on screen test moved to RenderRoutine (render.c)
; Todd 7/27/93
;	btst.b	#fOnScreen,Render_wObjectFlags+1(a2)
;	bne.s	.DoRender

;			(pRender->mRender)(p);
;		else
;			if (!pAnim->fPermSprite && pAnim->wCharBase)
.DontRender
	btst	#fPermSprite,Anim_wAnimFlags+1(a3)
	bne.s	.DontFree

	move.w	Anim_wMap(a3),d0
	and.w	#$7FF,d0
	tst.w	d0

	beq.s	.DontFree
;			{
;				vfree(pAnim->wCharBase);
	clr.l	d0
	move.w	Anim_wMap(a3),d0
	and.w	#$7FF,d0
	move.l	d0,(sp)				; pseudo-push wCharBase
	jsr	_vfree
;				pAnim->wCharBase = 0;
	and.w 	#$f800,Anim_wMap(a3)
;				pAnim->nPrevAnimNumber = ~0;
	move.b	#$ff,d0
	move.b	d0,Anim_nPrevAnimNumber(a3)
;				pAnim->nPrevFrameNumber = ~0;
	move.b	d0,Anim_nPrevFrameNumber(a3)
;			}
.DontFree:
	bra	.leave

.DoRender:
; end kts

;  	short            nFrame      = a->nFrameNumber;
	moveq	#0,d4
	move.b	Anim_nFrameNumber(a3),d4	; d4 = nFrame

;  	x = p->xPos - pCamera->xPos - p->xSize/2;

	move.l	_pSpriteCamera,a1
;	move.w	Render_xPos(a2),d0
	move.w	10(a6),d0			; 10(a6) = xPos
	add.w	Anim_xOffset(a3),d0
	sub.w	Render_xLoc(a1),d0
	move.w	Anim_xAnimSize(a3),d1
	lsr.w	#1,d1
	sub.w	d1,d0
	move.w	d0,d5			; d5 = x

;  	y = p->yPos - pCamera->yPos - p->ySize;

;	move.w	Render_yPos(a2),d0
	move.w	14(a6),d0			; 14(a6) = yPos
	add.w	Anim_yOffset(a3),d0
	sub.w	Render_yLoc(a1),d0
	sub.w	Anim_yAnimSize(a3),d0
	swap	d5
	move.w	d0,d5			; d5 = XXXXYYYY

;  	pSequence = pAnim->oAnims[nAnimToPlay];

	move.l	Anim_oAnims(a3),a1
	moveq	#0,d0
	move.b	Anim_nAnimNumber(a3),d0
	lsl.w	#2,d0
	move.l	(a1,d0.w),a4		; a4 = pSequence


;  	nNumFrames = pSequence->nNumFrames;

	clr.w	d1
	move.w	TSpriteAnim_nNumFrames(a4),d1
	bne.s	.100
	pea	noFramesText
	jsr	_Crash

.100

;  	while ( nFrame >= nNumFrames )
;    		nFrame -= nNumFrames;

.1:
	cmp.w	d1,d4
	blt.s	.2
	sub.w	d1,d4
	bra.s	.1
.2:

;  	pFrame = pSequence->frame+nFrame;

;	lsl.w	#3,d4
; Frame is no longer 8 bytes, but 14
; Should we pad two bytes to get back a shift (power of 2)?
; Todd 7/27/93
;;	mulu	#TSpriteAnimFrame__SIZEOF__,d4
; 	optimized 14n
	add.w	d4,d4				; * 2
	move.w	d4,d0				; save 2n
	lsl.w	#3,d4				; * 16
	sub.w	d0,d4				; 16n - 2n = 14n
	move.l	TSpriteAnim_frame(a4),a0
	lea	(a0,d4),a2			; a2 = pFrame

;  	if ( pAnim->nAnimNumber != pAnim->nPrevAnimNumber || pAnim->nFrameNumber != pAnim->nPrevFrameNumber )
;  	{

;;;; Note -- these depend on Anim having the following four variables in
;;;; the following order:
;;;;		nPhase
;;;;		nAnimToPlay
;;;;		nPrevPhase
;;;;		nPrevAnimToPlay

	move.w	Anim_nPrevFrameNumber(a3),d0
	cmp.w	Anim_nFrameNumber(a3),d0
	beq	.10

;    	pAnim->nPrevFrameNumber = pAnim->nFrameNumber;
;    	pAnim->nPrevAnimNumber = pAnim->nAnimNumber;

	move.w	Anim_nFrameNumber(a3),Anim_nPrevFrameNumber(a3)

;    	if (pAnim->wCharBase)
;      		vfree(pAnim->wCharBase);

	move.w	Anim_wMap(a3),d0
	andi.w	#$7FF,d0
	beq	.3

	ext.l	d0
	move.l	d0,(a7)
	jsr	_vfree
	and.w	#$f800,Anim_wMap(a3)
.3:
	lea	_oMSH,a5
;	pAnim->wCharBase = vmalloc(numChars);

	move.l	TLSpriteAnimFrame_pCharset(a2),a0	; get ptr to charset
	move.l	(a0),d0		; get # of chars
	lsr.w	#5,d0		; convert charsize to # chars

        beq     .leave          ; if #chars is zero, do no more. Craig

	ext.l	d0
	move.l	d0,(a7)		; now push THAT
	move.w	d0,d7		; d7 = # of chars
	jsr	_vmalloc
	tst.w	d0
	beq	.Fail

	andi.w	#$7FF,d0
	andi.w	#$f800,Anim_wMap(a3)
	or.w	d0,Anim_wMap(a3)
.Ok:

;	oMSH.nNumChars += numChars;

	add.w	d7,TMapSpriteHeader_nNumChars(a5)

;	if ( oMSH.nNumChars > oMSH[a-...]nBufferSize
;		Crash("RenderSprite:  Exceeded DMA Buffer Size");

	move.w	TMapSpriteHeader_nBufferSize(a5),d2
	cmp.w	TMapSpriteHeader_nNumChars(a5),d2
	bpl.s	.Ok2

	pea	noBufferText
	jsr	_Crash
.Ok2:

; I have to detect if the charset is going to cross a 64K boundary,
; and enqueue TWO DMAs if it does (the first part up to the edge of the
; boundary, and then the remainder)
;
;
	move.l	TLSpriteAnimFrame_pCharset(a2),d2	; get start address
	addq.l	#4,d2
	moveq	#0,d1
	move.w	d7,d1		; d7 = # of characters
	lsl.l	#5,d1		; # of bytes
	move.l	d1,d4		; store # of bytes total
	add.l	d2,d1		; last
	eor.l	d2,d1		; magic
	and.l	#$ff0000,d1	; strip off trash
	beq.s	.Ok3		; just do the one DMA

;
; Oh shit -- have to do TWO DMAs
;

	move.l	TMapSpriteHeader_pDeferDMA(a5),(a7)	; DMA descriptor buffer
	move.l	d2,-(sp)				; start address
	or.l	#$ffff,d2				; up to end of bank
	move.l	d2,d3					; save first ...
	addq.l	#1,d3					; ... byte of 2nd part
	sub.l	(sp),d2					; subtract out start
	addq	#1,d2					; m-n + 1
	lsr.l	#1,d2					; divide by two
	sub.l	d2,d4					; compute ...
	sub.l	d2,d4					; remaining # of bytes
	move.l	d2,-(sp)				; # of words
	lsl.l	#5,d0					; char # from malloc
	swap	d4					; count in hi word
	move.w	d0,d4					; byte address in VDP
	add.w	d2,d4
	add.w	d2,d4					; byte address of next word
	move.l	d0,-(sp)
	jsr	_DeferedDMACopy				; first part
	move.l	a0,TMapSpriteHeader_pDeferDMA(a5)

	lea	12(sp),sp				; pop parms


;
; D3 is the address of the source data for the next DMA
; D4 (hi word) is the # of bytes remaining, D4 (lo word) is VDP address
;
; done with the first one, let's do it again
;

	move.l	a0,(a7)
	move.l	d3,-(a7)	; source data address

	swap	d4              ; get # of bytes
	lsr.w	#1,d4		; div 2
	moveq	#0,d0
	move.w	d4,d0
	move.l	d0,-(sp)	; # of words

	swap	d4		; get VDP address
	moveq	#0,d0
	move.w	d4,d0
	move.l	d0,-(sp)	; VDP address

	jsr	_DeferedDMACopy	; destroys d0-d2, a0,a1
	lea	12(sp),sp

	addq.w	#2,TMapSpriteHeader_nNumDMAs(a5)	; had to do TWO
	bra.s	.10

.Ok3:
	move.l	TMapSpriteHeader_pDeferDMA(a5),(a7)
	move.l	TLSpriteAnimFrame_pCharset(a2),-(sp)
	addq.l	#4,(a7)		; skip over size
	moveq	#0,d1
	move.w	d7,d1		; d7 = # of characters
	lsl.l	#4,d1		; * 16 = # of words
	move.l	d1,-(sp)
	lsl.l	#5,d0		; char # from vmalloc
	move.l	d0,-(sp)
	jsr	_DeferedDMACopy
	move.l	a0,TMapSpriteHeader_pDeferDMA(a5)
	lea	12(sp),sp

;    	oMSH.nNumDMAs++;

	addq.w	#1,TMapSpriteHeader_nNumDMAs(a5)

;  	}

.10:
;  	desc = a->wPalette | a->wSpriteStat;
;  	AddLSpriteBase(pFrame->ls,x,y,desc,a->wCharBase);

	moveq	#0,d0
	move.w	Anim_wMap(a3),d0
	and.w	#$7FF,d0
	move.l	d0,(sp)               		; a->wCharBase
	move.w	Anim_wMap(a3),d0
	and.w	#$f800,d0
	move.l	d0,-(sp)			; a->wSpriteStat | a->wPalette
	move.w	d5,d0
	move.l	d0,-(sp)			; y
	swap	d5
	move.w	d5,d0
	move.l	d0,-(sp)			; x
	move.l	TSpriteAnimFrame_ls(a2),-(sp)	; pFrame->ls
	jsr	_AddLSpriteBase
	bra.s	.leave
;  	return;

.Fail:
	movea.l	_RenderSpritePanic,a0
	move.l	a0,d0
	beq.s	.Fail2
	jsr	(a0)				; if exists, call Panic Trap
.Fail2
	andi.w	#$f800,Anim_wMap(a3)
	move.w	#-1,Anim_nPrevFrameNumber(a3)	; force reload next frame
.leave
	movem.l	-36(a6),d3-d7/a2-a5
	unlk	a6
	rts

;	}

;--------------------------------------------------------------------------

noFramesText:
	dc.b	'RenderSprite:  NumFrames == 0',10
	dc.b	'(d0 = 4*nAnimToPlay) ',10
	dc.b	'(a3.l contains pData)',10
	dc.b	'(a4.l contains pSequence)',0

noMemText:
	dc.b	'RenderSprite:  vmalloc failed',10
	dc.b	'(d1.w contains # of chars requested)',10
	dc.b	'(d5.w contains # of chars free)',10
	dc.b	'(d6.w contains largest block)',0
noBufferText:
	dc.b	'RenderSprite:  Exceeded DMA Buffer Size',10
	dc.b	'(d2.w contains attempted buffer size)',0

	ds.w	0		; force alignment

;==============================================================================

	xdef	_RenderSemiPermSprite
_RenderSemiPermSprite:

	link	a6,#-40
	movem.l	d3-d7/a2-a5,-36(a6)

;	{
	move.l	8(a6),a2		; a2 = pRender
	move.l	12(a6),a3		; a3 = PAnim


;	if (p->fOnScreen)

	btst.b	#fOnScreen,Render_wObjectFlags+1(a2)
	beq	.leave

;  	short            nFrame      = a->nPhase;

	moveq	#0,d4
	move.b	Anim_nFrameNumber(a3),d4	; d4 = nFrame

;  	x = pRender->xPos - pSpriteCamera->xPos - pAnim->xSize/2;

	move.l	_pSpriteCamera,a1
	move.w	Render_xLoc(a2),d0
	add.w	Anim_xOffset(a3),d0
	sub.w	Render_xLoc(a1),d0
	move.w	Anim_xAnimSize(a3),d1
	lsr.w	#1,d1
	sub.w	d1,d0
	move.w	d0,d5			; d5 = x

;  	y = pRender->yPos - pSpriteCamera->yPos - pRender->ySize;

	move.w	Render_yLoc(a2),d0
	add.w	Anim_yOffset(a3),d0
	sub.w	Render_yLoc(a1),d0
	sub.w	Anim_yAnimSize(a3),d0
	swap	d5
	move.w	d0,d5			; d5 = XXXXYYYY

;  	pSequence = pAnim->oAnims[nAnimToPlay];

	move.l	Anim_oAnims(a3),a1
	moveq	#0,d0
	move.b	Anim_nAnimNumber(a3),d0
	lsl.w	#2,d0
	move.l	(a1,d0.w),a4		; a4 = pSequence


;  	nNumFrames = pSequence->nNumFrames;

	clr.w	d1
	move.w	TSpriteAnim_nNumFrames(a4),d1
	bne.s	.100
	pea	noFramesText
	jsr	_Crash

.100

;  	while ( nFrame > nNumFrames )
;    		nFrame -= nNumFrames;

.1:
	cmp.w	d1,d4
	blt.s	.2
	sub.w	d1,d4
	bra.s	.1
.2:

;  	pFrame = pSequence->frame+nFrame;

;	mulu	#TSpriteAnimFrame__SIZEOF__,d4
; 	optimized 14n
	add.w	d4,d4				; * 2
	move.w	d4,d0				; save 2n
	lsl.w	#3,d4				; * 16
	sub.w	d0,d4				; 16n - 2n = 14n
	move.l	TSpriteAnim_frame(a4),a0
	lea	(a0,d4),a2			; a2 = pFrame

;  	if ( pAnim->nAnimToPlay != pAnim->nPrevAnimToPlay || pAnim->nPhase != pAnim->nPrevPhase )
;  	{

;;;; Note -- these depend on Render having the following four variables in
;;;; the following order:
;;;;		nPhase
;;;;		nAnimToPlay
;;;;		nPrevPhase
;;;;		nPrevAnimToPlay

	move.w	Anim_nPrevFrameNumber(a3),d0
	cmp.w	Anim_nFrameNumber(a3),d0
	beq	.10

;    	pAnim->nPrevFrameNumber = pAnim->nFrameNumber;
;    	pAnim->nPrevAnimNumber = pAnim->nAnimNumber;

	move.w	Anim_nFrameNumber(a3),Anim_nPrevFrameNumber(a3)

;    	temp = oMSH.pCharBuffer;

	lea	_oMSH,a5
	move.l	TMapSpriteHeader_pCharBuffer(a5),d7

;    	oMSH.pCharBuffer = MakeMapSpriteChars
;      	( pFrame->map, pSequence->pCharSet, oMSH.pCharBuffer,
;          	pFrame->ls );

	move.l	TSpriteAnimFrame_ls(a2),(a7)
	move.l	TMapSpriteHeader_pCharBuffer(a5),-(a7)
	move.l	TSpriteAnim_pCharSet(a4),a0
	lea	TCharSetData_wData(a0),a0
	move.l	a0,-(a7)
;	move.l	TSpriteAnim_pCharSet(a4),-(a7)

;	move.l	TSpriteAnimFrame_map(a2),-(a7)
	jsr	_MakeMapSpriteChars
	lea	12(a7),a7
	move.l	a0,TMapSpriteHeader_pCharBuffer(a5)

;    	numChars = (oMSH.pCharBuffer - temp) >> 5;

	move.l	d7,d6			; d6 = temp
	sub.l	d7,a0
	moveq	#0,d7
	move.w	a0,d7
	lsr.l	#5,d7			; d7 = numChars
        beq     .leave                  ; if #chars is zero, do no more. Craig

;	oMSH.nNumChars += numChars;

	add.w	d7,TMapSpriteHeader_nNumChars(a5)

;	if ( oMSH.nNumChars > oMSH[a-...]nBufferSize
;		Crash("RenderSprite:  Exceeded DMA Buffer Size");

	move.w	TMapSpriteHeader_nBufferSize(a5),d2
	cmp.w	TMapSpriteHeader_nNumChars(a5),d2
	bpl.s	.Ok2

	pea	noBufferText
	jsr	_Crash



.Ok2:
;    	oMSH.pDeferDMA = DeferedDMACopy(a->wCharBase<<5,numChars<<4,temp,oMSH.pDeferDMA);

	move.l	TMapSpriteHeader_pDeferDMA(a5),(a7)
	move.l	d6,-(sp)
	moveq	#0,d1
	move.w	d7,d1
	lsl.l	#4,d1
	move.l	d1,-(sp)
	moveq	#0,d0
	move.w	Anim_wMap(a3),d0
	andi.w	#$7FF,d0
	lsl.l	#5,d0
	move.l	d0,-(sp)
	jsr	_DeferedDMACopy
	move.l	a0,TMapSpriteHeader_pDeferDMA(a5)
	lea	12(sp),sp


;    	oMSH.nNumDMAs++;

	addq.w	#1,TMapSpriteHeader_nNumDMAs(a5)

;  	}

.10:
;  	desc = pAnim->wPalette | pAnim->wSpriteStat;
;  	AddLSpriteBase(pFrame->ls,x,y,desc,a->wCharBase);

	moveq	#0,d0
	move.w	Anim_wMap(a3),d0
	and.w	#$7FF,d0
	move.l	d0,(sp)               		; pAnim->wCharBase
	move.w	Anim_wMap(a3),d0
	and.w	#$f800,d0
	move.l	d0,-(sp)			; pAnim->wSpriteStat | pAnim->wPalette
	move.w	d5,d0
	move.l	d0,-(sp)			; y
	swap	d5
	move.w	d5,d0
	move.l	d0,-(sp)			; x
	move.l	TSpriteAnimFrame_ls(a2),-(sp)	; pFrame->ls
	jsr	_AddLSpriteBase
.leave
	movem.l	-36(a6),d3-d7/a2-a5
	unlk	a6
	rts

;	}

	xdef	_SetRenderSpritePanic
_SetRenderSpritePanic:
	move.l	_RenderSpritePanic,a0
	move.l	4(a7),_RenderSpritePanic
	rts



;==============================================================================

	BSECTION	.bss
			ds.w	0	; force alignment
_RenderSpritePanic	ds.l	1	; pointer

;------------------------------------------------------------------------------

