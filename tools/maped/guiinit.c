//=============================================================================
// guiinit.c:
//=============================================================================

#include <stdio.h>

#include "\gfxtools\lib\gfx.h"
#include "\gfxtools\lib\mouse.h"
#include "\gfxtools\lib\general.h"
#include "\gfxtools\lib\global.h"
#include "\gfxtools\lib\t.h"
#include "\gfxtools\lib\gui.h"

#include "guidefs.h"
#include "guifunc.h"
#include "guiinit.h"
#include "conio.h"

//=============================================================================

extern struct _gad *firstgadget;
extern struct _keygad *firstkeygadget;

//=============================================================================

uint swaphilo (uint a)
{
 return (((a & 0xff)<<8) + (a >> 8));
}

//=============================================================================

static int nNextColor = 0;

void
	InitSystemPalette( void )
	{
   RGBcolor pal[16];
	int i,a,b;

	for ( i=0; i<16; ++i )
		{
		b = 0x0F;
	   for( a=0; a<16; ++a )
			{
	      pal[a].r  = b<<2;
	      pal[a].g  = b<<2;
	      pal[a].b  = b<<2;
			--b;
			}
		SQ_set_palette( i*16, 16, (RGBcolor far *)pal );
		}
	}


int
	FindColor( int r, int g, int b )
	{
	extern RGBcolor vga_pal[256];
	int i;

//	SQ_get_palette( 0, 256, pal );
	for ( i=255; i>=0; --i )
		{
		if ( ( vga_pal[i].r == r ) && ( vga_pal[i].g == g ) && ( vga_pal[i].b == b ) )
			break;
		}
	return( i );
	}


void
	InitPalette( char *name, PALETTE *palette )
	{
   FILE *fp;
   unsigned int palbuffer[256];
   RGBcolor pal[256];
	int nColors;
	int i;

   fp = fopen (name,"rb");
   if ( !fp )
		Error( "Cannot open palette file..." );

   fseek (fp,0,2);                             // get fsize
   nColors =  ftell (fp) / 2;
   fseek (fp,0,0);                            // back to beginning

   fread( palbuffer, nColors, sizeof(int), fp );
   fclose( fp );

	palette->nOffset = nNextColor;

   for ( i=0; i<nColors; ++i )
		{
#if defined(GENESIS)
		palbuffer[i] = swaphilo (palbuffer[i]);
		pal[ i ].b   = ((palbuffer[i]>>8)&15) << 2;
		pal[ i ].g   = ((palbuffer[i]>>4) &15) << 2;
		pal[ i ].r   = (palbuffer[i]&15)       << 2;
#endif

#if defined(SNES)
		pal[ i ].b   = ((palbuffer[i]>>10)&31) << 1;
		pal[ i ].g   = ((palbuffer[i]>>5) &31) << 1;
		pal[ i ].r   = (palbuffer[i]&31)       << 1;
#endif
		}

	SQ_set_palette( nNextColor, nColors, (RGBcolor far *)pal );
	nNextColor += nColors;

	palette->szFilename = strdup( name );
	}

#if 0
	b = 0xf;
   for( a=64; a<64+16; ++a )
		{
      pal[a].r  = b<<2;
      pal[a].g  = b<<2;
      pal[a].b  = b<<2;
		b--;
		}
   for ( ; a<128; ++a)											// make lots of greys
      {
      pal[a].r  = a;
      pal[a].g  = a;
      pal[a].b  = a;
      }
   for ( ; a<256; ++a )											// make reverse of bottom 128 colors
   	{
      pal[a].r  = pal[255-a].r ^ 0xff;
      pal[a].g  = pal[255-a].g ^ 0xff;
      pal[a].b  = pal[255-a].b ^ 0xff;
      }
#endif



//=============================================================================

uint brushBuffer[MAXSELECTY*MAXSELECTX];
extern uchar font6[];

//=============================================================================

void ProjectStats( void )
	{
	int i;

	printf( "CHARACTER\tMAP\t\tPALETTE\n" );
	printf( "---------\t---------\t---------\n" );
	for ( i=0; i<num_of_maps; ++i )
		{
		printf( "%-12s\t%-12s\t%-12s\n",
			tilesets[i].szFilename,
			maps[i*2].szFilename,
			palfilename[i] );
		}
	printf( "---------\t---------\t---------\n" );
	printf( "%d\t%d\t%d\n", num_of_chars, num_of_maps, num_of_pals );
	}

