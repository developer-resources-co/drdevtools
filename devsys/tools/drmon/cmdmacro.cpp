/*************************************************************************
 * cmdmacro.c -- Command Line macro support.                             *
 *************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "error.hpp"
#include "cmdmacro.hpp"

_cmdmacro cmdMacroBase;

void
InitCmdMacros(void)
{
	cmdMacroBase.prev = cmdMacroBase.next = NULL;
	cmdMacroBase.name = cmdMacroBase.text = "";
	return;
}

char *
FindCmdMacro( char* name )
{
//	printf( "FindCmdMacro(%s)\n", name );
	_cmdmacro* mPtr = cmdMacroBase.next;

	while ( mPtr )
		{
//		printf( "name=%s\tmPtr->name=%s\n", name, mPtr->name );
		if ( !strcmp(name,mPtr->name) )
			return( mPtr->text );
		mPtr = mPtr->next;
		}

	return NULL;
}

_cmdmacro *
FindLastCmdMacro(void)
{
	_cmdmacro *mPtr;
	mPtr = &cmdMacroBase;

	while(mPtr->next)
		mPtr=mPtr->next;

	return mPtr;
}

errorcode
AddCmdMacro(char *name, char *text)
{

	_cmdmacro *mPtr;
	mPtr = FindLastCmdMacro();

	if(mPtr->next = (_cmdmacro *)malloc(sizeof(_cmdmacro)))
	{
		mPtr->next->prev = mPtr;
		mPtr = mPtr->next;
		mPtr->name = (char *)malloc(strlen(name)+1);
		if (mPtr->name)
		{
			strcpy(mPtr->name,name);
			mPtr->text = (char *)malloc(strlen(text)+1);
			if (mPtr->text)
			{
				strcpy(mPtr->text,text);
				mPtr->next=NULL;
				return(NOERR);
			}
			else
			{
				free(mPtr->name);
				mPtr->prev->next = NULL;
				free(mPtr);
			}
		}
		else
		{
			mPtr->prev->next = NULL;
			free(mPtr);
		}
	}
	return (ERROR_NOMEM);
}















