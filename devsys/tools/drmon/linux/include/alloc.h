// Shim <alloc.h> — Borland far-heap header. farmalloc/farfree etc. are macros
// in linux_compat.hpp; this just pulls in the standard allocator.
#ifndef DRMON_SHIM_ALLOC_H
#define DRMON_SHIM_ALLOC_H
#include <stdlib.h>
#endif
