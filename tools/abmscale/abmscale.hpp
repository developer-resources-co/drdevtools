//==============================================================================
// abmscale.hpp:
//==============================================================================

#include <pclib/grphport.hpp>

extern rastGraphPort* display;

extern unsigned char far* image[];
extern unsigned char far* output_image[];

#define oldGetPixel(x,y)	( *(image[(y)]+(x)) )
