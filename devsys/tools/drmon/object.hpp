//=============================================================================
//  object.h: object handling definitions
//=============================================================================

#ifndef DRMON_object_H
#define DRMON_object_H

//=============================================================================

#include "input.hpp"
#include "layer.hpp"

//=============================================================================

class _object
{
private:
	_object *next;
	_object *prev;
	int nCounter;
	int nDelay;

// private functions
	void Construct();

public:
// link accessors
	_object *Next(void) { return(next); }
	_object *Next(_object *newNext) { next = newNext; return(next); }

	_object *Prev(void) { return(prev); }
	_object *Prev(_object *newPrev) { prev = newPrev; return(prev); }

// constructors and public routines
	_object();
	_object( void (*routine)(_object *) );
	~_object();
	void Update();

	int Delay()			{ return( nDelay ); }
	int Delay( int newDelay )		{ nDelay = newDelay;	 return( Delay() ); }

// ugly public data
//    unsigned int flags;				// what are flags used for?

// input stuff
    FLAG (*inputRoutine)(_input *,_object *);		// called for every keystroke
	unsigned int inputFlags;			// different input enables
	_inputRemap remap;
// execution data
    void (*routine)(_object *);	// called every frame for update
// window data
    _layer *layer;				// note: not all objects need have a layer
// really ugly, overloaded fields
	unsigned long addr;								// user defined, dumps use to store address
    char *dataPtr,*dataPtr2;						// user defined, menus use dataPtr to point to menu
	char *dataPtr3,*dataPtr4;						// user defined
	char *dataPtr5;									// user defined
	int data1,data2,data3,data4,data5,data6,data7;	// user defined, for counters, positions etc.
	long longData,longData2;
};

//============================================================================

extern _object* pObjBase;

//=============================================================================

void InitObjects(void);

_object* GetLastObject(void);

void UpdateObjects(void);

void DoNothing( _object* );

_object* AddObject( void (*routine)( _object* ) = DoNothing, _object* oPtr = NULL);

void DeleteObject( _object* );

//=============================================================================

#endif

//=============================================================================

