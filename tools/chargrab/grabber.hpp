//==============================================================================
// grabber.hpp:
//==============================================================================

#include <pclib/iff.hpp>
#include "iffilbm.hpp"
#include "iffpbm.hpp"
#include "iffanim.hpp"
#include <pclib/grphport.hpp>

class Picture
	{
public:
	rastGraphPort* rp;
	ilbmBMHD* BitmapHeader;
	ilbmCMAP* ColorMap;
	ilbmBODY* oldBODY;
	pbmBODY*  newBODY;

	Picture* pNext;

	Picture();
	Picture( istream& input );
	~Picture( void );

	friend iffWrite& operator<<( iffWrite& s, Picture& pict );
	};


/***** OLD CHARGRAB *****/
extern boolean nukeChars;
extern boolean bTile0Blank;
extern int nLimitChars;
extern boolean checkRedundantChars;
extern boolean checkFlips;



#define CHAR_X 8
#define CHAR_Y 8
#define MAXGRID_X 400
#define MAXGRID_Y 32

#define MEGAMAXCHARS 2048


void OpenExistingCharFile (char *OutputFile);
int CheckDups (int a,int b);
void WriteChars (char *OutputFile);
void WriteMap (char *MapFile);
void FlipCharX (void);
void FlipCharY (void);

extern int charSize;

#define MAXCHARS 2048
#define CHARMASK   0x07FF
#define HFLIPMASK  0x0800
#define VFLIPMASK  0x1000
#define PRIBITMASK 0x8000

#define NUM_PALS	4
#define PALBIT     13
#define PALMASK    0x6000   		// AND Mask for Pal 0
//#define PAL1MASK   0x2000   		// OR mask
//#define PAL2MASK   0x4000   		// OR mask
//#define PAL3MASK   0x6000   		// OR mask

#define PAL0MASK   (~PALMASK)		// AND Mask for Pal 0
#define PAL1MASK	(PAL0MASK<<1)
#define PAL2MASK	(PAL0MASK<<2)
#define PAL3MASK	(PAL0MASK<<3)
/***** OLD CHARGRAB *****/



















