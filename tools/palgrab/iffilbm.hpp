//==============================================================================
// ilbm.h: header for ilbm parser built on iff parser
//==============================================================================
/*

Documentation:

	Abstract:

	History:
			Created	? Kevin T. Seghetti
			First documented 10-19-92 02:08pm

	Class Hierarchy:

	Dependancies:
			iff

	Restrictions:

	Example:

*/
//==============================================================================
// use only once insurance

#ifndef PCLIB_ilbm_H
#define PCLIB_ilbm_H

//==============================================================================
// dependancies

#include <pclib/iff.hpp>

//==============================================================================
// this should be inside the class

const iffTag tagILBM = MakeTag('I','L','B','M');

const iffTag tagCMAP = MakeTag('C','M','A','P');
const iffTag tagCAMG = MakeTag('C','A','M','G');
const iffTag tagBMHD = MakeTag('B','M','H','D');
const iffTag tagBODY = MakeTag('B','O','D','Y');

//==============================================================================
// class declaration

class ilbmCMAP
{
	uint numColors;
	ubyte *colorPtr;
public:
	ilbmCMAP() { numColors = 0; colorPtr = NULL; }
	~ilbmCMAP() { delete[] colorPtr; }
	errorcode Read(iffRead *readPtr);
	ubyte Red(uint colNum) const { if(colNum > numColors) return(0); return colorPtr[colNum*3]; }
	ubyte Green(uint colNum) const { if(colNum > numColors) return(0); return colorPtr[(colNum*3)+1]; }
	ubyte Blue(uint colNum) const { if(colNum > numColors) return(0); return colorPtr[(colNum*3)+2]; }
//	ubyte *GetColorPtr() { return(colorPtr); }
	ulong GetNumColors() const { return(numColors); }

	errorcode Write( iffWrite* pWrite );
};

//==============================================================================
// class declaration

class ilbmBMHD
{

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
	// functions
	ilbmBMHD() { w = h = x = y = nPlanes = masking = compression = transparentColor =
				 xAspect = yAspect = pageWidth = pageHeight = 0; }
	errorcode Read(iffRead *readPtr);
	errorcode Write( iffWrite* pWrite );
};

//==============================================================================
// class declaration

class ilbmCAMG
{
	ulong viewModes;
public:
	ilbmCAMG() { viewModes = 0; }
// !!! MASK OFF BADBITS AS PER COMMODORE DOCUMENTATION
	errorcode Read(iffRead *readPtr) { viewModes = readPtr->GetChunkLong(); return(NOERR);}
	ulong GetViewModes() const { return(viewModes); }
};

//==============================================================================
// class declaration

class ilbmBODY
{
	ilbmBMHD *bmhd;
	ilbmBODY() { };					// cannot create without bmhd pointer

protected:
	int ilbmBODY::decompPlaneLine( iffRead* readPtr, ubyte *plane_line_ptr, int byte_length, ubyte compression);
	int ilbmBODY::decompLine( iffRead* readPtr, ubyte* line_ptr );

	int ilbmBODY::compLine( iffWrite* pWrite, ubyte* line_ptr );

public:
	ilbmBODY(ilbmBMHD *newBMHD) {  bmhd = newBMHD; }
	errorcode Read(iffRead *readPtr, errorcode parseLine(ubyte *line));
//	errorcode Write( iffWrite* pWrite, errorcode parseLine( ubyte* line ) );
};

//==============================================================================

#endif

//==============================================================================
