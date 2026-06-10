//==============================================================================
// stdstrm.cpp: various output streams
//==============================================================================

#include <fstream.h>

#include <pclib/stdstrm.hpp>
#include <pclib/strmnull.hpp>

// note: we did it this way to insure strmnull gets constructed before the
// standard streams

#include <pclib/strmnull.cpp>

//==============================================================================

#ifdef __BORLANDC__
#define createAssignableOStream _drostream_withassign
#endif

#ifdef __WATCOMC__
#define createAssignableOStream ostream
#endif

createAssignableOStream cprogress(cnull);
createAssignableOStream cstats(cnull);
createAssignableOStream cdebug(cnull);
createAssignableOStream cwarn(cerr);
createAssignableOStream cerror(cerr);
createAssignableOStream cfatal(cerr);
createAssignableOStream cuser(cout);

//==============================================================================

ostream&
_RedirectStream(char* selector)
{
	switch(*selector)
	 {
		case 'n':
			return(cnull);
		case 's':
			return(cout);
		case 'e':
			return(cerr);
		case 'f':
			ofstream* file;
			file = new ofstream(selector+1);
			if(file && file->good())
				return(*file);
			cerr << "unable to open print file " << selector+1 << 'n';
	 }
	return(cnull);
};

//==============================================================================

void
RedirectStandardStream(char* str)
{
	switch(*str)
	 {
		case 'w':
			cwarn = _RedirectStream(str+1);
			break;
		case 'e':
			cerror = _RedirectStream(str+1);
			break;
		case 'f':
			cfatal = _RedirectStream(str+1);
			break;
		case 's':
			cstats = _RedirectStream(str+1);
			break;
		case 'p':
			cprogress = _RedirectStream(str+1);
			break;
		case 'd':
			cdebug = _RedirectStream(str+1);
			break;
	 }
}

//============================================================================