void
	InitGUI( int argc, char *argv[] )
	{
   int a,b;
   FILE *fp;
	char **argv_follow;

   if (!CheckMouse ()) Error ("Mouse Driver Not Found");

	SetGfxMode( 0x13 );
	InitSystemPalette();
	selected.buffer = brushBuffer;

	// Parse command-line arguments
	for ( argv_follow=argv, ++argv_follow; *argv_follow; ++argv_follow )
		{
		if ( ( **argv_follow == '-' ) || ( **argv_follow == '/' ) )
			{
			char ch;

			(*argv_follow)++;
			switch ( ch = *(*argv_follow)++ )
				{
				case 'm':
				case 'M':
					multiLayer = TRUE;
					break;
				case 'c':
				case 'C':
					clipSave = TRUE;
					break;
#if 0
				case 'I':
				case 'i':
					interactiveMode = TRUE;
					break;
#endif
				case 'x':
				case 'X':
					sscanf( *argv_follow, "%d", &defaultXSize );
					break;
				case 'y':
				case 'Y':
					sscanf( *argv_follow, "%d", &defaultYSize );
					break;
				default:
					Usage();
					printf( "Invalid switch %c\n", ch );
					exit( 0 );
				}
			}
		else
			{ // Parse filename stuff
			char *szExt;
			FLAG fBroken = FALSE;

			if ( szExt = strrchr( *argv_follow, '.' ) )
				{
				if ( stricmp( szExt, ".map" ) == 0 )
					{
					char szBufferName[128];

					// not right -- not &tilesets[a] -- deal with later...
					NewMap( *argv_follow, &maps[num_of_maps*2], defaultXSize, defaultYSize, NULL, FALSE );
					sprintf( szBufferName, "%s.Spare", maps[num_of_maps*2].szFilename );
					NewMap( szBufferName, &maps[num_of_maps*2+1], defaultXSize, defaultYSize, NULL, TRUE );

					// Initialize other files to be same as previous version
					if ( num_of_maps )
						{
						maps[num_of_maps*2].tileset = maps[(num_of_maps-1)*2].tileset;
						}

					++num_of_maps;
					}
				else if ( stricmp( szExt, ".chr" ) == 0 )
					{
					InitChars( *argv_follow, &tilesets[num_of_maps-1] );
					maps[(num_of_maps-1)*2].tileset =
						maps[(num_of_maps-1)*2+1].tileset = &tilesets[num_of_chars];
					++num_of_chars;
					}
				else if ( stricmp( szExt, ".pal" ) == 0 )
					{
					InitPalette( *argv_follow, &(tilesets[num_of_maps-1].palette) );
//					strcpy( palfilename[num_of_maps-1], *argv_follow );
					++num_of_pals;
					}
				else
					fBroken = TRUE;
				}
			else
				fBroken = TRUE;

			if ( fBroken )
				{ // "What should I do?"
				printf( "Unrecognized filetype %s\n", *argv_follow );
				}
			}
		}

	ProjectStats();

	num_of_maps = (num_of_maps*2)-1;

	AdjustUndoBuffer();

   InitText( font6 );

   ShowMouse();
   SetMouseBounds( 0, 639, 0, 199 );
   SetMouse( 320, 100 );

   firstgadget = &MapBoxGadget;
   firstkeygadget = &ShowBrushGadget;
	}

//=============================================================================

void
SetCharSet( TILESET *ts )
{
	chardata = ts->chardata;
	chars = ts->chars;
	charpaloff = ts->palette.nOffset;
}

//=============================================================================

