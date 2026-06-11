//=============================================================================
// command.c: command parser interface
//=============================================================================

#include        <ctype.h>
#include        <time.h>

#include        "moninc.hpp"
#include        "display.hpp"
#include        "express.hpp"
#include        "word.hpp"
#include        "help.hpp"

//=============================================================================

#define         CMDLEN          70
#define         CMD_XPOS        2
#define         CMD_YPOS        16
#define         CMD_XSIZE 75
#define         CMD_YSIZE 6

#define         CMDGAD_UP               GAD_USER+1
#define         CMDGAD_DOWN         CMDGAD_UP+1
#define         CMDGAD_SCROLL   CMDGAD_DOWN+1
#define         CMDGAD_SBOX             CMDGAD_SCROLL+1
#define         CMDGAD_CMD              CMDGAD_SBOX+1
#define         DELIMITERS              " :,="

#define         MAXLONG 0x7fffffff

//=============================================================================

_object *commandWindow;         /* who is the command window talking to? */
_object *cmdObjPtr;

//=============================================================================

static char * RESVDWORD[] =
	{
		"?",
		"ADD",
		"ALL",
		"ALPHA",
		"ASCII",
		"BCLEAR",
		"BREAK",
		"BSET",
		"BYTE",
		"CLEAR",
		"CLOSE",
		"CODE",
		"COMMAND",
		"CONSOLE",
		"DEBUG",
		"DYNAMICRUNNING",
		"DYNAMICUPDATE",
		"EXECUTE",
		"EXPRESSION",
		"FILE",
		"FSEND",
		"GET",
		"HELP",
		"HIT",
		"LINE",
		"LOADBINARY",
		"LOADMACRO",
		"LOADSYM",
		"LONG",
		"MEMORY",
		"NAME",
		"OFF",
		"ON",
		"OPEN",
		"OVER",
		"PARAMETER",
		"POSITION",
#ifdef SNES
		"PPU",
#endif
		"QUIT",
		"REGISTER",
		"REPOSITION",
		"RESET",
		"RESIZE",
		"RESTART",
		"RUNNOUPDATE",
		"RUNWITHUPDATE",
		"SAVEBINARY",
		"SAVEMACRO",
		"SAVESYM",
		"SCLEAR",
/*		"SEARCHTEXT",*/
		"SEARCHLIST",
		"SELECT",
		"SEND",
		"SET",
		"SIZE",
		"SORT",
		"SOURCE",
		"STATIC",
		"STEP",
		"STOP",
		"SYMBOL",
		"TEXTVIEW",
		"VALUE",
#ifdef GENESIS
		"VCR",
		"VDP",
		"VSR",
#endif
		"WAIT",
		"WATCH",
		"WLOAD",
		"WORD",
		NULL
	};

//=============================================================================

static unsigned char WORDTYPE[] =
	{
		WT_COMMAND,             /* ? */
		WT_PARAM,               /* ADD */
		WT_PARAM,               /* ALL */
		WT_PARAM,       /* ALPHA */
		WT_PARAM,               /* ASCII */
		WT_COMMAND,             /* BCLEAR */
		WT_HARDWINDOW,  /* BREAK */
		WT_COMMAND,             /* BSET */
		WT_PARAM,               /* BYTE */
		WT_PARAM,               /* CLEAR */
		WT_COMMAND,             /* CLOSE */
		WT_PARAM,               /* CODE */
		WT_HARDWINDOW,  /* COMMAND */
		WT_HARDWINDOW,  /* CONSOLE */
		WT_HARDWINDOW,  /* DEBUG */
		WT_PARAM,               /* DYNAMIC RUNNING */
		WT_PARAM,               /* DYNAMIC UPDATE*/
		WT_COMMAND,             /* EXECUTE */
		WT_HARDWINDOW,  /* EXPRESSION */
		WT_PARAM,               /* FILE */
		WT_COMMAND,             /* FSEND */
		WT_COMMAND,             /* GET */
		WT_COMMAND,             /* HELP */
		WT_COMMAND,             /* HIT */
		WT_COMMAND,             /* LINE */
		WT_COMMAND,             /* LOAD */
		WT_COMMAND,             /* LOADMACRO */
		WT_COMMAND,             /* LOADSYM */
		WT_PARAM,               /* LONG */
		WT_HARDWINDOW,  /* MEMORY (was DUMP) */
		WT_COMMAND,             /* NAME */
		WT_PARAM,               /* OFF */
		WT_PARAM,               /* ON */
		WT_COMMAND,             /* OPEN */
		WT_COMMAND,             /* OVER */
		WT_COMMAND,             /* PARAMETER */
		WT_COMMAND,             /* POSITION */
#ifdef SNES
		WT_PARAM,               /* PPU */
#endif
		WT_COMMAND,             /* QUIT */
		WT_HARDWINDOW,  /* REGISTER */
		WT_COMMAND,             /* REPOSITION */
		WT_COMMAND,             /* RESET */
		WT_COMMAND,             /* RESIZE */
		WT_COMMAND,             /* RESTART */
		WT_COMMAND,             /* RUNNOUPDATE */
		WT_COMMAND,             /* RUN WITHUPDATE */
		WT_COMMAND,             /* SAVE */
		WT_COMMAND,             /* SAVEMACROS */
		WT_COMMAND,             /* SAVESYM */
		WT_COMMAND,             /* SCLEAR */
//		WT_HARDWINDOW,  /* SEARCH */
		WT_HARDWINDOW,  /* SEARCHLIST */
		WT_COMMAND,             /* SELECT */
		WT_COMMAND,             /* SEND */
		WT_COMMAND,             /* SET */
		WT_COMMAND,             /* SIZE */
		WT_PARAM,               /* SORT */
		WT_HARDWINDOW,  /* SOURCE */
		WT_PARAM,               /* STATIC */
		WT_COMMAND,             /* STEP */
		WT_COMMAND,             /* STOP */
		WT_HARDWINDOW,  /* SYMBOL */
		WT_HARDWINDOW,  /* TEXTVIEW */
		WT_PARAM,               /* VALUE */
#ifdef GENESIS
		WT_PARAM,               /* VCR */
		WT_PARAM,               /* VDP */
		WT_PARAM,               /* VSR */
#endif
		WT_COMMAND,             /* WAIT */
		WT_HARDWINDOW,  /* WATCH */
		WT_PARAM,               /* WLOAD */
		WT_PARAM,               /* WORD */
		WT_INVALID              /* NULL */
	};

