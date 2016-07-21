//======================================================================
//  File:        list.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface of the list
//               classes.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef LIST_H
#define LIST_H

#include <assert.h>
#include "iterator.h"
#include <iostream.h>

//----------------------------------------------------------------------
//	class list
//		arbitrary size lists of elements
//		permits insertion and removal only
//		from the front of the list
//----------------------------------------------------------------------

template <class T> class BuddLink;
template <class T> class listIterator;

template <class T> class list
{
public:
    // constructors
    list();
    list(const list<T> & source);
    virtual ~list();

    // operations
    void  	  add(T value);
    virtual void  deleteAllValues();
    list *     	  duplicate() const;
    T &           firstElement() const;
    virtual int   includes(T value) const;
    int           isEmpty() const;
    virtual void  removeFirst();
    unsigned int  length() const;
    void          reverse();
    T &           operator [](unsigned int);
    T             pop();
    int           operator == (list<T> & other);  
    void          nconc(list<T> & other);
    void          prepend(list<T> & other);

protected:
    // data field
    BuddLink<T> *     ptrToFirstLink;

    // friends
    friend class  listIterator<T>;
};



//----------------------------------------------------------------------
//	class BuddLink
//		facilitator for operations on lists
//		maintains a single element for the linked list class
//----------------------------------------------------------------------

template <class T> class BuddLink
{
public:
    // insert a new element following the current value
    BuddLink<T> * insert(T val);

private:
    // constructor
    BuddLink(T linkValue, BuddLink * nextPtr);
    BuddLink(const BuddLink &);

    // duplicate
    BuddLink *    duplicate() const;

    // data areas
    T            value;
    BuddLink *    ptrToNextLink;

    // friends
    friend class list<T>;
    friend class listIterator<T>;
};



//----------------------------------------------------------------------
//	class listIterator
//		implements iterator protocol for linked lists
//		also permits removal and addition of elements
//----------------------------------------------------------------------
                                              
template <class T> class listIterator : public iterator<T>
{
public:
    // constructor
    listIterator(list<T> & aList);
    listIterator(const listIterator &);

    // iterator protocol
    virtual int  init();
    virtual T &  operator ()();
    virtual int  operator !();
    virtual int  operator ++();
    virtual void operator =(T value);

    // new methods specific to list iterators
    void         removeCurrent();
    void         addBefore(T newValue);
    void         addAfter(T newValue);
    void         cloneFrom(listIterator &);

protected:
    // data areas
    BuddLink<T> *    currentLink;
    BuddLink<T> *    previousLink;
    list<T> &    theList;
};


//----------------------------------------------------------------------
//	class orderedList
//		a list structure where each element
//		is maintained in sequence based on the
//		less-than comparison operator
//----------------------------------------------------------------------

template <class T> class orderedList : public list<T>
{
public:
    // change the addition method
    void  add(T value);
};

//----------------------------------------------------------------------
//	class doubleEndedList
//		a variation on lists - can add elements to end
//		as well as to front
//----------------------------------------------------------------------

template <class T> class doubleEndedList : public list<T> {
public:
	// constructors
	doubleEndedList ();
	doubleEndedList (const doubleEndedList &);

	// override the following methods from class list
	virtual void 	add (T value);
	virtual void	deleteAllValues ();
	virtual void	removeFirst ();

	// add a new element to the end of the list
	void		addToEnd (T value);

protected:
	// data area -- link to end
	BuddLink<T> * ptrToLastLink;
};

//----------------------------------------------------------------------
//	class list implementation
//----------------------------------------------------------------------

template <class T> list<T>::list() : ptrToFirstLink(0)
{
    // no further initialization
}



template <class T> list<T>::~list()
{
    // empty all elements from the list
    deleteAllValues();
}



template <class T> void list<T>::add(T val)
{
    // add a new value to the front of a linked list
    ptrToFirstLink = new BuddLink<T>(val, ptrToFirstLink);
    assert(ptrToFirstLink != 0);
}



