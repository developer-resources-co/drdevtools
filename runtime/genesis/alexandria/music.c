
/* --------------------------------------------------------------------------

                       Music Driver -- Ver 1.0
                     ---------------------------

	Written by:	Ray Ewan
				Alexandria, Inc.

	Date:		April 12, 1993


Modifications:



Overview:

		This module will provide context-sensitive music and sound effects.

		MusicInit(const char *, const char *, const char *, const char *);

		MusicSetBackgroundSeq(const TMusicBackSeq *);
		MusicPlayBackgroundSeq(void);
		MusicStopBackgroundSeq(void);

		MusicSetInterruptSeq(const TMusicIntSeq *);
		MusicPlayInterruptSeq(void);
		MusicStopInterruptSeq(void);

		MusicPlayOverdubSeq(short);
		MusicStopOverdubSeq(void);

		MusicPauseAll(void);
		MusicContinueAll(void);

-------------------------------------------------------------------------- */

/* -----  include files  ----- */

#include <stdlib.h>
#include <gentypes.h>

#include "gems.h"
#include "music.h"


PRIVATE	const TMusicBackSeq	*backgroundMusic;

PUBLIC	const TMusicIntSeq	*interruptMusic;

PRIVATE	short	nBackgroundSeq,		/* current background sequence number */
				nInterruptSeq,		/* current interrupt sequence number  */
				nInterruptSeqLoc,	/* interrupt sequence array location */
				nOverdubSeq;		/* current overdub sequence number    */

PRIVATE	short	backgroundPaused;	/* TRUE if background music paused */


extern	char	patchbank[],		/* patch data */
				envbank[],			/* envelope data */
				seqbank[],			/* sequence data */
				sampbank[];			/* digitized sample data */






/* --------------------------------------------------------------------------

Function:		MusicInit

Scope:			PUBLIC

Usage:			(void) MusicInit(patchbank, envbank, seqbank, sampbank)
				char	*patchbank;		- patch data
				char	*envbank;		- envelope data
				char	*seqbank;		- sequence data
				char	*sampbank;		- digitized sample data

Returns:		Nothing

Description:	Called once at the start of the game program to prepare
				all music related routines

-------------------------------------------------------------------------- */

PUBLIC void
MusicInit(const char *patchbank, const char *envbank, const char *seqbank, const char *sampbank)
{
#if defined( GEMS )
	gemsinit(patchbank, envbank, seqbank, sampbank);

	nBackgroundSeq = nInterruptSeq = nInterruptSeqLoc = nOverdubSeq = 0;
	interruptMusic = (void *) 0;
#endif
}





/* --------------------------------------------------------------------------

Function:		MusicSetBackgroundSeq

Scope:			PUBLIC

Usage:			(void) MusicSetBackgroundSeq(oBackMusic)
				TMusicBackSeq	*oBackMusic;		- sequence to play

Returns:		Nothing

Description:	Tells music driver which background sequence to play

-------------------------------------------------------------------------- */

PUBLIC void
MusicSetBackgroundSeq(const TMusicBackSeq *oBackMusic)
{
#if defined( GEMS )
	backgroundMusic = oBackMusic;
#endif
}





/* --------------------------------------------------------------------------

Function:		MusicPlayBackgroundSeq

Scope:			PUBLIC

Usage:			(void) MusicPlayBackgroundSeq()

Returns:		Nothing

Description:	Starts Gems playing the specified background music

-------------------------------------------------------------------------- */

PUBLIC void
MusicPlayBackgroundSeq()
{
#if defined( GEMS )
	gemsstopall();					/* stop all sequences */
	gemsstartsong(backgroundMusic->nSeqNo);
#endif
}





/* --------------------------------------------------------------------------

Function:		MusicStopBackgroundSeq

Scope:			PUBLIC

Usage:			(void) MusicStopBackgroundSeq(void)

Returns:		Nothing

Description:	Stops Gems from playing the background music

-------------------------------------------------------------------------- */

