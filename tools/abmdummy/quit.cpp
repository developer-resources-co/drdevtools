
#include <iostream.h>

#include <pclib/grphport.hpp>

extern screenGraphPort* screen;

int
	ctrlBreakHandler( void )
	{
	delete screen;
	cerr << "^C";
	Quit();
	}