template <class T> void list<T>::deleteAllValues()
{
    // clear all Buddlinks from the list.  Note if T is of type
    // pointer, this does NOT delete the objects that are pointed to. 
    // The caller is responsible for doing that. 
    BuddLink<T> * next;

    for (BuddLink<T> * p = ptrToFirstLink; p != 0; p = next)
    {
	// delete the element pointed to by p
	next = p->ptrToNextLink;
	p->ptrToNextLink = 0;
	delete p;
    }

    // mark that the list contains no elements
    ptrToFirstLink = 0;
}


template <class T> list<T> * list<T>::duplicate() const
{
    list<T> * newlist = new list<T>;
    assert(newlist != 0);

    // copy list
    if (ptrToFirstLink)
        newlist->ptrToFirstLink = ptrToFirstLink->duplicate();

    // return the new list
    return newlist;
}

template <class T> list<T>::list(const list<T> & source)
{
    // duplicate elements from source list
    if (source.isEmpty())
	ptrToFirstLink = 0;
    else
    {
	BuddLink<T> * firstLink = source.ptrToFirstLink;
	ptrToFirstLink = firstLink->duplicate();
    }
}

template <class T> T & list<T>::firstElement() const
{
    // return first value in list
    assert(ptrToFirstLink != 0);
    return ptrToFirstLink->value;
}

template <class T> T list<T>::pop() 
{
  T answer = firstElement();
  removeFirst();
  return answer;
}
 
template <class T> int list<T>::includes(T v) const
{
    // loop to test each element
    for (BuddLink<T> * p = ptrToFirstLink; p; p = p->ptrToNextLink)
	if (v == p->value)
	    return 1;

    // not found
    return 0;
}



template <class T> int list<T>::isEmpty() const
{
    // test to see if the list is empty
    // list is empty if the pointer to the first link is null
    return ptrToFirstLink == 0;
}



template <class T> void list<T>::removeFirst()
{
    // make sure there is a first element
    assert(ptrToFirstLink != 0);

    // save pointer to the removed node
    BuddLink<T> * p = ptrToFirstLink;

    // reassign the ptrToFirstLink node
    ptrToFirstLink = p->ptrToNextLink;

    // recover memory used by the first element
    delete p;
}

template <class T> void list<T>::reverse() 
{
  // reverse the list.  We do this by popping each element off of the
  // list and pushing it onto the result list.  Finally, the result is
  // assigned back to be the original list.
  BuddLink<T> * result = 0;
  BuddLink<T> * current;

  while (ptrToFirstLink) {
    // pop first element into current
    current = ptrToFirstLink;
    ptrToFirstLink = current->ptrToNextLink;
    // push current onto result
    current->ptrToNextLink = result;
    result = current;
  }
  ptrToFirstLink = result;
}

template <class T> unsigned int list<T>::length() const
{
  // compute the length of the list
  unsigned int answer = 0;
  BuddLink<T> * ptr = ptrToFirstLink;

  while (ptr) {
    answer++;
    ptr = ptr->ptrToNextLink;
  }
  return answer;
}


template <class T> 
T & list<T>::operator [] (unsigned int index) 
{
  // return the nth element of a list, treating the list as an indexable structure. 
  // returning a reference allows assignment to the value.
  BuddLink<T> * ptr = ptrToFirstLink;
  while (ptr && (index > 0)) {
    index--;
    ptr = ptr->ptrToNextLink;
  }
  assert(ptr);
  return ptr->value;
}

  
template <class T>
int list<T>::operator == (list<T> & other)
{
  // two lists are equal if all of their elements are equal
  BuddLink<T> * ourptr = ptrToFirstLink;
  BuddLink<T> * otherptr = other.ptrToFirstLink;
  for (; ourptr && otherptr;
       ourptr = ourptr->ptrToNextLink, otherptr = otherptr->ptrToNextLink) {
    if (!(ourptr->value == otherptr->value)) return 0;
  }
  // both lists should have ended at the same time.  
  return (!ourptr && !otherptr);
}

