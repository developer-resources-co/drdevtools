//=============================================================================
// guidefs1.h
//=============================================================================

int DrawGUI (void);
int GUIMainLoop (void);

extern struct _gad QuitGadget,MapBoxGadget,MapFullBoxGadget;
extern struct _keygad ShowBrushGadget;
extern short MouseX,MouseY,mapBotEdge;
extern struct _gad *firstgadget;
extern struct _keygad *firstkeygadget;

//=============================================================================

struct _gad
{
    int hotkey;
    int (*hitfunction) (int x,int y,struct _gad *gadget);
    long hitdata;
    int x;
    int y;
    int width;
    int height;
    int (*drawfunction) (struct _gad *gadget);
    long drawdata;
    struct _gad *next;
};

//=============================================================================

struct _keygad
{
    int hotkey;
    int (*hitfunction) (int x,int y,struct _gad *gadget);
    long hitdata;
    struct _keygad *next;
};

//=============================================================================
