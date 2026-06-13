/* SubZone data structure.  Due to the possibility of scrolling off the side
 * of a level, a Zone of Control can actually be fragmented:
 *
 *		+--------------------+
 *		|                    |
 *		|                    |
 *		|  ZZZZZZZZ          |       ZOC in one piece
 *		|  ZZZZZZZZ          |
 *		|  ZZZZZZZZ          |
 *		|  ZZZZZZZZ          |
 *		|  ZZZZZZZZ          |
 *		|                    |
 *		|                    |
 *		|                    |
 *		+--------------------+
 *
 *		+--------------------+
 *		|                    |
 *		|                    |
 *		|                    |       ZOC in two pieces
 *		|ZZ            ZZZZZZ|
 *		|ZZ            ZZZZZZ|
 *		|ZZ            ZZZZZZ|
 *		|ZZ            ZZZZZZ|
 *		|ZZ            ZZZZZZ|
 *		|                    |
 *		|                    |
 *		+--------------------+
 *
 *		+--------------------+
 *		|ZZZ            ZZZZZ|
 *		|ZZZ            ZZZZZ|
 *		|ZZZ            ZZZZZ|       ZOC in four pieces
 *		|                    |
 * 		|                    |
 *		|                    |
 *		|                    |
 *		|                    |
 *      |ZZZ            ZZZZZ|
 *		|ZZZ            ZZZZZ|
 *		+--------------------+
 *
 */

typedef struct Coordinate
{
		unsigned short	xPosition;    /* X-coordinate */
		unsigned short	yPosition;    /* Y-coordinate */
} Coordinate;

typedef struct Fragment
{
		char			number;    /* How many Fragments */
		Coordinate		ULC;     /* Upper-Left corner       */
		Coordinate		LRC;     /* Lower-Right corner      */
} Fragment;

typedef Fragment ZOC[4];

/* By convention,
 *	+ If the ZOC is in one piece, that piece is in ZOC[0].
 *  + If the ZOC is in two pieces, the left-most or top-most piece is
 *		in ZOC[0], and the right-most or bottom-most piece is in ZOC[1]
 *  + If the ZOC is in four pieces, the upper-left piece is in ZOC[0],
 *		the upper-right piece is in ZOC[1], the lower-left piece is in
 *		ZOC[2] and the lower-right piece is in ZOC[3].
 */
