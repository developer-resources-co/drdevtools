//==============================================================================
// grabber.hpp:
//==============================================================================

#include <pclib/grphport.hpp>

extern rastGraphPort* display;

extern unsigned char far* image[];

#define oldGetPixel(x,y)	( *(image[(y)]+(x)) )
