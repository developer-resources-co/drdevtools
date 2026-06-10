//=============================================================================
// grphPort.h:
//=============================================================================
/*

Documentation:

	Abstract:

	History:
			Created	? Kevin T. Seghetti
			First Documented 10-19-92 02:19pm

	Class Hierarchy:


				graphPort
                    ³
                    ³
           ÚÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
           ³                 ³                   ³
       rastGraphPort    clippedGraphPort   nullGraphPort
           ³
           ³
           ³
     screenGraphPort

	Dependancies:
		general
		color

	Restrictions:

		clipped & masked gport don't handle Erase correctly

	Example:

*/
//==============================================================================
// use only once insurance

#if !defined(GFXTOOLS_graphPort_H)
#define GFXTOOLS_graphPort_H

//==============================================================================
// Dependancies

#include <conio.h>					// for text_info

#include "pclib/general.hpp"
#include "pclib/color.hpp"

//==============================================================================
// class declaration

class point
{
public:
	point(const point& p) { x = p.X(); y = p.Y(); }
	point& operator=(const point& p) { x = p.X(); y = p.Y(); return(*this); }
	point& operator+=(const point& p) { x += p.x; y += p.y; return *this; }
	point& operator-=(const point& p) { x -= p.x; y -= p.y; return *this; }

	point operator+(const point& a) const { point res = a; res += *this; return res; }
	point operator-(const point& a) const { point res = a; res -= *this; return res; }

	const point& Move(uint x, uint y) { point::x = x; point::y = y; return (*this);}

	point() { x = y = 0;}
	point(int x, int y) { Move(x,y); }
	int X(void) const {return(x); }
	int Y(void) const {return(y); }

	int X(uint newX) { x = newX; return(X()); }
	int Y(uint newY) { y = newY; return(Y()); }

private:
	int x,y;
};


#ifndef _WINDOWS

//==============================================================================
// class declaration

class graphPort
{
public:
	uint GetWidth(void) { return(width); }
	uint GetHeight(void) { return(height); }

	virtual void PutPixel(const point &) = 0;
	virtual void PutPixel(void) { PutPixel(currentPoint); }
	virtual const color& GetPixel(const point &) = 0;
	virtual const color& GetPixel(void) { return(GetPixel(currentPoint));};

	virtual void Move(const point &p) { currentPoint = p; }
	virtual const point& Where(void) { return(currentPoint); }

	virtual void SetColor(const color& c) { currentColor = c;}
	virtual const color& GetColor(void) { return(currentColor); }

	virtual void Line(const point &p, const point &p2) = 0;
	void LineTo(const point &p) { Line(currentPoint, p); }

	virtual void HLine(const point &p, int len) = 0;
	void HLineTo(int len) { HLine(currentPoint, len); }

	virtual void VLine(const point &p, int len) = 0;
	void VLineTo(int len) { VLine(currentPoint, len); }

	virtual void Rectangle(const point &p, const point &p2) = 0;
	void RectangleTo(const point &p) { Rectangle(currentPoint, p); }

	virtual void Box(const point &p, const point &p2) = 0;
	void BoxTo(const point &p) { Box(currentPoint, p); }

	virtual void Erase(void) = 0;

	virtual ~graphPort() {};

// LockPoint? LockColor?
protected:
	graphPort(uint xSize, uint ySize, const color& c = color());	// this base class cannot be used directly
	graphPort( graphPort& ) { }										// this base class cannot be used directly

	uint width,height;
	color currentColor;
	point currentPoint;
private:
	graphPort();							// must init with parameters
};

//==============================================================================
// class declaration

class rastGraphPort : public graphPort
{
public:
	rastGraphPort(uint xSize, uint ySize, const color& c = color());
	rastGraphPort(ubyte *map[],uint xSize, uint ySize, const color& c = color());
	rastGraphPort( rastGraphPort& );
	~rastGraphPort();

	virtual void PutPixel(const point& p)
		{ lineArrayPtr[ p.Y() ][ p.X() ] = currentIndex; }
	virtual void PutPixel(void) { graphPort::PutPixel(); }

	virtual const color& GetPixel(const point&);

	virtual colorMapIndex GetPixelIndex(const point& p)
		{ return lineArrayPtr[ p.Y() ][ p.X() ]; }
	virtual colorMapIndex GetPixelIndex(void) { return(GetPixelIndex(currentPoint));}

	virtual void Line(const point &p, const point &p2);

