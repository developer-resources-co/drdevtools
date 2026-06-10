//=============================================================================
//  dos_stubs.cpp — Phase 1 no-op implementations of the Borland DOS/BIOS/conio
//  facilities drmon calls. There is no real hardware/terminal yet (link-only
//  milestone); a real ncurses/DAP front end replaces the console bits later.
//=============================================================================
#include <dos.h>
#include <conio.h>
#include <bios.h>
#include <dir.h>
#include <io.h>
#include <ctype.h>

extern "C" {

// Fake low memory backing MK_FP (1 MB + 64 KB so seg:ofs never overruns).
unsigned char _dos_fakemem[0x110000];

// --- interrupts / register calls ---
int  int86 (int, union REGS *, union REGS *)                 { return 0; }
int  int86x(int, union REGS *, union REGS *, struct SREGS *) { return 0; }
int  intdos (union REGS *, union REGS *)                     { return 0; }
int  intdosx(union REGS *, union REGS *, struct SREGS *)     { return 0; }
void intr(int, struct REGPACK *)                            {}
void segread(struct SREGS *)                                {}
void geninterrupt(int)                                      {}

// --- port I/O (no dev board attached) ---
unsigned char inportb(int)               { return 0; }
unsigned int  inport (int)               { return 0; }
void          outportb(int, unsigned char) {}
void          outport (int, unsigned int)  {}
unsigned char inp (int)                  { return 0; }
unsigned int  inpw(int)                  { return 0; }
void          outp (int, int)            {}
void          outpw(int, unsigned int)   {}

void delay(unsigned)  {}
void enable(void)     {}
void disable(void)    {}

unsigned char peekb(unsigned, unsigned)               { return 0; }
unsigned int  peek (unsigned, unsigned)               { return 0; }
void          pokeb(unsigned, unsigned, unsigned char) {}
void          poke (unsigned, unsigned, unsigned int)  {}

// --- conio (no terminal yet) ---
void gettextinfo(struct text_info *ti) {
    if (ti) {
        ti->winleft = 1; ti->wintop = 1; ti->winright = 80; ti->winbottom = 25;
        ti->attribute = 7; ti->normattr = 7; ti->currmode = 3;
        ti->screenheight = 25; ti->screenwidth = 80; ti->curx = 1; ti->cury = 1;
    }
}
void clrscr(void)                 {}
void gotoxy(int, int)             {}
int  wherex(void)                 { return 1; }
int  wherey(void)                 { return 1; }
int  getch(void)                  { return 0; }
int  getche(void)                 { return 0; }
int  kbhit(void)                  { return 0; }
int  putch(int c)                 { return c; }
int  cputs(const char *)          { return 0; }
int  cprintf(const char *, ...)   { return 0; }
void textattr(int)                {}
void textcolor(int)               {}
void textbackground(int)          {}
void window(int, int, int, int)   {}
void clreol(void)                 {}
void delline(void)                {}
void insline(void)                {}
void highvideo(void)              {}
void lowvideo(void)               {}
void normvideo(void)              {}
void gettext(int, int, int, int, void *) {}
void puttext(int, int, int, int, void *) {}
void _setcursortype(int)          {}

// --- bios ---
int _bios_keybrd(int)             { return 0; }

// --- dir / io ---
int  findfirst(const char *, struct ffblk *, int) { return -1; }
int  findnext(struct ffblk *)                      { return -1; }
int  fnsplit(const char *, char *d, char *p, char *n, char *e) {
    if (d) *d = 0; if (p) *p = 0; if (n) *n = 0; if (e) *e = 0; return 0;
}
void fnmerge(char *path, const char *, const char *, const char *, const char *) {
    if (path) *path = 0;
}
int  getcurdir(int, char *dir)    { if (dir) *dir = 0; return 0; }
long filelength(int)              { return 0; }
int  setmode(int, int)            { return 0; }
int  chsize(int, long)            { return 0; }

// --- DOS/Borland path split/merge (functional POSIX implementations) ---
void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext) {
    const char *p = path ? path : "";
    if (drive) drive[0] = 0;                       // POSIX has no drive letter
    const char *slash = strrchr(p, '/');
    const char *base  = slash ? slash + 1 : p;
    if (dir) {
        if (slash) { size_t n = (size_t)(slash - p) + 1; memcpy(dir, p, n); dir[n] = 0; }
        else dir[0] = 0;
    }
    const char *dot = strrchr(base, '.');
    if (ext)   { if (dot) strcpy(ext, dot); else ext[0] = 0; }
    if (fname) { size_t n = dot ? (size_t)(dot - base) : strlen(base);
                 memcpy(fname, base, n); fname[n] = 0; }
}

void _makepath(char *path, const char *, const char *dir, const char *fname, const char *ext) {
    if (!path) return;
    path[0] = 0;
    if (dir && dir[0]) {
        strcpy(path, dir);
        size_t n = strlen(path);
        if (n && path[n-1] != '/') { path[n] = '/'; path[n+1] = 0; }
    }
    if (fname) strcat(path, fname);
    if (ext && ext[0]) { if (ext[0] != '.') strcat(path, "."); strcat(path, ext); }
}

// --- DOS directory scan + drive (no filesystem walk yet; "no match") ---
unsigned _dos_findfirst(const char *, unsigned, struct find_t *) { return 1; }
unsigned _dos_findnext(struct find_t *)                          { return 1; }
int  getdisk(void)        { return 0; }   // drive A:
int  setdisk(int)         { return 1; }   // pretend 1 drive

// --- Borland case-converting string ops ---
char *strupr(char *s) { if (s) for (char *p = s; *p; ++p) *p = (char)toupper((unsigned char)*p); return s; }
char *strlwr(char *s) { if (s) for (char *p = s; *p; ++p) *p = (char)tolower((unsigned char)*p); return s; }

} // extern "C"
