//=============================================================================
//  menumbar.h: menubar simulating pull-down type menus header file
//=============================================================================

_menu *
GetMenu(_menu *mPtr,int choice);

void
UnHighlightMenu(_menu *mPtr,int currentMenu,_layer *lPtr);

void
HighlightMenu(_menu *mPtr,int currentMenu,_layer *lPtr);

void
MenuBarActivate(_object *oPtr);

void
MenuBarDeactivate(_object *oPtr);

void
RenderMenuTitles(_layer *lPtr,_menu *mPtr,int currentMenu);

void
MenuRoutine(_object *oPtr);

int
CountMenus(_menu *mPtr);

_menu *
FindLastMenu(_menu *mPtr);

void
AddMenu(_menu *mPtr,_menu *m2Ptr);

FLAG
MenuBarKeys(_input *in,_object *oPtr);

//=============================================================================
