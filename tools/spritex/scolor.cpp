/*
 * color.c -- do the magic palette shit.
 */

// pclib
#include <pclib\general.h>
#include <pclib\iff.h>
#include <pclib\grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"

#include	"utypes.hpp"
#include	"global.hpp"
#include	"sq.hpp"


RGBcolor
XorColor(RGBcolor in)
{
	RGBcolor c;

	c.r = in.r ^ 0x3f;
	c.g = in.g ^ 0x3f;
	c.b = in.b ^ 0x3f;

	return c;
}

RGBcolor
HalfBrite(RGBcolor in)
{
	RGBcolor c;

	c.r = in.r / 2;
	c.g = in.g / 2;
	c.b = in.b / 2;

	return c;
}

void
FixColors(void)
{
	short i;

	RGBcolor inPal[16];

	RGBcolor outPal[64];

	SQ_get_palette ( 0, 16, (RGBcolor far *)inPal );
	for ( i = 0 ; i < 16 ; i ++ )
	{
		outPal[i] = HalfBrite(inPal[i]);
		outPal[i+48] = outPal[i+32] =outPal[i+16] = inPal[i];
	}
	SQ_set_palette (0, 64, (RGBcolor far *)outPal );
	return;
}
