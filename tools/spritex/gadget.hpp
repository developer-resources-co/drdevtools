/*
 * gadget.h -- gadget structure definitions.
 * (c) 1991, Developer Resources
 * Written By Scott Statton
 *
 */

struct _gadget {
	struct _gadget *next;
	struct _gadget *prev;
	short offsetX;
	short offsetY;
	word sizeX;
	word sizeY;
	word identity;
	byte type;
	void far *dataPtr1;
	void far *dataPtr2;
	void (*routine)(struct _gadget *, struct _input *);
	void far *dataPtr3;
	};

struct _string {
	char *text;
	unsigned int cursorX;
	unsigned int xSize;
	};




/***************************************************************************/

struct _multigadget {
	word offsetX;
	word offsetY;
	word sizeX;
	word sizeY;
	word identity;
	byte type;
	void (*routine)(struct _gadget *, struct _input *);
	};

/***************************************************************************/

/*
 * prototypes from file GADGET.C
 */

void
CreateMultipleGadgets(struct _multigadget mgPtr[], int n);

void
KillGadget(struct _gadget *gPtr);

struct _gadget *
FindGadget(struct _gadget *,word identity);

struct _gadget *
FindTail(struct _gadget *gPtr);

struct _gadget *
CreateGadget(struct _gadget *, word x, word y, word sizeX, word sizeY, byte type, word identity,
				void far *dp1, void far *dp2, void far *dp3, void (*routine)(struct _gadget *,struct _input *));

void
InitGadgets(void);

void
NullRoutine(struct _gadget *, struct _input *);

boolean
DoHitGadget(struct _gadget *, struct _input *);

extern struct _gadget gadgetBase;

boolean
StrGadgInput(struct _string *,struct _input *);

void
KillMultipleGadgets(struct _gadget *);
