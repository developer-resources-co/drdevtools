//=============================================================================
// break.hpp
//=============================================================================

class _breakList : public _stringList
{
public:
	_breakList() : _stringList() { addr = inst = count = flags = 0; conditionExpr = NULL; }
	~_breakList() { if(conditionExpr) delete conditionExpr; }
	char* conditionExpr;
	ULONG addr;						// address of breakpoint
	UWORD inst;						// instruction replaced with break
	UWORD count;					// number of iterations before vanish
	UBYTE flags;					// flags with this breakpoint

	const char* const Condition(char* newCondExpr)
	 {
		if(conditionExpr) delete conditionExpr;
		if(newCondExpr)
		 {
			conditionExpr = new char[strlen(newCondExpr)+1];
			if(conditionExpr)
				strcpy(conditionExpr,newCondExpr);
		 }
	 }
	const char* const Condition() { return(conditionExpr); }
	ULONG Address() { return(addr); }
	ULONG Address(ULONG newAddress) { addr = newAddress; return(addr); }

private:
};

#define BRKB_ACTIVE 0
#define BRKF_ACTIVE 1<<BRKB_ACTIVE

//=============================================================================

void
InitBreakPts(void);

void
AddBrkPt(ULONG addr,unsigned int count,char* conditional);

struct _breakList *
FindBreak(ULONG addr);

void
BreakWinClear(struct _breakList *bPtr);

void
BreakClear(ULONG addr);

void
BreakClearAll(void);

void
BreakSetGetCount(ULONG breakAddr, void *dataPtr);

void
ValidateBreakPts(void);

boolean
BreakPointHit(ULONG addr);


//============================================================================
// break commands
enum
{
	BREAK_SET = 1,
	BREAK_SETONCE,
	BREAK_SETCOUNT,
	BREAK_SETCOND,
	BREAK_CLEAR,
	BREAK_CLEARALL,
	BREAK_TOGGLE
};

//=============================================================================
