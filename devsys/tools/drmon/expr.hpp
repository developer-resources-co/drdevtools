//=============================================================================
// expr.hpp
//=============================================================================

void
GetExpr(void (*expRoutine)(ULONG result,void *dataPtr),void *dataPtr,char *title,_layer *backLPtr);

void
GetMultiExpr(void (*expRoutine)(ULONG result,void *dataPtr),void *dataPtr,char *title,_layer *backLPtr);


enum { EXPRLEN = 53 };

int DoExp(const char *isPtr);
extern ULONG	exprAnswer;				/* place to put the result */
extern char expString[];

//=============================================================================
