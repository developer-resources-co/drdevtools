//==============================================================================
// kts attemt to make this useabe
//==============================================================================

#include <stdio.h>

// what is lmalloc?
// oh well, I'll just try this

#define lmalloc malloc

#define FLAG unsigned char
#define TRUE 1
#define FALSE 0


FLAG cont = TRUE;				// global continue flag

/*************************************************************************
** New Improved CONVERT routine.                                        **
** Takes *.MID file from Dr. T's and create Z80 source file.            **
*************************************************************************/

typedef unsigned char byte;
typedef	unsigned short word;

//#include "voltable.i"


static word history[128];



/* values for event structure flags */

#define 	IN_USE				0x01
#define		OVER_TIME			0x02
#define 	COMPLETED			0x04
#define 	COMMAND				0x08

//==============================================================================
/* values for event codes */
						/* bit 2= "command/note" bit 1,0 = "opcode " */

#define		NOTE_ON				2
#define		NOTE_OFF			3
#define		PROG_CHANGE 		4
#define		PITCH_BEND			5
#define		EXTEND_COMMAND		6
#define		TEMPO				7

//------------------------------------------------------------------------------

struct EVENT {
	byte	channel;
	byte	keycode;
	byte	velocity;
	byte	flags;
	long	delay;
	word	duration;
	word	param;
	long	start_time;
	};

extern char * lmalloc();

byte Frequent();

static byte command,old_command;
static byte ch;

static char *notename[] = {
		"NOTE_C",
		"NOTE_CS",
		"NOTE_D",
		"NOTE_DS",
		"NOTE_E",
		"NOTE_F",
		"NOTE_FS",
		"NOTE_G",
		"NOTE_GS",
		"NOTE_A",
		"NOTE_AS",
		"NOTE_B" };

char buff[1000];

//==============================================================================

void
Borland(void)
{
	int	i;
	for ( i = 0 ; i < 1000 ; i++ );
	return;
}



char
safefgetc(FILE *handle)
{
	if(feof(handle))
	 {
		cont = FALSE;
		return(0);
	 }
	else
		return(fgetc(handle));
}

//==============================================================================

char frameTbl[] = "/-\\|";

void
SpinCursor(int divisor)
{
	static int curs = 0;
	curs++;
	putchar(13);
	putchar(frameTbl[(curs/divisor) &3]);
}

/* Read Variable Number.  Eats up to four bytes */

long
ReadVarNum(FILE *f)
{
	long	i=0;	/* value to eat */
	unsigned char	c;		/* temp */
	int		j;

	for ( j = 0 ; j < 4 ; j++ )
	{
		i <<= 7;
		c = safefgetc(f);
		i |= ( c & 0x7f );

		if (!( c & 0x80 ))
			return i;

	}
	return i;
}

long
ReadLong(FILE *f)
{
	long	i=0;
	int		j;

	for ( j = 0 ; j < 4 ; j++ )
	{
		i <<=8;
		i |= safefgetc(f);
	}
	return i;
}


void
SkipChunk(FILE *f)
{
	long	i = 0;

	long	j;

	for ( j = 0 ; j < 4 ; j++ )
		safefgetc(f);

	i = ReadLong(f);

	fseek(f, i, 1);
	return;
}

//==============================================================================

