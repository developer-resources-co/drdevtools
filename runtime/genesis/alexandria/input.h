/*============================================================================*/
/* input.h: interface to input.asm											  */
/*============================================================================*/

#ifndef _INPUT_H_
#define _INPUT_H_

#include <alexdef.h>

/*============================================================================*/

typedef struct Input
{
	WORD proportionalX;
	WORD proportionalY;
	UWORD booleans;
	WORD xSensitivity;
	WORD ySensitivity;

	long controllerRoutine;	/* pointer to controller routine to call */
	long controllerGeneric;	/* used by controller routine */
	UWORD oldBooleans;		/* last joystick input */
	UWORD edge;				/* bits that changed since last call */
	UWORD downEdge;			/* bits that have gone on since last call */
	UWORD upEdge;   		/* bits that have gone off since last call */
	WORD type;				/* type of controller */
} TInput;


#define InputBooleans(inputStruct) ((inputStruct)->booleans)
#define InputBooleansDownEdge(inputStruct) ((inputStruct)->downEdge)
#define InputBooleansUpEdge(inputStruct) ((inputStruct)->upEdge)
#define InputProportionalX(inputStruct) ((inputStruct)->proportionalX)
#define InputProportionalY(inputStruct) ((inputStruct)->proportionalY)
#define InputType(inputStruct) ((inputStruct->type))

enum
{
	INPUT_TYPENONE,
	INPUT_TYPEOLDJOY,
	INPUT_TYPEJOY,
	INPUT_TYPEMENACER,
	INPUT_TYPEACTIVATOR,
	INPUT_TYPEMOUSE,
	INPUT_TYPETRAKBALL,
	INPUT_TYPEMAX
};

enum
{
	INPUTBOOLB_A,
	INPUTBOOLB_B,
	INPUTBOOLB_C,
	INPUTBOOLB_D,
	INPUTBOOLB_E,
	INPUTBOOLB_F,
	INPUTBOOLB_G,
	INPUTBOOLB_H,
	INPUTBOOLB_I,
	INPUTBOOLB_J,
	INPUTBOOLB_K,
	INPUTBOOLB_PAUSE,
	INPUTBOOLB_UP,
	INPUTBOOLB_DOWN,
	INPUTBOOLB_LEFT,
	INPUTBOOLB_RIGHT
};

enum
{
INPUTBOOLF_A		=	1<<INPUTBOOLB_A,
INPUTBOOLF_B		=	1<<INPUTBOOLB_B,
INPUTBOOLF_C		=	1<<INPUTBOOLB_C,
INPUTBOOLF_D		=	1<<INPUTBOOLB_D,
INPUTBOOLF_E		=	1<<INPUTBOOLB_E,
INPUTBOOLF_F		=	1<<INPUTBOOLB_F,
INPUTBOOLF_G		=	1<<INPUTBOOLB_G,
INPUTBOOLF_H		=	1<<INPUTBOOLB_H,
INPUTBOOLF_I		=	1<<INPUTBOOLB_I,
INPUTBOOLF_J		=	1<<INPUTBOOLB_J,
INPUTBOOLF_K		=	1<<INPUTBOOLB_K,
INPUTBOOLF_PAUSE	=	1<<INPUTBOOLB_PAUSE,
INPUTBOOLF_UP		=	1<<INPUTBOOLB_UP,
INPUTBOOLF_DOWN		=	1<<INPUTBOOLB_DOWN,
INPUTBOOLF_LEFT		=	1<<INPUTBOOLB_LEFT,
INPUTBOOLF_RIGHT	=	1<<INPUTBOOLB_RIGHT
};

#define INPUTBOOLF_START	INPUTBOOLF_PAUSE

#define JOYCONTROLLER_1 0
#define JOYCONTROLLER_2 1

/*============================================================================*/

void InitInput(TInput *, UWORD port);

UWORD ReadInput(TInput *);

/*============================================================================*/

#endif

/*============================================================================*/
