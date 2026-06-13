/*  A Bison parser, made from expr.y with Bison version GNU Bison version 1.22
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	NAME	258
#define	NUMBER	259
#define	EOL	260
#define	LOR	261
#define	LAND	262
#define	EQ	263
#define	NE	264
#define	LT	265
#define	LTE	266
#define	GT	267
#define	GTE	268
#define	LSHIFT	269
#define	RSHIFT	270
#define	PLUS	271
#define	MINUS	272
#define	MULT	273
#define	DIVIDE	274
#define	MOD	275
#define	INC	276
#define	DEC	277
#define	ASSIGN	278
#define	LNOT	279
#define	BNOT	280
#define	BOR	281
#define	XOR	282
#define	BAND	283
#define	UMINUS	284

#line 1 "expr.y"

#include "expr.h"
#include <string.h>
#include <assert.h>



#line 7 "expr.y"
typedef union {
  long dval;
  struct symtab* symp;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		73
#define	YYFLAG		-32768
#define	YYNTBASE	35

#define YYTRANSLATE(x) ((unsigned)(x) <= 284 ? yytranslate[x] : 39)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    32,
    33,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    27,     2,     2,
     2,     2,    26,    34,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    28,    29,    30,    31
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     7,     9,    13,    17,    21,    25,    29,
    33,    36,    39,    42,    45,    48,    51,    54,    58,    62,
    66,    70,    74,    78,    82,    86,    90,    94,    98,   102,
   106,   110,   113,   119,   123,   125,   127,   129
};

static const short yyrhs[] = {    36,
     0,    35,    36,     0,    38,     0,     5,     0,    32,    38,
    33,     0,    38,    16,    38,     0,    38,    17,    38,     0,
    38,    18,    38,     0,    38,    19,    38,     0,    38,    20,
    38,     0,    17,    38,     0,    24,    38,     0,    25,    38,
     0,    21,    38,     0,    38,    21,     0,    22,    38,     0,
    38,    22,     0,    34,    38,    37,     0,    38,    14,    38,
     0,    38,    15,    38,     0,    38,    10,    38,     0,    38,
    11,    38,     0,    38,    12,    38,     0,    38,    13,    38,
     0,    38,     8,    38,     0,    38,     9,    38,     0,    38,
    30,    38,     0,    38,    29,    38,     0,    38,    28,    38,
     0,    38,     7,    38,     0,    38,     6,    38,     0,    26,
    38,     0,    38,    26,    38,    27,    38,     0,     3,    23,
    38,     0,    37,     0,     4,     0,     3,     0,     3,    37,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    35,    36,    39,    40,    43,    46,    47,    48,    49,    55,
    61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
    81,    82,    83,    84,    85,    86,    87,    88
};

static const char * const yytname[] = {   "$","error","$illegal.","NAME","NUMBER",
"EOL","LOR","LAND","EQ","NE","LT","LTE","GT","GTE","LSHIFT","RSHIFT","PLUS",
"MINUS","MULT","DIVIDE","MOD","INC","DEC","ASSIGN","LNOT","BNOT","'?'","':'",
"BOR","XOR","BAND","UMINUS","'('","')'","'@'","statement_list","statement","paren_expr",
"expression",""
};
#endif

static const short yyr1[] = {     0,
    35,    35,    36,    36,    37,    38,    38,    38,    38,    38,
    38,    38,    38,    38,    38,    38,    38,    38,    38,    38,
    38,    38,    38,    38,    38,    38,    38,    38,    38,    38,
    38,    38,    38,    38,    38,    38,    38,    38
};

static const short yyr2[] = {     0,
     1,     2,     1,     1,     3,     3,     3,     3,     3,     3,
     2,     2,     2,     2,     2,     2,     2,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     2,     5,     3,     1,     1,     1,     2
};

static const short yydefact[] = {     0,
    37,    36,     4,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     1,    35,     3,     0,    38,    11,    14,    16,
    12,    13,    32,     0,     0,     2,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    15,    17,     0,     0,     0,     0,    34,     5,    18,
    31,    30,    25,    26,    21,    22,    23,    24,    19,    20,
     6,     7,     8,     9,    10,     0,    29,    28,    27,     0,
    33,     0,     0
};

static const short yydefgoto[] = {    12,
    13,    14,    15
};

static const short yypact[] = {    70,
   -15,-32768,-32768,    86,    86,    86,    86,    86,    86,    86,
    86,    46,-32768,-32768,   195,    86,-32768,-32768,-32768,-32768,
-32768,-32768,   195,   115,   143,-32768,    86,    86,    86,    86,
    86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
    86,-32768,-32768,    86,    86,    86,    86,   195,-32768,-32768,
   219,   242,   324,   324,    38,    38,    38,    38,   295,   295,
     0,     0,   -12,   -12,   -12,   170,   265,   288,   311,    86,
   195,    11,-32768
};

static const short yypgoto[] = {-32768,
     1,    13,    -4
};


#define	YYLAST		346


static const short yytable[] = {    18,
    19,    20,    21,    22,    23,    24,    25,    16,    42,    43,
    73,    48,    26,    17,     0,     0,    10,    39,    40,    41,
    42,    43,    51,    52,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    62,    63,    64,    65,    50,     0,    66,
    67,    68,    69,     0,     0,    72,     0,     0,     1,     2,
     3,    35,    36,    37,    38,    39,    40,    41,    42,    43,
     0,     0,     4,     0,     0,    71,     5,     6,     0,     7,
     8,     9,     1,     2,     3,     0,     0,    10,     0,    11,
     0,     0,     0,     0,     0,     0,     4,     0,     1,     2,
     5,     6,     0,     7,     8,     9,     0,     0,     0,     0,
     0,    10,     4,    11,     0,     0,     5,     6,     0,     7,
     8,     9,     0,     0,     0,     0,     0,    10,     0,    11,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,     0,     0,     0,
    44,     0,    45,    46,    47,     0,     0,    49,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,     0,     0,     0,    44,     0,
    45,    46,    47,     0,    10,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
    42,    43,     0,     0,     0,    44,    70,    45,    46,    47,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,     0,     0,     0,
    44,     0,    45,    46,    47,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    43,     0,     0,     0,     0,     0,    45,    46,    47,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,     0,     0,     0,     0,     0,    45,
    46,    47,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,     0,     0,     0,
     0,     0,     0,    46,    47,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
    37,    38,    39,    40,    41,    42,    43,    47,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43
};

static const short yycheck[] = {     4,
     5,     6,     7,     8,     9,    10,    11,    23,    21,    22,
     0,    16,    12,     1,    -1,    -1,    32,    18,    19,    20,
    21,    22,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    25,    -1,    44,
    45,    46,    47,    -1,    -1,     0,    -1,    -1,     3,     4,
     5,    14,    15,    16,    17,    18,    19,    20,    21,    22,
    -1,    -1,    17,    -1,    -1,    70,    21,    22,    -1,    24,
    25,    26,     3,     4,     5,    -1,    -1,    32,    -1,    34,
    -1,    -1,    -1,    -1,    -1,    -1,    17,    -1,     3,     4,
    21,    22,    -1,    24,    25,    26,    -1,    -1,    -1,    -1,
    -1,    32,    17,    34,    -1,    -1,    21,    22,    -1,    24,
    25,    26,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    -1,    -1,    -1,
    26,    -1,    28,    29,    30,    -1,    -1,    33,     6,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    22,    -1,    -1,    -1,    26,    -1,
    28,    29,    30,    -1,    32,     6,     7,     8,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    -1,    -1,    -1,    26,    27,    28,    29,    30,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    -1,    -1,    -1,
    26,    -1,    28,    29,    30,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    21,    22,    -1,    -1,    -1,    -1,    -1,    28,
    29,    30,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    -1,    -1,    -1,
    -1,    -1,    -1,    29,    30,     8,     9,    10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
    16,    17,    18,    19,    20,    21,    22,    30,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/lib/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#define YYLEX		yylex(&yylval, &yylloc)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
long yyparse();

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_bcopy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 184 "/usr/local/lib/bison.simple"
long
yyparse()
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 5:
#line 43 "expr.y"
{ yyval.dval = yyvsp[-1].dval; ;
    break;}
case 6:
#line 46 "expr.y"
{ yyval.dval = yyvsp[-2].dval + yyvsp[0].dval; ;
    break;}
case 7:
#line 47 "expr.y"
{ yyval.dval = yyvsp[-2].dval - yyvsp[0].dval; ;
    break;}
case 8:
#line 48 "expr.y"
{ yyval.dval = yyvsp[-2].dval * yyvsp[0].dval; ;
    break;}
case 9:
#line 50 "expr.y"
{ if ( yyvsp[0].dval == 0 )
	yyerror( "divide by zero" );
    else
	yyval.dval = yyvsp[-2].dval / yyvsp[0].dval;
  ;
    break;}
case 10:
#line 56 "expr.y"
{ if ( yyvsp[0].dval == 0 )
      yyerror( "divide by zero" );
    else
      yyval.dval = yyvsp[-2].dval % yyvsp[0].dval;
  ;
    break;}
case 11:
#line 61 "expr.y"
{ yyval.dval = -yyvsp[0].dval; ;
    break;}
case 12:
#line 62 "expr.y"
{ yyval.dval = !yyvsp[0].dval; ;
    break;}
case 13:
#line 63 "expr.y"
{ yyval.dval = ~yyvsp[0].dval; ;
    break;}
case 14:
#line 64 "expr.y"
{ yyval.dval = yyvsp[0].dval; ++yyval.dval; ;
    break;}
case 15:
#line 65 "expr.y"
{ yyval.dval = yyvsp[-1].dval; yyval.dval++; ;
    break;}
case 16:
#line 66 "expr.y"
{ yyval.dval = yyvsp[0].dval; --yyval.dval; ;
    break;}
case 17:
#line 67 "expr.y"
{ yyval.dval = yyvsp[-1].dval; yyval.dval--; ;
    break;}
case 18:
#line 68 "expr.y"
{ yyval.dval = MemRead( yyvsp[0].dval, yyvsp[-1].dval ); ;
    break;}
case 19:
#line 69 "expr.y"
{ yyval.dval = yyvsp[-2].dval << yyvsp[0].dval; ;
    break;}
case 20:
#line 70 "expr.y"
{ yyval.dval = yyvsp[-2].dval >> yyvsp[0].dval; ;
    break;}
case 21:
#line 71 "expr.y"
{ yyval.dval = yyvsp[-2].dval < yyvsp[0].dval; ;
    break;}
case 22:
#line 72 "expr.y"
{ yyval.dval = yyvsp[-2].dval <= yyvsp[0].dval; ;
    break;}
case 23:
#line 73 "expr.y"
{ yyval.dval = yyvsp[-2].dval > yyvsp[0].dval; ;
    break;}
case 24:
#line 74 "expr.y"
{ yyval.dval = yyvsp[-2].dval >= yyvsp[0].dval; ;
    break;}
case 25:
#line 75 "expr.y"
{ yyval.dval = yyvsp[-2].dval == yyvsp[0].dval; ;
    break;}
case 26:
#line 76 "expr.y"
{ yyval.dval = yyvsp[-2].dval != yyvsp[0].dval; ;
    break;}
case 27:
#line 77 "expr.y"
{ yyval.dval = yyvsp[-2].dval & yyvsp[0].dval; ;
    break;}
case 28:
#line 78 "expr.y"
{ yyval.dval = yyvsp[-2].dval ^ yyvsp[0].dval; ;
    break;}
case 29:
#line 79 "expr.y"
{ yyval.dval = yyvsp[-2].dval | yyvsp[0].dval; ;
    break;}
case 30:
#line 80 "expr.y"
{ yyval.dval = yyvsp[-2].dval && yyvsp[0].dval; ;
    break;}
case 31:
#line 81 "expr.y"
{ yyval.dval = yyvsp[-2].dval || yyvsp[0].dval; ;
    break;}
case 32:
#line 82 "expr.y"
{ printf( "%d\n", yyvsp[0].dval ); ;
    break;}
case 33:
#line 83 "expr.y"
{ yyval.dval = (yyvsp[-4].dval ? yyvsp[-2].dval : yyvsp[0].dval); ;
    break;}
case 34:
#line 84 "expr.y"
{ yyvsp[-2].symp->value = yyvsp[0].dval; ;
    break;}
case 35:
#line 85 "expr.y"
{ yyval.dval = yyvsp[0].dval; ;
    break;}
case 37:
#line 87 "expr.y"
{ yyval.dval = yyvsp[0].symp->value; ;
    break;}
case 38:
#line 88 "expr.y"
{
					      if ( yyvsp[-1].symp->funcptr )
						  yyval.dval = (yyvsp[-1].symp->funcptr)(yyvsp[0].dval);
					      else {
						  printf( "%s not a function\n", yyvsp[-1].symp->name );
						  yyval.dval = 0;
      }
  ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 465 "/usr/local/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 98 "expr.y"


/* look up a symbol table entry, add if not present  */
struct symtab* symlook( char* s )
{
  char* p;
  struct symtab* sp;

  for ( sp=symtab; sp < &symtab[ NSYMS ]; ++sp )
  {
      /* is it already known? */
      if ( sp->name && !strcmp( sp->name, s ) )
	  return sp;
      

      /* is it free? */
      if ( !sp->name )
      {
	  sp->name = strdup( s );
	  return sp;
      }

      /* otherwise continue to next */
  }
  yyerror( "Too many symbols" );
  exit( 1 );
}


addfunc( char* name, long (*func)() )
{
    struct symtab* sp = symlook( name );
    assert( sp );
    sp->funcptr = func;
}


long numberofones( long val )
{
long ret_val = 0;

for ( ; val; val >>= 1 )
  {
  if ( val & 1 )
    ++ret_val;
  }
return ret_val;
}


define_func( char* s1, char* s2 )
{
}


MemRead( void* addr, int size )
{
return 0;
}


#if 0

main()
{
#if 0
    extern double sqrt(), exp(), log();
    
    addfunc( "sqrt", sqrt );
    addfunc( "exp", exp );
    addfunc( "log", log );
#endif

    addfunc( "numberofones", numberofones );

    yyparse();
}

#endif
   

         


      
