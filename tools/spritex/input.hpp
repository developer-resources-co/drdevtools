//=============================================================================
//   input.hpp: handle i/o
//=============================================================================

#define	CLIPBOARDSIZE 160

#define INP_NONE 0
#define INP_KEY 1
#define INP_MOUSEMOVE 2
#define INP_MOUSE_LEFTBUTTON_DOWN 3
#define INP_MOUSE_LEFTBUTTON_UP 4
#define INP_MOUSE_MIDDLEBUTTON_DOWN 5
#define INP_MOUSE_MIDDLEBUTTON_UP 6
#define INP_MOUSE_RIGHTBUTTON_DOWN 7
#define INP_MOUSE_RIGHTBUTTON_UP 8
#define INP_RESIZE 9
#define INP_COMMAND 10
#define INP_MOUSE_LEFTBUTTON_SHIFTDOWN 11
#define INP_MOUSE_RIGHTBUTTON_SHIFTDOWN 12

#define MOUSEB_BLEFT BUTTON_LEFT
#define MOUSEF_BLEFT 1<<MOUSEB_BLEFT

#define MOUSEB_BRIGHT BUTTON_RIGHT
#define MOUSEF_BRIGHT 1<<MOUSEB_BRIGHT

//=============================================================================
// input enables

#define INPB_KEY 0
#define INPF_KEY 1<<INPB_KEY

#define INPB_MOUSEMOVE 1
#define INPF_MOUSEMOVE 1<<INPB_MOUSEMOVE

#define INPB_MOUSEBUTTONS 2
#define INPF_MOUSEBUTTONS 1<<INPB_MOUSEBUTTONS

#define INPB_CASESENSE 3
#define INPF_CASESENSE 1<<INPB_CASESENSE

//=============================================================================

//#define INP_ENAB_KEY 1
//#define INP_ENAB_MOUSEMOVE 2
//#define INP_ENAB_MOUSEMOVEBDOWN 4
//#define INP_ENAB_MOUSELDONW 8
//#define INP_ENAB_MOUSELUP 16
//#define INP_ENAB_MOUSERDOWN 32
//#define INP_ENAB_MOUSERUP 64

//=============================================================================

struct _input
{
	int inputType;
	unsigned int fullKey;
	short mouseX,mouseY,mouseButtons;
	short oldMouseX,oldMouseY,oldMouseButtons;
	int inputMask;
};

//=============================================================================

extern struct _input inBase;
extern boolean mouseBEnable;
extern char copyBuffer[CLIPBOARDSIZE];					// buffer for cut/copy/paste

extern char strString[];

//=============================================================================

void
ErasePointer(void);

void
DrawPointer(unsigned char far *screen,int mouseX, int mouseY);

void
ActivateFrontWindow(void);

void
UpdateCursor(void);

unsigned char
GetShiftStatus(void);

unsigned int
GetDirectKey(void);

boolean
InputPending(struct _input *in);

void
InitMouse(void);

void
InitInput(void);

void
TurnOffMouseMove(void);

void
TurnOnMouseMove(void);
//=============================================================================
