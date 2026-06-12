//=============================================================================
//  desktop.cpp — Option A: one drmon process driving N terminals.
//
//  The live per-desktop globals (window list, object list, framebuffer, cursor,
//  console history, per-window singleton flags) always hold the ACTIVE desktop.
//  switchDesktop() saves them into the outgoing Desktop and loads the incoming
//  one, then points the ncurses front end at that terminal's SCREEN.
//
//  Swapping the intrusive list heads (layBase/statTextBase/cmdTextBase) by value
//  is safe because every list/layer operation references the head's FIXED GLOBAL
//  address (layer.cpp's Tail() walks from &layBase; the lists are reached via
//  (_list*)&statTextBase): only the head's contents move, and only the active
//  desktop's list is ever traversed, so nodes stay consistent with the head.
//=============================================================================
#include "moninc.hpp"
#include "display.hpp"   // InitDisplay / SetupDisplay (not pulled in by moninc.hpp)

#if defined(__GNUC__)
#include <ncurses_io.h>
#include <pty.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

//-----------------------------------------------------------------------------
// The live globals we virtualize (each defined in its own .cpp; we only swap
// them here). Types mirror the definitions exactly.
extern _layerBase  layBase;
extern FLAG        refreshAll, refreshEnable;
extern _object    *pObjBase;
extern _object    *frontObj;
extern char far   *screen, *screen2;
extern char far   *scrBuffer, *scrBuffer2;
extern int         cursorX, cursorY;
extern FLAG        curOn;
extern int         cursorInsert;
extern _object    *menuBarObject;
extern _object    *managerObjPtr;
extern _layer     *messageLayer;
extern _object    *commandWindow, *cmdObjPtr;
extern _statText   statTextBase;
extern _cmdText    cmdTextBase;
extern FLAG        menuUp;
extern _gadget    *pGadgDown;
extern FLAG        regOpen;     extern _object *regObjPtr;
extern FLAG        breakOpen;   extern _object *breakObjPtr;
extern FLAG        symbolOpen;  extern _object *symbolObjPtr;
extern FLAG        sourceOpen;
extern FLAG        cmdOpen;
extern FLAG        aboutOpen;   extern _object *aboutObjPtr;
extern FLAG        exprOpen;    extern _object *exprObjPtr;
#ifdef SPC700
extern FLAG        spcRegOpen;  extern _object *spcRegObjPtr;
#endif
extern _object    *searchObjPtr;
extern unsigned short screenWidth, screenHeight;   // SHARED (uniform terminal size)

//-----------------------------------------------------------------------------

Desktop *g_curDesktop = NULL;
Desktop *g_desktops[DRMON_MAX_DESKTOPS];
int      g_numDesktops = 0;

// Copy the live globals into / out of a Desktop. Intrusive heads (layBase,
// statTextBase, cmdTextBase) are copied by value — safe, see the header note.
static void desktop_save(Desktop *d)
{
    d->layBase = layBase; d->refreshAll = refreshAll; d->refreshEnable = refreshEnable;
    d->pObjBase = pObjBase; d->frontObj = frontObj;
    d->screen = screen; d->screen2 = screen2; d->scrBuffer = scrBuffer; d->scrBuffer2 = scrBuffer2;
    d->cursorX = cursorX; d->cursorY = cursorY; d->curOn = curOn; d->cursorInsert = cursorInsert;
    d->menuBarObject = menuBarObject; d->managerObjPtr = managerObjPtr; d->messageLayer = messageLayer;
    d->commandWindow = commandWindow; d->cmdObjPtr = cmdObjPtr;
    d->statTextBase = statTextBase; d->cmdTextBase = cmdTextBase; d->menuUp = menuUp; d->pGadgDown = pGadgDown;
    d->regOpen = regOpen; d->regObjPtr = regObjPtr;
    d->breakOpen = breakOpen; d->breakObjPtr = breakObjPtr;
    d->symbolOpen = symbolOpen; d->symbolObjPtr = symbolObjPtr;
    d->sourceOpen = sourceOpen; d->cmdOpen = cmdOpen;
    d->aboutOpen = aboutOpen; d->aboutObjPtr = aboutObjPtr;
    d->exprOpen = exprOpen; d->exprObjPtr = exprObjPtr;
#ifdef SPC700
    d->spcRegOpen = spcRegOpen; d->spcRegObjPtr = spcRegObjPtr;
#endif
    d->searchObjPtr = searchObjPtr;
}

