//=============================================================================
//  monkeys.h
//=============================================================================

//=============================================================================
// string gadget

#define KEY_CTRLLEFT 			(KEY_EXT|0X73)
#define KEY_CTRLRIGHT 			(KEY_EXT|0X74)

#define KEY_INSERT	   			(KEY_EXT|0X52)
#define KEY_DELETE				(KEY_EXT|0X53)

#define CMD_CUT  				KEY_ALTU
#define CMD_COPY				KEY_ALTC
#define CMD_PASTE				KEY_ALTV

//=============================================================================
// global execution stuff

#define	CMD_RUN					KEY_F2
#define	CMD_STOP				KEY_F3
#define	CMD_RUNNOUPDATE			KEY_F4

#define	CMD_STEP				KEY_F7
#define CMD_STEPOVER			KEY_F8

#define	CMD_STEPASSEMBLY		KEY_CTRLF7
#define CMD_STEPOVERASSEMBLY	KEY_CTRLF8

#define	CMD_STEPSOURCE			KEY_ALTF7
#define CMD_STEPOVERSOURCE		KEY_ALTF8

// global window stuff

#define CMD_LOGWINDOW			KEY_ALTP
#define CMD_NEXTWINDOW			KEY_F6
#define CMD_LOCALMENU			KEY_CTRLF10
#define CMD_MOVEWIN				KEY_F9
#define CMD_MENU 				KEY_F10
#define CMD_CLOSEWINDOW			KEY_ALTQ
#define CMD_QUIT  				KEY_ALTX
#define CMD_DOSSHELL			KEY_ALTD
#define CMD_ZOOMWINDOW			KEY_ALTZ

#define CMD_OPENEXPR			KEY_ALTE
#define	CMD_OPENCMD				KEY_ALTK
#define	CMD_OPENWATCH			KEY_ALTW
#define CMD_OPENMEMORY			KEY_ALTM
#define CMD_OPENREG 			KEY_ALTR
#define CMD_OPENBREAK 			KEY_ALTB
#define CMD_OPENCONSOLE 		KEY_ALTO
#define CMD_OPENSYMBOL			KEY_ALTS
#define CMD_OPENFILEVIEW		KEY_ALTT
#define CMD_OPENHELPFILE		KEY_F1
#define CMD_OPENWINDOWHELPFILE	KEY_CTRLF1
#define CMD_OPENSOURCEFILE		KEY_ALTL
#define CMD_OPENABOUT			KEY_ALTA
#define CMD_OPENASCII			KEY_ALTY
#define CMD_OPENPLATHELP		KEY_ALTP
#define CMD_OPENPROJINFO		KEY_ALTI

// local window commands

// dump window
#define CMD_MEMORYDOWNLINE			KEY_DOWN
#define CMD_MEMORYUPLINE			KEY_UP
#define CMD_MEMORYPGDWN				KEY_PGDWN
#define CMD_MEMORYPGUP				KEY_PGUP
#define CMD_MEMORYDOWN				KEY_LEFT
#define CMD_MEMORYUP				KEY_RIGHT
#define CMD_MEMORYRUNTOHERE			KEY_CTRLH
#define CMD_MEMORYSETBREAKHERE		KEY_CTRLS
#define CMD_MEMORYSETBREAKONCEHERE	KEY_CTRLO
#define CMD_MEMORYSETBREAKCOUNTHERE	KEY_CTRLN
#define CMD_MEMORYCLEARALLBREAKS	KEY_CTRLA
#define CMD_MEMORYCLEARBREAK		KEY_CTRLC
#define CMD_MEMORYCHANGEMEM			KEY_CTRLM
#define CMD_PROCHELP				KEY_CTRLP

#define CMD_MEMORYBYTE		KEY_CTRLB
#define CMD_MEMORYWORD		KEY_CTRLW
#define CMD_MEMORYLONG		KEY_CTRLL
#define CMD_MEMORYCODE		KEY_CTRLD
#define CMD_MEMORYASCII		KEY_CTRLY
#define CMD_MEMORYGOTO		KEY_CTRLG

