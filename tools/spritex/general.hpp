void truncate (char *s, int l);
void Swap (int *a,int *b);
void Error (char *a);
void far *SafeFarMalloc (unsigned long size);
void *SafeMalloc (unsigned size);
void MemError (unsigned long size,char type,unsigned long left);
void Beep (void);
void DebugChar( char c );
void FarMemCpy (void far *to, void far *from, unsigned length);
void Debug ( char *message );
unsigned char *
FindChar(char *sourceString,unsigned char c);

void
PadString(unsigned char *buffer,int offset);

void
UnPadString(unsigned char *buffer);

int
UnPaddedStrlen(unsigned char *buffer);

char *
DupString(char *);