//=============================================================================

FLAG cmdOpen = boolean::FALSE;

char cmdString[CMDLEN+2];
char selectedWindowName[80];
char tempWindowName[80];
char *eatenWord;
char eatenDelim;

_cmdText cmdTextBase;

FILE *scriptFile = NULL;        // Tell command window, no currently waiting script

clock_t timeToGoOn = 0L;

int cmdLinesSaved = 25;         // SLS - History length
int cmdLineCount  = 0;
int cmdHistLine = -1;           // KTS 9/19/91 for command history
							// -1 is special, indicates not currently doing history retrival

//=============================================================================

void WriteSlaveDatum(unsigned long addr,unsigned long datum,unsigned int len)
	{
	unsigned long temp;

#ifndef EMUL
	if (len)
	{
		switch(len)
			{
			case 1:
				temp = datum & 0xFF;
				break;
			case 2:
#if WORDSWAP
				temp = SwapWord(datum);
#else
				temp = datum;
#endif
				break;
			case 3:
#if WORDSWAP
				temp = SwapLong((datum & 0xFFFFFF) << 8);
#else
				temp = datum & 0xFFFFFF;
#endif
				break;
			case 4:
			default:
#if WORDSWAP
				temp = SwapLong(datum);
#else
				temp = datum;
#endif
				break;
			}
		WriteSlaveData(addr,(char*)&temp,len);
		}
#endif
	}

//=============================================================================

void
AddCmdGadgets(_object* pObject,_window* pWindow)
{
	_gadget  *gPtr;
							// input gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 70;
	gPtr->ySize = 1;
	gPtr->yPos = pWindow->ySize -2;
	gPtr->xPos = 1;
	SetGadgString(gPtr,"                                                                      ");
	gPtr->gNum = CMDGAD_CMD;
	gPtr->gadgAttr = windowBorderAttr;
	gPtr->gType = GADTYPE_STRING;
	gPtr->mode = STRGADF_MODE_INSERT;
	AddVScrollGadgets(pWindow,CMDGAD_UP,CMDGAD_DOWN,CMDGAD_SCROLL,CMDGAD_SBOX);
}

//=============================================================================

	ULONG   cmdAnswer;                              /* place to put the result */

char *
GetNumbers( char *s, int count, ULONG *answer, errorcode *error)
{
#if 0
	char    c,*s2;
	int             i;
	token   *tsPtr,tokenStream[100];
	char    *delims;

	delims = ",";

	if (!s || !*s)
	{
		*error = ERROR_SYNTAX;
		return s;
	}
	s = SkipSpace(s);
	for ( i = 0 ; i < count ; i ++ )
	{
		if ( i == (count-1) )
			delims = DELIMITERS;
		tsPtr = tokenStream;
		s2 = FindWord(s,delims,&c);
		if (!(*error = TokenizeExpression(&tsPtr,&s)))
		{
			tsPtr = tokenStream;
			*error = EvalExpression(answer+i,&tsPtr);
			if (*error) break;
		}
		s = s2;
	}
	if (*error && s)
		s[strlen(s)] = c;
#endif
	return s;
}

/*************************************************************************
 *   FindWord -- Search through a char * (s) comparing against a string of
 *       delimiters (d).  When any of the delimiters is found, copy it into
 *       variable c, replace it with a 0 and return
 * If c is NULL, the delimiter isn't returned
 *************************************************************************/