static void desktop_load(Desktop *d)
{
    layBase = d->layBase; refreshAll = d->refreshAll; refreshEnable = d->refreshEnable;
    pObjBase = d->pObjBase; frontObj = d->frontObj;
    screen = d->screen; screen2 = d->screen2; scrBuffer = d->scrBuffer; scrBuffer2 = d->scrBuffer2;
    cursorX = d->cursorX; cursorY = d->cursorY; curOn = d->curOn; cursorInsert = d->cursorInsert;
    menuBarObject = d->menuBarObject; managerObjPtr = d->managerObjPtr; messageLayer = d->messageLayer;
    commandWindow = d->commandWindow; cmdObjPtr = d->cmdObjPtr;
    statTextBase = d->statTextBase; cmdTextBase = d->cmdTextBase; menuUp = d->menuUp; pGadgDown = d->pGadgDown;
    regOpen = d->regOpen; regObjPtr = d->regObjPtr;
    breakOpen = d->breakOpen; breakObjPtr = d->breakObjPtr;
    symbolOpen = d->symbolOpen; symbolObjPtr = d->symbolObjPtr;
    sourceOpen = d->sourceOpen; cmdOpen = d->cmdOpen;
    aboutOpen = d->aboutOpen; aboutObjPtr = d->aboutObjPtr;
    exprOpen = d->exprOpen; exprObjPtr = d->exprObjPtr;
#ifdef SPC700
    spcRegOpen = d->spcRegOpen; spcRegObjPtr = d->spcRegObjPtr;
#endif
    searchObjPtr = d->searchObjPtr;
}

//-----------------------------------------------------------------------------

Desktop *MakeDesktop(int ncHandle)
{
    if (g_numDesktops >= DRMON_MAX_DESKTOPS) return NULL;
    Desktop *d = new Desktop();   // value-init: pointers NULL, list heads empty, layBase.buffer 0
    if (!d) return NULL;
    d->nc = ncHandle;
    g_desktops[g_numDesktops++] = d;
    return d;
}

void switchDesktop(Desktop *d)
{
    if (!d || d == g_curDesktop) return;
    if (g_curDesktop) desktop_save(g_curDesktop);
    desktop_load(d);
    g_curDesktop = d;
#if defined(__GNUC__)
    drmon_nc_select(d->nc);
    InvalidatePointer();   // the mouse-backdrop cache pointed into the old desktop's scrBuffer
#endif
}

// Build the per-desktop UI into the (already-selected) current globals. Mirrors
// the per-desktop slice of Init() in drmon.cpp; the global, once-only init
// (slave I/O, ncurses, input, breakpoints, symbols) is NOT repeated here.
void InitDesktop(void)
{
    // refreshEnable is statically TRUE in layer.cpp, but a value-initialized Desktop
    // zeroes it — without this, _layerBase::Display() no-ops and the terminal stays
    // blank (just the background). refreshAll is (re)set by SetupDisplay below.
    refreshEnable = boolean::TRUE;
    pObjBase = new _object;
    SetupScreen();      // size + allocate this desktop's screen[] for its terminal
    InitDisplay();
    SetupDisplay();     // allocate this desktop's scrBuffer
    InitListBase((_list *)&statTextBase);
    InitListBase((_list *)&cmdTextBase);
    commandWindow = NULL;
    cmdObjPtr = NULL;
    InitManager();      // Manager + menu bar + message bar objects, into pObjBase/layBase
}

//-----------------------------------------------------------------------------
// Windows > New Window: open a real terminal (xterm on a PTY) driven by this
// process, and build a fresh desktop on it. Restores the caller's active desktop
// before returning (the caller is mid-frame on it). 0 on success, -1 on failure.

#if defined(__GNUC__)

