/* list.c -- object list manager.  Owns the lists of objects, and processes
    them.

Craig June 11, 1993: Written

*/

#include <stdlib.h>
#include <debug.h>
#include <assert.h>

#include <list.h>

#define ERROR_CHECK 1


TDataBlock* theObjectLists;

#define SIZEOF_DBLOCK_HEADER (sizeof(TDataBlock)-sizeof(void*))
#define LIST_INCREMENT 10

#define SetColorZero(xx) *(ULONG*)0xc00004=0xc0000000;*(UWORD*)0xc00000=xx;


TListReturn ListInit (UWORD nNumLists) {
UWORD nn,ii;
TDataBlock *tlists;
TDataBlock** tt;
Object** uu;

    if (theObjectLists) {
        for (nn=theObjectLists->count, tt=(TDataBlock**)theObjectLists->data;
        nn>0;
        --nn,++tt) {
            if (*tt) {
                free (*tt);
                *tt=NULL;
            } else {
                for (ii=(*tt)->count, uu=(Object**)((*tt)->data);
                ii>0;
                --ii,++uu) {
                     if (!ERROR_CHECK || (*uu && (*uu)->mDestruct))
                        (*uu)->mDestruct(*uu);
                    *uu=0;
                }
            }
        }
        tlists = realloc (theObjectLists,
                            nNumLists*sizeof(void*)+SIZEOF_DBLOCK_HEADER);
    } else {
        tlists = malloc (nNumLists*sizeof(void*)+SIZEOF_DBLOCK_HEADER);
    }

    if (!tlists) {
        theObjectLists = NULL;
        return LIST_OutOfMemory;
    }

    tlists->size = nNumLists;
    tlists->count = 0;

    for (nn=nNumLists, tt=(TDataBlock**)(tlists->data); nn>0; --nn) {
        *tt++=NULL;
    }


    theObjectLists = tlists;

    return LIST_NoError;
}




TListReturn ListAddObject (Object* pObject, UWORD nListNumber) {
TDataBlock** tlist;
TDataBlock* tmp;
UWORD ii;

    if (!theObjectLists)
        return LIST_NotInitialized;

    if (nListNumber >= theObjectLists->size)
        return LIST_InvalidListNumber;

    if (!pObject)
        return LIST_InvalidData;

    tlist = (TDataBlock**) (theObjectLists->data + nListNumber);

    if (!*tlist) {
        *tlist = (TDataBlock*)
                malloc (LIST_INCREMENT*sizeof(void*)+SIZEOF_DBLOCK_HEADER);
        if (!*tlist)
            return LIST_OutOfMemory;
        (*tlist)->size = LIST_INCREMENT;
        (*tlist)->count = 0;
    } else if ( (*tlist)->count >= (*tlist)->size ) {
         tmp = (TDataBlock*) realloc( *tlist,
                ((*tlist)->size+LIST_INCREMENT)*sizeof(void*)
                        + SIZEOF_DBLOCK_HEADER );
        if (!tmp)
            return LIST_OutOfMemory;

        (*tlist) = tmp;
        tmp->size += LIST_INCREMENT;

        for (ii=tmp->count; ii<tmp->size; ++ii)
            tmp->data[ii] = NULL;
    }

    (*tlist)->data[(*tlist)->count] = (void*) pObject;
    (*tlist)->count++;

    return LIST_NoError;
}


TListReturn ListRemoveObject (Object* pObject, UWORD nListNumber) {
TDataBlock** tlist;
Object** tmp;
UWORD ii;

    if (!theObjectLists)
        return LIST_NotInitialized;

    if (nListNumber >= theObjectLists->size)
        return LIST_InvalidListNumber;

    tlist = (TDataBlock**) (theObjectLists->data + nListNumber);

    if (!*tlist || (*tlist)->count == 0)
        return LIST_DataNotFound;

    for (ii=(*tlist)->count, tmp = (Object**) ((*tlist)->data);
    ii>0;
    --ii, ++tmp) {

         if (pObject == *tmp) {
            (*tlist)->count--;

            if (tmp != (Object**)((*tlist)->data+(*tlist)->count))
                *tmp = (*tlist)->data[(*tlist)->count];
            else
                *tmp = NULL;

            return LIST_NoError;
        }
    }

    return LIST_DataNotFound;
}


TListReturn ListRemoveAllLists (Object* pObject) {
int ii;

    if (!theObjectLists)
        return LIST_NotInitialized;

    for (ii=0;ii<theObjectLists->size; ++ii)
        ListRemoveObject (pObject, ii);
}


TListReturn ListUpdateObjects (UWORD nListNumber) {
TDataBlock** tlist;
WORD ii;
Object* obj;
#ifdef SPEED
UWORD color = 0x060;
#endif
    if (!theObjectLists)
        return LIST_NotInitialized;

    if (nListNumber >= theObjectLists->size)
        return LIST_InvalidListNumber;

    tlist = (TDataBlock**) (theObjectLists->data + nListNumber);

    if (!*tlist || (*tlist)->count == 0)
        return LIST_DataNotFound;

    for (ii=0; ii<(*tlist)->count;  ++ii) {
        #ifdef SPEED
            SetColorZero(color);color+=4;
        #endif
        obj = (Object*) ((*tlist)->data[ii]);

        assert (obj);

        if (!obj) continue;

        if (obj->fDelete) {
            --ii;    /* this is to account for the removal */
            ListRemoveAllLists (obj);
            obj->mDestruct(obj);

        }

        else if (obj->fActive) {
            if (obj->nTickDelay == 0)
                obj->mTick (obj);
            else
                obj->nTickDelay--;
        }
    }

    return LIST_NoError;
}


TListReturn ListDestroyObjects (UWORD nListNumber) {
TDataBlock** tlist;
Object** tmp;
UWORD ii;
Object* obj;

    if (!theObjectLists)
        return LIST_NotInitialized;

    if (nListNumber >= theObjectLists->size)
        return LIST_InvalidListNumber;

    tlist = (TDataBlock**) (theObjectLists->data + nListNumber);

    if (!*tlist || (*tlist)->count == 0)
        return LIST_DataNotFound;

    while ((*tlist)->count>0) {

        obj =   (Object*) ((*tlist)->data[0]);

        if (obj) {
            ListRemoveAllLists(obj);
            if (!obj->fPermanent)
                obj->mDestruct(obj);
        }
    }

    return LIST_NoError;

}


void* ListStart (UWORD nListNumber){
TDataBlock** tlist;
void* tptr;

    if (!theObjectLists)
        return NULL;

    if (nListNumber >= theObjectLists->size)
        return NULL;

    tlist = (TDataBlock**) (theObjectLists->data + nListNumber);

    if (!*tlist || (*tlist)->count == 0)
        return NULL;


    tptr = malloc (sizeof (TDataBlock*)+sizeof(UWORD));
    *((TDataBlock**)tptr) = *tlist;
    *(((UWORD*)tptr)+2) = 0;

    return tptr;
}

void* ListStartNoHeap (UWORD nListNumber,void* data){
TDataBlock** tlist;
void* tptr;

    if (!theObjectLists)
        return NULL;

    if (nListNumber >= theObjectLists->size)
        return NULL;

    tlist = (TDataBlock**) (theObjectLists->data + nListNumber);

    if (!*tlist || (*tlist)->count == 0)
        return NULL;


    tptr = data;
    *((TDataBlock**)tptr) = *tlist;
    *(((UWORD*)tptr)+2) = 0;

    return tptr;
}



/* EOF */