char *
FindWord(char *s, char *d, char *c)
{
	char    *d2;

	d2 = d;         /* place to restore delimiters */

	while ( *s )
	{
		d = d2;                         /* point to 1st delim */
		for ( ; *d ; d++ )      /* and check all of them */
		{
			if ( *s == *d ) /* is the delim in the string? */
			{
				if ( c ) *c = *d;       /* yes - c = the delimiter */
				*s = 0;         /*   ... zterm the string */
				return(++s);
			}
		}
	s++;

	}
	return(NULL);                   /* the string DIDN'T contain any ! */
}

//=============================================================================

char *
UnParseWord(void)
{
	eatenWord[strlen(eatenWord)] = ' ';
	return eatenWord;
}

/*************************************************************************
 *       ParseWord -- Given a string, break off the first delimited word,
 *       and search through the list of RESVDWORDS until you find a match.
 *       The given string will match the first longer word that contains it.
 *************************************************************************/
char *
ParseWord (char * s, unsigned int *word)
{
	char    *s2,*s3,c;
	char    **testword;
	char    inWord[80],*tPtr;
	int             i;
	unsigned int w;

	int nMatches = 0;


	*word = w = -1;
	c = 0;
	s = SkipSpace(s);
	s2 = FindWord(s,DELIMITERS,&c);
	if(!*s)
		return(s2);							// kts 07-05-93 04:00pm
	s3 = s;
	eatenWord = s;
	eatenDelim = c;
	tPtr=inWord;
	while(  *s3 && (*s3 != '\n') )
	 *tPtr++ = toupper(*s3++);
	*tPtr = 0;
	for ( testword = RESVDWORD ; *testword ; *testword++ )
	{
		w++;
		if (!(i = strncmp(*testword,inWord,strlen(inWord))))            /* do we have a match? */
		{
			*word = w | WORDTYPE[w]<<8;
			++nMatches;
		}
		else
			if ( i > 0 ) /* I'm past the end of the testwords */
				break;          /* get out of this for loop */
	}
	if ( nMatches == 1 )
		return( s2 );                                   // ret_s

	if ( nMatches > 1 )
		*word = -1;

	strcpy(tempWindowName,s);
	tempWindowName[strlen(s)] = 0;
	if (FindWindow(tempWindowName))
	{
		*word = WT_WINDOWNAME << 8;
		strcpy (selectedWindowName,tempWindowName);
		return(s2);
	}

	if (FindCmdMacro(s))
	{
		*word = WT_MACRO <<8;
	}


	if ( c == ':' ) /* macro definition */
		*word = WT_COMMAND<<8 | RW_MACRODEF;

	if ( ( (s2) && (*s2 == '=') ) || c == '=')
		*word = WT_SYMBOL<<8;

	s[strlen(s)] = c;

	return (s);
}

//=============================================================================

_object *
ParseHardWindow( unsigned int word )
{
	_object *oPtr;

	switch(word & 0xff)
	{

		case RW_BREAK:  /* break window */
		oPtr = FindWindow("BreakPoints");
		break;

		case RW_COMMAND:
		oPtr = FindWindow("Command Window");
		break;

		case RW_MEMORY: /* dump window */
		oPtr = FindWindow("Memory");
		break;

		case RW_EXPRESSION: /* expression parser window */
		oPtr = FindWindow("Expression Window");
		break;

		case RW_SYMBOL: /* symbol table */
		oPtr = FindWindow("Symbol");
		break;

		case RW_WATCH:  /* Watch window */
		oPtr = FindWindow("Watch");
		break;

		case RW_TEXT:   /* TextView window */
		oPtr = FindWindow("TextView");
		break;

		case RW_SOURCE:         /* source debug window */
		oPtr = FindWindow("Source");
		break;

		case RW_REGISTER:
		oPtr = FindWindow("Register");
		break;

		case RW_CONSOLE:
		oPtr = FindWindow("Console");
		break;

		default:                /* error! */
		oPtr = NULL;
		break;
	}
	return oPtr;
}

//=============================================================================

char *
ParseWindow(char *inString, _object **oPtr)
{
	unsigned int windowName;
	char *s;

	s = inString;
	*oPtr = NULL;
	if(inString)
	{
		inString = ParseWord(SkipSpace(inString) , &windowName );
		if ((( windowName & 0xff00) >> 8 ) == WT_HARDWINDOW ) {
			*oPtr = ParseHardWindow( windowName);
			if (!(*oPtr))
				inString = s;
			}
			else
			if (( (windowName & 0xff00) >>8 ) == WT_WINDOWNAME )
				*oPtr = FindWindow(selectedWindowName);
				/* else {
					inString = UnParseWord();
					} */
	}

	if(!(*oPtr))
		*oPtr = commandWindow;
	return(inString);
}

//=============================================================================

char *
ExecFile(char * s, errorcode *error)
{
	scriptFile = fopen(s,"r");
	if (!scriptFile)
	{
		*error = ERROR_FILENOTFOUND;
		return(s);
	}
	*error = ContinueScript();
	return NULL;
}

//=============================================================================

