//=============================================================================
// graphPort.h:
//=============================================================================

#if !defined(GFXTOOLS_graphPort_H)
#define GFXTOOLS_graphPort_H

//==============================================================================

class point
{
	uint x,y;
public:
	point() { x = y = 0;}
	point(uint x, uint y) { Move(x,y); }
	Move(uint x, uint y) { point::x = x; point::y = y; }
	uint GetX(void) {return(x);
	uint GetY(void) {return(y);
};

//==============================================================================

class color
{
	ubyte red,green,blue;
	ubyte hue,saturation,value;
public:
	ubyte GetRed()			{ return(red); }
	ubyte GetGreen()		{ return(green); }
	ubyte GetBlue()			{ return(blue); }
	ubyte GetHue()			{ return(hue); }
	ubyte GetSaturation() 	{ return(saturation); }
	ubyte GetValue() 		{ return(value); }

	void SetRGB(r = red, g = green, b = blue) { red = r; green = g; blue = b; }
	void SetHSV(h = hue, s = saturation, v = vaule) { hue = h; saturation = s; value = v; }
};

//==============================================================================

class graphPort
{
protected:
	uint width,height;
	point currentPoint;
	color currentColor;
public:
	virtual void PutPixel(point &p = currentPoint,color &c = currentColor);
	virtual color GetPixel(point &p = currentPoint);

	virtual void Move(point &p) { currentPoint::Move(p); }
	virtual point Where(point &p) { return(currentPoint); }

	virtual void Line(point &p, point &p = currentPoint, color &c = currentColor);
	virtual void LineTo(point &p, color &c = currentColor) { Line(currentPoint, p, c); }

	virtual void Rectangle(point &p, point &p = currentPoint, color &c = currentColor);
	virtual void RectangleTo(point &p, color &c = currentColor) { Rectangle(currentPoint, p, c); }

	virtual void Box(point &p, point &p = currentPoint, color &c = currentColor);
	virtual void BoxTo(point &p, color &c = currentColor) { Box(currentPoint, p, c); }

// LockPoint? LockColor?
};

#endif

//==============================================================================

class screengraphPort : public graphPort
{

public:

};

//==============================================================================

class clippedGraphPort : public graphPort
{

public:

}

//==============================================================================
//==============================================================================
