// Copyright (c) 1992
// Dennis M. Carleton
// May be distributed freely but not used for profit
//
// Description: definitions for the SET template
//

//-------------------------------------------------------------------------
//	USAGE EXAMPLE
//	{
//	 set<int>	a, b, c, d;
//
//	 a[3](1, 2, 3);				// init the set
//	 b[3](4, 5, 6);				// init the set
//	 c[3](7, 8, 9);
//
//	 x < 4;					// add to the set
//	 x < c;
//
//	 d = I(a, b)				// intersection = { 4 }
//	 d.dumpSet();
//
//	 d = U(b,c);				// union { 4, 5, 6, 7, 8, 9 }
//	 d.dumpSet();
//      };
//
//-------------------------------------------------------------------------


#ifndef _SET_H_
#define _SET_H_

//
// includes
//

#include <iostream.h>

#include <stdarg.h>

#include <pclib/boolean.hpp>
//

// Copyright (c) 1992
// Dennis M. Carleton
// May be distributed freely but not used for profit
//
// Description: definitions for the 'typedList' template
//

#ifndef _LIST_H_
#define	_LIST_H_

//
// includes
//

//
// generic class listMember
//

template <class R> class listMember
{
public:
    R			*element;
    listMember<R>	*next;
    listMember<R>	*previous;
};

//
// generic class typedList
//

template <class T> class typedList
{
private:
    listMember<T>	*first;
    listMember<T>	*current;
public:
			typedList();
			~typedList();
    void		append(T* newMember);
    void		clear();
    void 		reset();
    T*			operator++();
    T*			getCurrent();
    T*			removeCurrent();
};



//
// typedList methods
//

//
// Function:	 typedList
// Abstract:	 constructor - initializes the structure
// Parameters:	 none
// Return Value: none
//

template <class T> typedList<T>::typedList()
			   : first(NULL), current(NULL)
{
};



//
// Function:	 ~typedList
// Abstract:	 destructor - removes the elements in the list, 
//			destroys the listMember nodes
// Parameters:	 none
// Return Value: none
//

template <class T> typedList<T>::~typedList()
{
 clear();
};


//
// Function:	 append
// Abstract:	 adds an element to the list
// Parameters:	 T* - a pointer to the element of type "T" to be appended
// Return Value: none
//

template <class T> void typedList<T>::append(T* newMember)
{
listMember<T> 	*newNode = new listMember<T>,
		*tempNode = first;

 newNode->element = newMember;
 if (!first)
    {
     first  = newNode;
     first->next = first;
     first->previous = first;
    }
 else    
    {
     while (tempNode->next != first)
	tempNode = tempNode->next;
     tempNode->next = newNode;
     newNode->next = first;
     first->previous = newNode;
     newNode->previous = tempNode;
    };
};


//
// Function:	 clear
// Abstract:	 deletes all of the elements in the list
// Parameters:	 none
// Return Value: none
//

template <class T> void typedList<T>::clear()
{
 if (first)
    {
     first->previous->next = NULL;
     while(first)
	{
         current = first;
	 first = first->next;
	 delete current->element;
 	 delete current;
	};
    };

 first = NULL;
 current = NULL;
};


//
// Function:	 reset
// Abstract:	 resets the current pointer to NULL, s.t. a call to operator++
//		 will result in the first element of the list being returned
// Parameters: 	 none
// Return Value: none
//

template <class T> void typedList<T>::reset()
{
 current = NULL;
};


//
// Function:	 operator++
// Abstract:	 returns a pointer to the next element in the list
// Parameters:	 none
// Return Value: T* - a pointer to the next element in the list
//

template <class T> T* typedList<T>::operator++()
{
 if (!current)
    {
     current = first;
    }
 else
    {
     current = current->next;
     if (current == first)
	current = NULL;
    };

 if (current)
    return current->element;
 return NULL;
};


//
// Function:	 getCurrent
// Abstract:	 returns a pointer to the current item
// Parameters:	 none
// Return Value: T* - a pointer to the current item
//

template <class T> T* typedList<T>::getCurrent()
{
 if (current)
     return current->element;
 return NULL;
};


//
// Function:	 removeCurrent
// Abstract:	 removes the current element in the list
// Parameters:	 none
// Return Value: T* - returns a pointer to the current item, so that the
//		 caller can delete the element
//

template <class T> T* typedList<T>::removeCurrent()
{
listMember<T>	*tempNode = current;
T		*tempElement = current->element;

 if (current)
    {
     if (current->next == current)
	{
	 current = NULL;
	 first = NULL;
        }
     else
	{
	 current->previous->next = current->next;
         current->next->previous = current->previous;
         current = current->next;
         if (current == first)
 	     current = NULL; 
         if (tempNode == first)
 	     first = first->next;
	};

     delete tempNode;
     return tempElement;
    };
 return NULL;
};

#endif

// end list.h



//============================================================================
//
// class template SET
//

template <class T> class set : public typedList<T>
{
protected:
    int	initNum;
public:
	set();					// constructor
	set(set<T>& x);				// copy constructor
//
set<T>&	operator[](int x);			// how many in the init
set<T>&	operator()(...);			// define the set
set<T>&	operator=(set<T>&);			// assignment operator
//
void	operator<(T x);				// add element to set
void	operator<(set<T>& x);			// add set to set
//
friend  set<T> operator+(set<T>& x, set<T>& y)		// union
friend  set<T> operator*(set<T>& x, set<T>& y);		// intersection
//
boolean operator>(T x);				// is a member of
boolean operator>(set<T> x);			// is a subset of
//
//
friend ostream& operator<<(ostream& s, set<T>& x);
void dumpSet();					// print the set
};