errorcode
ContinueScript(void)
{
	char tempBuffer[160];
	errorcode error;
	FLAG continueScript = boolean::TRUE;

	do
	{
		if(fgets(tempBuffer,160,scriptFile))
		{
			if(*tempBuffer != ';' && *tempBuffer != '\\' )
				PrintToCmdWindow(tempBuffer);

			if(*tempBuffer == '\\')
				*tempBuffer = ' ';

			error = ParseCmdLine(tempBuffer);
		}
		if(timeToGoOn)
			continueScript = boolean::FALSE;
	}
	while (!feof(scriptFile) && !(error) && continueScript);

	if(continueScript)
	{
		fclose(scriptFile);
		scriptFile = NULL;
	}

	return error;
}

//=============================================================================


char *
EvalWait(char *s, errorcode *error)
{
	ULONG waitTime;

	s = GetNumbers(s, 1, &waitTime, error);
	if (!(*error) && scriptFile)
		timeToGoOn = clock()+waitTime;
	return s;

}

//=============================================================================

char *
EvalCommand(unsigned int word, char * s , errorcode *error )
{
#if 0
	token   tokenStream[100];       /* reasonably large */
	token   *tsPtr;                         /* pointer into the stream */
	ULONG theAnswer,answer[10];
	unsigned int windowName;
	_object *oPtr;
	_input fakeInput;
	char    c,*string,*s2;
	int tempInt;                                            // kts 10/3/91

	*error = NOERR;
	tsPtr = tokenStream;

	switch(word & 0xff)
	{
		case    RW_EVAL:                /* expression evaluator */
			if (!(*error = TokenizeExpression(&tsPtr,&s)))
			{
				tsPtr = tokenStream;
				*error = EvalExpression(&theAnswer,&tsPtr);
				if (!(*error))
				{
					sprintf(textBuffer,"Decimal: %ld  Hex: %lX\n",theAnswer,theAnswer);

					PrintToCmdWindow(textBuffer);
				}

			}
			break;

		case    RW_EXECUTE:
			s = ExecFile(s,error);
			break;

		case    RW_MACRODEF:
			s2 = FindWord(s,DELIMITERS,&c); /* s = name, s2 = string */
			*error = AddCmdMacro(s,SkipSpace(s2));
			if(!*error)
				s = NULL;
			break;

		case    RW_OPEN:
			windowName = 0;
			*error = ERROR_NOSUCHWINDOW;
			if(s)
				s = ParseWord( s , &windowName );
			if ((( windowName & 0xff00) >> 8 ) == WT_HARDWINDOW )
			{
				*error = NOERR;
				switch(windowName & 0xff)
				{

					case RW_BREAK:  /* break window */
					OpenBreak();
					commandWindow = GetLastObject();
					break;

					case RW_COMMAND:
					OpenCmdWindow();
					commandWindow = GetLastObject();
					break;

					case RW_MEMORY: /* memory dump window */
					OpenMemory();
					commandWindow = GetLastObject();
					break;

					case RW_EXPRESSION: /* expression parser window */
					OpenExpWindow();
					commandWindow = GetLastObject();
					break;

					case RW_REGISTER:
					OpenReg();
					commandWindow = GetLastObject();
					break;

					case RW_SOURCE: /* source level debugger */
					OpenSource();
					commandWindow = GetLastObject();
					break;

					case RW_SYMBOL: /* symbol table */
					OpenSymbol();
					commandWindow = GetLastObject();
					break;

					case RW_CONSOLE:
						OpenConsole();
						commandWindow = GetLastObject();
						break;

					case RW_WATCH:
						OpenWatch();
						commandWindow = GetLastObject();
						break;

					case RW_TEXT:
						OpenTextView();
						commandWindow = GetLastObject();
						break;

					case RW_SEARCHLIST: /* memory-search results */
						OpenSearchWindow();
						commandWindow = GetLastObject();
						break;

					default:                /* error! */
					*error = ERROR_NOSUCHWINDOW;
					commandWindow = NULL;
					break;
				}
			}
			break;

		case    RW_CLOSE:
			s = ParseWindow(s,&oPtr);
			if (oPtr->layer->data == oPtr)
			{
				if (oPtr == commandWindow)
					commandWindow = NULL;
				fakeInput = inBase;
				fakeInput.inputType = INP_KEY;
				fakeInput.fullKey = CMD_CLOSEWINDOW;
				(oPtr->inputRoutine)(&fakeInput,oPtr);
			}
			else
				*error = ERROR_NOSUCHWINDOW;
			break;
		case    RW_SIZE:
		case    RW_RESIZE:
			s = ParseWindow(s,&oPtr);
			if (oPtr->layer->data == oPtr)
			{
				s = GetNumbers(s,2,(unsigned long *)&answer,error);
				if (!(*error))
				{
					oPtr->layer->xSize = answer[0];
					oPtr->layer->ySize = answer[1];
					ForceWindowResize((_window*)oPtr->layer,oPtr,oPtr->inputRoutine);
					PrintTitle((_window*)oPtr->layer,windowTitleAttr);

				}
			}
			else
				*error = ERROR_NOSUCHWINDOW;
			break;

		case    RW_REPOSITION:
		case    RW_POSITION:
			s = ParseWindow(s,&oPtr);
			if (oPtr && oPtr->layer->data == oPtr)
			{
				s = GetNumbers(s,2,(unsigned long *)&answer,error);
				if (!(*error))
				{
					oPtr->layer->xPos = answer[0];
					oPtr->layer->yPos = answer[1];
					ForceWindowResize((_window*)oPtr->layer,oPtr,oPtr->inputRoutine);
					PrintTitle((_window*)oPtr->layer,windowTitleAttr);
				}
			}
			else
				*error = ERROR_NOSUCHWINDOW;
			break;


		case    RW_QUIT:
			cont = boolean::FALSE;
			break;

		case    RW_RUN:
			DoSlaveRunWithUpdates();
			break;

		case    RW_RUNNOUPDATE:
			SlaveRunNoUpdate();
			break;

		case    RW_STOP:
			SlaveStop();
			break;

		case    RW_STEP:
			SlaveStep();
			break;

		case    RW_OVER:
			SlaveStepOver();
			break;

		case    RW_RESET:
			SlaveReset();
			break;

		case    RW_RESTART:
			SlaveRestart();
			break;

		case    RW_NAME:
			c = 0;
			s = ParseWindow(s,&oPtr);
			if (oPtr && oPtr->layer->data == oPtr)
			{
				{
					s2 = FindWord(s,DELIMITERS,&c);
					string = (char *)malloc(strlen(s)+1);
					if(string)
					{
						strcpy(string,s);
						s = s2;
						if(oPtr->layer->title)
							free(oPtr->layer->title);
						oPtr->layer->title = string;
						ForceWindowResize((_window*)oPtr->layer,oPtr,oPtr->inputRoutine);
						PrintTitle((_window*)oPtr->layer,windowTitleAttr);
					}
					else if(s && *s) s[strlen(s)] = c;
				}
			}
			else
				*error = ERROR_NOSUCHWINDOW;
			break;


		case    RW_SELECT:
			break;

		case    RW_LOAD:
			s2 = FindWord(s," ,\t",&c);
			string = (char *)malloc(strlen(s)+1);
			if (string)
			{
				s = SkipSpace(s);
				strcpy (string,s);
				s2 = GetNumbers(s2,1,(unsigned long *)&answer,error);
				if (!(*error))
				{
					*error = LoadBinaryFile(answer[0],string);
					free(string);
				}
			}
			s = s2;
			break;

		case    RW_SAVE:
			s2 = FindWord(s," ",&c);
			string = (char *)malloc(strlen(s)+1);
			if (string)
			{
				strcpy (string,s);
				string[strlen(s)] =0;
				s2 = GetNumbers(s2,2,(unsigned long *)&answer,error);
				if (!(*error))
				{
					*error = SaveBinaryFile(answer[0],answer[1],string);
					free(string);
				}
			}
			s = s2;
			break;

		case    RW_LOADSYM:
			*error = LoadSymbol(SkipSpace(s));
			if(!*error) *s = 0;
			break;

		case    RW_LOADMACRO:
			*error = LoadMacros(SkipSpace(s));
			if(!*error) *s = 0;
			break;

		case    RW_SAVEMACRO:
			*error = SaveMacros(SkipSpace(s));
			if(!*error) *s = 0;
			break;

		case    RW_BSET:
			s = GetNumbers(s,1,(unsigned long *)&answer,error);
			if(!(*error))
				AddBrkPt(answer[0],0,NULL);
			break;

		case    RW_BCLEAR:
			s = GetNumbers(s,1,(unsigned long *)&answer,error);
			if(!(*error))
				BreakClear(answer[0]);
			break;

		case    RW_SCLEAR:
			s = SkipSpace(s);
			if(!(*error))
				*error = SymbolNameClear(s);
			if(!*error) *s = 0;
			break;

	case    RW_SET:                                                         // set genmon parameter
		{
												// CAUTION: KTS wrote this, it
												// may not go with the grain of
												// the rest of the cmd parser
			s2 = FindWord(s,DELIMITERS,NULL);
			if(*s2 == 0)
			 {
				*error = ERROR_MISSINGPARAM;
				break;
			 }

			char *s3 = s;
			s = s2;
			s = GetNumbers(s,1,&answer[0],error);
			*error = SetConfigParm(s3,(uint)answer[0]);
			break;
		}

		case    RW_HELP:
			s = s;
			OpenHelpFile();
			break;

		case    RW_SEND:
//                      s = DebugSend(s,error);
			break;

		case    RW_GET:
//                      DebugGet();
			break;

		case    RW_WAIT:
			s = EvalWait(s, error);
			break;

		case    RW_HIT:
			timeToGoOn = MAXLONG;
			break;

		default:
			*error = ERROR_SYNTAX;
			break;
	}
	if (cmdObjPtr)
		cmdObjPtr->layer->ToFront();
	ActivateFrontWindow();
#endif
	return(s);
}