	virtual void HLine(const point &p, int len);
	virtual void XorHLine(const point &p, int len, ubyte val = 0xff);
	virtual void VLine(const point &p, int len);

	virtual void Rectangle(const point &p, const point &p2);
	virtual void Box(const point &p, const point &p2);
	virtual void XorBox(const point &p, const point &p2, ubyte val = 0xff);
	virtual void SetColor(const color& c);
	void SetColorIndex(int newIndex);

		// plots entire graphPort onto dest port, starting at dest ports
		// current point position

	virtual void PlotOnPort(rastGraphPort &destPort);

	virtual void Xor(ubyte val = 0xff);
	virtual void Erase(void);

	virtual void Palette(colorPalette &newPalette) { *mapCols = newPalette; }

protected:
	enum { MAX_RASTPORT_LINES = 512 };
	colorPalette *mapCols;					//
	colorMapIndex currentIndex;
private:
	void rastGraphPort::Construct(const color&);
	boolean ownBuffer;
	rastGraphPort();
public:
	ubyte* lineArrayPtr[ MAX_RASTPORT_LINES ];
};

//==============================================================================
// class declaration

class screenGraphPort : public rastGraphPort
{
public:
	screenGraphPort(uint xSize, uint ySize, uint numColors, const color& c = color());
	~screenGraphPort();
	void SetColor(const color& c);

	virtual void Palette(colorPalette &newPalette) { rastGraphPort::Palette(newPalette); ForcePaletteUpdate();  }

private:

typedef struct _rgbcolor
{
    uchar r;
    uchar g;
    uchar b;
} RGBcolor;


	screenGraphPort();						// default constructor not allowed
#ifdef __BORLANDC__
	struct text_info ti;
#endif
	version paletteVersion;
	void PaletteUpdate(void);
	void ForcePaletteUpdate(void);
	void SetIBMPalette( int start, int count, RGBcolor* rgb );
};

//==============================================================================
// class declaration

class clippedGraphPort : public rastGraphPort
{
public:
	clippedGraphPort(rastGraphPort *g, const point &origin, uint xSize, uint ySize);

	void PutPixel(const point& p) { point myP(p); myP+=origin;
									if(myP.X() >= 0 && myP.X() < rightEdge
									&& myP.X() >= 0 && myP.Y() < botEdge)
										gPort->PutPixel(myP);
								}
	void PutPixel() { if(currentPoint.X() >= 0 && currentPoint.X() < rightEdge
					    && currentPoint.Y() >= 0 && currentPoint.Y() < botEdge)
					  		gPort->PutPixel(currentPoint);
								}
	void SetColor(const color& c) { gPort->SetColor(c);}
	const color& GetColor(void) { return(gPort->GetColor()); }
	void Move(const point &p) { currentPoint = p; currentPoint += origin; }

	virtual const point& Where(void) { return((gPort->Where())-origin);	}

	const color& GetPixel(const point& p)
								{ point myP(p); myP+=origin;
									if(myP.X() >= 0 && myP.X() < rightEdge
									&& myP.Y() >= 0 && myP.Y() < botEdge)
										return(gPort->GetPixel(myP));
								  else
									return(currentColor);
								}

	const color& GetPixel(void)
								{ if(currentPoint.X() >= 0 && currentPoint.X() < rightEdge
									&& currentPoint.Y() >= 0 && currentPoint.Y() < botEdge)
										return(gPort->GetPixel(currentPoint));
								  else
									return(currentColor);
								}

	virtual colorMapIndex GetPixelIndex(const point& p)
								{ point myP(p); myP+=origin;
									if(myP.X() >= 0 && myP.X() < rightEdge
									&& myP.Y() >= 0 && myP.Y() < botEdge)
										return(gPort->GetPixelIndex(myP));
								  else
									return(0);
								}


	virtual colorMapIndex GetPixelIndex(void)
								{ if(currentPoint.X() >= 0 && currentPoint.X() < rightEdge
									&& currentPoint.Y() >= 0 && currentPoint.Y() < botEdge)
										return(gPort->GetPixelIndex(currentPoint));
								  else
									return(0);
								}

	void Line(const point &p, const point &p2) { gPort->Line(p,p2);}
	void HLine(const point &p, int len);
	void VLine(const point &p, int len);

	void Rectangle(const point &p, const point &p2);
	void Box(const point &p, const point &p2);

