
#include <iostream.h>

#include <pclib/grphport.hpp>

extern screenGraphPort* screen;

#include "grabber.hpp"

int
	ctrlBreakHandler( void )
	{
	delete screen;
	cerr << "^C";
	Quit();
	}



