//=============================================================================
//	message.h: message bar code header
//=============================================================================

void ClearMessageBar(void);
void PrintMessageBar( char *fmt, ... );
void PrintWarning(char *text);

#define EXCEPTION_BASE 4

void
MessageBarTick(void);

void
ClearMessageBarSection(int x,int size);

void
PrintMessageBarSection(int x,int size,char *text);

void
PrintMessageStatus(int status);

void
PrintWarning(char *text);

#define STATUS_STOPPED 0
#define STATUS_RUNNOUPDATE 1
#define STATUS_RUNNING  2
#define STATUS_SLAVE_DEAD 3

void
InitMessageBar(void);
