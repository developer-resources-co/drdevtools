
#include <iostream.h>

#include <pclib/general.hpp>

#include "abmsplit.h"

int
	ctrlBreakHandler( void )
	{
	delete display;
	cerr << "^C";
	Quit();
	}



