//==============================================================================
// grphPort.cpp:
//==============================================================================

#include <assert.h>

#ifdef DOSX286
#include <phapi.h>
#endif

#include "error.hpp"
#include "grphport.hpp"

//==============================================================================

graphPort::graphPort(uint xSize, uint ySize, const color& c)
{
	width = xSize;
	height = ySize;
	currentColor = c;
}

//==============================================================================

#include "rgrport.cpp"
#include "sgrport.cpp"

//==============================================================================
