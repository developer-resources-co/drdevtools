//=====================================================================================================================================
// color.h:
//==============================================================================
/*

Documentation:

	Abstract:
		the color class is an implementation/hardware independent representation
		of a real-world color, in RGB & HSV simultaniously.

	Class Hierarchy:
				  color
                    ³
                    ³
         		colorPalette

*/

//==============================================================================
// representation of a real-world color, in RGB or HSV format

#if !defined(GFXTOOLS_color_H)
#define GFXTOOLS_color_H

#include "pclib\general.hpp"

//==============================================================================

class color
{
public:
	color(uint r = 0, uint g = 0, uint b = 0) { SetRGB(r,g,b); }
//	color(const color& c) { }							// default copy constructor ok

	boolean operator==(const color &c) const;

	ubyte GetRed()			const	{ return(red); }
	ubyte GetGreen()		const	{ return(green); }
	ubyte GetBlue()			const	{ return(blue); }
	uword GetHue()			const	{ return(hue); }
	ubyte GetSaturation()	const	{ return(saturation); }
	ubyte GetValue()		const 	{ return(value); }

	void SetRGB(uint r, uint g, uint b) { red = r; green = g; blue = b; ConvertToHSV();}
	void SetHSV(uint h, uint s, uint v) { hue = h; saturation = s; value = v; ConvertToRGB();}
private:
	void ConvertToRGB();
	void ConvertToHSV();
	ubyte red,green,blue;
	uword hue;							// in degrees
	ubyte saturation,value;
};

//==============================================================================

inline boolean color::operator==(const color &c) const
{
	if
	 (
		red == c.red &&
		green == c.green &&
		blue == c.blue
	 )
		return(boolean::TRUE);
	else
		return(boolean::FALSE);
}

//==============================================================================

typedef ubyte colorMapIndex;						// actual # ploted into map
const uint COLORMAPINDEX_MAX = 255;
const uint COLORMAPINDEX_COUNT = 256;

//==============================================================================
//==============================================================================
// derivation of the color class, maintaining a palette of colors

class colorPalette
//class colorPalette : public color
{
public:
	colorPalette();
	~colorPalette();

	colorMapIndex Lookup(const color &c) const;
	colorMapIndex Create(const color &c);
	colorMapIndex GetMapIndex(const color &c);
	int GetColorCount() const { return(colorCount); }
	const color& GetColor(colorMapIndex cmi)  const { return(arrayPtr[cmi]); }
	const version& GetVersion(void) const {return(currentVer); }

//	void SetRGB(uint r, uint g, uint b) { color::SetRGB(r,g,b); currentVer++; }
//	void SetHSV(uint h, uint s, uint v) { color::SetHSV(h,s,v); currentVer++; }

private:
	uint colorCount;							    	// # of entries in array
	color arrayPtr[COLORMAPINDEX_MAX];							// variable size array
	version currentVer;							// will get default constructed, so will start at version 1
};

//==============================================================================

inline colorPalette::colorPalette()
{
	colorCount = 0;

}

//==============================================================================

inline colorPalette::~colorPalette()
{
}

//==============================================================================

inline colorMapIndex colorPalette::Create(const color &c)
{
	if(colorCount != COLORMAPINDEX_MAX)
	 {
		arrayPtr[colorCount] = c;
		currentVer++;
		return(colorCount++);
	 }
	return(colorCount);
}

//==============================================================================

inline colorMapIndex colorPalette::GetMapIndex(const color &c)
{
	colorMapIndex cmi;
	cmi = Lookup(c);
	if(cmi == COLORMAPINDEX_MAX)		// not found, create one
		cmi = Create(c);
	return(cmi);
}

//==============================================================================

#endif

//==============================================================================
