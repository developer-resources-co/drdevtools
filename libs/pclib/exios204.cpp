
#include "hstream.hpp"

hstreambuf hsb();

void main()
	{
	char line[200];

	cout = &hsb;
	for (;;)
		{
		cin.getline( line, 200 );
		if ( !cin.good() ) break;
		cout << line << endl;
		}
	}