//=============================================================================

char *
EvalAssign(char *s, errorcode *error)
{
	char    *s2,c;
	ULONG   theAnswer,address;
	unsigned int size;

	s2 = FindWord(s,DELIMITERS,&c);
	if (*s2 != '=' && c != '=')
	{
		*error = ERROR_SYNTAX;
		s[strlen(s)]=c;
		return(s);
	}
	if(*s2 == '=')
		s2++;

	s = SkipSpace(s);

	if (*s == '@')
	{
		s++;
		if(isdigit(*s))
			size = *s++ - '0';
		else
			size = 2;
		GetNumbers(s,1,&address,error);
		if (!*error)
		{
			s2 = GetNumbers(SkipSpace(s2),1,&theAnswer,error);
			if(!*error)
				WriteSlaveDatum(address,theAnswer,size);
		}
	}
	else
	{
		s2 = GetNumbers(SkipSpace(s2),1,&theAnswer,error);
		if (!(*error))
		{
			if (!SetRegAsSymbol(s,theAnswer))
				AddSymbol(theAnswer,s);
		}
	}

	return (s2);
}

//=============================================================================

errorcode
ParseCmdLine(char *localCmd)
{
	unsigned int word;
	unsigned int wordType;
	errorcode error;
	char *s,*s2,s3[160],c;
	_input fakeInput;
	_object *oPtr;

	error = 0;
	localCmd = KillTrailNL(localCmd);
	if(*localCmd == '#' || *localCmd == ';')
		return(NOERR);
	s = localCmd;
	while (s && *s && !error)
		{
		s = ParseWord(s,&word);                         /* eat a single text word */
		wordType = (word & 0xff00) >> 8;        /* top bits are flavor */

		switch(wordType)
		{
			case WT_MACRO:
				s2 = FindWord(s,DELIMITERS,&c);
				strcpy(s3,FindCmdMacro(s));
				s = s2;
				if (s3)
					error = ParseCmdLine(s3);
				break;

			case WT_COMMAND:
				s = EvalCommand(word,s,&error);
				break;

			case WT_SYMBOL:
				s = EvalAssign(s,&error);
				break;

			case WT_WINDOWNAME:
				oPtr = FindWindow(selectedWindowName);
				if (oPtr)
				{
					commandWindow = oPtr;   /* SLS 2 Oct 1991 */
					fakeInput.inputType = INP_COMMAND;
					fakeInput.errPtr = &error;
					fakeInput.cmdText = &s;
					(oPtr->inputRoutine)(&fakeInput,oPtr);
				}
				else error = ERROR_NOSUCHWINDOW;
				break;

			case WT_HARDWINDOW:
				oPtr = ParseHardWindow(word);
				if (oPtr)
				{
					commandWindow = oPtr;
					fakeInput.inputType = INP_COMMAND;
					fakeInput.errPtr = &error;
					fakeInput.cmdText = &s;
					(oPtr->inputRoutine)(&fakeInput,oPtr);
				}
				else error = ERROR_NOSUCHWINDOW;
				break;

			case WT_PARAM:
				s = UnParseWord();
				if (commandWindow)
				{
					fakeInput.inputType = INP_COMMAND;
					fakeInput.errPtr = &error;
					fakeInput.cmdText = &s;
					if(!(commandWindow->inputRoutine)(&fakeInput,commandWindow))
						error = ERROR_WINDOWNOGROK;
				}
				else error = ERROR_NODEFAULTWINDOW;
				break;

			default:
				if (commandWindow)
				{
					fakeInput.inputType = INP_COMMAND;
					fakeInput.errPtr = &error;
					fakeInput.cmdText = &s;
					if(!(commandWindow->inputRoutine)(&fakeInput,commandWindow))
						error = ERROR_WINDOWNOGROK;
				}
				else error = ERROR_NODEFAULTWINDOW;
				break;

		}
	s = SkipSpace(s);
	}
	return(error);
}

