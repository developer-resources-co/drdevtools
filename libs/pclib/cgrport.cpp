//==============================================================================
// rgrPort.cpp:
//==============================================================================

#include "grphport.hpp"

//==============================================================================

void
clippedGraphPort::HLine(const point &p, int len)
{
	point myP(p);
	int otherEdge = myP.X() + len;

	if(myP.Y() < 0 || myP.Y() >= height)
		return;

	if(myP.X() < 0)
	 {
		if(otherEdge < 0)
			return;
		myP.X(0);
	 }

	if(myP.X() >= width)
	 {
		if(otherEdge >= width)
			return;
		myP.X(width-1);
	 }

	if(otherEdge < 0)
		otherEdge = 0;

	if(otherEdge >= width)
		otherEdge = width-1;

	otherEdge -= myP.X();
	myP += origin;
	gPort->HLine(myP,otherEdge);
}

//==============================================================================

void
clippedGraphPort::VLine(const point &p, int len)
{
	point myP(p);
	int otherEdge = p.Y() + len;

	if(myP.X() < 0 || myP.X() >= width)
		return;

	if(myP.Y() < 0)
	 {
		if(otherEdge < 0)
			return;
		myP.Y(0);
	 }

	if(myP.Y() >= height)
	 {
		if(otherEdge >= height)
			return;
		myP.Y(height - 1);
	 }

	if(otherEdge < 0)
		otherEdge = 0;

	if(otherEdge >= height)
		otherEdge = height - 1;

	otherEdge -= myP.Y();
	myP += origin;
	gPort->VLine(myP,otherEdge);
}

//==============================================================================

void
clippedGraphPort::Rectangle(const point &p, const point &p2)
{
	point ur,ll;

	ur.X(p2.X());
	ur.Y(p.Y());

	ll.X(p.X());
	ll.Y(p2.Y());

	HLine(p,p2.X()-p.X());
	VLine(ur,p2.Y()-ur.Y());
	HLine(p2,p.X()-p2.X());
	VLine(ll,p.Y()-ll.Y());
}

//==============================================================================

void
clippedGraphPort::Box(const point &p, const point &p2)
{
	int len = p2.X() - p.X();
	int height = p2.Y() - p.Y();
	point tp(p);
	point tp2(0,(height < 0)?-1:1);

	if(height < 0)
		height = -height;

	height++;
	while(height--)
	 {
		HLine(tp,len);
		tp += tp2;
	 }
}

//==============================================================================

void
clippedGraphPort::PlotOnPort(rastGraphPort &destPort)
{
	point sPoint(0,0),dPoint;
	point tempPoint(currentPoint);
	for(int y = destPort.Where().Y(); y < destPort.Where().Y()+GetHeight(); y++)
	 {
		dPoint.Y(y);
		sPoint.X(0);
		for(int x = destPort.Where().X(); x < destPort.Where().X()+GetWidth(); x++)
		 {
			dPoint.X(x);
			destPort.SetColorIndex(GetPixelIndex(sPoint));

			destPort.PutPixel(dPoint);
			sPoint.X(sPoint.X()+1);
		 }
		sPoint.Y(sPoint.Y()+1);
	 }
	currentPoint = tempPoint;
}

//==============================================================================

void
clippedGraphPort::Erase(void)
{
	gPort->Box(origin,point(rightEdge,botEdge));
}

//==============================================================================

void
clippedGraphPort::Xor(ubyte val)
{
	gPort->XorBox(origin,point(rightEdge,botEdge),val);
}

//==============================================================================
