%{
#undef YY_INPUT
#define YY_INPUT( b, r, ms )  ( r = my_yyinput( b, ms ) )

#include <assert.h>
#include <ctype.h>
#include "y.tab.h"
#include "expr.h"

long btol( char *s );
%}

%%

[0-9]+    { yylval.dval = atoi( yytext );  return NUMBER; }

\$[0-9A-Fa-f]+     { sscanf( yytext+1, "%x", &yylval.dval ); return NUMBER; }

0[xX][0-9A-Fa-f]+  { sscanf( yytext+2, "%x", &yylval.dval ); return NUMBER; }

%[01\.]+	{ yylval.dval = btol( yytext + 1 ); return NUMBER; }

0[bB][01\.]+	{ yylval.dval = btol( yytext + 2 ); return NUMBER; }


[ \t] ; /* ignore whitespace */

[A-Za-z][A-Za-z0-9]* { /* return symbol pointer */
    struct symtab* sp = symlook( yytext );
    yylval.symp = sp;
    return NAME;
    }

"++" { return INC; }

"--" { return DEC; }

"<<" { return LSHIFT; }

">>" { return RSHIFT; }

"!"  { return LNOT; }

"~"  { return BNOT; }

"<"  { return LT; }

"<=" { return LTE; }

">"  { return GT; }

">=" { return GTE; }

"==" { return EQ; }

"="  { return ASSIGN; }

"!=" { return NE; }

"&&" { return LAND; }

"&"  { return BAND; }

"||" { return LOR; }

"|"  { return BOR; }

"^"  { return XOR; }

"+"  { return PLUS; }

"-"  { return MINUS; }

"*"  { return MULT; }

"/"  { return DIVIDE; }

"%"  { return MOD; }

, |
; |
\n  { return EOL; }

.    return yytext[ 0 ];

%%

long btol( char *s )
{
	long value = 0;

	while( *s )
	{
		assert( *s == '0' || *s == '1'  || *s == '.' );
		if( *s != '.' )
		{
			value <<= 1;
			value |= *s - '0';
		}
		s++;
	}
	return value;
}

int yywrap( void )
{
	return 0;
}


void yyerror( char * message )
{
	fprintf( stderr, message );
}


int
my_yyinput( char* buf, int max_size )
{
  printf( "> " );
  fflush( stdout );
  fgets( buf, max_size, stdin );
  return( strlen( buf ) );
}


