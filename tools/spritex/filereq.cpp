/*
 * filereq.c -- handle the simple file requester
 */
// pclib
#include <pclib\general.h>
#include <pclib\iff.h>
#include <pclib\grphport.h>

// local
#include "menu.hpp"
#include "sprite.hpp"

#include <ctype.h>
#include <dos.h>
#include <dir.h>

#include "global.hpp"
#include "utypes.hpp"
#include "general.hpp"
#include "spmain.hpp"
#include "input.hpp"
#include "image.hpp"
#include "gadget.hpp"
#include "keys.hpp"
#include "mouse.hpp"

#include "filereq.hpp"

#define MAXFILES 200

#define FRWINX 27
#define FRWINY 26

#define FRBASEX 10
#define FRBASEY 10
#define FRSIZEX 208
#define FRSIZEY 186
#define FRCANCELX 145
#define FRCANCELY 106
#define FRCANCELSIZEX 48
#define FRCANCELSIZEY 10
#define FRTITLEX	47
#define FRTITLEY	2
#define FRDOITX		149
#define FRDOITY		87
#define FRDOITGADX	145
#define FRDOITGADY	85
#define FRDOITGADSX 52
#define FRDOITGADSY 10

#define FRUPBUTTONX 130
#define FRUPBUTTONY 39

#define FRPGUPBUTTONX 130
#define FRPGUPBUTTONY 25

#define FRPGDNBUTTONX 130
#define FRPGDNBUTTONY 147

#define FRPGUDBUTTONSX 7
#define FRPGUDBUTTONSY 13

#define FRDNBUTTONX 130
#define FRDNBUTTONY	139

#define FRUDBUTTONSIZEX 7
#define FRUDBUTTONSIZEY 7

#define FRTEXTX 9
#define FRTEXTY  169
#define FRTEXTSX 72
#define FRTEXTSY 8

#define FR_RETURN 999			/* codes for file-requester passback */
#define FR_TOUCH 998
#define FR_KEYRETURN 997


struct _directory {
	char name[13];
	char type;
	} nameArray[MAXFILES];

struct _gadget frGadgetBase = { NULL, NULL, 0, 0, 0, 0, 0, 0, NULL, NULL, NullRoutine , NULL} ;

boolean fileActive;
boolean frQuitboolean;	/* do we need to quit ? */
int fileDelta, selFile, selLine;
extern char far filereqbitmap;
char *currDir;	/* current directory */

struct _string textBox = { "            ",0,12 };

void
HilightLine(int line)
{
	HilightRegion( FRBASEX + FRWINX, FRBASEY + FRWINY + line*8, 12*8, 8, 16 );
}

int
ScanDir()
{
	int i,done,fileCount;
	struct ffblk fileBlock;

	fileCount = 0;
	done = findfirst("*.*",&fileBlock,FA_DIREC);
	while(!done)
	{
		if((fileBlock.ff_attrib & FA_DIREC) && fileCount < MAXFILES) {
			strcpy(nameArray[fileCount].name,fileBlock.ff_name);
			nameArray[fileCount].type = 'D';
			}
		else if(fileCount < MAXFILES) {
			strcpy(nameArray[fileCount].name,fileBlock.ff_name);
			nameArray[fileCount].type = ' ';
			}
		PadString(nameArray[fileCount++].name,12);
		done = findnext(&fileBlock);
	}
	return(fileCount);
}

void
UpdateFileRequester(int fileNumber, int numFiles )
{
	int i,j;

	for ( i = fileNumber, j=0 ; j < numFiles ; i++,j++ ) 
		{
		font->Move( point(FRBASEX+FRWINX,FRBASEY+FRWINY+(8*j)) );
		font->PrintLine( nameArray[i].name );
//		T_xy_string(nameArray[i].name, FRBASEX + FRWINX, FRBASEY + FRWINY + (8*j));

		font->Move( point( FRBASEX+FRWINX-16,FRBASEY+FRWINY+(8*j)) );
		font->PrintLine( nameArray[i].type == 'D' ? "[]" : "  " );
//		T_xy_string( (nameArray[i].type == 'D' ? "[]" : "  ") , FRBASEX + FRWINX-16,
//			FRBASEY + FRWINY + ( 8*j));
		}

	for ( ; j < 16 ; j ++ )
		{
		font->Move( point(FRBASEX+FRWINX-16,FRBASEY+FRWINY+(8*j)) );
		font->PrintLine( "              " );
//		T_xy_string("              ",FRBASEX+FRWINX-16,FRBASEY+FRWINY+(8*j));
		}
}

