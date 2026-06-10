//==============================================================================
// rgrPort.cpp:
//==============================================================================

#include "pclib/grphport.hpp"

//==============================================================================

void
maskedGraphPort::HLine(const point &p, int len)
{
	point myP(p);
	int otherEdge = p.X() + len;

	if(myP.Y() < yOffset || myP.Y() > botEdge)
		return;

	if(myP.X() < xOffset)
	 {
		if(otherEdge < xOffset)
			return;
		myP.X(xOffset);
	 }

	if(myP.X() >= rightEdge)
	 {
		if(otherEdge >= rightEdge)
			return;
		myP.X(rightEdge);
	 }

	if(otherEdge < xOffset)
		otherEdge = xOffset;

	if(otherEdge >= rightEdge)
		otherEdge = rightEdge;

	gPort->HLine(myP,otherEdge - myP.X());
}

//==============================================================================

void
maskedGraphPort::VLine(const point &p, int len)
{
	point myP(p);
	int otherEdge = p.Y() + len;

	if(myP.X() < xOffset || myP.X() > rightEdge)
		return;

	if(myP.Y() < yOffset)
	 {
		if(otherEdge < yOffset)
			return;
		myP.Y(yOffset);
	 }

	if(myP.Y() >= botEdge)
	 {
		if(otherEdge >= botEdge)
			return;
		myP.Y(botEdge);
	 }

	if(otherEdge < yOffset)
		otherEdge = yOffset;

	if(otherEdge >= botEdge)
		otherEdge = botEdge;

	gPort->VLine(myP,otherEdge - myP.Y());
}

//==============================================================================

void
maskedGraphPort::Rectangle(const point &p, const point &p2)
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
maskedGraphPort::Box(const point &p, const point &p2)
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