//=============================================================================

errorcode
DoCmd(_gadget *gPtr)
{
	int     i;
	char    localCmd[CMDLEN+2];

	strcpy(localCmd,gPtr->gadgText);                /* make a copy to fidget with */
	UnPadString(localCmd);                                  /* and neaten it up */
	i = strlen(localCmd);

	localCmd[i++] = '\n';
	localCmd[i] = 0;
	if(*localCmd != ';' && *localCmd != '\\' )
		PrintToCmdWindow(localCmd);

	if(*localCmd == '\\')
		*localCmd = ' ';

	/* here we're ready to begin parsing the actual command text */
	return(ParseCmdLine(localCmd));
}

//=============================================================================

void
CmdError(errorcode error)
{
	int     i;

	strcpy(textBuffer,SYSERRORS[error]);
	UnPadString(textBuffer);
	i = strlen(textBuffer);
	textBuffer[i++] = '\n';
	textBuffer[i] = 0;
	PrintToCmdWindow(textBuffer);
	return;
}

//=============================================================================

void
DeleteCmdLine(void)
{
	_cmdText *sPtr;

	sPtr = (_cmdText *)cmdTextBase.link.next;

	if(cmdLineCount < cmdLinesSaved)
		return;
	free(sPtr->textPtr);
	DeleteListNode((_list *)sPtr);
}

