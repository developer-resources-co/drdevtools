//============================================================================
//	charts.c: different reference charts
//============================================================================

#include	"base.hpp"
#include	"global.hpp"
#include	"general.hpp"
#include	"config.hpp"
#include	"screen.hpp"
#include	"list.hpp"
#include	"keys.hpp"
#include	"monkeys.hpp"
#include	"input.hpp"
#include	"gadget.hpp"
#include	"object.hpp"
#include	"listrect.hpp"
#include	"layer.hpp"
#include	"window.hpp"
#include	"message.hpp"

#define chartsChar	asciiChar
#define chartsAttr	asciiAttr

//============================================================================

static char chartText[] =
{
"\
зддддддддбддддбддддбддддбддддбддддбддддбддддбдддд©\n\
Ё    MSD Ё  0 Ё  1 Ё  2 Ё  3 Ё  4 Ё  5 Ё  6 Ё  7 Ё\n\
ЁLSD     Ё 000Ё 001Ё 010Ё 011Ё 100Ё 101Ё 110Ё 111Ё\n\
цддддддддеддддеддддеддддеддддеддддеддддеддддедддд╢\n\
Ё0  0000 Ё NULЁ DLEЁ SP Ё  0 Ё  @ Ё  P Ё  ` Ё  p Ё\n\
Ё1  0001 Ё SOHЁ DC1Ё  ! Ё  1 Ё  A Ё  Q Ё  a Ё  q Ё\n\
Ё2  0010 Ё STXЁ DC2Ё  \" Ё  2 Ё  B Ё  R Ё  b Ё  r Ё\n\
Ё3  0011 Ё ETXЁ DC3Ё  # Ё  3 Ё  C Ё  S Ё  c Ё  s Ё\n\
цддддддддеддддеддддеддддеддддеддддеддддеддддедддд╢\n\
Ё4  0100 Ё EOTЁ DC4Ё  $ Ё  4 Ё  D Ё  T Ё  d Ё  t Ё\n\
Ё5  0101 Ё ENQЁ NAKЁ  % Ё  5 Ё  E Ё  U Ё  e Ё  u Ё\n\
Ё6  0110 Ё ACKЁ SYNЁ  & Ё  6 Ё  F Ё  V Ё  f Ё  v Ё\n\
Ё7  0111 Ё BELЁ ETBЁ  ' Ё  7 Ё  G Ё  W Ё  g Ё  w Ё\n\
цддддддддеддддеддддеддддеддддеддддеддддеддддедддд╢\n\
Ё8  1000 Ё BS Ё CAMЁ  ( Ё  8 Ё  H Ё  X Ё  h Ё  x Ё\n\
Ё9  1001 Ё HT Ё EM Ё  ) Ё  9 Ё  I Ё  Y Ё  i Ё  y Ё\n\
ЁA  1010 Ё LF Ё SUBЁ  * Ё  : Ё  J Ё  Z Ё  j Ё  z Ё\n\
ЁB  1011 Ё VT Ё ESCЁ  + Ё  ; Ё  K Ё  [ Ё  k Ё  { Ё\n\
цддддддддеддддеддддеддддеддддеддддеддддеддддедддд╢\n\
ЁC  1100 Ё FF Ё FS Ё  , Ё  < Ё  L Ё  \\ Ё  l Ё  | Ё\n\
ЁD  1101 Ё CR Ё GS Ё  - Ё  = Ё  M Ё  ] Ё  m Ё  } Ё\n\
ЁE  1110 Ё SO Ё RS Ё  . Ё  > Ё  N Ё  ^ Ё  n Ё  ~ Ё\n\
ЁF  1111 Ё SI Ё US Ё  / Ё  ? Ё  O Ё  _ Ё  o Ё DELЁ\n\
юддддддддаддддаддддаддддаддддаддддаддддаддддадддды\
"
};

//============================================================================

static FLAG asciiOpen = boolean::FALSE;
static _object *asciiObjPtr = NULL;

//============================================================================

static void
ASCIIRoutine(_object *oPtr)
{
}

//=============================================================================

static FLAG
ASCIIInput(_input *in,_object *oPtr)
{
    _window *pWindow;
    FLAG inputUsed;
    unsigned char far *chr;
	_gadget *gPtr;

    pWindow = (_window *)oPtr->layer;
    inputUsed = boolean::FALSE;

	switch(in->inputType)
	 {
		case INP_KEY:
			if(in->fullKey == CMD_CLOSEWINDOW)
			 {
				asciiOpen = boolean::FALSE;
				asciiObjPtr = NULL;
				delete pWindow;
				delete oPtr;
				inputUsed = boolean::TRUE;
	 	 	 }
			break;
		case INP_MOUSE_LEFTBUTTON_DOWN:
			if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 {
				if(gPtr->gNum == GGAD_CLOSE)
				 {
					asciiOpen = boolean::FALSE;
					asciiObjPtr = NULL;
					delete pWindow;
					delete oPtr;
					inputUsed = boolean::TRUE;
				 }
			 }
			break;
	 }
    return(inputUsed);
}

//=============================================================================

FLAG
OpenASCIIChart(void)
{
    _object *oPtr;
    _window *pWindow;
	if(asciiOpen == boolean::TRUE)
	 {
		asciiObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return(boolean::TRUE);
	 }

	oPtr = new _object;

	asciiOpen = boolean::TRUE;
	asciiObjPtr = oPtr;
    pWindow = new _window(15,1,50,24,"ASCII Chart",(unsigned char)asciiAttr,(char)asciiChar);
	if ( !pWindow )
	 {
		PrintMessageBar( "Unable to open ASCII window" );
		return(boolean::FALSE);
	 }

	PrintLay(pWindow,chartText,0,0);

	pWindow->data = (void *)oPtr;
    oPtr->inputRoutine = ASCIIInput;
	oPtr->layer = pWindow;
	oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;

	AddSysGadgets(pWindow);
	DrawGadgets(pWindow);
	ActivateFrontWindow();
	return(boolean::TRUE);							// in case activated from menu
}

//=============================================================================

static char chart65816Text[] =
{
"\
зддддддддбддддбддддбддддбддддбддддбддддбддддбдддд©\n\
Ё    MSN Ё  0 Ё  1 Ё  2 Ё  3 Ё  4 Ё  5 Ё  6 Ё  7 Ё\n\
ЁLSN     Ё0000Ё0001Ё0010Ё0011Ё0100Ё0101Ё0110Ё0111Ё1000Ё1001Ё1010Ё1011Ё1100Ё1101Ё1110Ё1111Ё\n\
цддддддддеддддеддддеддддеддддеддддеддддеддддедддд╢\n\
Ё0  0000 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё1  0001 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё2  0010 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё3  0011 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
цддддддддеддддеддддеддддеддддеддддеддддеддддедддд╢\n\
Ё4  0100 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё5  0101 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё6  0110 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё7  0111 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
цддддддддеддддеддддеддддеддддеддддеддддеддддедддд╢\n\
Ё8  1000 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё9  1001 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
ЁA  1010 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
ЁB  1011 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
цддддддддеддддеддддеддддеддддеддддеддддеддддедддд╢\n\
ЁC  1100 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
ЁD  1101 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
ЁE  1110 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
ЁF  1111 Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
Ё        Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё    Ё\n\
юддддддддаддддаддддаддддаддддаддддаддддаддддадддды\
"
};

//=============================================================================
