//==============================================================================
// grabber.h:
//==============================================================================

void OpenExistingCharFile (char *OutputFile);
int CheckDups (int a,int b);
void WriteChars (char *OutputFile);
void WriteMap (char *MapFile);
void FlipCharX (void);
void FlipCharY (void);

extern int charSize;

#define FLAG unsigned char
#define TRUE 1
#define FALSE 0



#define SNES 1
#define GENESIS 0





#if GENESIS
#define MAXCHARS 2048
#endif

#if SNES
#define MAXCHARS 1024
#endif


//==============================================================================
