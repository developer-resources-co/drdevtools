//=============================================================================
//  ncurses_io.cpp — Phase 1.5 ncurses front end for drmon on Linux.
//
//  drmon renders into a CGA-style video buffer of 16-bit cells (char + attribute
//  byte). This file blits that buffer to the terminal via ncurses and feeds the
//  keyboard back in the DOS extended-key format drmon's input layer expects
//  (a normal key returns its ASCII; a special key returns 0 then a BIOS scan
//  code). It replaces the no-op keyboard stubs from dos_stubs.cpp.
//=============================================================================
#include <ncurses.h>
#include <stdlib.h>

extern "C" {

static int           g_inited = 0;
static unsigned char g_shift  = 0;   // backing byte for input.cpp's keyboardStatus

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

void drmon_nc_shutdown(void)
{
    if (g_inited) { endwin(); g_inited = 0; }
}

void drmon_nc_init(void)
{
    if (g_inited) return;
    initscr();
    cbreak();
    noecho();
    nonl();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);                // drmon polls; never block the main loop
    curs_set(0);
    if (has_colors()) {
        start_color();
        for (int bg = 0; bg < 8; ++bg)
            for (int fg = 0; fg < 8; ++fg)
                init_pair((short)(bg * 8 + fg + 1), (short)fg, (short)bg);
    }
    atexit(drmon_nc_shutdown);            // always restore the terminal
    g_inited = 1;
}

unsigned char *drmon_nc_shiftbyte(void) { return &g_shift; }

// Translate one CP437 cell character to a renderable ncurses chtype.
static chtype cellChar(unsigned char c)
{
    if (c >= 0x20 && c < 0x7F) return (chtype)c;          // printable ASCII
    switch (c) {
        case 0xB3: return ACS_VLINE;
        case 0xC4: return ACS_HLINE;
        case 0xDA: return ACS_ULCORNER;
        case 0xBF: return ACS_URCORNER;
        case 0xC0: return ACS_LLCORNER;
        case 0xD9: return ACS_LRCORNER;
        case 0xC3: return ACS_LTEE;
        case 0xB4: return ACS_RTEE;
        case 0xC2: return ACS_TTEE;
        case 0xC1: return ACS_BTEE;
        case 0xC5: return ACS_PLUS;
        // double-line box -> single-line approximations
        case 0xCD: return ACS_HLINE;  case 0xBA: return ACS_VLINE;
        case 0xC9: return ACS_ULCORNER; case 0xBB: return ACS_URCORNER;
        case 0xC8: return ACS_LLCORNER; case 0xBC: return ACS_LRCORNER;
        case 0xB0: case 0xB1: case 0xB2: return ACS_CKBOARD;  // shades
        case 0xDB: return ACS_BLOCK;
        case 0x18: return ACS_UARROW;  case 0x19: return ACS_DARROW;
        case 0x1A: return ACS_RARROW;  case 0x1B: return ACS_LARROW;
        case 0x07: return '*';         // bullet
        case 0x00: case 0x20: return ' ';
        default:   return (c >= 0x80) ? (chtype)' ' : (chtype)c;
    }
}

// Blit drmon's video buffer (w*h cells of {char, attr}) to the terminal.
void drmon_nc_blit(const unsigned char *buf, int w, int h)
{
    if (!g_inited || !buf) return;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned char ch   = buf[(y * w + x) * 2];
            unsigned char attr = buf[(y * w + x) * 2 + 1];
            chtype cell = cellChar(ch) | COLOR_PAIR(pairFor(attr));
            if (attr & 0x08) cell |= A_BOLD;             // bright foreground
            mvaddch(y, x, cell);
        }
    }
    refresh();
}

// ---- keyboard: ncurses keys -> DOS extended-key byte stream ----------------
static int  pend[4];
static int  pendHead = 0, pendTail = 0;

static void push(int b) { if (pendTail < 4) pend[pendTail++] = b; }

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
    if (pendHead < pendTail) return;          // bytes still buffered
    pendHead = pendTail = 0;
    int k = getch();                          // nodelay: ERR if nothing
    if (k != ERR) translate(k);
}

int drmon_nc_keyready(void) { pump(); return pendHead < pendTail; }

int drmon_nc_getbyte(void)
{
    while (pendHead >= pendTail) { pump(); if (pendHead >= pendTail) napms(2); }
    return pend[pendHead++];
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

} // extern "C"
