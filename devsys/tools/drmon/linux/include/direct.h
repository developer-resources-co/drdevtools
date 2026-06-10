// Shim <direct.h> — MSVC/Borland directory ops. Map to POSIX equivalents.
#ifndef DRMON_SHIM_DIRECT_H
#define DRMON_SHIM_DIRECT_H
#include <unistd.h>     // getcwd, chdir, rmdir
#include <sys/stat.h>   // mkdir
#endif
