/*============================================================================*/
/*	 drmon.h: drmon header file										     */
/*============================================================================*/

extern FLAG cont,haveMouse;
extern _object *frontObj;
extern FLAG (*oldInputRoutine)(_input *,_object *);
extern char *xferBuffer;
extern char textBuffer[160+_MAX_PATH];
extern FLAG modeUpdate;

extern char *progName;
extern int monSpeed;

//=============================================================================

void
UnInit(int err,char *errText);

void
Init(void);

//=============================================================================
