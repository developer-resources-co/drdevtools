/*
 * spgadget.hpp -- enum types for gadget identity, and type in sprite editor
 * (c) 1991, Developer Resources
 * Written By Scott Statton
 */


enum gadgetidentity {

	GAD_NONE,
	GAD_PANE_1X1,
	GAD_PANE_1X2,
	GAD_PANE_1X3,
	GAD_PANE_1X4,
	GAD_PANE_2X1,
	GAD_PANE_2X2,
	GAD_PANE_2X3,
	GAD_PANE_2X4,
	GAD_PANE_3X1,
	GAD_PANE_3X2,
	GAD_PANE_3X3,
	GAD_PANE_3X4,
	GAD_PANE_4X1,
	GAD_PANE_4X2,
	GAD_PANE_4X3,
	GAD_PANE_4X4,
	GAD_QUIT,
	GAD_LOAD,
	GAD_SAVE,
	GAD_LEFT,
	GAD_RIGHT,
	GAD_GRID,
	GAD_LOADIFF,
	GAD_LOADSPR,
	GAD_NUKE,
	GAD_CENTER,
	GAD_SETCHAR,
	GAD_SHOWINFO,
	GAD_MAIN_SCREEN };

enum gadgettype {
	GTYPE_NONE,
	GTYPE_HIT };

void
SpriteInitGadgets(void);

extern boolean spriteboolean;

void NewSpriteSizeRoutine (struct _gadget *, struct _input *);
void SetCharRoutine(struct _gadget *, struct _input *);
void LoadRoutine(struct _gadget *, struct _input *);
void LoadSPRRoutine(struct _gadget *, struct _input *);
void LoadIFFRoutine(struct _gadget *, struct _input *);
void ShowInfoRoutine(struct _gadget *, struct _input *);
MessageBox(struct VIEWPORT *vp,int backColor, int borderColor, unsigned char *text);

