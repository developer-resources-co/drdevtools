//=============================================================================
//  Shim <conio.h> for the Linux drmon build (Phase 1). Text-mode console API;
//  no-op stubs in dos_stubs.cpp (real ncurses backend is a later phase).
//=============================================================================
#ifndef DRMON_SHIM_CONIO_H
#define DRMON_SHIM_CONIO_H

#ifdef __cplusplus
extern "C" {
#endif

struct text_info {
    unsigned char winleft, wintop, winright, winbottom;
    unsigned char attribute, normattr, currmode;
    unsigned char screenheight, screenwidth;
    unsigned char curx, cury;
};

void gettextinfo(struct text_info *ti);
void clrscr(void);
void gotoxy(int x, int y);
int  wherex(void);
int  wherey(void);
int  getch(void);
int  getche(void);
int  kbhit(void);
int  putch(int c);
int  cputs(const char *s);
int  cprintf(const char *fmt, ...);
void textattr(int a);
void textcolor(int c);
void textbackground(int c);
void window(int l, int t, int r, int b);
void clreol(void);
void delline(void);
void insline(void);
void highvideo(void);
void lowvideo(void);
void normvideo(void);
void gettext(int l, int t, int r, int b, void *buf);
void puttext(int l, int t, int r, int b, void *buf);
void _setcursortype(int t);

#ifdef __cplusplus
}
#endif
#endif
