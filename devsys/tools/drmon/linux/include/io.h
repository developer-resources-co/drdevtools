// Shim <io.h> — Borland low-level file I/O. Most maps to POSIX; the DOS-only
// bits (filelength/setmode/chsize) are stubbed in dos_stubs.cpp.
#ifndef DRMON_SHIM_IO_H
#define DRMON_SHIM_IO_H

#include <unistd.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

long filelength(int handle);
int  setmode(int handle, int mode);
int  chsize(int handle, long size);

#ifdef __cplusplus
}
#endif
#endif
