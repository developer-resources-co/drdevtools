
#ifndef APP_H
#define APP_H

#include <stdlib.h>
//#include <dir.h>
#include <conio.h>

#include <pclib/filename.hpp>

class App
	{
public:
	App( int argc, char* argv[] );
	~App();

	const char* const operator()()					{ return( fn->file() ); }

	filename* fn;

private:
	char szStartingDirectory[ _MAX_PATH ];
	int disk;
#ifdef __MSDOS__
	int x, y;
	text_info ti;
	size_t cbScreen;
	void far* pScreenMem;
	void far* pPhysicalScreenMem;
#endif
	};

#endif