template <class T>
void list<T>::nconc (list<T> & other)
{
  // attach all BudLink's in other onto the end of our list.
  // answer zero out the other list so that it is empty.

  if (ptrToFirstLink) {
    // find the last BuddLink in our list and point it to the list of
    // links from other.  Zero out the other list.
    if (other.ptrToFirstLink) {
      BuddLink<T> * ourptr = ptrToFirstLink;
      if (ourptr) {
	for (; ourptr->ptrToNextLink; ourptr = ourptr->ptrToNextLink);
	// ourptr is now the last link
	ourptr->ptrToNextLink = other.ptrToFirstLink;
	other.ptrToFirstLink = 0;
      }
      else {
	ptrToFirstLink = other.ptrToFirstLink;
	other.ptrToFirstLink = 0;
      }
    }
    else {
      // other is already empty, so do nothing
    }
  }
  else {
    ptrToFirstLink = other.ptrToFirstLink;
    other.ptrToFirstLink = 0;
  }
}

template <class T>
void list<T>::prepend (list<T> & other)
{
  // attach all of BuddLink's in other onto the head of ourlist in the
  // same order.  Hence if our list was (1 2 3) and other is (a b c)
  // after this operation our list will be (a b c 1 2 3) and other
  // will be empty.
  
  // find the last BuddLink in other and point it to our first link
  // (if any).
  if (other.ptrToFirstLink) {
    BuddLink<T> * otherptr = other.ptrToFirstLink;
    for (; otherptr->ptrToNextLink; otherptr = otherptr->ptrToNextLink);
    otherptr->ptrToNextLink = ptrToFirstLink;
    ptrToFirstLink = other.ptrToFirstLink;
    other.ptrToFirstLink = 0;
  }
}

//----------------------------------------------------------------------
//	class link implementation
//----------------------------------------------------------------------

template <class T> BuddLink<T> * BuddLink<T>::insert(T val)
{
    // insert a new link after current node
    ptrToNextLink = new BuddLink<T>(val, ptrToNextLink);

    // check that allocation was successful
    assert(ptrToNextLink != 0);
    return ptrToNextLink;
}

template <class T> BuddLink<T>::BuddLink(T val, BuddLink<T> * nxt)
    : value(val), ptrToNextLink(nxt)
{
    // create and initialize a new link field
}

template <class T> BuddLink<T>::BuddLink(const BuddLink<T> & source)
	: value(source.value), ptrToNextLink(source.ptrToNextLink)
{
}

template <class T> BuddLink<T> * BuddLink<T>::duplicate() const
{
    BuddLink<T> * newlink;

    // if there is a next field. copy remainder of list
    if (ptrToNextLink != 0)
	newlink = new BuddLink<T>(value, ptrToNextLink->duplicate());
    else
	newlink = new BuddLink<T>(value, 0);

    // check that allocation was successful
    //assert(newlink != 0);
    return newlink;
}

//----------------------------------------------------------------------
//	class listIterator implementation
//----------------------------------------------------------------------

template <class T> listIterator<T>::listIterator(list<T> & aList)
    : theList(aList)
{
    // create and initialize a new list
    init();
}

template <class T> listIterator<T>::listIterator(const listIterator<T> & x)
	: theList(x.theList)
{
	init();
}

template <class T> int listIterator<T>::init()
{
    // set the iterator to the first element in the list
    previousLink = 0;
    currentLink = theList.ptrToFirstLink;
    return currentLink != 0;
}



template <class T> T & listIterator<T>::operator ()()
{
    // return value of current element
    // check to see if there is a current element
    assert(currentLink != 0);

    // return value associated with current element
    return currentLink->value;
}



template <class T> int listIterator<T>::operator !()
{
    // test for termination of the iterator
    // if current link references a removed value,
    // update current to point to next position
    if (currentLink == 0)
	if (previousLink != 0)
	    currentLink = previousLink->ptrToNextLink;

    // now see if currentLink is valid
    return currentLink != 0;
}



template <class T> int listIterator<T>::operator ++()
{
    // move current pointer to nect element
    // special processing if current link is deleted
    if (currentLink == 0)
    {
	if (previousLink == 0)
	    currentLink = theList.ptrToFirstLink;
	else
	    currentLink = previousLink->ptrToNextLink;
    }
    else
    {
	// advance pointer
	previousLink = currentLink;
	currentLink = currentLink->ptrToNextLink;
    }

    // return true if we have a valid current element
    return currentLink != 0;
}



