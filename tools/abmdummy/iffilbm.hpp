//==============================================================================
// iffilbm.hpp:  header for ilbm parser built on iff parser
//==============================================================================
/*

Documentation:

	Abstract:

	History:
			Created	? Kevin T. Seghetti
			First documented 10-19-92 02:08pm
			28 Nov 93  WBNIV  Added constructors which accept iffRead parameter
			28 Nov 93  WBNIV  Added friend functions for printing the classes
			30 Nov 93  WBNIV  Added operators << and >>.

	Class Hierarchy:

	Dependancies:
			iff

	Restrictions:

	Example:

*/
//==============================================================================

#ifndef PCLIB_ilbm_H
#define PCLIB_ilbm_H

//==============================================================================
// dependancies

#include <pclib/iff.hpp>
#include <pclib/grphport.hpp>

//==============================================================================
// this should be inside the class

const iffTag tagILBM = MakeTag('I','L','B','M');

const iffTag tagCMAP = MakeTag('C','M','A','P');
const iffTag tagDPPS = MakeTag('D','P','P','S');
const iffTag tagCRNG = MakeTag('C','R','N','G');
const iffTag tagTINY = MakeTag('T','I','N','Y');
const iffTag tagCAMG = MakeTag('C','A','M','G');
const iffTag tagBMHD = MakeTag('B','M','H','D');
const iffTag tagBODY = MakeTag('B','O','D','Y');

//==============================================================================
// class declaration

class ilbmCMAP
	{
	uint numColors;
	ubyte* colorPtr;

	void Construct()  { numColors = 0; colorPtr = NULL; }
	errorcode Write( iffWrite* pWrite );
	errorcode Read(iffRead *readPtr);

public:
	// File I/O
	friend ostream& operator<<( ostream& s, ilbmCMAP& cmap );
	friend iffWrite& operator<<( iffWrite& s, ilbmCMAP& cmap )
		{ cmap.Write( &s );  return s; }
	friend iffRead& operator>>( iffRead& s, ilbmCMAP& cmap )
		{ cmap.Read( &s );  return s; }

	ilbmCMAP() { Construct(); }
	ilbmCMAP( iffRead* pRead ) { Construct(); *pRead >> *this; }

	~ilbmCMAP() { delete[] colorPtr; }
	ubyte Red(uint colNum) const { if(colNum > numColors) return(0); return colorPtr[colNum*3]; }
	ubyte Green(uint colNum) const { if(colNum > numColors) return(0); return colorPtr[(colNum*3)+1]; }
	ubyte Blue(uint colNum) const { if(colNum > numColors) return(0); return colorPtr[(colNum*3)+2]; }
//	ubyte *GetColorPtr() { return(colorPtr); }
	ulong GetNumColors() const { return(numColors); }
	};

//==============================================================================
// class declaration

class ilbmBMHD
	{
private:
	errorcode Read(iffRead *readPtr);
	errorcode Write( iffWrite* pWrite );

public:
	enum
	 {
		mskNone,
		mskHasMask,
		mskHasTransparentColor,
		mskLasso
	 };

	enum
	 {
		cmpNone,
		cmpByteRun1
	 };

	uword w, h;
	word x,y;
	ubyte nPlanes;
	ubyte masking;
	ubyte compression;
	uword transparentColor;
	ubyte xAspect, yAspect;
	word pageWidth, pageHeight;

	// File I/O
	friend ostream& operator<<( ostream& s, ilbmBMHD& bmhd );
	friend iffWrite& operator<<( iffWrite& s, ilbmBMHD& bmhd )
		{ bmhd.Write( &s );  return s; }
	friend iffRead& operator>>( iffRead& s, ilbmBMHD& bmhd )
		{ bmhd.Read( &s );  return s; }

	// constructors
	ilbmBMHD() { w = h = x = y = nPlanes = masking = compression = transparentColor =
				 xAspect = yAspect = pageWidth = pageHeight = 0; }
	ilbmBMHD( iffRead* readPtr )	{ *readPtr >> *this; }
	};

//==============================================================================
// class declaration

class ilbmCAMG
{
	ulong viewModes;
	errorcode Read(iffRead *readPtr) { viewModes = readPtr->GetChunkLong(); return(NOERR);}
public:
	ilbmCAMG() { viewModes = 0; }
// !!! MASK OFF BADBITS AS PER COMMODORE DOCUMENTATION
	ulong GetViewModes() const { return(viewModes); }

	// File I/O
#if 0
	friend ostream& operator<<( ostream& s, ilbmBMHD& bmhd );
	friend iffWrite& operator<<( iffWrite& s, ilbmBMHD& bmhd )
		{ bmhd.Write( &s );  return s; }
	friend iffRead& operator>>( iffRead& s, ilbmBMHD& bmhd )
		{ bmhd.Read( &s );  return s; }
#endif
};

//==============================================================================
// class declaration

class ilbmBODY
	{
	ilbmBMHD* bmhd;
	ilbmBODY() { };					// cannot create without bmhd pointer

	errorcode Read(iffRead *readPtr, errorcode parseLine(ubyte *line));
//	errorcode Write( iffWrite* pWrite, errorcode parseLine( ubyte* line ) );

protected:
	int ilbmBODY::decompPlaneLine( iffRead* readPtr, ubyte *plane_line_ptr, int byte_length, ubyte compression);
	int ilbmBODY::decompLine( iffRead* readPtr, ubyte* line_ptr );

	int ilbmBODY::compLine( iffWrite* pWrite, ubyte* line_ptr );

public:
	ilbmBODY( ilbmBMHD* newBMHD ) {  bmhd = newBMHD; }
	ilbmBODY( ilbmBMHD* newBMHD, iffRead* pRead, rastGraphPort* rp );

	// File I/O
#if 0
	friend iffWrite& operator<<( iffWrite& s, ilbmBMHD& bmhd )
		{ bmhd.Write( &s );  return s; }
	friend iffRead& operator>>( iffRead& s, ilbmBMHD& bmhd )
		{ bmhd.Read( &s );  return s; }
#endif
	};

//==============================================================================

#endif

//==============================================================================
