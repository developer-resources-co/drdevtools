uint swaphilo (uint a);
void InitPalette (char *name, PALETTE *);
void InitChars (char *name, TILESET *);
void InitGUI ( int argc, char *argv[] );
void NewMap (char *filename, MAP *mapPtr,uint defaultX, uint defaultY, TILESET *, FLAG);
void FlipCharX (uchar huge *sourcebuffer,uchar huge *destbuffer);
void FlipCharY (uchar huge *sourcebuffer,uchar huge *destbuffer);
