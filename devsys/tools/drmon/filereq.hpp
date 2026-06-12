//=============================================================================
//  filereq.h: file requester header file
//=============================================================================

void
AddFileReqGadgets(_object *oPtr);

void
DoFileReq(char *title,int xPos,int yPos,void (*fileRoutine)(void *data,char *path,char *fileName),void *data);

FLAG
FileReqInput(_input *in,_object *oPtr);

extern char patternString[_MAX_PATH];

//=============================================================================
