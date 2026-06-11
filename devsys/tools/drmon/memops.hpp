//=============================================================================
// memops.h
//=============================================================================

extern ULONG	globTemp,globTemp2;
extern menuItems *memOpsMenu;

//=============================================================================

FLAG
OpenMemSearchWindow(_menuItem *iPtr,_object *oPtr,int choice);

// Client-side memory search (defined in search.cpp); wired into memopsMenu.
FLAG
DoMemSearch(_menuItem *iPtr,_object *oPtr,int choice);

FLAG
OpenMemNextWindow(_menuItem *iPtr,_object *oPtr,int choice);

FLAG
OpenMemCompareWindow(_menuItem *iPtr,_object *oPtr,int choice);

FLAG
OpenMemFillWindow(_menuItem *iPtr,_object *oPtr,int choice);

FLAG
OpenMemCopyWindow(_menuItem *iPtr,_object *oPtr,int choice);

FLAG
OpenMemLoadWindow(_menuItem *iPtr,_object *oPtr,int choice);

FLAG
OpenMemSaveWindow(_menuItem *iPtr,_object *oPtr,int choice);

errorcode
LoadBinaryFile(ULONG addr,char *fileName);

errorcode
SaveBinaryFile(ULONG outStart,ULONG outLen,char *fileName);

//=============================================================================
