//=============================================================================
//  macro.c: all macro handling routines
//=============================================================================

#include	"base.hpp"
#include	"global.hpp"

#include	"list.hpp"
#include	"layer.hpp"
#include	"input.hpp"
#include	"filereq.hpp"
#include	"message.hpp"
#include	"error.hpp"
#include	"keys.hpp"
#include	"menu.hpp"

//=============================================================================

#define MACROMAXLEN 1000

//#define NUMKEYS 2048
#define NUMKEYS 1<<KEY_MAXBIT

#define MACHEADERLEN 13
char macroName[MACHEADERLEN] = "DR MACROFILE";

int *keyTable[NUMKEYS];

//=============================================================================

int *macroStack[100];
int macroSP;
int *macroKeyPtr;
int macroRecBuff[MACROMAXLEN+1];
int macroRecKeyNum;
int macroRecKey;
FLAG macroRecording;

//=============================================================================

FLAG
NukeMacros(void)
{
	int i;
	for(i=0;i<NUMKEYS;++i)
		keyTable[i] = NULL;
	return(boolean::TRUE);
}

//=============================================================================

void
InitMacros(void)
{
	macroSP = 0;
	macroKeyPtr = 0;
	macroRecording = boolean::FALSE;
	NukeMacros();
}

//=============================================================================

void
KeyCopy(int *dest,int *src)
{
	while(*src)
		*dest++ = *src++;
	*dest = 0;
}

//=============================================================================

errorcode
LoadMacros(char *fileName)
{
	FILE *inFile;
	int i,key,in,keyCount,*kPtr;
	char *cPtr;
	int keyInBuff[MACROMAXLEN+1];
	char tempBuff[MACHEADERLEN+1];

	inFile = fopen(fileName,"rb");
	if(inFile == 0)
		return(ERROR_FILENOTFOUND);
	else
	 {
		for(i=0;i<MACHEADERLEN+1;i++)				// be sure to eat the ctrl-Z at the end
			tempBuff[i] = getc(inFile);
		if(strcmp(macroName,tempBuff))
			return(ERROR_NOTMACROFILE);
		else
		 {
			key = getc(inFile) | (getc(inFile)<<8);
			while(key)
	 	 	 {
				keyCount = 0;
				while((keyInBuff[keyCount] = getc(inFile) | (getc(inFile)<<8)) != 0)
					keyCount++;

				kPtr = (int *)malloc((keyCount*2)+2);
				if(kPtr)
				 {
					KeyCopy(kPtr,keyInBuff);
					if(keyTable[key])
						free(keyTable[key]);
					keyTable[key] = kPtr;
					key = getc(inFile) | (getc(inFile)<<8);
				 }
	 	 	 }
			fclose(inFile);
		 }
	 }
	return(NOERR);
}

//=============================================================================

void
LoadGUIMac(void *data,char *path, char *fileName)
{
	errorcode error;
	error = LoadMacros(fileName);
	if(error)
		PrintError(error);
}

//=============================================================================

FLAG
LoadGUIMacros(_menuItem *iPtr,_object *oPtr,int choice)
{
	strcpy(patternString,"*.mac       ");
	DoFileReq("Load Macros",20,1,LoadGUIMac,0);
	return(boolean::TRUE);
}

//=============================================================================

errorcode
SaveMacros(char *fileName)
{
	int i;
	FILE *outFile;
	int *cPtr;

	i = 0;
	outFile = fopen(fileName,"wb");
	if(outFile == 0)
		return(ERROR_CANTSAVEFILE);

	fprintf(outFile,"%s",macroName);
	putc(0,outFile);
	putc(26,outFile);
	while(i++ < NUMKEYS)
	 {
		if(keyTable[i] != 0)
		 {
			putc(i&0xff,outFile);
			putc(i>>0x8,outFile);			// print which key it is
			cPtr = keyTable[i];
			while(*cPtr)
			 {
				putc(*cPtr & 0xff,outFile);
				putc((*cPtr++ >> 8) & 0xff,outFile);
			 }
			putc(0,outFile);
			putc(0,outFile);
		 }
	 }

	putc(0,outFile);
	putc(0,outFile);
	fclose(outFile);
	return(NOERR);
}

//=============================================================================

void
SaveGUIMac(void *data,char *path, char *fileName)
{
	errorcode error;
	error = SaveMacros(fileName);
	if(error)
		PrintError(error);
}

//=============================================================================

FLAG
SaveGUIMacros(_menuItem *iPtr,_object *oPtr,int choice)
{
	strcpy(patternString,"*.mac       ");
	DoFileReq("Save Macros",20,1,SaveGUIMac,0);
	return(boolean::TRUE);
}

//=============================================================================

FLAG
CreateMacro(_menuItem *iPtr,_object *oPtr,int choice)
{
	PrintMessageBar("Press Key to Record");
	// hit key to assign macro to
	macroRecKey = GetDirectKey();
	macroRecKeyNum = 0;					// # of keystrokes in macro
	macroRecording = boolean::TRUE;
	keyTable[macroRecKey] = 0;			// clear any existing macro
	PrintMessageBar("Recording Macro, press key again to finish");
	return(boolean::TRUE);
}

//=============================================================================

void
FinishMacroRecording(void)
{
	int *kPtr;
	macroRecording = boolean::FALSE;
	if(keyTable[macroRecKey] != 0)
		free(keyTable[macroRecKey]);

	if(macroRecKeyNum)					// in case user records nothing
	 {
		macroRecBuff[macroRecKeyNum++] = 0;		// terminate string
		kPtr = (int *)malloc((macroRecKeyNum*2)+2);
		if(kPtr)
		 {
			KeyCopy(kPtr,macroRecBuff);
			keyTable[macroRecKey] = kPtr;
		 }
	 }
	ClearMessageBar();
}

//=============================================================================
// begin playback of a keyboard macro

void
StartMacro(int fullKey)
{
	if(macroKeyPtr)
		macroStack[++macroSP] = macroKeyPtr;
	macroKeyPtr = keyTable[fullKey];
}

//=============================================================================

int *
PopMacro(void)
{
	if(macroSP)
		return(macroStack[macroSP--]);
	else
		return(NULL);
}

//=============================================================================

int
GetMacroKey(void)
{
	int fullKey;
	if(macroKeyPtr)
	 {
		fullKey = *macroKeyPtr++;
		if(!*macroKeyPtr)
			macroKeyPtr = PopMacro();
		return(fullKey);
	 }
	else
		return(GetDirectKey());
}

//=============================================================================

unsigned int
GetFullKey(void)
{
    unsigned int fullKey;
    unsigned char key;
	int *oldMacroKeyPtr;

	oldMacroKeyPtr = macroKeyPtr;

	fullKey = GetMacroKey();

	if(keyTable[fullKey])
	 {
		StartMacro(fullKey);
		fullKey = GetFullKey();
	 }
	if(macroRecording)
	 {
		if(fullKey == macroRecKey)					// prevent macro loops
		 {
			FinishMacroRecording();
			fullKey = 0;
		 }
		else
			if(oldMacroKeyPtr == 0)
				if(macroRecKeyNum < MACROMAXLEN)
					macroRecBuff[macroRecKeyNum++] = fullKey;
	 }
	return(fullKey);
}

//=============================================================================