void
main( argc, argv )
int	argc;
char	**argv;
{


	FILE	*infile,*outfile;

	long estim_events;	/* estimated number of events based on
				   file size */

	long blocksize;	/* block size allocated for buffer based
				   on estim_events */

	long	timeinc;	/* time increment from each event */

	int	evtype;		/* event type */

	register struct	EVENT *current_event,*events;	/* pointer to base of event array */

	struct	EVENT tmpevent; /* place to store temporary event */

	long	filelen;	/* length of input file */
	long	last_event,this_event;	/* wall clock variables */

	int	num_events;	/* actual number of events */
	int highwater;  /* how many events to allow before aborting */
	int	foo,i,j;

	word	defaultVelocity;

	int	fileLength;

	for ( i = 0 ; i < 128 ; history[i++] = 0);

	num_events =  0;

	last_event = this_event = 0L;

	putchar ( '\n' );

	if ( argc < 2 ) {
		printf("convert:  Usage: convert <infile> [<outfile>]\n");
		exit(0);
		}

	if ( argc < 3 )
		outfile = stdout;

	infile = fopen ( argv[1] , "rb" );

	if ( !infile ) {
		fprintf(stderr,"convert:  Couldn't open input file %s.\n",argv[1]);
		exit(1);
		}

	fseek ( infile, 0L, 2 );	/* go to end of file */
	filelen = ftell ( infile );	/* get length of file */

	fprintf(stderr,"convert: File %s is %ld bytes long.\n",argv[1],filelen);

	fseek( infile, 0L, 0);		/* go to beginning of file */
	SkipChunk(infile);			/* skip the MThd */

	fseek( infile, 8L, 1 );		/* skip the header & the length */
	ReadVarNum(infile);			/* eat the superfluous Delta-time */

	estim_events = ( filelen - 23L ) / 3;

	highwater = estim_events - 2;

	fprintf(stderr,"convert: Estimated number of events: %ld\n",estim_events);


	blocksize = (long) estim_events * (long) sizeof(struct EVENT);

	events = (struct EVENT *) lmalloc ( blocksize );

	if ( !events ) {
		fprintf(stderr,"convert:  Couldn't MALLOC %d bytes",blocksize);
		exit(20);
		}


	/* by this point we have the input file open and are ready to
	   start filling the events structure;  */

	current_event = events;

	for ( ; ; )
	 {
		evtype = GetEvent ( infile, &tmpevent );
		if(!cont)
			break;
		if ((evtype == EXTEND_COMMAND) && (tmpevent.keycode == 0x2f)) break;

		SpinCursor(50);					// spin every 50 events
		timeinc = tmpevent.delay;

#ifdef DEBUG
		switch(evtype) {
			case NOTE_ON:
				fprintf(stderr,"NoteOn   ");
				break;
			case NOTE_OFF:
				fprintf(stderr,"NoteOff  ");
				break;
			case PROG_CHANGE:
				fprintf(stderr,"ProgChg  ");
				break;
			case PITCH_BEND:
				fprintf(stderr,"PitchBnd ");
				break;
			case EXTEND_COMMAND:
				fprintf(stderr,"Extend   ");
				break;
			case TEMPO:
				fprintf(stderr,"Tempo    ");
				break;
			}

		fprintf(stderr,"Channel: %1d  Key:%3d  Velo:%3d ",
			tmpevent.channel,tmpevent.keycode,tmpevent.velocity);
		fprintf(stderr,"Delay:%3d Event#:%5d\n",
			tmpevent.delay,	num_events);
#endif
		if ((evtype == NOTE_ON) && (tmpevent.velocity == 0)) evtype = NOTE_OFF;

		if ( num_events == 341 ) Borland();

		if ( num_events >= highwater) {
			fprintf(stderr,"convert: Number of events exceeds estimated number.\n");
			exit(20);
		}

		switch ( evtype ) {

			case NOTE_ON:
				TurnOn( current_event , &tmpevent, this_event  );
				current_event +=1;
				num_events++;
				break;

			case NOTE_OFF:
				TurnOff( events, &tmpevent, num_events, this_event);
				events[num_events-1].delay += timeinc;
				break;

			case PROG_CHANGE:
			case PITCH_BEND:
			case TEMPO:
				ComEvent ( current_event, &tmpevent, this_event);
				events[num_events-1].delay += timeinc;
				current_event +=1;
				num_events++;
				break;

			case EXTEND_COMMAND:
				events[num_events-1].delay += timeinc;
				break;
			}

		this_event += timeinc;

		}

	fclose ( infile );
	fprintf(stderr,"convert: Actual number of events: %d\n",num_events+1);

	defaultVelocity = Frequent();

	if ( argc > 2 ) {	/* declared an output file */
		outfile = fopen ( argv[2] , "w" );
		if ( !outfile ) {
			fprintf(stderr,"convert:  Couldn't open output file %s.\n",
				argv[2]);
			exit(1);
			}
		}

	fileLength = 0;

	fprintf(outfile,"; %s\n ",argv[1]);

	//fprintf(outfile, "\tDB\tMUS_VELOCITY,%d\n",volTable[defaultVelocity]);
	fprintf(outfile, "\tDB\tMUS_VELOCITY,%d\n",defaultVelocity);

	fileLength += 2;

	for ( i = 0 ; i < num_events ; i++ ) {
		foo = events[i].flags;
		if ( foo & COMMAND )
			fileLength += WriteCommandEvent ( outfile, &events[i], i+1 );
		  else
			if ( events[i].velocity == defaultVelocity )
				fileLength += WriteNoteOnEvent ( outfile, &events[i],i+1);
			  else
				fileLength += WriteVeloOnEvent ( outfile, &events[i], i+1 );
		}

	free(events);
	fclose ( outfile );
	fprintf(stderr,"convert: File write complete.\n");
	fprintf(stderr,"convert: %d bytes of executable sound data.\n",fileLength);

	exit(0);
}

