//==============================================================================
// list.h: doubly linked list handler
//==============================================================================
/*

Documentation:

	Abstract:
		doubly linked intrusive list mechanism

	Class Hierarchy:
					node
                     │
				nodeInterface<template>
                     │
				    list

	Dependancies:
		none
	Restrictions:
		cannot have a node in more than one list

	Example:
		see the iff class until I write one

*/
//==============================================================================

#ifndef PCLIB_LIST_H
#define PCLIB_LIST_H

//==============================================================================

class node
{
protected:
	node *prev;
	node *next;
public:
	node() { prev = 0; next = 0; }
	node(node *n) { next = n->next; if(next) next->prev = this; n->next = this;}
	virtual ~node() { if(next) next->prev = prev; if(prev) prev->next = next;}
	node *GetNextNode() {return(next);}
	node *GetPrevNode() {return(prev);}
	void NodeInsert(node *nPtr) {nPtr->next = next; if(next) next->prev = nPtr; next = nPtr; nPtr->prev = this; }
};

//==============================================================================

template<class T>
class nodeInterface : public node
{
public:
		T *GetNext() {return((T *) GetNextNode());}
		T *GetPrev() {return((T *) GetPrevNode());}
		void Insert(T *tPtr) {node::NodeInsert((node *)tPtr);}
};

//==============================================================================

template<class T>
class list : public nodeInterface<T>
{
public:
		list() : nodeInterface<T>() { }
		void DeleteAll() {	while(this->next) delete this->next; }
		~list() { DeleteAll(); }
		T *Last()
		 {
			nodeInterface<T> *node = (T *)this;
			while(node->GetNext())
				node = node->GetNext();
		 	return((T *) node);             			// return ptr to last node
		 }
		void Append(T *tPtr) { Last()->Insert(tPtr);}

		uint Count(void)
		 {
			uint count = 0;
			T *tPtr = GetNext();

			while(tPtr)
			 {
				count++;
				tPtr = tPtr->GetNext();
			 }

			return(count);
		 }

		T *GetItem(uint index)
	 	{
			T *tPtr = GetNext();
			while(index-- && tPtr)
		 	{
				tPtr = tPtr->GetNext();
		 	}
			return(tPtr);
	 	}


};

//==============================================================================

template<class T>
class list_iterator
{
	list<T> *lPtr;
	nodeInterface<T> *currentNode;
	list_iterator();
public:
	list_iterator(list<T> *l) { lPtr = l; currentNode = l->GetNext();}
	nodeInterface<T> *next() { if(currentNode) currentNode = currentNode->GetNext(); return(currentNode);}
	nodeInterface<T> *reset() { currentNode = lPtr->GetNext(); return(currentNode);}
};

//==============================================================================

#endif

//==============================================================================
