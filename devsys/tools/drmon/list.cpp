//============================================================================
//	list.c: linked list manager
//============================================================================

#include <stdlib.h>
#include "list.hpp"

//============================================================================

void
InitListBase(_list *base)
{
	base->next = NULL;
	base->prev = NULL;
}

//============================================================================
// inserts node2 after node

void
InsertListNode(_list *node,_list *node2)
{
	if(node->next)
		node->next->prev = node2;
	node2->next = node->next;
	node->next = node2;
	node2->prev = node;
}

//============================================================================

_list *
SkipNodes(_list *lPtr,int count)
{
	while(count-- > 0 && lPtr)
		lPtr = lPtr->next;
	return(lPtr);
}

//============================================================================

_list *
GetFirstNode(_list *node)
{
	while (node->prev)
		node = node->prev;
	return(node);
}

//============================================================================

_list *
GetLastNode(_list *node)
{
	while(node->next)
		node = node->next;
	return(node);
}

//============================================================================
// add node to end of list

void
AddListNode(_list *node,_list *node2)
{
	InsertListNode(GetLastNode(node),node2);
}

//============================================================================

int
CountListItems(_list *headNode)
{
	int i;
	_list *node;
	i = 0;
	if(!headNode)
		return(i);
	node = headNode->next;
	while(node)
	 {
		node = node->next;
		i++;
	 }
	return(i);
}

//============================================================================
// note: does not free node memory!
// removes node from list and returns it's pointer

_list *
RemoveListNode(_list *lPtr)
{
	_list *l2Ptr;

	l2Ptr = lPtr->prev;

	if(l2Ptr)
		l2Ptr->next = lPtr->next;
	if(lPtr->next)
		lPtr->next->prev = l2Ptr;
	return(lPtr);
}

//============================================================================
// does free node memory

void
DeleteListNode(_list *lPtr)
{
	RemoveListNode(lPtr);
	free(lPtr);
}

//============================================================================
// frees an entire linked list of nodes
void
DeleteList(_list *lPtr)
{
	while(lPtr->next)
		DeleteListNode(lPtr->next);
}

//============================================================================
