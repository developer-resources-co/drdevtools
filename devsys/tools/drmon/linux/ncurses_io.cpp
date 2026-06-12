//=============================================================================
//  ncurses_io.cpp — ncurses front end for drmon on Linux.
//
//  drmon renders into a CGA-style video buffer of 16-bit cells (char + attribute
//  byte). This file blits that buffer to the terminal via ncurses and feeds the
//  keyboard back in the DOS extended-key format drmon's input layer expects
//  (a normal key returns its ASCII; a special key returns 0 then a BIOS scan
//  code). It replaces the no-op keyboard stubs from dos_stubs.cpp.
//
//  Multi-terminal (Option A): one process can drive N terminals, each its own
//  ncurses SCREEN via newterm(). All per-terminal state lives in an NcCtx; the
//  drawing/input entry points act on the *current* ctx (drmon_nc_select). With a
//  single terminal (the common case) there is exactly one ctx and behaviour is
//  identical to the old single-stdscr front end.
//=============================================================================
#define _XOPEN_SOURCE_EXTENDED 1
#include <locale.h>
#include <langinfo.h>
#include <string.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <wchar.h>
#include <signal.h>
#include <unistd.h>

extern "C" {

#define DRMON_NC_MAX 8

// One terminal. drmon_nc_select() points g_cur at one of these and makes its
// ncurses SCREEN current (set_term); every draw/input call then acts on it.
typedef struct {
    SCREEN       *sp;        // ncurses screen for this terminal (newterm)
    FILE         *outf;      // output stream (stdout for the primary; a PTY otherwise)
    int           in_fd;     // input fd, for an idle poll() across terminals
    int           mx, my;    // latest mouse cell position
    int           mbtn;      // sticky button mask (0x01 left, 0x02 right)
    int           resized;   // sticky: terminal resized, relayout pending
    int           pend[6];   // pending translated key bytes
    int           pendHead, pendTail;
    int           used;
} NcCtx;

static NcCtx          g_ctx[DRMON_NC_MAX];
static int            g_nctx   = 0;     // number of ctx slots created
static NcCtx         *g_cur    = NULL;  // the currently-selected terminal
// Shift/modifier backing byte for input.cpp's keyboardStatus. Stays a single
// global (input.cpp caches its address once): only the active terminal's
// modifiers matter at any instant, and ncurses exposes no portable shift state.
static unsigned char  g_shift  = 0;
static int            g_inited = 0;

// CGA colour index (0..15) -> ncurses base colour (0..7); bright = index >= 8.
static const short kCga2Curses[16] = {
    COLOR_BLACK, COLOR_BLUE,  COLOR_GREEN, COLOR_CYAN,
    COLOR_RED,   COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE,
    COLOR_BLACK, COLOR_BLUE,  COLOR_GREEN, COLOR_CYAN,
    COLOR_RED,   COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE,
};

static int pairFor(unsigned char attr)   // attr: fg=low nibble, bg=bits 4-6
{
    int fg = kCga2Curses[attr & 0x0F];
    int bg = kCga2Curses[(attr >> 4) & 0x07];
    return bg * 8 + fg + 1;               // pairs 1..64
}

// ---- per-terminal setup / teardown ----------------------------------------

// Configure one freshly-created SCREEN: input modes, mouse, colours. Everything
// here is per-SCREEN in ncurses (set_term selects which one we touch).
static void nc_setup_term(NcCtx *c)
{
    set_term(c->sp);
    cbreak();
    noecho();
    nonl();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);                // drmon polls; never block the main loop
    set_escdelay(25);                     // quick ESC vs Alt-combo disambiguation
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);  // clicks, drag, motion
    mouseinterval(0);                     // raw press/release, no click synthesis
    // Force any-motion tracking (1003) + SGR coords (1006) on THIS terminal so menu
    // hover-highlight works (see the long note in nc_global_once's predecessor).
    fputs("\033[?1003h\033[?1006h", c->outf);
    fflush(c->outf);
    curs_set(0);                          // drmon draws its own caret; hide the hw cursor
    if (has_colors()) {
        start_color();
        for (int bg = 0; bg < 8; ++bg)
            for (int fg = 0; fg < 8; ++fg)
                init_pair((short)(bg * 8 + fg + 1), (short)fg, (short)bg);
    }
}

