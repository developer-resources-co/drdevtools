// Shim <dir.h> — Borland directory/path API. Stubs in dos_stubs.cpp.
#ifndef DRMON_SHIM_DIR_H
#define DRMON_SHIM_DIR_H

#include <stdlib.h>

#define MAXPATH  260
#define MAXDRIVE 3
#define MAXDIR   256
#define MAXFILE  256
#define MAXEXT   256

#ifdef __cplusplus
extern "C" {
#endif

struct ffblk {
    char     ff_reserved[21];
    char     ff_attrib;
    unsigned ff_ftime;
    unsigned ff_fdate;
    long     ff_fsize;
    char     ff_name[256];
};

int  findfirst(const char *path, struct ffblk *ff, int attrib);
int  findnext(struct ffblk *ff);
int  fnsplit(const char *path, char *drive, char *dir, char *name, char *ext);
void fnmerge(char *path, const char *drive, const char *dir, const char *name, const char *ext);
int  getcurdir(int drive, char *dir);

#ifdef __cplusplus
}
#endif
#endif