	void Erase(void);     // { gPort->Erase(); }
	virtual void Xor(ubyte val = 0xff);

	virtual void PlotOnPort(rastGraphPort &destPort);

	void MovePort(const point &p) { origin = p; CalcEdges();}
	void Resize(uint xSize, uint ySize) { width = xSize; height = ySize; CalcEdges();}
private:
	point origin;
	uint rightEdge, botEdge;
	rastGraphPort *gPort;
	clippedGraphPort();
	void CalcEdges(void) { rightEdge = origin.X() + width;	botEdge = origin.Y() + height; }
};

//==============================================================================
// inlines

inline clippedGraphPort::clippedGraphPort(rastGraphPort *g, const point& origin, uint xSize, uint ySize)
: rastGraphPort(xSize, ySize, color())
{
	gPort = g;
	clippedGraphPort::origin = origin;
	CalcEdges();
}

//==============================================================================
// class declaration

class maskedGraphPort : public graphPort
{
public:
	maskedGraphPort(graphPort *g, point& origin, uint xSize, uint ySize);

	void PutPixel(const point& p) { if(p.X() >= origin.X() && p.X() < rightEdge
								  && p.Y() >= origin.Y() && p.Y() < botEdge)
										gPort->PutPixel(p);
								}

	void PutPixel() { if(currentPoint.X() >= origin.X() && currentPoint.X() < rightEdge
								  && currentPoint.Y() >= origin.Y() && currentPoint.Y() < botEdge)
										gPort->PutPixel(currentPoint);
								}

	void SetColor(const color& c) { gPort->SetColor(c);}
	const color& GetColor(void) { return(gPort->GetColor()); }

	const color& GetPixel(const point& p)
								{ if(p.X() >= origin.X() && p.X() < rightEdge
								  && p.Y() >= origin.Y() && p.Y() < botEdge)
										return(gPort->GetPixel(p));
								  else
									return(currentColor);
								}

	const color& GetPixel(void)
								{ if(currentPoint.X() >= origin.X() && currentPoint.X() < rightEdge
								  && currentPoint.Y() >= origin.Y() && currentPoint.Y() < botEdge)
										return(gPort->GetPixel(currentPoint));
								  else
									return(currentColor);
								}

	void Line(const point &p, const point &p2) { gPort->Line(p,p2);}
	void HLine(const point &p, int len);
	void VLine(const point &p, int len);

	void Rectangle(const point &p, const point &p2);
	void Box(const point &p, const point &p2);

	void Erase(void) { gPort->Erase(); }

	void MovePort(const point &p) { origin = p; CalcEdges();}
	void Resize(uint xSize, uint ySize) { width = xSize; height = ySize; CalcEdges();}

private:
	point origin;
	uint rightEdge, botEdge;
	graphPort *gPort;
	maskedGraphPort();
	void CalcEdges(void) { rightEdge = origin.X() + width;	botEdge = origin.Y() + height; }
};

//==============================================================================
// inlines

inline maskedGraphPort::maskedGraphPort(graphPort *g, point& origin, uint xSize, uint ySize)
: graphPort(xSize, ySize, color())
{
	gPort = g;
	maskedGraphPort::origin = origin;
	CalcEdges();
}

//==============================================================================
// class declaration: null graph port is a bit bucket

class nullGraphPort : public graphPort
{
public:
	nullGraphPort() : graphPort(0, 0, color()) {}

	~nullGraphPort() {}

	void PutPixel(const point &) {}

	const color& GetPixel(const point&) {return currentColor;}

	void Line(const point &p, const point &p2) {}

	void HLine(const point &p, int len) {}
	void VLine(const point &p, int len) {}

	void Rectangle(const point &p, const point &p2) {}
	void Box(const point &p, const point &p2) {}

	void Erase(void) {}

protected:
private:
};

//==============================================================================

class nullRastGraphPort : public rastGraphPort
{
public:
	nullRastGraphPort() : rastGraphPort(0, 0, color()) {}

	~nullRastGraphPort() {}

	void PutPixel(const point &) {}

	const color& GetPixel(const point&) {return currentColor;}

	void Line(const point &p, const point &p2) {}

	void HLine(const point &p, int len) {}
	void VLine(const point &p, int len) {}

	void Rectangle(const point &p, const point &p2) {}
	void Box(const point &p, const point &p2) {}

	void Erase(void) {}

protected:
private:
};

//==============================================================================

#endif

#endif

//==============================================================================


