//=============================================================================
// command.h
//=============================================================================

struct _cmdText
{
	_list link;
	char *textPtr;
	unsigned char charAttr;
};

//=============================================================================

extern _cmdText cmdTextBase;
extern _object *commandWindow;		/* who is the command window talking to? */
extern _object *cmdObjPtr;

//=============================================================================

char *
ExecFile(char * s, errorcode *error);

void
SlaveStep(void);

char *
ParseWord (char * s, unsigned int *word);

char *
GetNumbers( char *s, int count, ULONG *answer, errorcode *error);

errorcode
ParseCmdLine(char *localCmd);

void PrintToCmdWindow(char *);

errorcode
ContinueScript(void);

char *
FindWord(char *s, char *d, char *c);

void
ForceWindowResize ( _layer *lPtr, _object *oPtr,FLAG(*inputRoutine)(_input *,_object *));

char *
EvalAssign(char *s, errorcode *error);

//=============================================================================