template <class T> void listIterator<T>::operator =(T val)
{
    // modify value of current element
    assert(currentLink != 0);

    // modify value of the current link
    currentLink->value = val;
}



template <class T> void listIterator<T>::removeCurrent()
{
    // remove the current element from a list
    // make sure there is a current element
    assert(currentLink != 0);

    // case 1, removing first element
    if (previousLink == 0)
	theList.ptrToFirstLink = currentLink->ptrToNextLink;

    // case 2, not removing the first element
    else
	previousLink->ptrToNextLink = currentLink->ptrToNextLink;

    // delete current node
    delete currentLink;

    // and set current pointer to null
    currentLink = 0;
}



template <class T> void listIterator<T>::addBefore(T val)
{
    // add a new element to list before current value
    // case 1, not at start
    if (previousLink)
	previousLink = previousLink->insert(val);

    // case 2, at start of list
    else
    {
	//theList.list<T>::add(val);
	// g++ change
	theList.add(val);
	previousLink = theList.ptrToFirstLink;
	currentLink = previousLink->ptrToNextLink;
    }
}



template <class T> void listIterator<T>::addAfter(T val)
{
    // a a new element to list after current value
    // case 1, not at start
    if (currentLink != 0)
	currentLink->insert(val);

    // case 2, at end of list
    else if (previousLink != 0)
	currentLink = previousLink->insert(val);

    // case 3, start of list
    else
	//theList.list<T>::add(val);
	// g++ change
	theList.add(val);
}


template <class T> void listIterator<T>::cloneFrom (listIterator<T> & old)
{
  // old is another iterator of the same type.
  // make it point to the same spot as self
  assert (&theList == &old.theList);
  // copy the values of the pointers from old
  currentLink = old.currentLink;
  previousLink = old.previousLink;
}


//----------------------------------------------------------------------
//	class orderedList implementation
//----------------------------------------------------------------------

template <class T> void orderedList<T>::add(T val)
{
    // add a new value to an ordered list
    // loop over values smaller than current
    listIterator<T> itr(*this);

    for (itr.init(); !itr; ++itr)
        if (val < itr())
        {
            // found location to insert value
            itr.addBefore(val);
            return;
        }

    // add to end of list if not yet inserted
    itr.addBefore(val);
}

//----------------------------------------------------------------------
//	class doubleEndedList implementation
//----------------------------------------------------------------------

template <class T> doubleEndedList<T>::doubleEndedList() 
	: list<T>()
{
	ptrToLastLink = 0;
}

template <class T> doubleEndedList<T>::
	doubleEndedList(const doubleEndedList<T> & x)
	: list<T>(x), ptrToLastLink(x.ptrToLastLink)
{
}

template <class T> void doubleEndedList<T>::add(T val)
{	// add an element to the front of a double ended list
	// only need to handle addition to empty list

	if (isEmpty()) {
		list<T>::add(val);
		ptrToLastLink = ptrToFirstLink;
		}
	else
		list<T>::add(val);
}

template <class T> void doubleEndedList<T>::addToEnd(T val)
{	
	// add a new element to the end of a double ended list
	// if there is an end, add to it

	if (ptrToLastLink != 0)
		ptrToLastLink = ptrToLastLink->insert(val);

	// otherwise, just add to front
	else
		add(val);
}

template <class T> void doubleEndedList<T>::deleteAllValues()
{	
	// delete all values from collection
	// invoke the list method to do the actual work

	list<T>::deleteAllValues();

	// then set the pointer to the last element to zero
	
	ptrToLastLink = 0;
}

template <class T> void doubleEndedList<T>::removeFirst()
{
	// remove the first element from double ended list
	// invoke the method from list to do the work

	list<T>::removeFirst();

	// only do something different if we removed last element
	
	if (isEmpty())
		ptrToLastLink = 0;
}

