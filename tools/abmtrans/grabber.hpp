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
