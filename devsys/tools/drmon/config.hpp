/*============================================================================*/
/*	 config.hpp: sfxmon configuration code header */
/*============================================================================*/

#include "control.hpp"

extern uint windowTitleMoveAttr;
extern uint windowTitleAttr,windowTitleHighlightAttr,windowBorderAttr;
extern uint menuAttr,menuHighlightAttr,menuBarAttr,menuBarHighlightAttr;
extern uint menuChar;
extern uint backgroundAttr,backgroundChar;
extern uint backgroundAttr2,backgroundChar2;
extern  int xShadow, yShadow;
extern uint mouseAttr,mouseChar;
extern uint fileReqAttr,fileReqChar;
extern uint listAttr,listChar;
extern uint reqAttr,reqChar;
extern uint breakAttr,breakChar;
extern uint symbolAttr,symbolChar;
extern uint messageBarAttr;

extern uint regAttr,regChar;
extern uint statusAttr,statusChar;
extern uint dumpAttr,dumpChar;
extern uint dumpPCAttr,dumpBreakAttr,dumpCursorAttr;
extern uint textViewAttr,textViewChar;
extern uint strAttr,strChar;
extern uint exprAttr,exprChar;
extern uint projinfoAttr,projinfoChar;
extern uint asciiAttr,asciiChar;
extern uint procAttr,procChar;

extern uint commandAttr,commandChar;
extern uint watchAttr,watchChar;
extern uint sourceAttr,sourceChar;
extern uint sourcePCAttr,sourceCursorAttr;

extern uint gadgPropAttr,gadgPropHighlightAttr;

extern uint *configPtrs[];
extern char *configNames[];

//==============================================================================

enum
{
	PARM_DISPMODE = 1,
	PARM_TWOSCREEN,
	PARM_WRITEPROTECT,
	PARM_VIRTUALXSIZE,
	PARM_VIRTUALYSIZE
};

/*============================================================================*/

void
SetupConfig(void);

void
LoadConfig(void);

void
SaveConfig(void);

errorcode
SetConfigParm(char *name, uint value);

void
SetConfigParm(uint which, uint value);

int
FindConfigName(char *s);

ULONG GetParamAsSymbol(char *string);

/*============================================================================*/
