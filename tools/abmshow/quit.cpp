
#include <iostream.h>

#include <pclib/general.hpp>

#include "abmshow.hpp"

int
	ctrlBreakHandler( void )
	{
	delete display;
	cerr << "^C";
	Quit();
	}



