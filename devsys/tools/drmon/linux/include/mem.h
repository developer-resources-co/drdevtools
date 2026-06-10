// Shim <mem.h> — Borland mem ops live in <string.h>; movmem/setmem are macros
// in linux_compat.hpp.
#ifndef DRMON_SHIM_MEM_H
#define DRMON_SHIM_MEM_H
#include <string.h>
#endif
