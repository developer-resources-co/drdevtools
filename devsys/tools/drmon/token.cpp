//=============================================================================
// token.cpp: tokenizer for expresion parser for drmon
//=============================================================================
/*

	Expression Tokenizer.  Scott Statton, 6 June 1991

	This routine will, given a token * and a char * will tokenize an expression.

	The format will be compatible with EvalExpression().

	Known Bugs:

	Future Additions:

*/

//=============================================================================

#include <stdio.h>
#include <ctype.h>
#include "base.hpp"
#include "global.hpp"
#include "error.hpp"
#include "express.hpp"

//=============================================================================

#define sbump *(*s)++

//=============================================================================

errorcode TokenizeExpression( token * * t , char * * s )
{
	errorcode error;
	int	numParens = 0;	/* parenthesis balance */
	ubyte size;

	while ( **s ) {		/* stream through bytes */

		switch (sbump) {
			case ' ':		/* skip any whitespace */
			case '\t':
#ifdef CRISWHITE
			case 10:
			case 13:
#endif
				break;
			case '\'':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '$':
			case '%':
				(*s)--;	/* put back that character */
				AddToken(t,TOKEN_CONST);
#pragma warn -pia
				if (error = AddConst(t,s))
#pragma warn +pia
					return error;
				break;
			case '=':
				if(**s == '=')
				 {
					sbump;
					AddToken(t,TOKEN_EQUAL);
				 }
				else
					return ERROR_MISSINGPARAM;			// kts for now
				break;
			case '!':
				if(**s == '=')
				 {
					sbump;
					AddToken(t,TOKEN_NOTEQUAL);
				 }
				else
					return ERROR_MISSINGPARAM;			// kts for now
//				AddToken(t,TOKEN_LOGICALNOT);
				break;
			case '<':
				if(**s == '<')
				 {
					sbump;
					AddToken(t,TOKEN_LSHIFT);
				 }
				else if(**s == '=')
				 {
					sbump;
					AddToken(t,TOKEN_LESSTHANOREQUAL);
				 }
				else
					AddToken(t,TOKEN_LESSTHAN);
				break;
			case '>':
				if(**s == '>')
				 {
					sbump;
					AddToken(t,TOKEN_RSHIFT);
				 }
				else if(**s == '=')
				 {
					sbump;
					AddToken(t,TOKEN_GREATERTHANOREQUAL);
				 }
				else
					AddToken(t,TOKEN_GREATERTHAN);
				break;
			case '+':
				AddToken(t,TOKEN_PLUS);
				break;
			case '-':
				AddToken(t,TOKEN_MINUS);
				break;
			case '*':
				AddToken(t,TOKEN_MULT);
				break;
			case '/':
				AddToken(t,TOKEN_DIV);
				break;
			case '(':
				AddToken(t,TOKEN_LPAREN);
				numParens++;
				break;
			case ')':
				AddToken(t,TOKEN_RPAREN);
				numParens--;
				break;
			case '&':
				AddToken(t,TOKEN_AND);
				break;
			case '|':
				AddToken(t,TOKEN_OR);
				break;
			case '^':
				AddToken(t,TOKEN_CARET);
				break;
			case '\\':
				AddToken(t,TOKEN_MOD);
				break;
			case '~':
				AddToken(t,TOKEN_NOT);
				break;
			case 0:		/* null */
#ifndef CRISWHITE
			case 13:	/* carriage return */
			case 10:	/* line feed */
#endif
				AddToken(t,TOKEN_END);
				break;
			case '@':
				AddToken(t,TOKEN_AT);
				size = 2;
				while (**s != '(' ) {
					size = ( isdigit(**s) ? (**s) - '0':  size ) ;
					(*s)++;
					}
				AddToken(t, size );
				(*s)++;					/* eat the parenthesis */
				numParens++;
				break;
			default:					/* assume a variable */
				(*s)--;					/* put 1st char of vname back */
				AddToken(t,TOKEN_VAR);
				if (error = AddVar(t,s))
					return error;
				break;
			}	/* end of switch statement */
		} /* end of while statement */
		AddToken(t,TOKEN_END);
	if (numParens) return ERROR_MISBALPAREN;	/* misbalanced parens */
	return NOERR;
}