// search window
#define CMD_SEARCH				KEY_CTRLS
#define CMD_SEARCHLIST			KEY_CTRLL
#define CMD_SEARCHCLEAR			KEY_CTRLC

// break window
#define CMD_SETBREAK			KEY_CTRLS
#define CMD_SETBREAKONCE		KEY_CTRLO
#define CMD_SETBREAKCOUNT		KEY_CTRLN
#define CMD_SETBREAKCOND		KEY_CTRLE
#define CMD_CLEARBREAK			KEY_CTRLC
#define CMD_CLEARALLBREAKS		KEY_CTRLA
#define CMD_TOGGLEBREAK			KEY_CTRLT

// watch window
#define CMD_SETWATCH			KEY_CTRLS
#define CMD_CLEARWATCH			KEY_CTRLC
#define CMD_CLEARALLWATCHS		KEY_CTRLA

// symbol window
#define	CMD_BREAKHERE			KEY_CTRLB
#define	CMD_BREAKHEREONCE		KEY_CTRLO
#define CMD_LOADSYMBOLS			KEY_CTRLL
#define CMD_SETSYMBOL			KEY_CTRLS
#define CMD_CLEARSYMBOL			KEY_CTRLC
#define CMD_CLEARALLSYMBOLS		KEY_CTRLA

// file viewer/source debug
#define	CMD_LOADTEXT			KEY_CTRLL
#define	CMD_TEXTSEARCH			KEY_CTRLF
#define	CMD_TEXTNEXT			KEY_CTRLN
#define	CMD_TEXTGOTO			KEY_CTRLG

// source debug only
#define CMD_SOURCESETBREAK			KEY_CTRLS
#define CMD_SOURCESETBREAKONCE		KEY_CTRLO
#define CMD_SOURCECLEARBREAK		KEY_CTRLC
#define CMD_SOURCECLEARALLBREAKS	KEY_CTRLA
#define CMD_SOURCERUNTOHERE			KEY_CTRLH
#define CMD_SOURCEGOTOSYMBOL		KEY_CTRLY

// console window
#define	CMD_CONSOLECLEAR		KEY_CTRLC

// command window
#define	CMD_PREVHIST			KEY_UP
#define	CMD_NEXTHIST			KEY_DOWN

// pop-up menu
#define CMD_LOCALMENU_ABORT		KEY_ESC
#define CMD_LOCALMENU_PREVITEM	KEY_UP
#define CMD_LOCALMENU_NEXTITEM	KEY_DOWN
#define CMD_LOCALMENU_SELECT	KEY_RETURN
#define CMD_LOCALMENU_TOP	KEY_HOME
#define CMD_LOCALMENU_BOTTOM	KEY_END

// menu bar
#define CMD_MENUBAR_NEXTMENU	KEY_RIGHT
#define CMD_MENUBAR_PREVMENU	KEY_LEFT
#define CMD_MENUBAR_SHOWMENU	KEY_RETURN

// all list gadgets
#define CMD_LIST_ABORT			KEY_ESC
#define CMD_LIST_DONE			KEY_RETURN
#define CMD_LIST_DOWN			KEY_DOWN
#define CMD_LIST_UP				KEY_UP
#define CMD_LIST_DOWNPAGE		KEY_PGDWN
#define CMD_LIST_UPPAGE			KEY_PGUP
#define CMD_LIST_TOP			KEY_HOME
#define CMD_LIST_BOTTOM			KEY_END

// movement
#define CMD_MOVE_LEFT			KEY_LEFT
#define CMD_MOVE_RIGHT			KEY_RIGHT
#define CMD_MOVE_DOWN			KEY_DOWN
#define CMD_MOVE_UP				KEY_UP
#define CMD_MOVE_QUIT			KEY_ESC
#define CMD_MOVE_QUIT2			KEY_RETURN

//==============================================================================