PUBLIC void
MusicStopBackgroundSeq()
{
#if defined( GEMS )
	gemsstopall();
#endif
}





/* --------------------------------------------------------------------------

Function:		MusicSetInterruptSeq

Scope:			PUBLIC

Usage:			(void) MusicSetInterruptSeq(oIntMusic)
				TMusicIntSeq	*oIntMusic;		- sequence to play

Returns:		Nothing

Description:	Tells music driver which interrupt sequence to play

-------------------------------------------------------------------------- */

PUBLIC void
MusicSetInterruptSeq(const TMusicIntSeq *oIntMusic)
{
#if defined( GEMS )
	if (!backgroundPaused) {
		gemspauseall();
		backgroundPaused = TRUE;
	}

	interruptMusic = oIntMusic;
	nInterruptSeqLoc = 0;
#endif
}





/* --------------------------------------------------------------------------

Function:		MusicPlayInterruptSeq

Scope:			PUBLIC

Usage:			(void) MusicPlayInterruptSeq()

Returns:		Nothing

Description:	Suspends current background music and plays specified
				sequence

-------------------------------------------------------------------------- */

PUBLIC void
MusicPlayInterruptSeq()
{
#if defined( GEMS )
	gemsstorembox(MUSIC_EndIntSeqMBox,0);

	gemsstartsong(nInterruptSeq = (interruptMusic + nInterruptSeqLoc)->nSeqNo);

	nInterruptSeqLoc = (interruptMusic + nInterruptSeqLoc)->nNextSeqNo;
#endif
}





/* --------------------------------------------------------------------------

Function:		MusicStopInterruptSeq

Scope:			PUBLIC

Usage:			(void) MusicStopInterruptSeq()

Returns:		Nothing

Description:	Suspends current interrupt music and resumes playing

				sequence

-------------------------------------------------------------------------- */

PUBLIC void
MusicStopInterruptSeq()
{
#if defined( GEMS )
	short	tempo;

	gemsstopsong(nInterruptSeq);
	interruptMusic = (void *) 0;

	if (backgroundPaused) {
		tempo = gemsreadmbox(MUSIC_BGTempoMBox) + MUSIC_TempoAdjust;
		gemsresumeall();
		gemssettempo(tempo);
		backgroundPaused = FALSE;
	}
#endif
}





/* --------------------------------------------------------------------------

Function:		MusicPlayOverdubSeq

Scope:			PUBLIC

Usage:			(void) MusicPlayOverdubSeq(short nSeqNo)

Returns:		Nothing

Description:	Plays specified sequence without suspending background
				sequence

-------------------------------------------------------------------------- */

PUBLIC void
MusicPlayOverdubSeq(short nSeqNo)
{
#if defined( GEMS )
	gemsstartsong(nOverdubSeq = nSeqNo);
#endif
}





/* --------------------------------------------------------------------------

Function:		MusicStopOverdubSeq

Scope:			PUBLIC

Usage:			(void) MusicStopOverdubSeq()

Returns:		Nothing

Description:	Stops playing current overdub sequence

-------------------------------------------------------------------------- */

PUBLIC void
MusicStopOverdubSeq()
{
#if defined( GEMS )
	gemsstopsong(nOverdubSeq);
#endif
}





/* --------------------------------------------------------------------------

Function:		MusicPauseAll

Scope:			PUBLIC

Usage:			(void) MusicPauseAll()

Returns:		Nothing

Description:	Temporarily suspends all music

-------------------------------------------------------------------------- */

PUBLIC void
MusicPauseAll()
{
#if defined( GEMS )
	gemspauseall();
#endif
}





/* --------------------------------------------------------------------------

Function:		MusicResumeAll

Scope:			PUBLIC

Usage:			(void) MusicResumeAll()

Returns:		Nothing

Description:	Resumes playing all suspended music

-------------------------------------------------------------------------- */

PUBLIC void
MusicResumeAll()
{
#if defined( GEMS )
	gemsresumeall();
#endif
}