//=============================================================================

_cmdText *
NewCmdLine(char*text,unsigned char attr)
{
	_cmdText *tPtr;
	tPtr = (_cmdText *)malloc(sizeof(_cmdText));
	if(tPtr)
	 {
		AddListNode((_list *)&cmdTextBase,(_list *)tPtr);
		tPtr->textPtr = text;
		tPtr->charAttr = attr;
		cmdLineCount++;
	 }
	return(tPtr);
}

//=============================================================================

void
PrintToCmdWindow(char *text)
{
	char*tPtr,*tPtr2;
	int charCount,i;
	while(*text != 0)
	 {
		charCount = StrToNLLen(text);
		tPtr = (char*)malloc(charCount+1);
		if(tPtr)
		 {
			tPtr2 = tPtr;
			for(i=0;i<charCount;++i)
				*tPtr2++ = *text++;
			*tPtr2 = 0;                                                     // terminate line
			if(*text == '\n')
				text++;                                                         // skip newline char
			DeleteCmdLine();                                        // for each line added, delete a line
			NewCmdLine(tPtr,commandAttr);
		 }
	 }
}

//=============================================================================

void
CopyHistLine(int line, _window *pWindow)
{
	_cmdText *tPtr;
	_gadget *gPtr;
	tPtr = (_cmdText *)cmdTextBase.link.next;
	tPtr = (_cmdText *)SkipNodes((_list *)tPtr,line);
	gPtr = FindGadget(&pWindow->gadgBase,CMDGAD_CMD);
	strcpy(gPtr->gadgText,tPtr->textPtr);
	PadString(gPtr->gadgText,gPtr->xSize-1);
}

//=============================================================================

int
GetHistCount(void)
{
	_cmdText *tPtr;
	tPtr = (_cmdText *)cmdTextBase.link.next;
	if(tPtr)
		return(CountListItems((_list *)tPtr));
	else
		return(-1);
}

//=============================================================================


FLAG
CmdWinInput(_input *in,_object *oPtr)
{
	FLAG inputUsed = boolean::TRUE;
	FLAG iUsed = boolean::FALSE;
	FLAG leave = boolean::FALSE;
	FLAG cont;                                              // used locally
	int i;

// expression parser stuff
	errorcode error;                        /* place to put an error */
	char    *isPtr;                         /* pointer to the input stream */
	char    *cPtr;                          // used locally
	_gadget  *gPtr;
    _window *pWindow;
    _listRectDesc *lrPtr;

    pWindow = (_window *)oPtr->layer;

	if (scriptFile)                 // are we running a script?

		switch(in->inputType)
		{
			case INP_KEY:
					switch(in->fullKey)
					{
						case KEY_CTRLC:
							fclose(scriptFile);
							scriptFile = NULL;
							timeToGoOn = 0L;
							break;
						default:
							if(timeToGoOn == MAXLONG)
								timeToGoOn = 0L;
							inputUsed = boolean::FALSE;
							break;
					}
				break;
			case INP_MOUSE_LEFTBUTTON_DOWN:
				if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
				{
					switch(gPtr->gNum)
					{
						default:
							inputUsed = boolean::FALSE;
							break;
					}
				}
				break;
			case INP_RESIZE:
				AddCmdGadgets(oPtr,pWindow);
				pWindow->activeGadget = FindGadget(&pWindow->gadgBase,CMDGAD_CMD);
				pWindow->activeGadget->cursorX = pWindow->cursorX - (CMD_XPOS-1);
				pWindow->cursorY = pWindow->ySize -2;
				UpdateCursor();                                                         // kts since ActivateStringGadget would reset the cursorX
				break;
			default:
				inputUsed = boolean::FALSE;
				break;
		}
	else
		switch(in->inputType)
		{
			case INP_KEY:
				if(pWindow->activeGadget && pWindow->activeGadget->gType == GADTYPE_STRING)
					iUsed = StrGadgInput(pWindow,pWindow->activeGadget,in);
				if(iUsed == boolean::FALSE)
					switch(in->fullKey)
					{
						case CMD_CLOSEWINDOW:
							leave = boolean::TRUE;
							break;
						case CMD_OPENWINDOWHELPFILE:
							OpenRefFile(EXT_HELP,HELPNAME,"Command:");
							break;
						case CMD_PREVHIST:
							if(cmdHistLine > 0)
							{
								cmdHistLine--;
								CopyHistLine(cmdHistLine,pWindow);
							}
							if(cmdHistLine == -1)
							{
								i = GetHistCount();
								if(i >= 0)
								 {
									cmdHistLine = i;
									CopyHistLine(cmdHistLine,pWindow);
								 }
							}
							break;
						case CMD_NEXTHIST:
							if(cmdHistLine != -1)
							{
								if(cmdHistLine < GetHistCount())
								{
									cmdHistLine++;
									CopyHistLine(cmdHistLine,pWindow);
								}
							}
							break;
						case KEY_RETURN:
							cmdHistLine = -1;                                       // forget place in history
							switch(pWindow->activeGadget->gNum)
							{
								case CMDGAD_CMD:
									gPtr = pWindow->activeGadget;
#pragma warn -pia
									if(error = DoCmd(gPtr))
#pragma warn +pia
										CmdError(error);
									else
									{
										*(gPtr->gadgText) = 0; /* erase the string in the gadget */
										gPtr->cursorX = 0;
									}
									ActivateStrGadget(pWindow,gPtr);
									break;
							}

							break;
						default:
							inputUsed = boolean::FALSE;
							break;
					}
				break;
			case INP_MOUSE_LEFTBUTTON_DOWN:
				if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
				{
					switch(gPtr->gNum)
					{
						case GGAD_CLOSE:
							leave = boolean::TRUE;
							break;
						case CMDGAD_CMD:
							pWindow->activeGadget = gPtr;
							ActivateStrGadget(pWindow,gPtr);
							break;
						default:
							if(gPtr->gType == GADTYPE_STRING)
							{
								pWindow->activeGadget = gPtr;
								ActivateStrGadget(pWindow,gPtr);
							}
							else
								inputUsed = boolean::FALSE;
							break;
					}
				}
				break;
			case INP_RESIZE:
				AddCmdGadgets(oPtr,pWindow);
				pWindow->activeGadget = FindGadget(&pWindow->gadgBase,CMDGAD_CMD);
				pWindow->activeGadget->cursorX = pWindow->cursorX - (CMD_XPOS-1);
				pWindow->cursorY = pWindow->ySize -2;
				UpdateCursor();                                                         // kts since ActivateStringGadget would reset the cursorX
				break;
			default:
				inputUsed = boolean::FALSE;
				break;
		}

	if(inputUsed)
		DrawGadgets(pWindow);

	if(leave)
	 {
//		CloseWindow(pWindow);
		bool notPopup = !oPtr->dataPtr;
		delete pWindow;
		cmdObjPtr = NULL;                               // go away
		commandWindow = NULL;                   // and no default window
//		DeleteObject(oPtr);
		delete oPtr;
		if(notPopup)
			cmdOpen = boolean::FALSE;
	 }
	return(inputUsed);
}

