//=============================================================================
//  Shim <dos.h> for the Linux drmon build (Phase 1). Declares the Borland DOS
//  facilities the source uses; implementations are no-op stubs in dos_stubs.cpp.
//=============================================================================
#ifndef DRMON_SHIM_DOS_H
#define DRMON_SHIM_DOS_H

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

struct WORDREGS { unsigned int ax, bx, cx, dx, si, di, cflag, flags; };
struct BYTEREGS { unsigned char al, ah, bl, bh, cl, ch, dl, dh; };
union  REGS     { struct WORDREGS x; struct BYTEREGS h; };
struct SREGS    { unsigned int es, cs, ss, ds; };
struct REGPACK  { unsigned int r_ax,r_bx,r_cx,r_dx,r_bp,r_si,r_di,r_ds,r_es,r_flags; };

// Fake 1 MB + 64 KB low memory so MK_FP-derived pointers are always valid.
extern unsigned char _dos_fakemem[];
#define MK_FP(seg,ofs) ((void *)(_dos_fakemem + ((((unsigned long)(unsigned short)(seg))<<4) + (unsigned short)(ofs))))
#define FP_SEG(p) ((unsigned short)((((unsigned char*)(p)) - _dos_fakemem) >> 4))
#define FP_OFF(p) ((unsigned short)((((unsigned char*)(p)) - _dos_fakemem) & 0xF))

int  int86 (int intno, union REGS *in, union REGS *out);
int  int86x(int intno, union REGS *in, union REGS *out, struct SREGS *s);
int  intdos (union REGS *in, union REGS *out);
int  intdosx(union REGS *in, union REGS *out, struct SREGS *s);
void intr(int intno, struct REGPACK *r);
void segread(struct SREGS *s);
void geninterrupt(int intno);

unsigned char inportb(int port);
unsigned int  inport (int port);
void          outportb(int port, unsigned char val);
void          outport (int port, unsigned int val);
unsigned char inp (int port);
unsigned int  inpw(int port);
void          outp (int port, int val);
void          outpw(int port, unsigned int val);

void delay(unsigned ms);
void enable(void);
void disable(void);

unsigned char peekb(unsigned seg, unsigned ofs);
unsigned int  peek (unsigned seg, unsigned ofs);
void          pokeb(unsigned seg, unsigned ofs, unsigned char v);
void          poke (unsigned seg, unsigned ofs, unsigned int  v);

// --- DOS directory scan (Borland/MSC) + current-drive ---
#define FA_RDONLY 0x01
#define FA_HIDDEN 0x02
#define FA_SYSTEM 0x04
#define FA_LABEL  0x08
#define FA_DIREC  0x10
#define FA_ARCH   0x20

struct find_t {
    char     reserved[21];
    char     attrib;
    unsigned wr_time;
    unsigned wr_date;
    long     size;
    char     name[13];
};

unsigned _dos_findfirst(const char *path, unsigned attrib, struct find_t *buf);
unsigned _dos_findnext(struct find_t *buf);
int  getdisk(void);
int  setdisk(int drive);

#ifdef __cplusplus
}
#endif
#endif