//=============================================================================

void AddToken(token * * stream, token t)	/* add a token to the stream */
{
		*(*stream)++ = t;
}

//=============================================================================

errorcode AddConst(token * * stream, char * * s )	/* add a constant */
{
	unsigned long	i;
	errorcode error = NOERR;

	switch (**s) {
		case '$':
			(*s)++;
			i = HexLong(s);
			break;
		case '%':
			(*s)++;
			i = BinLong(s);
			break;
		case '\'':
			(*s)++;
			i = ASCIILong(s,&error);
			break;
		default:
			i = DecLong(s);
		}

	if ( i <= MAXBYTE ) {
			AddToken ( stream, CLASS_BYTE );
			AddToken ( stream, i );
			}
		else
			if ( i <= MAXWORD ) {
					AddToken (stream, CLASS_WORD );
					AddWord (stream, i);
					}
				else {
					AddToken(stream, CLASS_LONG);
					AddLong (stream, i);
					}

	return(error);
}

//=============================================================================

void AddWord ( token * * stream, long i )
{
	AddToken (stream, i & MAXBYTE );
	AddToken (stream, i >> 8 );
}

//=============================================================================

void AddLong ( token * * stream, long i )
{
	int	j;

	for ( j = 0 ; j < 4 ; j++ ) {
		AddToken ( stream, i & MAXBYTE );
		i >>= 8 ;
		}
}

//=============================================================================

long BinLong ( char * * s )
{
	for ( long r=0; isdigit( ** s ) || ( **s == '_' ); (*s)++ )
		{
		if ( isdigit( **s ) )
			{
			r <<= 1;
			r += (**s) - '0';
			}
		}

	return r;
}

//=============================================================================

long ASCIILong ( char * * s , errorcode *error)
{
	long r = 0;

	while ( (**s) && ((**s) != '\'')) {
		r <<= 8;
		r += (**s);
		(*s)++;
		}
	if(**s)
		(*s)++;
	else
		*error = ERROR_SYNTAX;
	return r;
}

//=============================================================================

long DecLong ( char * * s )
{
	long r = 0;

	while ( isdigit ( ** s) ) {
		r *= 10L;
		r += (**s) - '0';
		(*s)++;
		}

	return r;
}

//=============================================================================

long HexLong ( char * * s )
{
	long r = 0;
	char	c;

	while ( isxdigit ( **s ) || ( **s == '_' ) )
		{
		if ( isxdigit( **s ) )
			{
			r <<= 4;
			r += digit(**s);
			}
		(*s)++;
		}
	return r;
}

//=============================================================================

errorcode AddVar(token * * t, char * * s ) 	/* add a varaible to the stream */
{
	errorcode error;
	char varName[VARSIZE];	/* maximum variable length */
	char * vnPtr = varName;
//	unsigned long far * varPtr = NULL;
	unsigned long var = 0;
	int vlen = 0;				/* variable length */

	if (!isalpha(**s) && (**s != 0x7f) && **s != '_' & **s != '.') return ERROR_INVALVAR;	/* invalid variable name */
	if ( **s == '.' ) **s = 0x7F;		// convert LLCHAR to internal representation
	while ( vlen++ < VARSIZE && (isalnum(**s) || (**s == '_' || **s == 0x7f || **s=='.') ))
		*vnPtr++ = sbump;
	*vnPtr = 0;

#ifdef MONDEBUG

	printf("Variable Name: %s\n",varName);

#endif

/*	if (varPtr = LookupVar(varName))
		AddPtr (t, (long far *)varPtr);
	   else
		return ERROR_NOSUCHVAR; */

	var = LookupVar(varName);
	if (var != OUT_OF_BAND)
		AddLong (t, var);
	   else
		return ERROR_NOSUCHVAR;

	return NOERR;
}

//=============================================================================

void AddPtr(token * * t, long far * ptr )
{
	long temp;

	temp = (long) ptr;

	AddLong(t,temp);

	return;
}

//=============================================================================
