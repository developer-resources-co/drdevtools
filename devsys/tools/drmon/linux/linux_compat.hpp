//=============================================================================
//  linux_compat.hpp — force-included (gcc/clang -include) before every TU so the
//  Borland/DOS-isms compile on a modern Linux toolchain. Phase 1 = compile + link.
//  See docs/plans/2026-06-10-port-drmon-linux.md.
//=============================================================================
#ifndef DRMON_LINUX_COMPAT_HPP
#define DRMON_LINUX_COMPAT_HPP

#if defined(__GNUC__) || defined(__clang__)

// --- segmented-memory keywords are meaningless on flat 64-bit; neutralize them ---
#define far
#define near
#define huge
#define _far
#define _near
#define _huge
#define __far
#define __near
#define __huge
#define _seg

// --- calling-convention keywords (no-ops on modern gcc/clang) ---
#define cdecl
#define _cdecl
#define __cdecl
#define pascal
#define _pascal
#define __pascal

// --- far heap == normal heap; far mem ops == normal mem ops ---
#include <stdlib.h>
#include <string.h>
#define farmalloc   malloc
#define farfree     free
#define farcalloc   calloc
#define farrealloc  realloc
#define farcoreleft()  ((unsigned long)(64UL*1024UL*1024UL))
#define coreleft()     ((unsigned long)(64UL*1024UL*1024UL))
#define _fmemcpy    memcpy
#define _fmemset    memset
#define _fmemmove   memmove
#define _fstrcpy    strcpy
#define _fstrcat    strcat
#define _fstrlen    strlen
#define movmem(src,dst,n)  memmove((void*)(dst),(const void*)(src),(size_t)(n))
#define setmem(dst,n,c)    memset((void*)(dst),(int)(c),(size_t)(n))

// --- Borland/MSVC path-split limits + helpers (used by filename.cpp et al.) ---
#ifndef _MAX_PATH
#define _MAX_PATH  260
#endif
#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#endif
#ifndef _MAX_DIR
#define _MAX_DIR   256
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif
#ifndef _MAX_EXT
#define _MAX_EXT   256
#endif
#ifdef __cplusplus
extern "C" {
#endif
void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);
void _makepath (char *path, const char *drive, const char *dir, const char *fname, const char *ext);
#ifdef __cplusplus
}
#endif

// --- Borland case-insensitive / case-converting string ops ---
#include <strings.h>            // strcasecmp / strncasecmp
#define stricmp  strcasecmp
#define strnicmp strncasecmp
#ifdef __cplusplus
extern "C" {
#endif
char *strupr(char *s);
char *strlwr(char *s);
#ifdef __cplusplus
}
#endif

#endif // __GNUC__
#endif // DRMON_LINUX_COMPAT_HPP
