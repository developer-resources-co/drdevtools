/* --------------------------------------------------------------------------

                       Music Driver -- Ver 1.0
                     ---------------------------

	Written by:	Ray Ewan
				Alexandria, Inc.

	Date:		April 12, 1993


Modifications:



Overview:

		This module will provide context-sensitive music and sound effects.

-------------------------------------------------------------------------- */

#ifndef	_MUSIC_H_
#define	_MUSIC_H_

#include <alexdef.h>
#include <gentypes.h>


#define	MUSIC_TempoAdjust	40		/* tempos store in mailbox are tempo - 40 */

#define	MUSIC_BGTempoMBox	1		/* mailbox where background music tempo is stored */
#define	MUSIC_IntTempoBox	2		/* mailbox where interrupt music tempo is stored */
#define	MUSIC_ODTempoBox	3		/* mailbox where overdub music tempo is stored */

#define	MUSIC_EndIntSeqMBox	5		/* mailbox where end of interrupt sequence flag is stored */

#define	MUSIC_ODLoopClear	500		/* this value indicates that there is */
									/* currently no looping overdub music/sound */
									/* effect playing */

typedef struct _TMusicBackSeq {
	short	nSeqNo;
	short	nEntryPoints;
} TMusicBackSeq;


typedef struct _TMusicIntSeq {
	short	nSeqNo;
	short	nNextSeqNo;
} TMusicIntSeq;





PUBLIC	void	MusicInit(const char *, const char *, const char *, const char *);

PUBLIC	void	MusicSetBackgroundSeq(const TMusicBackSeq *);
PUBLIC	void	MusicPlayBackgroundSeq(void);
PUBLIC	void	MusicStopBackgroundSeq(void);

PUBLIC	void	MusicSetInterruptSeq(const TMusicIntSeq *);
PUBLIC	void	MusicPlayInterruptSeq(void);
PUBLIC	void	MusicStopInterruptSeq(void);

PUBLIC	void	MusicPlayOverdubSeq(short);
PUBLIC	void	MusicStopOverdubSeq(void);

PUBLIC	void	MusicPauseAll(void);
PUBLIC	void	MusicResumeAll(void);

#endif

