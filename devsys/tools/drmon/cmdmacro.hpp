/*************************************************************************
 * macro support header file
 *************************************************************************/


struct _cmdmacro {
	_cmdmacro *prev;
	_cmdmacro *next;
	char *name;
	char *text;
	};

void
InitCmdMacros(void);

char *
FindCmdMacro(char *name);

_cmdmacro *
FindLastCmdMacro(void);

errorcode
AddCmdMacro(char *name, char *text);


