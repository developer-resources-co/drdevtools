/*
 * WORD.H -- List of reserved words
 */

enum {
		RW_EVAL,
		RW_ADD,
		RW_ALL,
	  	RW_ALPHA,
		RW_ASCII,
		RW_BCLEAR,
		RW_BREAK,
		RW_BSET,
		RW_BYTE,
		RW_CLEAR,
		RW_CLOSE,
		RW_CODE,
		RW_COMMAND,
		RW_CONSOLE,
		RW_DEBUG,
		RW_DYNAMICRUNNING,
		RW_DYNAMIC,
		RW_EXECUTE,
		RW_EXPRESSION,
		RW_FILE,
		RW_FSEND,
		RW_GET,
		RW_HELP,
		RW_HIT,
		RW_LINE,
		RW_LOAD,
		RW_LOADMACRO,
		RW_LOADSYM,
		RW_LONG,
		RW_MEMORY,
		RW_NAME,
		RW_OFF,
		RW_ON,
		RW_OPEN,
		RW_OVER,
		RW_PARAM,
		RW_POSITION,
#ifdef SNES
		RW_PPU,
#endif
		RW_QUIT,
		RW_REGISTER,
		RW_REPOSITION,
		RW_RESET,
		RW_RESIZE,
		RW_RESTART,
		RW_RUNNOUPDATE,
		RW_RUN,
		RW_SAVE,
		RW_SAVEMACRO,
		RW_SAVESYM,
		RW_SCLEAR,
/*		RW_SEARCH,*/
		RW_SEARCHLIST,
		RW_SELECT,
		RW_SEND,
		RW_SET,
		RW_SIZE,
		RW_SORT,
		RW_SOURCE,
		RW_STATIC,
		RW_STEP,
		RW_STOP,
		RW_SYMBOL,
		RW_TEXT,
		RW_VALUE,
#ifdef GENESIS
		RW_VCR,
		RW_VDP,
		RW_VSR,
#endif
		RW_WAIT,
		RW_WATCH,
		RW_WLOAD,
		RW_WORD,
		RW_MACRODEF
	};

enum {
		WT_INVALID,
		WT_COMMAND,
		WT_PARAM,
		WT_HARDWINDOW,
		WT_WINDOWNAME,
		WT_SYMBOL,
		WT_MACRO

	};

void PrintToCmdWindow(char *);

char *
FindWord(char *s, char *d, char *c);

char *
ParseWord (char * s, unsigned int *word);

char *
GetNumbers( char *s, int count, unsigned long *answer, errorcode *error);

//==============================================================================