void
InitChars( char *name, TILESET *ts )
{
   FILE *fp;
   register uint a,b;
   uint chars2;
   uchar tempchar[32];
	int i,p, l;
	char *ptr;

   fp = fopen (name,"rb");
   if (fp==NULL) Error ("Cannot open chars file...");
   fseek (fp,0,2);                             // get fsize
   chars =  ftell (fp) / 32;
   chars2 = chars*2;
   fseek (fp,0,0);                            // back to beginning
   if (chars>MAXCHARS) Error ("Too many chars!");

   ts->chardata = SafeFarMalloc ((ulong) 64L * chars * 4L);
	ts->chars = chars;
	SetCharSet( ts );

   printf ( "\nReading characters (%s)...\n", name );
   for (a=0;a<chars;a++)
		{

#if defined(GENESIS)
      fread (tempchar,1,32,fp);   // load char
      for (b=0;b<32;b++)
      	{
         chardata[(a<<6)+(b<<1)+1]=tempchar[b] & 15;
         chardata[(a<<6)+(b<<1)  ]=tempchar[b] >> 4;
         }
#endif

#if defined(SNES)

		fread (tempchar,1,32,fp);   // load char

	   for (p=0; p<8; ++p)
			{
			for (i=0; i<8; ++i )
				{
				int shifter = 1<<(7-i);

				chardata[(a<<6)+(p<<3)+i] =
					( ( (tempchar[p*2+0] & shifter) << i ) >> (7) ) |
					( ( (tempchar[p*2+1] & shifter) << i ) >> (6) ) |
					( ( (tempchar[p*2+16] & shifter) << i ) >> (5) ) |
					( ( (tempchar[p*2+17] & shifter) << i ) >> (4) )
					;
				}
			}
#endif
		}

	fclose (fp);
   printf ("Flipping X...\n");
   for (;a<chars2;a++)
   	FlipCharX (&chardata[(ulong) (a-chars)<<6],&chardata[(ulong) a<<6]);
   printf ("Flipping Y...\n");
   for (;a<chars*4;a++)
   	FlipCharY (&chardata[(ulong) (a-chars2)<<6],&chardata[(ulong) a<<6]);
   selected.xSize = 1;
   selected.ySize = 1;
	for (a=0;a<MAXSELECTY*MAXSELECTX;a++)
			*(selected.buffer + b) = 0;
	ts->szFilename = name;
}

//=============================================================================

void
NewMap(char *filename,MAP *mapPtr,uint defaultX, uint defaultY, TILESET *ts, FLAG fSpare )
{
	uint x,y,a,b;
	int temp,temp2;
	FILE *fp;

	if ((fp=fopen (filename,"rb"))!=NULL)
		{
		fread( &x, sizeof(int), 1, fp );
#if defined(GENESIS)
		x = swaphilo( x );
#endif

		fread( &y, sizeof(int), 1, fp );
#if defined(GENESIS)
		y = swaphilo( y );
#endif

// kts 5/22/92 why do this?
#if 0
		mapPtr->xSize = max( defaultX, x );
		mapPtr->ySize = max( defaultY, y );
#endif
		mapPtr->xSize = x;
		mapPtr->ySize = y;


		mapPtr->xOffset = 0;
		mapPtr->yOffset = 0;
		mapPtr->buffer = SafeFarMalloc(mapPtr->xSize*mapPtr->ySize*sizeof(int));
		if(!mapPtr->buffer)
			Error("Cannot allocate memory for map");

        for (a=0;a<y;a++)
        	for (b=0;b<x;b++)
			 {
				temp = fgetc(fp);
				if(temp == EOF)
					Error("Read fault in map");
				temp2 = fgetc(fp);
				if(temp2 == EOF)
					Error("Read fault in map");
#if defined(GENESIS)
				*(mapPtr->buffer + (a*mapPtr->xSize) + b) = (temp<<8)|temp2;
#endif

#if defined(SNES)
				*(mapPtr->buffer + (a*mapPtr->xSize) + b) = (temp2<<8)|temp;
#endif
			 }
        fclose (fp);
     }
	else
		{
		mapPtr->xSize = defaultX;
		mapPtr->ySize = defaultY;
		mapPtr->buffer = SafeFarMalloc(mapPtr->xSize*mapPtr->ySize*sizeof(int));
		if ( !mapPtr->buffer )
			Error( "Cannot allocate memory for map" );
		}

	mapPtr->tileset = ts;
	mapPtr->fSpare = fSpare;
	mapPtr->szFilename = strdup( filename );

	CalcUsageCount( mapPtr );
}

//=============================================================================


