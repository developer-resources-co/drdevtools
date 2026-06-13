/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
                        ----------------------------

Title:        Class Definition for Anim

File:        ANIM.H

See Also:    ANIM.C (support routines for class CANIM)

-------------------------------------------------------------------------- */

#ifndef _ANIM_H_
#define _ANIM_H_

#include <alexdef.h>
#include <funcdef.h>
#include <object.h>
#include <spanim.h>




/* structure definition for class Anim */


#define _ANIM \
    /* <<<<<<<<<<<  class variables  >>>>>>>>>> */\
    UBYTE    nFrameNumber;                /* current animation frame number */\
    UBYTE    nAnimNumber;                /* current animation set number */\
    UBYTE    nPrevFrameNumber;            /* previous animation frame number */\
    UBYTE    nPrevAnimNumber;            /* previous animation set number */\
    UBYTE    nNumFrames;                    /* number of frames in current animation set */\
    UBYTE   nLoopAnim;                  /* boolean: does anim loop or stop? */\
    UWORD   nFrameRate;                 /* fixed point frame rate (8.8) */\
    UWORD   nCurrentFrame;              /* used for frame rate timing */\
    struct    _Anim        *pNext;            /* points to next Anim object in chain */\
    const    TSpriteAnim    **oAnims;        /* pointer to array of animation sequences */\
    WORD                xAnimSize;        /* x width of current animation frame */\
    WORD                yAnimSize;        /* y width of current animation frame */\
    WORD                xOffset;        /* x offset from xPos to display sprite */\
    WORD                yOffset;        /* y offset from yPos to display sprite */\
    struct                        /* bit field definitions - describes when bit is TRUE */\
    {\
        UWORD    _fPermSprite        : 1;    /* sprite characters should not be cleared */\
                                            /* automatically from VDP                  */\
        UWORD    _fAnimComplete        : 1;    /* now displaying last frame of animation */\
        UWORD    _fHeapAllocated        : 1;    /* space allocated for this object from heap */\
        UWORD    _fDisplaySprite        : 1;    /* display this sprite */\
    } wAnimFlags;\
    union \
    {\
        UWORD    _Word;\
        struct \
        {\
            UWORD    _wCharBase    : 11;    /* location in characters (32 bytes/char) from start of VDP RAM */\
            UWORD    _fFlipX        :  1;    /* sprite flip x direction */\
            UWORD    _fFlipY        :  1;    /* sprite flip y direction */\
            UWORD    _wPalette    :  2;    /* palette number */\
            UWORD    _fPriority    :  1;    /* sprite priority bit */\
        } _Bits;\
    } wMap;\
\
    /* <<<<<<<<<<  class methods  >>>>>>>>>> */\
    void    (*mSetAnim)(struct _Anim *, UWORD);        /* method to set animation sequence */\
    void    (*mRenderSprite)(WORD xPosition, WORD yPosition, struct _Anim *);    /* method which will cause the animation to render itself */\
    void    (*mSpriteMethod)(WORD xPosition, WORD yPosition, struct _Anim *);    /* sprite rendering method (normal, fixed, semi-perm) */\
    void    (*mSpriteOn)(struct _Anim *);                /* turn sprite on */\
    void    (*mSpriteOff)(struct _Anim *);                /* turn sprite off */




/*===========================================================================*/

/* actual typedef structure definition */

typedef struct _Anim {
    _ANIM
} Anim;




/*===========================================================================*/

/* #defines used to eliminate prefixes to bit field references */

#define fPermSprite     wAnimFlags._fPermSprite
#define fAnimComplete   wAnimFlags._fAnimComplete
#define fHeapAllocated  wAnimFlags._fHeapAllocated
#define fDisplaySprite  wAnimFlags._fDisplaySprite

#define wCharBase       wMap._Bits._wCharBase
#define fFlipX          wMap._Bits._fFlipX
#define fFlipY          wMap._Bits._fFlipY
#define wPalette        wMap._Bits._wPalette
#define fPriority       wMap._Bits._fPriority



/* ---------- class method prototype definitions  ---------- */

Anim*   AnimAdd(Anim *pAnimBase);        /* add another Anim instance to the chain */
Anim*   AnimNew(void);                    /* create new instance of object Anim */
Anim*   AnimConstruct( Anim *this );        /* assign default values to variables and */
                                                 /* method pointers                        */
void    AnimDestruct( Anim *this );        /* destroy this instance of Anim */
void    AnimTick( Anim *this );            /* default tick routine */
void    AnimRenderSprite( WORD xPosition, WORD yPosition, Anim* this );    /* render sprite */
void    AnimOnScreen( Anim *this );        /* default on screen routine */
void    AnimOffScreen( Anim *this );        /* default off screen routine */
void    AnimSpriteOn( Anim* this );        /* turn sprite on */
void    AnimSpriteOff( Anim* this );        /* turn sprite off */
void    FreeObjectVRAM( Anim *this );        /* deallocates characters allcoated for sprite from VRAM */


#endif




/* ANIM.H -- EOF */

