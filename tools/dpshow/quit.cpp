
#include <iostream.h>

#include <pclib/general.hpp>

#include "grabber.hpp"

int
	ctrlBreakHandler( void )
	{
	cerr << "^C";
	Quit();
	}



