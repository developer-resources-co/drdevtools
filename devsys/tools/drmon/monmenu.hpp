//=============================================================================
//  monmenu.hpp: menu bar for drmon header file
//=============================================================================

void
ActivateMenuBar(void);

_menu *
SetupMenuBar(void);

void
InitMenuBar(void);

void
GetMenuBarLayer(void);

//==============================================================================

extern char* speedMenuItems[];
extern FLAG ((*memOpsMenuRoutines[])(_menuItem *iPtr,_object *oPtr,int choice));
extern char* memOpsMenuItems[];
extern char logFileName[_MAX_PATH];

//============================================================================
