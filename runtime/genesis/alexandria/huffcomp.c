/*============================================================================*/
/* huffcomp.c: adaptive huffman decompression code for target
/*============================================================================*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "debug.h"
#include "genesis.h"
#include "vdp.h"
#include "alexdef.h"
#include "compress.h"

/*============================================================================*/

/* pointers to parent nodes, except for prnt[TABLESIZE..TABLESIZE + N_CHAR - 1]
   which are used to get the positions of leaves corresponding to the codes.
*/

/* kinds of characters (256 bytes) */
#define N_CHAR (256)

#define TABLESIZE       (N_CHAR * 2 - 1)
#define ROOTPOSITION    (TABLESIZE - 1)
#define MAX_FREQ        0x8000

unsigned short *prnt;
unsigned short *son;
unsigned short *freq; 	       /* frequency table */
short  getbuf;
UBYTE getlen;

UBYTE *sourceBuffer;

/*============================================================================*/
/* Huffman coding */

short
GetBit(void) 		   /* get one bit */
{
	short i;

    while (getlen <= 8)
	 {
        getbuf |= (short)(*sourceBuffer++) << (8 - getlen);
        getlen += 8;
     }

    i = getbuf;
    getbuf <<= 1;
    getlen--;

    return (i < 0);
}

/*============================================================================*/

void InitHuffmanTree(void)                           /* initialization of tree */
{
	register short i, j;

 	getbuf = 0;
 	getlen = 0;

    for (i = 0; i < N_CHAR; i++)
	 {
        freq[i] = 1;
        son[i] = i + TABLESIZE;
        prnt[i + TABLESIZE] = i;
     }

    i = 0;
    j = N_CHAR;

    while (j <= ROOTPOSITION)
	 {
        freq[j] = freq[i] + freq[i + 1];
        son[j] = i;
        prnt[i] = prnt[i + 1] = j;
        i += 2;
        j++;
     }

    freq[TABLESIZE] = 0xffff;
    prnt[ROOTPOSITION] = 0;
}

/*============================================================================*/
/* reconstruction of tree */

void ReconstructHuffmanTree(void)
{
	register short	i, j, k;
	unsigned short f, l;

    /* collect leaf nodes in the first half of the table */
    /* and replace the freq by (freq + 1) / 2. */

    j = 0;

    for (i = 0; i < TABLESIZE; i++)
	 {
        if (son[i] >= TABLESIZE)
		 {
            freq[j] = (freq[i] + 1) / 2;
            son[j] = son[i];
            j++;
         }
     }

    /* begin constructing tree by connecting sons */
    for (i = 0, j = N_CHAR; j < TABLESIZE; i += 2, j++)
	 {
        k = i + 1;
        f = freq[j] = freq[i] + freq[k];

        for (k = j - 1; f < freq[k]; k--)
            ;

        k++;
        l = (j - k) * 2;
        memmove((char *)&freq[k], (char *)&freq[k + 1], l);
        freq[k] = f;
        memmove((char *)&son[k], (char *)&son[k + 1], l);
        son[k] = i;
     }

    /* connect prnt */
    for (i = 0; i < TABLESIZE; i++)
	 {
        if ((k = son[i]) >= TABLESIZE)
		 {
            prnt[k] = i;
         }
		else
		 {
            prnt[k] = prnt[k + 1] = i;
         }
     }
}

/*============================================================================*/
/* increment frequency of given code by one, and update tree */

void
UpdateHuffmanTree(unsigned short c)
{
	register short i, j, frequency, l;

    if (freq[ROOTPOSITION] == MAX_FREQ)
        ReconstructHuffmanTree();

    c = prnt[c + TABLESIZE];

    do
	 {
        frequency = ++freq[c];

        /* if the order is disturbed, exchange nodes */
        if (frequency > freq[l = c + 1])
         {
            while (frequency > freq[++l])
                ;

            l--;
            freq[c] = freq[l];
            freq[l] = frequency;

            i = son[c];
            prnt[i] = l;

            if (i < TABLESIZE)
                prnt[i + 1] = l;

            j = son[l];
            son[l] = i;

            prnt[j] = c;

            if (j < TABLESIZE)
                prnt[j + 1] = c;

            son[c] = j;

            c = l;
         }
     } while ((c = prnt[c]) != 0);   /* repeat up to root */
}

/*============================================================================*/

void DecodeAdaptiveHuffman(UBYTE *sourceBuffer,UBYTE *destBuffer,long size)

{
	register unsigned short c;

	prnt = malloc((TABLESIZE + N_CHAR)*sizeof(unsigned short));
	assert(prnt);
	son = malloc(TABLESIZE * sizeof(unsigned short));
	assert(son);
	freq = malloc((TABLESIZE + 1)*sizeof(unsigned short));        /* frequency table */
	assert(freq);

    InitHuffmanTree();
    while (size--)
	 {
    	c = son[ROOTPOSITION];
    	/* travel from root to leaf, */
    	/* choosing the smaller child node (son[]) if the read bit is 0, */
    	/* the bigger (son[]+1} if 1 */

    	while (c < TABLESIZE)
     	{
        	c += GetBit();
        	c = son[c];
     	}

    	c -= TABLESIZE;
    	UpdateHuffmanTree(c);
		*destBuffer = c;
	 }
	free(prnt);
	free(son);
	free(freq);
}

/*============================================================================*/

void DecodeAdaptiveHuffmanToVDP(UBYTE *srcBuffer,UWORD vdpAddr,long size)

{
	register unsigned short c;
	UWORD data;
	UWORD otherByte;
	UWORD decompPhase = 0;
	sourceBuffer = srcBuffer;

	prnt = malloc((TABLESIZE + N_CHAR)*sizeof(unsigned short));
	assert(prnt);
	son = malloc(TABLESIZE * sizeof(unsigned short));
	assert(son);
	freq = malloc((TABLESIZE + 1)*sizeof(unsigned short));        /* frequency table */
	assert(freq);

    InitHuffmanTree();
    while (size--)
	 {
    	c = son[ROOTPOSITION];
    	/* travel from root to leaf, */
    	/* choosing the smaller child node (son[]) if the read bit is 0, */
    	/* the bigger (son[]+1} if 1 */

    	while (c < TABLESIZE)
     	{
        	c += GetBit();
        	c = son[c];
     	}

    	c -= TABLESIZE;
    	UpdateHuffmanTree(c);

		decompPhase = decompPhase ^ 1;
		if(decompPhase)
			otherByte = c;
		else
	 	{
			data = (otherByte << 8) | c;
			*(UWORD *)VDP_DATA = data;
	 	}
	 }
	free(prnt);
	free(son);
	free(freq);
}

/*============================================================================*/
