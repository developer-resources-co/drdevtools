#ifndef _VMALLOC_H_
#define _VMALLOC_H_

#include <alexdef.h>

PUBLIC void InitVM(void);

PUBLIC void VMConsume(unsigned short, unsigned short);

PUBLIC unsigned short vmalloc(unsigned short nChars);

PUBLIC unsigned short vfree(unsigned short nChar);

PUBLIC unsigned short vtotalfree(void);

PUBLIC unsigned short vlargestblock(void);

GLOBAL unsigned short wBlock[];

#endif
