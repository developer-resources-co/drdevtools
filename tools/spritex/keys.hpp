//=============================================================================
//  keys.hpp
//=============================================================================

#define KEY_EXT		0X100
#define KEY_SHIFTED 0x200

#define KEY_F1 		KEY_EXT|59
#define KEY_F2 		KEY_EXT|60
#define KEY_F3 		KEY_EXT|61
#define KEY_F4 		KEY_EXT|62
#define KEY_F5 		KEY_EXT|63
#define KEY_F6 		KEY_EXT|64
#define KEY_F7 		KEY_EXT|65
#define KEY_F8 		KEY_EXT|66
#define KEY_F9 		KEY_EXT|67
#define KEY_F10		KEY_EXT|68

#define KEY_ESC     27
#define KEY_ALTESC	KEY_EXT|1

#define KEY_LEFT    KEY_EXT|75
#define KEY_RIGHT   KEY_EXT|77
#define KEY_UP	    KEY_EXT|72
#define KEY_DOWN    KEY_EXT|80

#define KEY_PGUP	KEY_EXT|73
#define KEY_PGDWN	KEY_EXT|81
#define KEY_CTRLHOME	KEY_EXT|119
#define KEY_HOME	KEY_EXT|71
#define KEY_END		KEY_EXT|79
#define KEY_CTRLEND		KEY_EXT|117

#define KEY_RETURN 	0xd
#define KEY_BACKSPACE 0x8
#define KEY_TAB 0x9

#define	KEY_CTRLA	1
#define	KEY_CTRLB	2
#define	KEY_CTRLC	3
#define	KEY_CTRLD	4
#define	KEY_CTRLE	5
#define	KEY_CTRLF	6
#define	KEY_CTRLG	7
#define	KEY_CTRLH	8
#define	KEY_CTRLI	9
#define	KEY_CTRLJ	10
#define	KEY_CTRLK	11
#define	KEY_CTRLL	12
#define	KEY_CTRLM	13
#define	KEY_CTRLN	14
#define	KEY_CTRLO	15
#define	KEY_CTRLP	16
#define	KEY_CTRLQ	17
#define	KEY_CTRLR	18
#define	KEY_CTRLS	19
#define	KEY_CTRLT	20
#define	KEY_CTRLU	21
#define	KEY_CTRLV	22
#define	KEY_CTRLW	23
#define	KEY_CTRLX	24
#define	KEY_CTRLY	25
#define	KEY_CTRLZ	26

#define KEY_ALTF1 	KEY_EXT|0X68
#define KEY_ALTF2 	KEY_EXT|0X69
#define KEY_ALTF3 	KEY_EXT|0X6A
#define KEY_ALTF4 	KEY_EXT|0X6B
#define KEY_ALTF5 	KEY_EXT|0X6C
#define KEY_ALTF6 	KEY_EXT|0X6D
#define KEY_ALTF7 	KEY_EXT|0X6E
#define KEY_ALTF8 	KEY_EXT|0X6F
#define KEY_ALTF9 	KEY_EXT|0X70
#define KEY_ALTF10	KEY_EXT|0X71

#define KEY_ALTA	KEY_EXT|0X1E
#define KEY_ALTB	KEY_EXT|0X30
#define KEY_ALTC	KEY_EXT|0X2E
#define KEY_ALTD	KEY_EXT|0X20
#define KEY_ALTE	KEY_EXT|0X12
#define KEY_ALTF	KEY_EXT|0X21
#define KEY_ALTG	KEY_EXT|0X22
#define KEY_ALTH	KEY_EXT|0X23
#define KEY_ALTI	KEY_EXT|0X17
#define KEY_ALTJ	KEY_EXT|0X24
#define KEY_ALTK	KEY_EXT|0X25
#define KEY_ALTL	KEY_EXT|0X26
#define KEY_ALTM	KEY_EXT|0X32
#define KEY_ALTN	KEY_EXT|0X31
#define KEY_ALTO	KEY_EXT|0X18
#define KEY_ALTP	KEY_EXT|0X2F
#define KEY_ALTQ	KEY_EXT|0X10
#define KEY_ALTR	KEY_EXT|0X13
#define KEY_ALTS	KEY_EXT|0X1F
#define KEY_ALTT	KEY_EXT|0X14
#define KEY_ALTU	KEY_EXT|0X16
#define KEY_ALTV	KEY_EXT|0X2F
#define KEY_ALTW	KEY_EXT|0X11
#define KEY_ALTX	KEY_EXT|0X2D
#define KEY_ALTY	KEY_EXT|0X15
#define KEY_ALTZ	KEY_EXT|0X2C

