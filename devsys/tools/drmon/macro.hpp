//=============================================================================
//  macro.h: macro handling routines header file
//=============================================================================

extern int *macroKeyPtr;

//=============================================================================

FLAG
NukeMacros(void);

void
InitMacros(void);

void
KeyCopy(int *dest,int *src);

FLAG
LoadGUIMacros(_menuItem *iPtr,_object *oPtr,int choice);

errorcode
LoadMacros(char *fileName);

errorcode
SaveMacros(char *fileName);

FLAG
SaveGUIMacros(_menuItem *iPtr,_object *oPtr,int choice);

FLAG
CreateMacro(_menuItem *iPtr,_object *oPtr,int choice);

void
FinishMacroRecording(void);

void
StartMacro(int fullKey);

int *
PopMacro(void);

int
GetMacroKey(void);

unsigned int
GetFullKey(void);

//=============================================================================