/*	if (! DoHitGadget ( spGadgetBase.next , in )) */

void
FRDoInput(struct _input *in)
{
	short i,j;
	if (!StrGadgInput(&textBox, in)) {
		switch(in->inputType)
		{
			case INP_KEY:
				HideMouse();
				switch (in->fullKey)
				{
					case KEY_ESC:
						frQuitboolean = boolean::TRUE;
						break;
					case KEY_UP:
						fileDelta--;
						break;
					case KEY_DOWN:
						fileDelta++;
						break;
					case KEY_PGDWN:
						fileDelta += 16;
						break;
					case KEY_PGUP:
						fileDelta -= 16;
						break;
					case KEY_RETURN:
						fileDelta = FR_RETURN;
						break;
				}
				ShowMouse();
				break;
			case INP_MOUSE_LEFTBUTTON_DOWN:
			case INP_MOUSE_RIGHTBUTTON_DOWN:
				HideMouse();
				DoHitGadget(frGadgetBase.next,in);
				ShowMouse();
				break;
		}
	}
	return;
}

void
CleanUpFRGadgets(void)
{
	struct _gadget *gPtr,*gPtr2;

	gPtr = FindTail(&frGadgetBase);	/* get first one */
	while (gPtr->prev) {
		gPtr2 = gPtr->prev;
		KillGadget(gPtr);
		gPtr = gPtr2;
		}
	return;
}

void
frCancelRoutine (struct _gadget *gPtr, struct _input *in)
{
	frQuitboolean = boolean::TRUE;
	return;
}

int
SetScanDir(char *dir)	/* set directory, and do ScanDir */
{
	chdir(dir);
	return(ScanDir());
}

void
frTouchRoutine(struct _gadget *gPtr, struct _input *in)
{
	fileDelta = FR_TOUCH;
	selLine = gPtr->identity;
	return;
}

void
frUpRoutine(struct _gadget *gPtr, struct _input *in)
{
	fileDelta = -1;
}
void
frDoitRoutine(struct _gadget *gPtr, struct _input *in)
{
	fileDelta = FR_RETURN;
}

void
frDownRoutine(struct _gadget *gPtr, struct _input *in)
{
	fileDelta = 1;
}

void
frPgUpRoutine(struct _gadget *gPtr, struct _input *in)
{
	fileDelta = -16;
}

void
frPgDnRoutine(struct _gadget *gPtr, struct _input *in)
{
	fileDelta = 16;
}

/*
void
frTextRoutine(struct _gadget *gPtr, struct _input *in)
{
	boolean continueboolean = boolean::TRUE;

	do {
		if(InputPending(in)) {
			if ((in->inputType) == INP_KEY) {
				switch (in->fullKey) {
					case KEY_ESC:
						return;
					case KEY_RETURN:
						fileDelta = FR_KEYRETURN;
						continueboolean = boolean::FALSE;
						break;
					default:
						StrGadgInput(gPtr,in);
						break;
				}
			}
		}
	} while(continueboolean);
}

 */

void
DrawCursor(struct _string *stPtr)
	{
	display->SetColorIndex( 0xEF );
	display->HLine( point(FRBASEX+FRTEXTX,FRBASEY+FRTEXTY+8), 105 );
	display->SetColorIndex( 0xF0 );
	display->HLine( point( FRBASEX+FRTEXTX+(stPtr->cursorX*8),FRBASEY+FRTEXTY+8 ), 8 );
	}

