//============================================================================
//	list.h: linked list manager
//============================================================================

#ifndef DRMON_list_H
#define DRMON_list_H

//============================================================================

class _list
{
public:

	_list() { next = prev = NULL; }

	_list *Prev() { return(prev); }
	_list *Prev(_list *newPrev) { prev = newPrev; return(prev); }

	_list *Next() { return(next); }
	_list *Next(_list *newNext) { next = newNext; return(next); }

	_list *next;
	_list *prev;
};

void
InitListBase(_list *base);

void
InsertListNode(_list *node,_list *node2);

_list *
SkipNodes(_list *lPtr,int count);

_list *
GetFirstNode(_list *node);

_list *
GetLastNode(_list *node);

void
AddListNode(_list *node,_list *node2);

int
CountListItems(_list *headNode);

_list *
RemoveListNode(_list *lPtr);

void
DeleteListNode(_list *lPtr);

void
DeleteList(_list *lPtr);

//============================================================================

#endif

//============================================================================