// Restore and tear down one terminal's SCREEN.
static void nc_close_term(NcCtx *c)
{
    if (!c->sp) return;
    set_term(c->sp);
    fputs("\033[?1003l\033[?1006l", c->outf);   // un-force the mouse modes we set
    fflush(c->outf);
    mousemask(0, NULL);
    endwin();
    delscreen(c->sp);
    c->sp = NULL;
    c->used = 0;
}

void drmon_nc_shutdown(void)
{
    if (!g_inited) return;
    for (int i = 0; i < g_nctx; ++i)
        if (g_ctx[i].used) nc_close_term(&g_ctx[i]);
    g_nctx = 0;
    g_cur = NULL;
    g_inited = 0;
}

// Async-signal-safe terminal restore for the fatal-signal handler. We can't loop
// fputs/mousemask over every SCREEN here; just write the mouse-disable + cursor-on
// bytes to the primary terminal (the one the user launched in) and endwin(). The
// process is dying anyway; spawned PTYs vanish with it.
static void drmon_nc_emergency_restore(void)
{
    if (!g_inited) return;
    static const char off[] = "\033[?1003l\033[?1006l\033[?25h";  // mouse off + cursor on
    ssize_t n = write(STDOUT_FILENO, off, sizeof(off) - 1);
    (void)n;
    endwin();
    g_inited = 0;
}

// Fatal/terminate signals: restore the terminal, then re-raise with the default
// disposition so the process still dies with the correct exit status (and a core
// dump for SIGSEGV) — but leaves a clean shell instead of leaking mouse modes.
static void drmon_nc_fatal(int sig)
{
    drmon_nc_emergency_restore();
    signal(sig, SIG_DFL);
    raise(sig);                           // delivered once this handler returns
}

// Process-wide, once-only setup: locale (for the Unicode box glyphs), atexit
// restore, and the signal dispositions. Independent of any single terminal.
static void nc_global_once(void)
{
    // ncurses can only emit the Unicode box-drawing glyphs in a UTF-8 locale.
    // Honour the user's locale, but force UTF-8 if it isn't (e.g. a bare C locale
    // in a container) so the borders never silently drop out.
    setlocale(LC_ALL, "");
    if (strcmp(nl_langinfo(CODESET), "UTF-8") != 0)
        setlocale(LC_ALL, "C.UTF-8");

    atexit(drmon_nc_shutdown);            // restore the terminal on a normal exit

    // DOS parity: the monitor swallowed Ctrl+Break and kept running (quit via
    // Alt+X). Match that: ignore SIGINT so Ctrl+C never reaches getch().
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_IGN;
    sigaction(SIGINT, &sa, NULL);

    // Any other death (crash or kill) must still un-leak the mouse modes that
    // atexit can't reach when a signal terminates us.
    sa.sa_handler = drmon_nc_fatal;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGHUP,  &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGBUS,  &sa, NULL);
    sigaction(SIGFPE,  &sa, NULL);
}

// Create an ncurses SCREEN on the given streams and configure it. Returns a
// handle (index) or -1. newterm() makes the new SCREEN current as a side effect.
static int nc_make(FILE *outf, FILE *inf, int in_fd, const char *type)
{
    if (g_nctx >= DRMON_NC_MAX) return -1;
    SCREEN *sp = newterm((char *)type, outf, inf);
    if (!sp) return -1;
    int h = g_nctx++;
    NcCtx *c = &g_ctx[h];
    memset(c, 0, sizeof *c);
    c->sp = sp; c->outf = outf; c->in_fd = in_fd; c->used = 1;
    nc_setup_term(c);
    return h;
}

void drmon_nc_init(void)
{
    if (g_inited) return;
    nc_global_once();
    // Primary terminal = handle 0, on stdout/stdin, honouring $TERM (NULL type).
    // newterm(NULL, stdout, stdin) is what initscr() does internally.
    int h = nc_make(stdout, stdin, STDIN_FILENO, NULL);
    if (h < 0) { fprintf(stderr, "drmon: cannot initialise terminal\n"); return; }
    g_cur = &g_ctx[h];
    g_inited = 1;
}