//==============================================================================



int
GetEvent ( fp , ep )
FILE *fp;
struct EVENT *ep;
{
	byte	foo,hibyte,midbyte,lobyte;
	byte	retvalue;

	foo = safefgetc ( fp );
	if ( foo & 0x80 )
		ch  = foo & 0xf;
	switch ( foo & 0xf0 ) {

		case 0x80:
			command = old_command = NOTE_OFF;
			foo = safefgetc(fp);
			break;

		case 0x90:
			command = old_command = NOTE_ON;
			foo = safefgetc(fp);
			break;

		case 0xc0:
			command = old_command = PROG_CHANGE;
			break;

		case 0xe0:
			command = old_command = PITCH_BEND;
			foo = safefgetc(fp);
			break;

		case 0xf0:
			old_command = command;
			command = EXTEND_COMMAND;
			break;

		case 0xa0:
		case 0xb0:
			fprintf(stderr,"\7\7\7panic:  I've got a command %x at position %ld\n",
				foo,ftell(fp));
			exit(1);

		}

	switch ( command ) {

		case NOTE_ON:
		case NOTE_OFF:
			ep->channel = ch;
			ep->keycode = foo;
			ep->velocity = safefgetc(fp);
			ep->delay = (ReadVarNum(fp)/8);
			break;

		case PROG_CHANGE:
			ep->channel = ch;
			ep->keycode = command;
			ep->param = safefgetc(fp);
			ep->delay = (ReadVarNum(fp)/8);
			break;

		case PITCH_BEND:
			ep->channel = ch;
			ep->keycode = command;
			lobyte = foo;
			hibyte = safefgetc(fp);
			ep->param = hibyte * 128 + lobyte;
			ep->delay = (ReadVarNum(fp)/8);
			break;

		case EXTEND_COMMAND:
			foo = safefgetc(fp);
			ep->keycode = foo;
			ep->param = extnd ( fp, foo);
			ep->delay = (ReadVarNum(fp)/8);
			if ( ep->keycode == 0x51 )
				command = TEMPO;
			break;
		}


	retvalue = command;
	command = old_command;
	return ( retvalue );
}

//==============================================================================

extnd ( fp, foo )
FILE *fp;
byte foo;
{
	byte	lobyte,midbyte,hibyte;

	int	length;
	word rv;
	long divisor;

	switch ( foo ) {

		case 0x51:		/* new tempo */
			length = safefgetc(fp);
			hibyte = safefgetc(fp);
			midbyte= safefgetc(fp);
			lobyte = safefgetc(fp);

			divisor = hibyte * 65536 + midbyte*256 + lobyte;
			rv = (word)( 60000000L / divisor);
			break;

		case 0x2f:		/* end of song */
			cont = FALSE;
			rv = 0;
			break;

		default:
			length = safefgetc(fp);
			for ( ; length ; length-- ) safefgetc(fp); /* throw this shit away */
			rv = 0;
			break;

		}
	return (rv);
}

//==============================================================================

/* 	DB	MUS_NOTE+MUS_CH0,1,NOTE_D1,255 */

int WriteVeloOnEvent ( fp , ep, evnum )
FILE *fp;
struct EVENT *ep;
int evnum;
{
	byte	foo;
	byte	octave,step;
	char	*s;
	int		eventLength;
	unsigned char longFlag = 0;

	octave = ep->keycode / 12;
	step   = ep->keycode % 12;

	octave -= 3;


	if ( ep->duration > 255 ) {

		fprintf(fp,"\tDB\tMUS_LONGVNOTE+MUS_CH");
		longFlag = 1;
		eventLength = 6;
		}

	  else	{

		fprintf(fp,"\tDB\tMUS_VNOTE+MUS_CH");
		eventLength = 5;
		}



	fprintf(fp,"%1x",ep->channel);

	if ( longFlag )
			fprintf(fp,",%d,%d",(ep->duration)&0xff,
				((ep->duration & 0xff00) >> 8));
		else
			fprintf(fp,",%d",ep->duration);

	fprintf(fp,",%d",ep->velocity);

	s = notename[step];

	fprintf(fp,",%s%1d",s,octave);

	fprintf(fp,",%d",ep->delay);
	if ( ep->delay > 255 )
		fprintf(stderr,"note %d exceeds 255 delay\n",evnum);

	fprintf(fp,"\t; %d\n",evnum);

	return eventLength;
}

