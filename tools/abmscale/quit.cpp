
#include <iostream.h>

#include <pclib/general.hpp>

#include "abmscale.hpp"

int
	ctrlBreakHandler( void )
	{
	delete display;
	cerr << "^C";
	Quit();
	}