// Open an additional terminal on a PTY pair (Option A: New Window). out_fd/in_fd
// are the PTY slave the spawned xterm displays. Returns a handle or -1. The
// previously-current terminal stays current.
int drmon_nc_open(int out_fd, int in_fd)
{
    if (!g_inited) return -1;
    FILE *outf = fdopen(out_fd, "w");
    FILE *inf  = fdopen(in_fd,  "r");
    if (!outf || !inf) return -1;
    int h = nc_make(outf, inf, in_fd, "xterm");   // PTY is driven by xterm
    if (g_cur) set_term(g_cur->sp);               // nc_make left the new one current
    return h;
}

// Make `handle` the current terminal for subsequent draw/input calls.
void drmon_nc_select(int handle)
{
    if (handle < 0 || handle >= g_nctx || !g_ctx[handle].used) return;
    g_cur = &g_ctx[handle];
    set_term(g_cur->sp);
}

// The input fd for `handle`, for an idle poll() across terminals (-1 if invalid).
int drmon_nc_infd(int handle)
{
    if (handle < 0 || handle >= g_nctx || !g_ctx[handle].used) return -1;
    return g_ctx[handle].in_fd;
}

// Tear down a non-primary terminal (handle 0 is never closed). Selects the
// primary if the closed terminal was current.
void drmon_nc_close(int handle)
{
    if (handle <= 0 || handle >= g_nctx || !g_ctx[handle].used) return;
    int wasCur = (g_cur == &g_ctx[handle]);
    nc_close_term(&g_ctx[handle]);
    if (wasCur) drmon_nc_select(0);
}

unsigned char *drmon_nc_shiftbyte(void) { return &g_shift; }

// CP437 (the SNES/PC code page drmon draws in) -> Unicode, so a UTF-8 terminal
// renders real box-drawing/blocks/shades instead of ncurses ACS ASCII fallbacks.
static const wchar_t kCp437[256] = {
    0x0020,0x263A,0x263B,0x2665,0x2666,0x2663,0x2660,0x2022,0x25D8,0x25CB,0x25D9,0x2642,0x2640,0x266A,0x266B,0x263C,
    0x25BA,0x25C4,0x2195,0x203C,0x00B6,0x00A7,0x25AC,0x21A8,0x2191,0x2193,0x2192,0x2190,0x221F,0x2194,0x25B2,0x25BC,
    0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,
    0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,
    0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,
    0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,0x0058,0x0059,0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,
    0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,
    0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x2302,
    0x00C7,0x00FC,0x00E9,0x00E2,0x00E4,0x00E0,0x00E5,0x00E7,0x00EA,0x00EB,0x00E8,0x00EF,0x00EE,0x00EC,0x00C4,0x00C5,
    0x00C9,0x00E6,0x00C6,0x00F4,0x00F6,0x00F2,0x00FB,0x00F9,0x00FF,0x00D6,0x00DC,0x00A2,0x00A3,0x00A5,0x20A7,0x0192,
    0x00E1,0x00ED,0x00F3,0x00FA,0x00F1,0x00D1,0x00AA,0x00BA,0x00BF,0x2310,0x00AC,0x00BD,0x00BC,0x00A1,0x00AB,0x00BB,
    0x2591,0x2592,0x2593,0x2502,0x2524,0x2561,0x2562,0x2556,0x2555,0x2563,0x2551,0x2557,0x255D,0x255C,0x255B,0x2510,
    0x2514,0x2534,0x252C,0x251C,0x2500,0x253C,0x255E,0x255F,0x255A,0x2554,0x2569,0x2566,0x2560,0x2550,0x256C,0x2567,
    0x2568,0x2564,0x2565,0x2559,0x2558,0x2552,0x2553,0x256B,0x256A,0x2518,0x250C,0x2588,0x2584,0x258C,0x2590,0x2580,
    0x03B1,0x00DF,0x0393,0x03C0,0x03A3,0x03C3,0x00B5,0x03C4,0x03A6,0x0398,0x03A9,0x03B4,0x221E,0x03C6,0x03B5,0x2229,
    0x2261,0x00B1,0x2265,0x2264,0x2320,0x2321,0x00F7,0x2248,0x00B0,0x2219,0x00B7,0x221A,0x207F,0x00B2,0x25A0,0x00A0,
};