//=============================================================================
// string gadget

#define KEY_CTRLLEFT 			KEY_EXT|0X73
#define KEY_CTRLRIGHT 			KEY_EXT|0X74

#define KEY_INSERT	   			KEY_EXT|0X52
#define KEY_DELETE				KEY_EXT|0X53

#define CMD_CUT  				KEY_ALTA
#define CMD_COPY				KEY_ALTC
#define CMD_PASTE				KEY_ALTP

//=============================================================================
// global execution stuff

#define	CMD_RUN					KEY_F2
#define	CMD_STOP				KEY_F3
#define	CMD_RUNNOUPDATE			KEY_F4
#define	CMD_STEP				KEY_F7
#define CMD_STEPOVER			KEY_F6

// global window stuff

#define CMD_NEXTWINDOW			KEY_F1
#define CMD_LOCALMENU			KEY_F8
#define CMD_MOVEWIN				KEY_F9
#define CMD_MENU 				KEY_F10
#define CMD_CLOSEWINDOW			KEY_ALTQ
#define CMD_QUIT  				KEY_ALTX
#define CMD_DOSSHELL			KEY_ALTD
#define CMD_OPENEXPR			KEY_ALTE
#define	CMD_OPENCMD				KEY_ALTK
#define	CMD_OPENWATCH			KEY_ALTW
#define CMD_OPENMEMORY			KEY_ALTM
#define CMD_OPENREG 			KEY_ALTR
#define CMD_OPENBREAK 			KEY_ALTB
#define CMD_OPENCONSOLE 		KEY_ALTO
#define CMD_OPENSYMBOL			KEY_ALTS
#define CMD_OPENFILEVIEW		KEY_ALTT
#define CMD_OPENHELPFILE		KEY_ALTH


// local window commands

// dump window
#define CMD_DUMPDOWNLINE		KEY_DOWN
#define CMD_DUMPUPLINE			KEY_UP
#define CMD_DUMPPGDWN			KEY_PGDWN
#define CMD_DUMPPGUP			KEY_PGUP
#define CMD_DUMPDOWN			KEY_LEFT
#define CMD_DUMPUP				KEY_RIGHT
#define CMD_DUMPRUNTOHERE		KEY_CTRLH
#define CMD_DUMPSETBREAKHERE	KEY_CTRLS
#define CMD_DUMPSETBREAKONCEHERE	KEY_CTRLO
#define CMD_DUMPSETBREAKCOUNTHERE	KEY_CTRLN
#define CMD_DUMPCLEARALLBREAKS		KEY_CTRLA
#define CMD_DUMPCLEARBREAK			KEY_CTRLC
#define CMD_DUMPCHANGEMEM			KEY_CTRLM

// search window
#define CMD_SEARCH				KEY_CTRLS
#define CMD_SEARCHLIST			KEY_CTRLL
#define CMD_SEARCHCLEAR			KEY_CTRLC

// break window
#define CMD_SETBREAK			KEY_CTRLS
#define CMD_SETBREAKONCE		KEY_CTRLO
#define CMD_SETBREAKCOUNT		KEY_CTRLN
#define CMD_CLEARBREAK			KEY_CTRLC
#define CMD_CLEARALLBREAKS		KEY_CTRLA

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

// file viewer
#define	CMD_LOADTEXT			KEY_CTRLL
#define	CMD_TEXTSEARCH			KEY_CTRLF
#define	CMD_TEXTNEXT			KEY_CTRLN

// command window
#define	CMD_PREVHIST			KEY_UP
#define	CMD_NEXTHIST			KEY_DOWN

// pop-up menu
#define CMD_LOCALMENU_ABORT		KEY_ESC
#define CMD_LOCALMENU_PREVITEM	KEY_UP
#define CMD_LOCALMENU_NEXTITEM	KEY_DOWN
#define CMD_LOCALMENU_SELECT	KEY_RETURN

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

//=============================================================================