void
CreateFileReqGadgets(void)
{
		int i;

		CreateGadget( &frGadgetBase, FRBASEX + FRCANCELX , FRBASEY + FRCANCELY,
		FRCANCELSIZEX, FRCANCELSIZEY,
		0, 0, NULL, NULL, NULL, frCancelRoutine );

		CreateGadget ( &frGadgetBase, FRBASEX+FRUPBUTTONX, FRBASEY+FRUPBUTTONY,
		FRUDBUTTONSIZEX, FRUDBUTTONSIZEY,
		0, 0, NULL, NULL, NULL, frUpRoutine );

		CreateGadget ( &frGadgetBase, FRBASEX+FRDNBUTTONX, FRBASEY+FRDNBUTTONY,
		FRUDBUTTONSIZEX, FRUDBUTTONSIZEY,
		0, 0, NULL, NULL, NULL, frDownRoutine );

		CreateGadget ( &frGadgetBase, FRBASEX+FRPGUPBUTTONX, FRBASEY+FRPGUPBUTTONY,
		FRPGUDBUTTONSX, FRPGUDBUTTONSY,
		0, 0, NULL, NULL, NULL, frPgUpRoutine );

		CreateGadget ( &frGadgetBase, FRBASEX+FRPGDNBUTTONX, FRBASEY+FRPGDNBUTTONY,
		FRPGUDBUTTONSX, FRPGUDBUTTONSY,
		0, 0, NULL, NULL, NULL, frPgDnRoutine );

		CreateGadget ( &frGadgetBase, FRBASEX+FRDOITGADX, FRBASEY+FRDOITGADY,
		FRDOITGADSX, FRDOITGADSY,
		0, 0, NULL, NULL, NULL, frDoitRoutine );

//		CreateGadget ( &frGadgetBase, FRBASEX+FRTEXTX, FRBASEY+FRTEXTY,
//			FRTEXTSX, FRTEXTSY, 0,0, (void *) &textBox, NULL, NULL, frTextRoutine);

		for ( i = 0 ; i < 16 ; i ++ )
			CreateGadget(&frGadgetBase, FRBASEX+FRWINX, FRBASEY+FRWINY+8*i,
				12*8, 8, 0, i, NULL, NULL, NULL, frTouchRoutine);
		return;
}

boolean
IsDirectory(char *s)
{
	struct ffblk fileBlock;
	int foo;

	if ( s[1] == ':' ) return boolean::TRUE;

	foo = findfirst(s,&fileBlock,FA_DIREC);
	if (!foo)
		return(fileBlock.ff_attrib & FA_DIREC);
	return(boolean::FALSE);
}


