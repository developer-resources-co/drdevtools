//==============================================================================
// text.hpp: text library built on graphport library
//==============================================================================

#ifndef PCLIB_text_H
#define PCLIB_text_H

#include <ctype.h>
#include <pclib/grphport.h>

//==============================================================================
// font handler in graphics modes

class gfxText
{
public:
	gfxText(rastGraphPort *newDestPort, ubyte *newFont, uint newCharCount, uint charWidth, uint charHeight, char newStartChar = ' ')
	 {
		destPort = newDestPort;
		charCount = newCharCount;
		width = charWidth;
		height = charHeight;
		startChar = newStartChar;
	 	fGrPort = new rastGraphPort(newFont, charCount*width, height);
		pos.X(0); pos.Y(0);
		charPort = new clippedGraphPort(fGrPort,point(),width, height);
	 }


	void
	PrintChar(char c)
		{
		if ( c == '\n' )
			NewLine();
		else
			{
			c = toupper(c) - startChar;
			if (c>=0 && c < charCount)
			 	{
				charPort->MovePort(point(c*width,0));				// move clipped port to correct char
				destPort->Move(pos);
				charPort->PlotOnPort(*destPort);						// actually plot the character on the graphport

				pos.X(pos.X()+width);		// move cursor to right
				if(pos.X() >= destPort->GetWidth())
		 			{
					NewLine();
		 			}
			 	}
			}
		}

	void
	PrintLine(const char *str)
	 {
		while(*str)
			PrintChar(*str++);
	 }

	const point &Where(void) { return(pos); }
	const point &Move(point newPos) { pos = newPos; return(pos); }


	int Width(void) { return(width); }
	int Height(void) { return(height); }

private:
	point pos;
	uchar *font;
	rastGraphPort *fGrPort,*destPort;
	clippedGraphPort *charPort;
	uint charCount;
	uint width, height;
	char startChar;

	void NewLine()	{ pos.X(0); pos.Y(pos.Y()+height); }

	gfxText() {}
};

//==============================================================================
#endif
//==============================================================================
