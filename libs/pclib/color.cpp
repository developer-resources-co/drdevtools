//==============================================================================
// color.cpp:
//==============================================================================

#include "color.hpp"

//==============================================================================

void
color::ConvertToRGB()
{

	uword p1,p2,p3;
	uword sat = saturation;

	if(hue >= 360)
		hue = 0;

	uword h = hue / 60;						// get quadrant info
	uword f = hue % 60;


	p1 = (value * ((1*255) - sat)) / 255;
	p2 = (value * ((1*255) - ((sat * f)/60)))/255;
	p3 = (value * ((1*255) - ((sat * (60-f))/60)))/255;


	switch(h)
	{
		case 0:
			red = value;
			green = p3;
			blue = p1;
			break;
		case 1:
			red = p2;
			green = value;
			blue = p1;
			break;
		case 2:
			red = p1;
			green = value;
			blue = p3;
			break;
		case 3:
			red = p1;
			green = p2;
			blue = value;
			break;
		case 4:
			red = p3;
			green = p1;
			blue = value;
			break;
		case 5:
			red = value;
			green = p1;
			blue = p2;
			break;
	}
}

//==============================================================================

void
color::ConvertToHSV()
{
	ubyte temp;
	uword min,max,dif;
	uword rl,gl,bl;

	// setup
	temp = (red>green)?red:green;
	max = (temp>blue)?temp:blue;

	min = (red<green)?red:green;
	min = (min<blue)?temp:blue;

	dif = max-min;

	// calculate value(highest of rgb)
	value = max;

	// calculate saturation(delta of rgb)
	if(max != 0)
		saturation = ((max - min)*255) / max;
	else
		saturation = 0;

	// calculate hue(phase angle of delta of rgb?)

	if(saturation != 0)
	 {
		rl = (max - red) / dif;
		gl = (max - green) / dif;
		bl = (max - blue) / dif;

		if(red == max)
		 {
			if(green == min)
				hue = (5*255)+bl;
			else
				hue = (1*255)-gl;
		 }
		if(green == max)
		 {
			if(blue = min)
				hue = (1*255)+rl;
			else
				hue = (3*255)-bl;
		 }
		else
		if(red == min)
			hue = (3*255)+gl;
		else
			hue = (5*255)-rl;

		// now adjust hue to be degrees
		hue /= 6;
	 }
	else
	;
//		hue = undefined;
		// leave hue alone so if saturation comes back up, it will be remembered
}

//==============================================================================
//==============================================================================

colorMapIndex colorPalette::Lookup(const color &c) const
{
	colorMapIndex cmi;
	for(cmi=0;cmi<colorCount && cmi != COLORMAPINDEX_MAX;cmi++)
	 {
		if(c == arrayPtr[cmi])
			return(cmi);
	 }
	return(COLORMAPINDEX_MAX);
}

//==============================================================================
