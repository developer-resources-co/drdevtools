/* --------------------------------------------------------------------------

                         Alexandria Genesis Library
						----------------------------

Title:			Header File for TEXT.ASM file

File:			TEXT.H

Description:	Contains all C Prototypes to interface to TEXT.ASM

-------------------------------------------------------------------------- */

#ifndef _TEXT_H
#define _TEXT_H



#include <alexdef.h>

#define CURSPOS( x, y )		( (y)<<16 | (x) )

extern unsigned short textAttr;

/* --------------------------------------------------------------------------

   pre:
   post:
	returns cursorY in hiword and cursorX in loword

-------------------------------------------------------------------------- */

extern LONG SaveCursor (void);



/* --------------------------------------------------------------------------

   pre:
	hiword of iCursor is cursorY, loword is cursorX
   post:
	restores cursor location from iCursor

-------------------------------------------------------------------------- */

extern void RestoreCursor (LONG iCursor);



/* --------------------------------------------------------------------------

   pre:
	nVDPCharSet is the character set base address
   post:
	initializes text stuff

-------------------------------------------------------------------------- */

extern void InitText (WORD nVDPCharSet);



/* --------------------------------------------------------------------------

   pre:
	sString is the empty string
	cChar is the character with which to fill the string
	nBytes is the number of times to repeat the character
   post:
	fills the string with the given character

-------------------------------------------------------------------------- */

extern void FillString
	(char *sString, char cChar, UWORD nBytes);



/* --------------------------------------------------------------------------

   pre:
	sBuffer is the area to store the string
	pInputStructure is the way to get the characters
	nChars is the number of characters to get
   post:
	reads the number of chars into the buffer

-------------------------------------------------------------------------- */

extern void GetTextString
	(char *sBuffer, void *pInputStructure, UWORD nChars);



/* --------------------------------------------------------------------------

   pre:
	sString is the string to print
	nChars is the number of characters to print
   post:
	prints the number of characters of the string
	returns a pointer to just beyond the end of the string

-------------------------------------------------------------------------- */

extern char *PrintTextCount
	(char *sString, UWORD nChars);



/* --------------------------------------------------------------------------

   pre:
	sString is the string to print
   post:
	prints the string
	returns a pointer to just beyond the end of the string

-------------------------------------------------------------------------- */

extern char *PrintText (char *sString);



/* --------------------------------------------------------------------------

   pre:
	c is the character to print
   post:
	prints the character

-------------------------------------------------------------------------- */

extern void PrintChar (char c);



/* --------------------------------------------------------------------------

   pre:
	iValue is the decimal value to print; must be less than 99,999
	nDigits is the number of digits to print
   post:
	prints the number of digits of the value

-------------------------------------------------------------------------- */

extern void PrintDecimalCount (LONG iValue, WORD nDigits);



/* --------------------------------------------------------------------------

   pre:
	iValue is the hex value to print
   post:
	prints the value

-------------------------------------------------------------------------- */

extern void PrintHex32 (LONG iValue);



/* --------------------------------------------------------------------------

   pre:
	nValue is the hex value to print
   post:
	prints the value

-------------------------------------------------------------------------- */

extern void PrintHex16 (WORD nValue);



/* --------------------------------------------------------------------------

   pre:
	cValue is the hex value to print
   post:
	prints the value

-------------------------------------------------------------------------- */

extern void PrintHex8 (char cValue);



#endif


/* TEXT.H --  EOF */