// Blit drmon's video buffer (w*h cells of {char, attr}) to the current terminal as
// wide chars. The buffer stride stays `w` (drmon's screenWidth), but never write past
// the actual terminal (COLS/LINES of the current SCREEN): clip the visible span.
// caretMode: -1 = no caret, 0 = overwrite (reverse block), 1 = insert (underline).
void drmon_nc_blit(const unsigned char *buf, int w, int h,
                   int caretX, int caretY, int caretMode)
{
    if (!g_inited || !g_cur || !buf) return;
    int vw = (w < COLS)  ? w : COLS;     // visible width  (buffer stride stays w)
    int vh = (h < LINES) ? h : LINES;    // visible height
    for (int y = 0; y < vh; ++y) {
        for (int x = 0; x < vw; ++x) {
            unsigned char ch   = buf[(y * w + x) * 2];
            unsigned char attr = buf[(y * w + x) * 2 + 1];
            wchar_t wc[2] = { kCp437[ch] ? kCp437[ch] : L' ', 0 };
            cchar_t cc;
            attr_t  a = (attr & 0x08) ? A_BOLD : 0;     // bright foreground
            if (caretMode >= 0 && x == caretX && y == caretY)
                a |= (caretMode == 1) ? A_UNDERLINE : A_REVERSE;
            setcchar(&cc, wc, a, (short)pairFor(attr), NULL);
            mvadd_wch(y, x, &cc);
        }
    }
    refresh();
}

// ---- keyboard: ncurses keys -> DOS extended-key byte stream ----------------

static void push(int b) { NcCtx *c = g_cur; if (c && c->pendTail < (int)(sizeof c->pend / sizeof c->pend[0])) c->pend[c->pendTail++] = b; }

// DOS keyboard scan codes for Alt+A .. Alt+Z (from keys.hpp KEY_ALT*).
static const unsigned char kAltScan[26] = {
    0x1E,0x30,0x2E,0x20,0x12,0x21,0x22,0x23,0x17,0x24,0x25,0x26,0x32,
    0x31,0x18,0x19,0x10,0x13,0x1F,0x14,0x16,0x2F,0x11,0x2D,0x15,0x2C,
};

static void translate(int k)
{
    if (k >= 32 && k < 127) { push(k); return; }
    switch (k) {
        case '\r': case '\n': case KEY_ENTER: push(13); break;
        case 27:               push(27); break;            // ESC
        case '\t':             push(9);  break;
        case 8: case 127: case KEY_BACKSPACE: push(8); break;
        case KEY_UP:    push(0); push(0x48); break;
        case KEY_DOWN:  push(0); push(0x50); break;
        case KEY_LEFT:  push(0); push(0x4B); break;
        case KEY_RIGHT: push(0); push(0x4D); break;
        case KEY_HOME:  push(0); push(0x47); break;
        case KEY_END:   push(0); push(0x4F); break;
        case KEY_PPAGE: push(0); push(0x49); break;
        case KEY_NPAGE: push(0); push(0x51); break;
        case KEY_IC:    push(0); push(0x52); break;
        case KEY_DC:    push(0); push(0x53); break;
        default:
            if (k >= KEY_F(1) && k <= KEY_F(10)) { push(0); push(0x3B + (k - KEY_F(1))); break; }
            if (k > 0 && k < 32) { push(k); break; }       // control keys
            break;
    }
}

