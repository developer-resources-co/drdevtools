#ifndef COMPAT_HPP
#define COMPAT_HPP

// Linux / modern-C++ build. Real shims for the DOS/Borland console and low-level
// headers live on the include path (devsys/tools/drmon/linux/include/), so the
// old Borland / Watcom / OS2 fallback macros (clrscr/gotoxy stubs, inportb->inp
// remaps) are no longer needed and have been dropped (Linux-only port).
#include <conio.h>
#include <dos.h>

#endif
