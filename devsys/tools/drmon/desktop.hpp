//=============================================================================
//  desktop.hpp — Option A: one drmon process, N terminals ("multiple monitors").
//
//  A "desktop" is one terminal's complete UI: its window list (layBase), object
//  list (pObjBase), front window, framebuffer (screen/scrBuffer), cursor, console
//  history, and per-window singleton flags. All desktops share the ONE MAME
//  connection + debug/breakpoint state (those globals are not virtualized here).
//
//  The live globals always hold the ACTIVE desktop. switchDesktop() saves them
//  into the outgoing Desktop and loads the incoming one. Swapping the intrusive
//  list heads (layBase/statTextBase/cmdTextBase) by value is safe: the layer/list
//  code hardcodes the head's fixed global address (e.g. layer.cpp's Tail() walks
//  from &layBase), so only the head's *contents* move and only the active list is
//  ever traversed — nodes stay consistent with the fixed head address.
//
//  Must be included AFTER the per-desktop globals' types are defined (layer.hpp,
//  object.hpp, console.hpp, command.hpp); it is included last in moninc.hpp.
//=============================================================================
#ifndef DRMON_DESKTOP_HPP
#define DRMON_DESKTOP_HPP

#define DRMON_MAX_DESKTOPS 8

struct Desktop
{
    int          nc;            // ncurses terminal handle (0 = primary)

    // saved copies of the per-desktop globals (authoritative only while inactive)
    _layerBase   layBase;
    FLAG         refreshAll, refreshEnable;
    _object     *pObjBase;
    _object     *frontObj;
    char *screen, *screen2, *scrBuffer, *scrBuffer2;
    unsigned short screenW, screenH, screenSz;   // this terminal's dimensions
    int          cursorX, cursorY;
    FLAG         curOn;
    int          cursorInsert;
    _object     *menuBarObject, *managerObjPtr;
    _layer      *messageLayer;          // the message bar (InitMessageBar delete+recreates it)
    _object     *commandWindow, *cmdObjPtr;
    _statText    statTextBase;
    _cmdText     cmdTextBase;
    FLAG         menuUp;
    _gadget     *pGadgDown;             // gadget under an in-progress mouse press
    // per-window singletons (each terminal can open its own)
    FLAG         regOpen;     _object *regObjPtr;
    FLAG         breakOpen;   _object *breakObjPtr;
    FLAG         symbolOpen;  _object *symbolObjPtr;
    FLAG         sourceOpen;
    FLAG         cmdOpen;
    FLAG         aboutOpen;   _object *aboutObjPtr;
    FLAG         exprOpen;    _object *exprObjPtr;
#ifdef SPC700
    FLAG         spcRegOpen;  _object *spcRegObjPtr;   // SNES audio co-CPU window
#endif
    _object     *searchObjPtr;
};

extern Desktop *g_curDesktop;                      // the active desktop
extern Desktop *g_desktops[DRMON_MAX_DESKTOPS];    // all desktops, for the main loop
extern int      g_numDesktops;

Desktop *MakeDesktop(int ncHandle);   // allocate + register an empty desktop
void     switchDesktop(Desktop *d);   // save current globals, load d's, select its terminal
void     InitDesktop(void);           // build the per-desktop UI into the current globals
int      NewDesktop(void);            // Windows > New Window: open a terminal + desktop; 0 / -1

#endif