void
FileReqRoutine(struct _gadget *gPtr, void (*routine)(void))
{
	int fileNumber, fileCount, numFiles;

	int maxDrive;

	boolean doitboolean = boolean::FALSE;
	boolean firstTimeboolean = boolean::TRUE;
	boolean inPndg;

	char currDir2[24];
	char *t;

	int i,j;

	struct GRAPHIC fileReq;
	fileReq.flag = boolean::FALSE;
	fileReq.imagePtr = &filereqbitmap;
	fileReq.offsetX = 0;
	fileReq.offsetY = 0;
	fileReq.sizeX = FRSIZEX;
	fileReq.sizeY = FRSIZEY;
	// Not previously initialized	
	fileReq.upLeftX =
	fileReq.upLeftY =
	fileReq.lowRightX =
	fileReq.lowRightY = 0;
	fileReq.midFlag = boolean::FALSE;


	struct VIEWPORT plotSpace = { 320, FRBASEX, 255, FRBASEY, 199 } ;

	maxDrive = setdisk(getdisk());

	TurnOffMouseMove();

	DrawClippedBitmap( &fileReq, &plotSpace, 0, 0 );

	fileDelta = selFile = fileNumber = 0;

	CreateFileReqGadgets();

	frQuitboolean = boolean::FALSE;

	font->Move( point(FRBASEX+FRTITLEX,FRBASEY+FRTITLEY) );
	font->PrintLine( (char far*)gPtr->dataPtr2 );
//	T_xy_string((char far *)gPtr->dataPtr2,FRBASEX+FRTITLEX, FRBASEY+FRTITLEY);

	font->Move( point(FRBASEX+FRDOITX,FRBASEY+FRDOITY) );
	font->PrintLine( (char far*)gPtr->dataPtr3 );
//	T_xy_string((char far *)gPtr->dataPtr3,FRBASEX+FRDOITX, FRBASEY+FRDOITY);

	currDir = getcwd(NULL,64);
	fileCount = SetScanDir(currDir);
	if (strlen(currDir) > 24)
	{
		strcpy(currDir2,"...");
		strncat(currDir2,&currDir[strlen(currDir)-21],21);
		free(currDir);
	}
	else
	{
		strcpy(currDir2,currDir);
		free(currDir);
	}
	ShowMouse();
	textBox.cursorX = UnPaddedStrlen(textBox.text);

	fileDelta = 0;
	while ( !frQuitboolean ) {
		if((inPndg = InputPending(&inBase)) || firstTimeboolean) {
			firstTimeboolean = boolean::FALSE;
			if (inPndg)
				FRDoInput(&inBase);

			if (fileDelta == FR_TOUCH)
				if ( selLine + fileNumber < fileCount ) {
					selFile = selLine+fileNumber;
					strcpy(textBox.text, nameArray[selFile].name);
				}

			if (fileDelta == FR_RETURN) {
				frQuitboolean = doitboolean = boolean::TRUE;
				strcpy((char *)gPtr->dataPtr1, textBox.text);
				if (((char *)gPtr->dataPtr1)[1] == ':' ) {
					i = toupper(*((char *)gPtr->dataPtr1)) - 'A';
					if ( i < maxDrive )
					 	setdisk(i);
				}
			}

			if (fileDelta == FR_RETURN || fileDelta == FR_TOUCH ) {
				t = DupString(textBox.text);
				UnPadString(t);
				if(IsDirectory(t)) {
					frQuitboolean = doitboolean = boolean::FALSE;
					chdir(t);
					currDir = getcwd(NULL,64);
					fileCount=SetScanDir(currDir);
					if (strlen(currDir) > 24)
					{
						strcpy(currDir2,"...");
						strncat(currDir2,&currDir[strlen(currDir)-21],21);
						free(currDir);
					}
					else
					{
						strcpy(currDir2,currDir);
						PadString(currDir2,24);
						free(currDir);
					}
					selFile = fileNumber = fileDelta = 0;
					}
				  else
					fileDelta = 0;
				free(t);
				}

			font->Move( point(FRBASEX+FRWINX-16, FRBASEY+FRWINY-10) );
			font->PrintLine( currDir2 );
//			T_xy_string(currDir2, FRBASEX + FRWINX - 16, FRBASEY + FRWINY - 10);


			selFile += fileDelta;
			selFile = max(selFile, 0);
			selFile = min(selFile, (fileCount-1));
			if (fileDelta) {
				strcpy(textBox.text,"            ");
				strcpy(textBox.text,nameArray[selFile].name);
				textBox.cursorX = UnPaddedStrlen(textBox.text);
			}

			font->Move( point(FRBASEX+FRTEXTX,FRBASEY+FRTEXTY) );
			font->PrintLine( textBox.text );
//			T_xy_string(textBox.text,FRBASEX+FRTEXTX, FRBASEY+FRTEXTY);
			DrawCursor(&textBox);
			fileDelta = 0;

			if ( selFile > fileNumber + 15 )
				fileNumber = selFile - 15;

			if ( selFile < fileNumber )
				fileNumber = selFile;

			fileNumber = min( (fileCount-1) - 15, fileNumber);
			fileNumber = max( fileNumber, 0 );
			numFiles = min ( (fileCount - fileNumber) , 16 );
			HideMouse(); UpdateFileRequester(fileNumber, numFiles); ShowMouse();
			HilightLine(selFile - fileNumber);
		}
	}

	CleanUpFRGadgets();

	HideMouse();
	display->SetColorIndex( 0 );
	display->Box( point(FRBASEX,FRBASEY), point(FRBASEX+FRSIZEX,FRBASEY+FRSIZEY) );
	RefreshImage();
	if (doitboolean)
		(*routine)();
	ShowMouse();
	TurnOnMouseMove();
	return;
}
