//==============================================================================
// stdstrm.hpp: various output streams
//==============================================================================

#ifndef _STDSTREAM_HPP_
#define _STDSTREAM_HPP_

//== C++ =======================================================================
#include <iostream.h>

//== Adept Creations (or Developer Resources) "Library" ========================

//==============================================================================

#ifdef __BORLANDC__
class _drostream_withassign : public ostream_withassign
{
public:
    // does no initialization
    _Cdecl _drostream_withassign();
    _Cdecl _drostream_withassign(ostream& from)  { ostream_withassign::operator=(from); };
	_drostream_withassign _FAR & _Cdecl operator= (ostream _FAR& from) { ostream_withassign::operator=(from); return(*this); }
};

#define assignableOStream _drostream_withassign
#endif

#ifdef __WATCOMC__
#define assignableOStream ostream
#endif

extern assignableOStream cprogress;
extern assignableOStream cstats;
extern assignableOStream cdebug;
extern assignableOStream cwarn;
extern assignableOStream cerror;
extern assignableOStream cfatal;
extern assignableOStream cuser;

//==============================================================================
// RedirectStandardStream takes a 2 character string, the first char indicates
// which stream to redirect, and the second indicates where to send it
// here is a list of standard streams and possible outputs:
//        w=warnings
//        e=errors
//        f=fatal
//        s=statistics
//        p=progress
//        d=debugging
//          n=null
//          s=standard out
//          e=standard err
//          f=file
// in a standard tool, this should be switched on '-p'
//          Example: -ppn will send the progress stream to null
//============================================================================

void
RedirectStandardStream(char* str);

//==============================================================================

#endif

//==============================================================================
