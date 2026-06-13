
#ifndef ILBM_H
#define ILBM_H

#define mskNone         0
#define mskHasMask      1
#define mskHasTransparentColor 2
#define mskLasso        3


/* choice of compression algorithm applied to the rows of all
 * source and mask planes. "cmpByteRun1" is the byte run encoding
 * described in appendix C. Do not compress across rows! */

#define cmpNone         0
#define cmpByteRun1     1




struct ColorRegister
{
	ubyte   red, green, blue;       /* color intensities */
};                                              /* 3 bytes long */

typedef struct ColorRegister COLORREGISTER;

#endif

