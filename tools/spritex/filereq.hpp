/*
 * filereq.h -- file requester header
 */

int ScanDir(void);
void FileReqRoutine(struct _gadget *, void (*routine)(void));
extern boolean fileActive;	/* file-requester gadget is up */
void frCancelRoutine(struct _gadget *, struct _input *);
