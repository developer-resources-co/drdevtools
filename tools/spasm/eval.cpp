//=============================================================================
// eval.cpp: expression parser evaluator for drmon
//=============================================================================

#include <ctype.h>
#if 0
#include	"base.hpp"
#endif
#include "global.hpp"
#if 0
#include "slaveio.hpp"
#endif

#include "error.hpp"
#include "express.hpp"



#define pbump *(*p)++

//=============================================================================

/*
	eval.c -- contains the code to evaluate a tokenized expression.

	Notes:
	E = F [ + E ]
	F = N [ * F ]
	N = # | ( E ) | VAR | @ | Function


	E(a+b*c+d)
	F(N(a)) + E(b*c+d)
	... + F(N(b)*F(N(c))
	E( a*b + c*d ) =
	F(N(a)*F(N(b))) + E(F(N(c)*F(N(d))))

	E(a*b*c+d*(e+f))

	E(F(N(a)*F(N(b)*F(N(c)))) + E(F(N(c)*F(N(E(F(N(e))+E(F(N(f))))))

	( a * ( b * c ) ) + ( d * [ ( e + f ) ] )

*/

//=============================================================================

errorcode
EvalExpression ( ULONG * lvalue , token ** p )
{
	errorcode error;
	long rvalue1;
	long rvalue2;

	if ( error = EvalFactor( (unsigned long *)&rvalue1, p ))
		return error;

	switch(pbump) {
		case TOKEN_END:
		case TOKEN_RPAREN:
			*lvalue = rvalue1;
			return NOERR;

		case TOKEN_EQUAL:
			if ( !(error = EvalExpression ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = (rvalue1 == rvalue2?1:0);
					return NOERR;
					}
				else
					return error;

		case TOKEN_NOTEQUAL:
			if ( !(error = EvalExpression ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = (rvalue1 != rvalue2?1:0);
					return NOERR;
					}
				else
					return error;


		case TOKEN_GREATERTHAN:
			if ( !(error = EvalExpression ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = (rvalue1 > rvalue2?1:0);
					return NOERR;
					}
				else
					return error;


		case TOKEN_LESSTHAN:
			if ( !(error = EvalExpression ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = (rvalue1 < rvalue2?1:0);
					return NOERR;
					}
				else
					return error;

		case TOKEN_LESSTHANOREQUAL:
			if ( !(error = EvalExpression ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = (rvalue1 <= rvalue2?1:0);
					return NOERR;
					}
				else
					return error;

		case TOKEN_GREATERTHANOREQUAL:
			if ( !(error = EvalExpression ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = (rvalue1 >= rvalue2?1:0);
					return NOERR;
					}
				else
					return error;


		case TOKEN_PLUS:
			if ( !(error = EvalExpression ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = rvalue1+rvalue2;
					return NOERR;
					}
				else
					return error;

		case TOKEN_MINUS:
			if ( !(error = EvalExpression ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = rvalue1-rvalue2;
					return NOERR;
					}
				else
					return error;
		case TOKEN_OR:
			if ( !(error = EvalExpression ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = rvalue1|rvalue2;
					return NOERR;
					}
				else
					return error;
		case TOKEN_CARET:
			if ( !(error = EvalExpression ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = rvalue1^rvalue2;
					return NOERR;
					}
				else
					return error;

		default:
			return ERROR_SYNTAX;
		}
	return ERROR_INTERNAL;
}

//=============================================================================

errorcode EvalFactor ( ULONG * lvalue, token * * p )
{
	errorcode error;
	long rvalue1,rvalue2;

	if ( error = EvalNumber ( (unsigned long *)&rvalue1, p ))
		return error;

	switch (pbump) {
		case TOKEN_MULT:
			if ( !(error = EvalFactor ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = rvalue1*rvalue2;
					return NOERR;
					}
				else
					return error;

		case TOKEN_DIV:
			if ( !(error = EvalFactor ( (unsigned long *)&rvalue2, p ))) {
					if ( rvalue2 )
							*lvalue = rvalue1/rvalue2;
						else
							return ERROR_DIVZERO;
					return NOERR;
					}
				else
					return error;
		case TOKEN_AND:
			if ( !(error = EvalFactor ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = rvalue1&rvalue2;
					return NOERR;
					}
				else
					return error;

		case TOKEN_MOD:
			if ( !(error = EvalFactor ( (unsigned long *)&rvalue2, p ))) {
					if( rvalue2)
							*lvalue = rvalue1%rvalue2;
						else
							return ERROR_DIVZERO;
					return NOERR;
					}
				else
					return error;

		case TOKEN_LSHIFT:
			if ( !(error = EvalFactor ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = rvalue1<<rvalue2;
					return NOERR;
					}
				else
					return error;

		case TOKEN_RSHIFT:
			if ( !(error = EvalFactor ( (unsigned long *)&rvalue2, p ))) {
					*lvalue = rvalue1>>rvalue2;
					return NOERR;
					}
				else
					return error;

		default:
			*lvalue = rvalue1;
			(*p)--;	/* put the bimbo BACK */
			return NOERR;
		}
	return ERROR_INTERNAL;
}

//=============================================================================

errorcode EvalNumber ( ULONG * lvalue, token * * p )
{
	errorcode error;
	long memPtr;
	long twiddle,shift;
	ubyte size;

	// WBNIV handle multiple unary minuses (and now pluses, too!)
	long sign = 1;
	while ( (**p == TOKEN_MINUS ) || (**p == TOKEN_PLUS) )
		{
		switch ( **p )
			{
			case TOKEN_MINUS:
				sign = -sign;
				break;
			case TOKEN_PLUS:
				break;
			}
		(*p)++;
		}

#if 0
	if ( **p == TOKEN_MINUS ) {
			sign = -1;
			(*p)++;
		}
		else
			sign = 1;
#endif

	shift = 0;

	if ( **p == TOKEN_CARET ) {
			shift = 16;
			(*p)++;
		}

	if ( **p == TOKEN_LESSTHAN ) {
			shift = 0;
			(*p)++;
		}

	if ( **p == TOKEN_GREATERTHAN ) {
			shift = 8;
			(*p)++;
		}

	if ( **p == TOKEN_NOT ) {
			twiddle = 0xffffffff;
			(*p)++;
		}
		else
			twiddle = 0;

	switch (pbump) {
		case TOKEN_CONST:
			if ( !(error =  EvalConst(lvalue, p ) ) )
					{ *lvalue *= sign; *lvalue ^= twiddle ; *lvalue >>= shift; }
			return error;

		case TOKEN_VAR:
			if ( ! (error = EvalVar(lvalue, p )))
					{ *lvalue *= sign; *lvalue ^= twiddle; *lvalue >>= shift; }
			return error;

		case TOKEN_AT:
			size = pbump;
			if (!(error = EvalExpression( (unsigned long *)&memPtr, p))) {
					*lvalue = ((MemRead(memPtr,size) * sign) ^ twiddle) >> shift ;
					return NOERR;
					}
				else
					return error;

		case TOKEN_LPAREN:
		   if ( ! (error = EvalExpression(lvalue, p) ) ) {
					*lvalue *= sign; *lvalue ^= twiddle; *lvalue >>=shift; }
		   return error;


		default:
			return ERROR_SYNTAX;
		}
	return ERROR_INTERNAL;
}

//=============================================================================

errorcode EvalConst( ULONG * lvalue, token * * p )
{
	int i;
	unsigned long j,k;

	switch (pbump) {
		case CLASS_NULL:
			*lvalue = 0;
			return NOERR;
		case CLASS_BYTE:
			*lvalue = pbump;
			return NOERR;
		case CLASS_WORD:
			*lvalue = (unsigned long)(pbump);
			*lvalue |= ((unsigned long)(pbump)) << 8;
			return NOERR;
		case CLASS_LONG:
		case CLASS_PTR:
			j = 0L;
			for ( i = 0; i < 32 ; i += 8 ) {
				k = (unsigned long) ((unsigned long)(*(*p)++) << i);
				j |= k;
				}
			*lvalue = j;
			return NOERR;
		default:
			return ERROR_SYNTAX;

		}
	return ERROR_INTERNAL;
}

//=============================================================================
