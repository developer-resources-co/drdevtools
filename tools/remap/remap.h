// chardiff.h


#define MAXCHARS 2000

int OpenExistingCharFile(uchar huge *buffer[MAXCHARS],char *fileName);
int FindChar( uchar huge *chr,uchar huge *charArray[MAXCHARS]);
void ReadDiffFile( char *inputFileName);

//int CheckDups (int a,int b);
//void WriteChars (char *OutputFile);
//void WriteMap (char *MapFile);
//void FlipCharX (void);
//void FlipCharY (void);


#define FLAG unsigned char
#define TRUE 1
#define FALSE 0