static void pump(void)
{
    NcCtx *c = g_cur;
    if (!c) return;
    if (c->pendHead < c->pendTail) return;    // bytes still buffered
    c->pendHead = c->pendTail = 0;
    int k = getch();                          // current SCREEN; nodelay: ERR if nothing
    if (k == ERR) return;

    if (k == KEY_MOUSE) {                      // update sticky mouse state; emits no key
        MEVENT ev;
        if (getmouse(&ev) == OK) {
            c->mx = ev.x; c->my = ev.y;
            if (ev.bstate & BUTTON1_PRESSED)  c->mbtn |= 0x01;   // MOUSEF_BLEFT
            if (ev.bstate & BUTTON1_RELEASED) c->mbtn &= ~0x01;
            if (ev.bstate & BUTTON3_PRESSED)  c->mbtn |= 0x02;   // MOUSEF_BRIGHT
            if (ev.bstate & BUTTON3_RELEASED) c->mbtn &= ~0x02;
        }
        return;
    }

    if (k == KEY_RESIZE) {                     // terminal resized; flag a relayout.
        c->resized = 1;
        return;
    }

    if (k == 27) {                            // ESC: bare ESC, or Alt+key (ESC-prefixed)
        int k2 = ERR;
        for (int i = 0; i < 8 && k2 == ERR; ++i) { k2 = getch(); if (k2 == ERR) napms(1); }
        if (k2 == ERR) { push(27); return; }              // bare ESC
        int letter = -1;
        if (k2 >= 'a' && k2 <= 'z') letter = k2 - 'a';
        else if (k2 >= 'A' && k2 <= 'Z') letter = k2 - 'A';
        if (letter >= 0) { push(0); push(kAltScan[letter]); return; }   // Alt+letter
        push(27); translate(k2); return;                  // ESC then some other key
    }
    translate(k);
}

int drmon_nc_keyready(void) { pump(); return g_cur && g_cur->pendHead < g_cur->pendTail; }

int drmon_nc_getbyte(void)
{
    NcCtx *c = g_cur;
    if (!c) return 0;
    while (c->pendHead >= c->pendTail) { pump(); if (c->pendHead >= c->pendTail) napms(2); }
    return c->pend[c->pendHead++];
}

int drmon_nc_bioskeybrd(int cmd)
{
    // 1 = _KEYBRD_READY, 3 = _KEYBRD_SHIFTSTATUS
    if (cmd == 1) return drmon_nc_keyready() ? 1 : 0;
    if (cmd == 3) return g_shift;
    return 0;
}

// Replaces the dos_stubs.cpp no-op so drmon's keyboard-ready polling works.
int _bios_keybrd(int cmd) { return drmon_nc_bioskeybrd(cmd); }

// ---- mouse: latest ncurses state, polled by mouse.cpp's GetMouse/CheckMouse -------------
int drmon_nc_havemouse(void) { return 1; }

int drmon_nc_getmouse(short *fx, short *fy)
{
    drmon_nc_keyready();                 // pump pending events so state is current
    int mx = g_cur ? g_cur->mx : 0, my = g_cur ? g_cur->my : 0;
    if (fx) *fx = (short)(mx * 8);       // drmon divides fine coords by 8 -> cell
    if (fy) *fy = (short)(my * 8);
    return g_cur ? g_cur->mbtn : 0;
}

// ---- viewport size: track the current terminal's COLS/LINES exactly ----
void drmon_nc_size(int *cols, int *rows)
{
    int c = COLS, r = LINES;
    if (c < 1)  c = 1;  else if (c > 255) c = 255;   // ceiling keeps buffers sane
    if (r < 1)  r = 1;  else if (r > 127) r = 127;
    if (cols) *cols = c;
    if (rows) *rows = r;
}

// Returns (and clears) the current terminal's sticky resize flag.
int drmon_nc_resized(void)
{
    drmon_nc_keyready();                 // pump so an idle-frame KEY_RESIZE is seen
    if (!g_cur) return 0;
    int r = g_cur->resized; g_cur->resized = 0; return r;
}

// Re-sync ncurses to the current terminal size. endwin()+refresh() is the documented
// way to make ncurses re-read the tty geometry and rebuild its screen model after a
// resize. Call before re-reading drmon_nc_size() in a relayout.
void drmon_nc_resync(void)
{
    if (!g_inited || !g_cur) return;
    endwin();
    refresh();
    clearok(stdscr, TRUE);               // force a full repaint on the next blit
}

} // extern "C"