static void reap_terminals(int) { while (waitpid(-1, NULL, WNOHANG) > 0) {} }

int NewDesktop(void)
{
    if (g_numDesktops >= DRMON_MAX_DESKTOPS) return -1;
    if (!getenv("DISPLAY"))                  return -1;   // xterm needs an X display

    struct winsize ws;
    ws.ws_row = screenHeight ? screenHeight : 25;
    ws.ws_col = screenWidth  ? screenWidth  : 80;
    ws.ws_xpixel = ws.ws_ypixel = 0;

    int master, slave;
    if (openpty(&master, &slave, NULL, NULL, &ws) < 0) return -1;
    fcntl(master, F_SETFD, 0);                 // master survives exec; xterm inherits it

    static int reaper_installed = 0;
    if (!reaper_installed) {
        struct sigaction sa; memset(&sa, 0, sizeof sa); sigemptyset(&sa.sa_mask);
        sa.sa_handler = reap_terminals; sa.sa_flags = SA_RESTART;
        sigaction(SIGCHLD, &sa, NULL);
        reaper_installed = 1;
    }

    pid_t pid = fork();
    if (pid < 0) { close(master); close(slave); return -1; }
    if (pid == 0) {                            // child -> xterm, owns the MASTER
        close(slave);
        char arg[64], geom[64], title[32];
        int off = 60 + g_numDesktops * 30;
        snprintf(arg,   sizeof arg,   "-Sxx/%d", master);          // xterm uses the master fd
        snprintf(geom,  sizeof geom,  "%hux%hu+%d+%d", screenWidth, screenHeight, off, off);
        snprintf(title, sizeof title, "drmon #%d", g_numDesktops);
        setsid();
        // metaSendsEscape: send Alt+key as ESC-prefixed so drmon's Alt-shortcuts
        // (Alt-M/Alt-R/...) decode (ncurses_io maps ESC-letter -> Alt scan code).
        execlp("xterm", "xterm", arg, "-geometry", geom, "-title", title,
               "-xrm", "XTerm*metaSendsEscape: true", (char *)NULL);
        _exit(127);
    }
    close(master);                             // parent (drmon) drives the SLAVE

    // CPR handshake: only a live, attached terminal answers ESC[6n with ESC[r;cR.
    // Confirms xterm came up before we commit a desktop to it (no ghost windows).
    struct termios tio, raw;
    int attached = 0;
    if (tcgetattr(slave, &tio) == 0) {
        raw = tio; cfmakeraw(&raw); tcsetattr(slave, TCSANOW, &raw);
        struct pollfd p; p.fd = slave; p.events = POLLIN; p.revents = 0;
        char rbuf[64]; int got = 0; rbuf[0] = 0;
        for (int t = 0; t < 60 && !attached; t++) {
            if (t % 8 == 0) { ssize_t w = write(slave, "\033[6n", 4); (void)w; }
            if (poll(&p, 1, 100) > 0 && (p.revents & POLLIN)) {
                int n = read(slave, rbuf + got, (int)sizeof rbuf - 1 - got);
                if (n > 0) { got += n; rbuf[got] = 0; if (strchr(rbuf, 'R')) attached = 1; }
            }
        }
        tcsetattr(slave, TCSANOW, &tio);
    }
    if (!attached) { kill(pid, SIGTERM); close(slave); return -1; }

    int slave2 = dup(slave);                   // ncurses wants distinct read/write streams
    int nc = drmon_nc_open(slave, slave2);
    if (nc < 0) { kill(pid, SIGTERM); close(slave); close(slave2); return -1; }

    Desktop *orig = g_curDesktop;              // caller is mid-frame on this one
    Desktop *d    = MakeDesktop(nc);
    if (!d) { drmon_nc_close(nc); return -1; }

    switchDesktop(d);                          // save orig, load empty d, select its terminal
    InitDesktop();                             // build d's UI into the now-current globals
    switchDesktop(orig);                       // restore the caller's desktop before returning
    return 0;
}

#else
int NewDesktop(void) { return -1; }            // multi-terminal is a Linux feature
#endif
