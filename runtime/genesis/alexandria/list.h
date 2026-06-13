/* list.h -- interface for SEGA object list manager functions


June 11, 1993 : Written

*/

#ifndef _LIST_H_
#define _LIST_H_

#include <gentypes.h>
#include <object.h>


typedef enum {
LIST_NoError=0,
LIST_OutOfMemory,
LIST_InvalidListNumber,
LIST_DataNotFound,
LIST_NotInitialized,
LIST_InvalidData
} TListReturn;

typedef struct {
UWORD size;		/* size of this block of data */
UWORD count;	/* number of used elements */
void* data[1];
} TDataBlock;

extern TDataBlock* theObjectLists;

/* Creates the specified # of lists */
TListReturn ListInit (UWORD nNumLists);

/* Adds the object to the #ed list */
TListReturn ListAddObject (Object* pObject, UWORD nListNumber);

/* removes the object from the #ed list */
TListReturn ListRemoveObject (Object* pObject, UWORD nListNumber);

/* removes the object from all lists */
TListReturn ListRemoveAllLists (Object* pObject);

/* calls object->routine for each object in the list, if nDelay-- == 0 */
TListReturn ListUpdateObjects (UWORD nListNumber);

/* calls object->mDestruct for each object in the list */
TListReturn ListDestroyObjects (UWORD nListNumber);


/* Iterator functions */

void* ListStart (UWORD nListNumber); /* returns NULL if error or empty */

/* void* ListNext (void* StartedList); returns NULL at end */

#define ListNext(StartedList) \
	(  (*(UWORD**)StartedList)[1] > ((UWORD*)StartedList)[2] +1 \
		? ( ((UWORD*)StartedList)[2]++ , StartedList )		    \
		: ( free(StartedList), NULL)						    \
	)

/* Object* ListObject (void* StartedList); returns NULL if error */

#define ListObject(StartedList) \
	( (*(Object***)StartedList)[((UWORD*)StartedList)[2] + 1] )

/* like ListStart & ListNext without the malloc & free */

typedef struct {void* aa;UWORD bb;} ListIteratorDataSpace;

void* ListStartNoHeap (UWORD nListNumber,void* data);

#define ListNextNoHeap(StartedList) \
	(  (*(UWORD**)StartedList)[1] > ((UWORD*)StartedList)[2] +1 \
		? ( ((UWORD*)StartedList)[2]++ , StartedList )		    \
		: ( NULL)						    \
	)


#endif

/* EOF */

