
#include <iostream.h>

#include <pclib/general.hpp>

#include "anmsplit.hpp"

int
	ctrlBreakHandler( void )
	{
	delete display;
	cerr << "^C";
	Quit();
	}