//////////////////////////////////////////////////////////////////////
//
// Generic function for constructing lists.  We include all sizes up
// to 16 elements.
//
template <class T>
list<T> * makeList(T arg1)
{
  list<T>* answer = new list<T>;
  answer->add(arg1);
  return answer;
}
template <class T>
list<T> * makeList(T arg1, T arg2)
{
  list<T>* answer = new list<T>;
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

template <class T>
list<T> * makeList(T arg1, T arg2, T arg3)
{
  list<T>* answer = new list<T>;
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4)
{
  list<T>* answer = new list<T>;
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5)
{
  list<T>* answer = new list<T>;
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6)
{
  list<T>* answer = new list<T>;
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6, T arg7)
{
  list<T>* answer = new list<T>;
  answer->add(arg7);
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6, T arg7,
		   T arg8)
{
  list<T>* answer = new list<T>;
  answer->add(arg8);
  answer->add(arg7);
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6, T arg7,
		   T arg8, T arg9)
{
  list<T>* answer = new list<T>;
  answer->add(arg9);
  answer->add(arg8);
  answer->add(arg7);
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6, T arg7,
		   T arg8, T arg9, T arg10)
{
  list<T>* answer = new list<T>;
  answer->add(arg10);
  answer->add(arg9);
  answer->add(arg8);
  answer->add(arg7);
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6, T arg7,
		   T arg8, T arg9, T arg10, T arg11)
{
  list<T>* answer = new list<T>;
  answer->add(arg11);
  answer->add(arg10);
  answer->add(arg9);
  answer->add(arg8);
  answer->add(arg7);
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6, T arg7,
		   T arg8, T arg9, T arg10, T arg11, T arg12)
{
  list<T>* answer = new list<T>;
  answer->add(arg12);
  answer->add(arg11);
  answer->add(arg10);
  answer->add(arg9);
  answer->add(arg8);
  answer->add(arg7);
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}
template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6, T arg7,
		   T arg8, T arg9, T arg10, T arg11, T arg12, T arg13)
{
  list<T>* answer = new list<T>;
  answer->add(arg13);
  answer->add(arg12);
  answer->add(arg11);
  answer->add(arg10);
  answer->add(arg9);
  answer->add(arg8);
  answer->add(arg7);
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}
template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6, T arg7,
		   T arg8, T arg9, T arg10, T arg11, T arg12, T arg13,
		   T arg14)
{
  list<T>* answer = new list<T>;
  answer->add(arg14);
  answer->add(arg13);
  answer->add(arg12);
  answer->add(arg11);
  answer->add(arg10);
  answer->add(arg9);
  answer->add(arg8);
  answer->add(arg7);
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}
template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6, T arg7,
		   T arg8, T arg9, T arg10, T arg11, T arg12, T arg13,
		   T arg14, T arg15)
{
  list<T>* answer = new list<T>;
  answer->add(arg15);
  answer->add(arg14);
  answer->add(arg13);
  answer->add(arg12);
  answer->add(arg11);
  answer->add(arg10);
  answer->add(arg9);
  answer->add(arg8);
  answer->add(arg7);
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}
template <class T>
list<T> * makeList(T arg1, T arg2, T arg3, T arg4, T arg5, T arg6, T arg7,
		   T arg8, T arg9, T arg10, T arg11, T arg12, T arg13,
		   T arg14, T arg15, T arg16)
{
  list<T>* answer = new list<T>;
  answer->add(arg16);
  answer->add(arg15);
  answer->add(arg14);
  answer->add(arg13);
  answer->add(arg12);
  answer->add(arg11);
  answer->add(arg10);
  answer->add(arg9);
  answer->add(arg8);
  answer->add(arg7);
  answer->add(arg6);
  answer->add(arg5);
  answer->add(arg4);
  answer->add(arg3);
  answer->add(arg2);
  answer->add(arg1);
  return answer;
}

//
//	==========insertion sort -- this is done differently by
//		cfront and g++
#ifdef __GNUG__

#include "list.c"

#endif

#ifndef __GNUG__

#include "vector.h"

template <class T> void listInsertionSort(vector<T> & v);
template <class T> ostream & operator << (ostream & ostr, list<T> & target);
template <class T> istream & operator >> (istream & istr, list<T> & target);

#endif


#endif
