//==============================================================================
// rgrPort.cpp:
//==============================================================================

#include <malloc.h>

void
rastGraphPort::Construct(const color& c)
	{
	mapCols = new colorPalette;
	assert( mapCols );
	currentIndex = mapCols->GetMapIndex( c );
	}

//==============================================================================

void
rastGraphPort::Erase(void)
{
//	for(uint i=0;i<width*height;i++)
//		mapPtr[i] = currentIndex;
}

void
rastGraphPort::Xor(ubyte val)
{
//	for(uint i=0;i<width*height;i++)
//		mapPtr[i] = mapPtr[i] ^ val;
}

//==============================================================================

rastGraphPort::rastGraphPort(uint xSize, uint ySize, const color &c)
: graphPort(xSize, ySize, c)
{
	assert( ySize <= MAX_RASTPORT_LINES );

	ownBuffer = boolean::TRUE;

	for ( uint i = 0; i < height; ++i )
		{
		lineArrayPtr[ i ] = new ubyte[ width ];
		assert( lineArrayPtr[ i ] );
		}

	Construct(c);
	Erase();
}

//==============================================================================

rastGraphPort::rastGraphPort(ubyte *map[],uint xSize, uint ySize, const color &c)
: graphPort(xSize, ySize, c)
{
	assert( ySize <= MAX_RASTPORT_LINES );

	ownBuffer = boolean::FALSE;
	if ( map )
		for ( uint i=0; i<ySize; ++i )
			lineArrayPtr[ i ] = map[ i ];
	Construct(c);
}

//==============================================================================

rastGraphPort::rastGraphPort( rastGraphPort& rgp ) : graphPort( rgp )
	{
	*this = rgp;

	ownBuffer = boolean::TRUE;
	for ( uint i = 0; i < height; ++i )
		{
		lineArrayPtr[ i ] = new ubyte[ width ];
		assert( lineArrayPtr[ i ] );
		memcpy( lineArrayPtr[ i ], rgp.lineArrayPtr[ i ], width );
		}

	Construct( color(0,0,0) );
	}

//==============================================================================

rastGraphPort::~rastGraphPort()
	{
	if ( ownBuffer )
		{
		for ( uint i=0; i<height; ++i )
			delete[] lineArrayPtr[ i ];
		}
	delete mapCols;
	}

//==============================================================================

/*
void
rastGraphPort::PutPixel(const point &p)
{
	mapPtr[p.X() + *(lineArrayPtr+p.Y())] = currentIndex;
}
 */

//==============================================================================

const color&
rastGraphPort::GetPixel(const point &p)
{
	colorMapIndex cmi;
//	cmi = mapPtr[p.X() + *(lineArrayPtr+p.Y())];
	cmi = lineArrayPtr[ p.Y() ][ p.X() ];
	return( mapCols->GetColor(cmi));
}

//==============================================================================

// first, a slow version

void
rastGraphPort::PlotOnPort(rastGraphPort &destPort)
{
	point sPoint(0,0),dPoint;
	point tempPoint(currentPoint);
	for(int y = destPort.Where().Y(); y <  destPort.Where().Y()+GetHeight(); y++)
	 {
		dPoint.Y(y);
		sPoint.X(0);
		for(int x = destPort.Where().X(); x <  destPort.Where().X()+GetWidth(); x++)
		 {
			dPoint.X(x);
			destPort.SetColorIndex(GetPixelIndex(sPoint));

			int foobar;
			if(GetPixelIndex(sPoint) > 2)
				foobar = 4;

			destPort.PutPixel(dPoint);
			sPoint.X(sPoint.X()+1);
		 }
		sPoint.Y(sPoint.Y()+1);
	 }
	currentPoint = tempPoint;
}

//==============================================================================

void
rastGraphPort::Line(const point &p, const point &p2)
{
	int lgStep = 1;
	int shStep = 1;
	int cycle;

	int x2 = p2.X();
	int y2 = p2.Y();

	point tp(p.X(),p.Y());							// temp point

	int lgDelta = x2 - tp.X();
	int shDelta = y2 - tp.Y();

	if(lgDelta < 0)
	 {
		lgDelta = -lgDelta;
		lgStep = -1;
	 }

	if(shDelta < 0)
	 {
		shDelta = -shDelta;
		shStep = -1;
	 }

	if(shDelta > lgDelta)
	 {
		cycle = shDelta/2;

		while(tp.Y() != y2)
		 {
			PutPixel(tp);
			tp.Y(tp.Y()+shStep);
			cycle += lgDelta;
			if(cycle >= shDelta)
		 	{
				tp.X(tp.X()+lgStep);
				cycle -= shDelta;
		 	}
		 }
		PutPixel(tp);
	 }
	else
	 {
		cycle = lgDelta/2;

		while(tp.X() != x2)
		 {
			PutPixel(tp);
			tp.X(tp.X()+ lgStep);
			cycle += shDelta;
			if(cycle >= lgDelta)
		 	{
				tp.Y(tp.Y()+ shStep);
				cycle -= lgDelta;
		 	}
		 }
		PutPixel(tp);
	 }
}

//==============================================================================

void
rastGraphPort::HLine(const point &p, int len)
{
	ubyte* mPtr = &( lineArrayPtr[ p.Y() ][ p.X() ] );

	if(len < 0)
	 {
		len = -len;
		len++;
		while(len--)
			*mPtr-- = currentIndex;
	 }
	else
	 {
		len++;
		while(len--)
			*mPtr++ = currentIndex;
	 }
}

//==============================================================================

void
rastGraphPort::XorHLine(const point &p, int len, ubyte val)
{
	ubyte* mPtr = &( lineArrayPtr[ p.Y() ][ p.X() ] );

	if(len < 0)
	 {
		len = -len;
		len++;
		while(len--)
		 {
			*mPtr = *mPtr ^ val;
			mPtr--;
		 }
	 }
	else
	 {
		len++;
		while(len--)
		 {
			*mPtr = *mPtr ^ val;
			mPtr++;
		 }
	 }
}

//==============================================================================

void
rastGraphPort::VLine(const point &p, int len)
{
	point tp = p;

	if(len < 0)
	 {
		len = -len;
		len++;
		while(len--)
		 {
			PutPixel(tp);
			tp.Y(tp.Y()-1);
		 }

	 }
	else
	 {
		len++;
		while(len--)
		 {
			PutPixel(tp);
			tp.Y(tp.Y()+1);
		 }
	 }
}

//==============================================================================

void
rastGraphPort::Rectangle(const point &p, const point &p2)
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
rastGraphPort::Box(const point &p, const point &p2)
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
rastGraphPort::XorBox(const point &p, const point &p2,ubyte val)
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
		XorHLine(tp,len,val);
		tp += tp2;
	 }
}

//==============================================================================

void
rastGraphPort::SetColor(const color& c)
{
	graphPort::SetColor(c);
	currentIndex = mapCols->GetMapIndex(c);
}

void
rastGraphPort::SetColorIndex(int newIndex)
{
	currentIndex = newIndex;
}

//==============================================================================
