//=============================================================================
//  manager.h: main control logic functions header file
//=============================================================================

extern _object *managerObjPtr;

//=============================================================================

FLAG
WindowMoveKeys(_input *in,_object *oPtr);

FLAG
GlobInput(_input *in,_object *oPtr);

void
MouseActivateWindow(_input *in);

void
Manager(_object *oPtr);

void
InitManager(void);

void
ForceWindowResize ( _layer *lPtr, _object *oPtr,FLAG(*inputRoutine)(_input *,_object *) );

//=============================================================================
