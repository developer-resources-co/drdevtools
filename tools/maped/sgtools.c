//=============================================================================
// sgtools.c:
//=============================================================================

#include "\gfxtools\lib\global.h"
#include "sgtools.h"
#include "stdio.h"
#include "stdlib.h"
#include "alloc.h"

void FlipCharX (uchar huge *sourcebuffer,uchar huge *destbuffer)
{
    register int a;
    register int b;
    for (a=0;a<64;a+=8)
      for (b=0;b<8;b++)
        destbuffer[a+b] = sourcebuffer[a+(7-b)];
}

void FlipCharY (uchar huge *sourcebuffer,uchar huge *destbuffer)
{
    register int a;
    register int b;
    for (a=0;a<64;a+=8)
      for (b=0;b<8;b++)
        destbuffer[a+b] = sourcebuffer[(56-a)+b];
}

//=============================================================================