//==============================================================================

int WriteNoteOnEvent ( fp , ep, evnum )
FILE *fp;
struct EVENT *ep;
int evnum;
{
	byte	foo;
	byte	octave,step;
	char	*s;
	int		eventLength;
	char 	longFlag = 0;

	octave = ep->keycode / 12;
	step   = ep->keycode % 12;

	octave -= 3;


	if ( ep->duration > 255 ) {

		fprintf(fp,"\tDB\tMUS_LONGNOTE+MUS_CH");
		longFlag = 1;
		eventLength = 5;
		}

	  else	{

		fprintf(fp,"\tDB\tMUS_NOTE+MUS_CH");
		eventLength = 4;
		}


	fprintf(fp,"%1x",ep->channel);

	if ( longFlag )
			fprintf(fp,",%d,%d",(ep->duration)&0xff,
				((ep->duration & 0xff00) >> 8));
		else
			fprintf(fp,",%d",ep->duration);

	s = notename[step];

	fprintf(fp,",%s%1d",s,octave);

	fprintf(fp,",%d",ep->delay);
	if ( ep->delay > 255 )
		fprintf(stderr,"note %d exceeds 255 delay\n",evnum);

	fprintf(fp,"\t; %d\n",evnum);

	return eventLength;
}

//==============================================================================

int WriteCommandEvent ( fp, ep, evnum )
FILE *fp;
struct EVENT *ep;
int evnum;

{

	int eventLength;

	switch	( ep->keycode ) {

		case PROG_CHANGE:
			fprintf(fp,"\tDB\tMUS_PCHANGE+MUS_CH%1x,%d\t; %d\n",
				ep->channel,ep->param,evnum);
			eventLength = 2;
			break;

		case PITCH_BEND:
		/*
			fprintf(fp,"\tDB\tMUS_PBEND+MUS_CH%1x,%d\t; %d\n",
				ep->channel,((ep->param)>>6)-128,evnum);
		*/
			eventLength = 2;
			break;

		case 0x51:
			fprintf(fp,"\tDB\tMUS_TEMPO,%d\t\t; %d\n",
				ep->param,evnum);
			eventLength = 2;
			break;
		default:
			fprintf(fp,"\t;  Unimplemented Command. Event # %d\n"
				,evnum);

			eventLength = 0;
		}

		if ( ep->delay > 0 ) {
			fprintf(fp,"\tDB\tMUS_NOP,%d\n",ep->delay);
			if ( ep->delay > 255 )
				fprintf(stderr,"note %d exceeds 255 delay\n",evnum);
			eventLength += 2;
			}

	return eventLength;
}

//==============================================================================

TurnOn ( dest, source, tick )
struct EVENT *dest,*source;
long tick;
{
#ifndef FUCKED
	dest->velocity = source->velocity;
	history[source->velocity]++;
	dest->channel = source->channel;
	dest->keycode = source->keycode;
	dest->delay   = source->delay;
	dest->duration = 0;
	dest->start_time = tick;
	dest->flags = IN_USE;
#endif
	return;
}

//==============================================================================

ComEvent ( dest, source, tick )
struct EVENT *dest,*source;
long tick;
{
#ifndef FUCKED
	dest->channel = source->channel;
	dest->keycode = source->keycode;
	dest->delay   = source->delay;
	dest->param   = source->param;
	dest->flags = COMMAND;
#endif
	return;
}

//==============================================================================

TurnOff ( base, tmp , i , tick )
struct EVENT *base,*tmp;
int i;
long tick;
{
	register struct EVENT *ep;
#ifndef FUCKED
	for ( ; i ; i-- ) {
		ep = base + i - 1;
		if ( (ep->channel == tmp->channel) &&
		  (ep->keycode == tmp->keycode) &&
		  (ep->flags & IN_USE ) ) {
			ep->flags &= ~IN_USE;
			ep->flags |= COMPLETED;
			ep->duration = tick - ep->start_time;
			break;
			}
		}
#endif
	return;
}

//==============================================================================

byte Frequent()
{
	int	i,j;
	byte foo;

	j = 0;
	foo = 0;
	for ( i = 0 ; i < 128 ; i ++ )
		if ( history[i] > j ) { foo = i; j = history[i]; }
	return(foo);
}

//==============================================================================