//
// methods
//

//
// Function:		set
// Abstract:		constructor
// Parameters:		none
// Return Value:	none
//

template <class T> set<T>::set()
			  : typedList<T>(), initNum(0)
{
};


//
// Function:		set
// Abstract:		copy constructor
// Parameters:		set<T>& - set being copied from
// Return Value:	none
//

template <class T> set<T>::set(set<T>& x)
		          : typedList<T>(), initNum(0)
{
 *this = x;
};



//
// Function:		[]
// Abstract:		set index initializer
// Parameters:		int x - the number of elements in the set initializer
// Return Value:	set<T>& - a reference to this set
//

template <class T> set<T>& set<T>::operator[](int x)
{
 initNum = x;
 return *this;
};



//
// Function:		()
// Abstract:		set definer
// Parameters:		members of the set, all of type <T>
// Return Value:	set<T>& - a reference to this set
//

template <class T> set<T>& set<T>::operator()(...)
{
va_list ap;
T	arg;
T	*argPtr;

 va_start(ap, 0);
 for (int i=0; i<initNum; i++)
     {
      arg = va_arg(ap, T);
      argPtr = new T;
      *argPtr = arg;
      append(argPtr);
     };
 va_end(ap);
 //
 initNum = 0;
 return *this;
};


//
// Function:		=
// Abstract:		assignment operator
// Parameters:		set<T>& x - the set to copy from
// Return Value:	none
//

template <class T> set<T>& set<T>::operator=(set<T>& x)
{
 clear();
 *this < x;
 return *this;
};



//
// Function:		<
// Abstract:		"add element to set"  operator
// Parameters:		T x - the element to add to the set
// Return Value:	none
//

template <class T> void set<T>::operator<(T x)
{
T	*elementPtr;
boolean	inSetFlag = boolean::FALSE;

 reset();
 while ((elementPtr = ++(*this)) != NULL)
    if (*elementPtr == x)
	inSetFlag = boolean::TRUE;

 if (!inSetFlag)
     {
      T	*argPtr;
      argPtr = new T;
      *argPtr = x;
      append(argPtr);
     };
};


//
// Function:		<
// Abstract:		"add set to set" operator
// Parameters:		set<T>& x - the set to add to this set
// Return Value:	none
//

template <class T> void set<T>::operator<(set<T>& x)
{
T	*memberPtr;

 while ((memberPtr = ++x) != NULL)
     (*this) < *memberPtr;
};


//
// Function:		U
// Abstract:		makes the union of two sets
// Parameters:		set<T>& x - the first set
//			set<T>& y - the second set
// Return Value:	set<T> - the set that is the union of the two sets
//

template <class T> set<T> operator+(set<T>& x, set<T>& y)
//template <class T> set<T> U(set<T>& x, set<T>& y)
{
set<T> z;

 z < x;
 z < y;
 return z;
};


//
// Function:		I
// Abstract:		makes the intersection of two sets
// Parameters:		set<T>& x - the first set
//			set<T>& y - the second set
// Return Value:	set<T> - the set that is the intersection of the 
//			two sets
//

template <class T> set<T> operator*(set<T>& x, set<T>& y)
{
set<T> 	z;
T	*xMemberPtr,
	*yMemberPtr;

 x.reset();
 while ((xMemberPtr = ++x) != NULL)
    {
     y.reset();
     while ((yMemberPtr = ++y) != NULL)
	if (*xMemberPtr == *yMemberPtr)
	    z < *xMemberPtr;
    };
 return z;
};


//		
// Function:		>
// Abstract:		"is a member of" operator
// Parameters:		T x - a data element of type 'T'
// Return Value:	TRUE is x is a member, FALSE otherwise
//

template <class T> boolean set<T>::operator>(T x)
{
T 	*memberPtr;
boolean	memberFlag = boolean::FALSE;

 reset();
 while((memberPtr = ++(*this)) != NULL)  
     if (*memberPtr == x)
         {
          memberFlag = boolean::TRUE;
          break;
        };
 return memberFlag;
};


//
// Function:		>
// Abstract:		"is a subset of" operator
// Parameters:		set<T> x - a set of elements of type 'T'
// Return Value:	TRUE is all elements in x are also in this set.
//			FALSE otherwise
//

template <class T> boolean set<T>::operator>(set<T> x)
{
T	*xMemberPtr,
	*memberPtr;
boolean	memberFlag,    
	subsetFlag = boolean::TRUE;

 x.reset();
 while ((xMemberPtr = ++x) != NULL)
    {
     reset();
     memberFlag = boolean::FALSE;
     while ((memberPtr = ++(*this)) != NULL)
	if (*xMemberPtr == *memberPtr)
	    {
	     memberFlag = boolean::TRUE;
	     break;
	    };
     if (!memberFlag)
        {
         subsetFlag = boolean::FALSE;
         break;
        };
    };
 return subsetFlag;
};


template <class T> ostream& operator<<(ostream& s, set<T>& x )
	{
	x.reset();
	s << "{ ";
	while(++(x))
		{
		s << *x.getCurrent() << " ";
		};
	return s << "}";
	}

template <class T> void set<T>::dumpSet()
{
 reset();
 cout << "{ ";
 while(++(*this))
    {
     cout << *getCurrent() << " ";
    };
 cout << "}" << endl;
};

#endif

// end set.h
