/*
 * image.hpp -- header file for image.c
 */


void LoadImage(void);

void DrawClippedBitmap(struct GRAPHIC *, struct VIEWPORT *,word,word);
void NewDrawClippedBitmap(Animation*, struct VIEWPORT *,word,word);

void RefreshImage(void);
