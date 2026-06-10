//=============================================================================
//  object.c: object handling
//=============================================================================

#include	"base.hpp"
#include	"global.hpp"
#include	"list.hpp"
#include	"input.hpp"
#include	"object.hpp"
#include	"layer.hpp"

//-----------------------------------------------------------------------------

_object* pObjBase;


void
DoNothing(_object *obj)
	{
	}


void
_object::Construct()
	{
	next = prev = NULL;
	nCounter = nDelay = 0;

	routine = DoNothing;
	inputRoutine = NULL;
	layer = NULL;

	inputFlags = INPF_KEY;
	_object* oPtr = this;

	}


_object::_object()
	{
	Construct();
	}


_object::_object( void (*routine)(_object *) )
	{
	Construct();
	this->routine = routine;
	}


_object::~_object()
	{
	// Remove from list
	if ( Prev() )
		Prev()->Next( Next() );
	if ( Next() )
		Next()->Prev( Prev() );
	}

//=============================================================================

_object*
GetLastObject( void )
	{
	_object* oPtr;
	for ( oPtr = pObjBase; oPtr->Next(); oPtr=oPtr->Next() )
		;
	return( oPtr );
	}

//=============================================================================

void
_object::Update()
	{
	_object* oPtr = this;						//&objBase;

	while ( oPtr = oPtr->Next() )
		{
		if ( --( oPtr->nCounter ) <= 0 )
			{
			oPtr->nCounter = oPtr->Delay();
			oPtr->routine( oPtr );
			}
		}
	}

//=============================================================================

_object*
AddObject(void (*routine)(_object *),_object* oPtr)
	{
	if(!oPtr)
		oPtr = new _object( routine );
	if(oPtr)
	 {
#if 0
		oPtr->Prev( oPtr->Tail() );
		oPtr->Tail( oPtr );
#else
		_object* o2Ptr = GetLastObject();
		oPtr->Prev( o2Ptr );
		o2Ptr->Next( oPtr );
#endif
	 }
	return( oPtr );
	}

//=============================================================================

void
DeleteObject(_object *oPtr)
	{
	delete oPtr;
	}

//=============================================================================