//=============================================================================

void
CommandRoutine(_object *oPtr)
{
	_cmdText *tPtr;
	int i,ySize,lineCount;
	errorcode error;

	if(scriptFile && (clock() >= timeToGoOn))
	{
		timeToGoOn = 0L;
		error = ContinueScript();
	}

	ySize = oPtr->layer->ySize-3;

	tPtr = (_cmdText *)cmdTextBase.link.next;
	lineCount = CountListItems((_list *)tPtr);
	if(lineCount >= ySize)
		tPtr = (_cmdText *)SkipNodes((_list *)tPtr,lineCount-(ySize-1));
	if (ySize)
		FillWindowSize(oPtr->layer,ySize);

	i = 0;
	while(i<ySize && tPtr != 0)
	{
		PrintLayAttrClip(oPtr->layer,tPtr->textPtr,1,i+1,tPtr->charAttr,oPtr->layer->xSize-1);
		i++;
		tPtr = (_cmdText *)tPtr->link.next;
	}
}

//=============================================================================

FLAG
OpenCmdWindow(void)
{
	_object *oPtr;
	_window *pWindow;

	if(cmdOpen == boolean::TRUE)
	{
		cmdObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return(boolean::TRUE);
	}
	cmdOpen = boolean::TRUE;
    pWindow = new _window(CMD_XPOS,CMD_YPOS,CMD_XSIZE,CMD_YSIZE,"Command Window",(unsigned char)commandAttr,(char)commandChar);
	assert( pWindow );

	pWindow->cursorOn = boolean::TRUE;
	pWindow->xMin = 15;
	pWindow->xMax = displayWidth;
	pWindow->yMin = 4;
	pWindow->yMax = displayHeight;

	oPtr = AddObject(CommandRoutine);
	cmdObjPtr = oPtr;
	oPtr->dataPtr = NULL;
	oPtr->inputFlags = INPF_KEY|INPF_MOUSEBUTTONS|INPF_CASESENSE;
	oPtr->inputRoutine = CmdWinInput;
	oPtr->layer =pWindow;
	pWindow->activeGadget = NULL;
	pWindow->data = (void *)oPtr;

	AddSysGadgets(pWindow);
	AddCmdGadgets(oPtr,pWindow);
	DrawGadgets(pWindow);
	pWindow->activeGadget = FindGadget(&pWindow->gadgBase,CMDGAD_CMD);
	ActivateStrGadget(pWindow,pWindow->activeGadget);
	ActivateFrontWindow();
	return (boolean::TRUE);
}


//=============================================================================

