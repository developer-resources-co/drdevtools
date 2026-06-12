//==============================================================================
// express.hpp:
//==============================================================================

#define _H_express
#define MAXBYTE			255

#define MAXWORD			65535

#define VARSIZE			32 	/* number of characters in a variable name */

#define NUMVARS			50	/* number of variables to store */

//==============================================================================

/* variable class constants */

#define CLASS_NULL			0
#define CLASS_BYTE			1
#define CLASS_WORD			2
#define CLASS_LONG			3
#define CLASS_PTR			4

/* error code constants */

//==============================================================================
/* token bytes */

enum
{
	TOKEN_END = 0,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_MULT,
	TOKEN_DIV,
	TOKEN_RPAREN,
	TOKEN_LPAREN,
	TOKEN_CONST,
	TOKEN_VAR,
	TOKEN_AT,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_CARET,
	TOKEN_MOD,
	TOKEN_LSHIFT,
	TOKEN_RSHIFT,
	TOKEN_NOT,
	TOKEN_EQUAL,
	TOKEN_LESSTHAN,
	TOKEN_GREATERTHAN,
	TOKEN_LESSTHANOREQUAL,
	TOKEN_GREATERTHANOREQUAL,
	TOKEN_NOTEQUAL,
	TOKEN_LOGICALNOT
};

//==============================================================================

#define digit(x) ((x >= '0' && x <= '9' ) ? x- '0' : toupper(x) - '7' )

typedef unsigned char token;
//typedef unsigned char byte;

//typedef unsigned short errorcode;

struct VARIABLE
{
	char name[VARSIZE];
	long value;
};

//==============================================================================
/* routines in "token.c" */

errorcode TokenizeExpression( token * * t , char * * s );
void AddToken(token * * stream, token t);	/* add a token to the stream */
errorcode AddConst(token * * stream, char * * s );	/* add a constant */
void AddWord ( token * * stream, long i );
void AddLong ( token * * stream, long i );
long DecLong ( char * * s );
long HexLong ( char * * s );
long BinLong ( char * * s );
long ASCIILong ( char * * s ,errorcode *error);
errorcode AddVar(token * * t, char * * s ); 	/* add a varaible to the stream */
void AddPtr(token * * t, long * p);
unsigned long LookupVar(char * s);

/* routines in "eval.c" */
errorcode EvalExpression ( ULONG * lvalue , token ** p );
errorcode EvalFactor ( ULONG * lvalue, token * * p );
errorcode EvalNumber ( ULONG * lvalue, token * * p );
errorcode EvalConst( ULONG * lvalue, token * * p );
errorcode EvalVar( ULONG * lvalue, token * * p );

//==============================================================================
