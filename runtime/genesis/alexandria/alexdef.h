/* --------------------------------------------------------------------------

                    Alexandria Genesis Library
					--------------------------

Title:			Standard Definitions

File:			ALEXDEF.H

Description:	This #include file contains standard definitions used
				for all C projects.

-------------------------------------------------------------------------- */

#ifndef	_ALEXDEF_H_
#define	_ALEXDEF_H_



#define	GLOBAL						/* used with variables */
#define	PUBLIC
#define	PRIVATE	static

#ifndef FALSE
enum { FALSE = 0, TRUE };
#endif

typedef	short			BOOL;		/* typedefs to ensure standard */
									/* storage sizes               */
typedef	long			LONG;
typedef	unsigned long	ULONG;

typedef	short			WORD;
typedef	unsigned short	UWORD;

typedef	signed char		BYTE;
typedef	unsigned char	UBYTE;


typedef union
{
	struct
	{
		WORD	_Int;
		UWORD	_Frac;
	} _FixPos;
	LONG _Whole;
} TFixedPoint;

#define Int		_FixPos._Int
#define Frac	_FixPos._Frac
#define Long	_Whole

#define FIXEDPOINT(a)	((long)(a*65536))

long fpadd(long num1, long num2);
long fpsub(long num1, long num2);
long fpmul(long num1, long num2);
long fpabs(long num);
long fpsgn(long num);

#endif


/* ALEXDEF.H -- EOF */

