//==============================================================================
// sprite.hpp: main header file for sprite editor
//==============================================================================

#ifndef SPRITE_sprite_H
#define SPRITE_sprite_H

//==============================================================================


#include <pclib/boolean.h>

/*
 * prototypes from SPMAIN.C
 */

boolean DoMainLoop(void);

const IMAGE_X = 0;
const IMAGE_Y = 8;


#include <pclib/grphport.h>
extern screenGraphPort* display;

#include <text.hpp>
extern gfxText* font;

extern unsigned char far* image[];
extern unsigned char far* output_image[];

#define oldGetPixel(image,x,y)	( *(image[(y)]+(x)) )


//==============================================================================

#endif

//==============================================================================
