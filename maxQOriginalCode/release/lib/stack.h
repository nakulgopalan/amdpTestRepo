//======================================================================
//  File:        stack.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface of the stack
//               classes.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef STACK_H
#define STACK_H

#include <assert.h>
#include <iterator.h>
#include <vector.h>
#include <list.h>

//----------------------------------------------------------------------
//	class stack
//		abstract class - simply defines protocol for
//		stack operations
//----------------------------------------------------------------------

template <class T> class stack
{
public:
    virtual void  deleteAllValues() = 0;
    virtual int   isEmpty() const = 0;
    virtual T     pop() = 0;
    virtual void  push(T value) = 0;
    virtual T     top() const = 0;
};



//----------------------------------------------------------------------
//	class stackVector
//		stack implemented using vector
//		vector will grow as necessary to avoid overflow
//----------------------------------------------------------------------

template <class T> class stackVector : public stack<T>
{
public:
    // constructor requires a starting size
    stackVector(unsigned int size);
    stackVector(const stackVector & v);

    // stack operations

    virtual void     deleteAllValues();
    virtual int      isEmpty() const;
    virtual T        pop();
    virtual void     push(T value);
    virtual T        top() const;

    // assignment
    stackVector & operator = (const stackVector &);

protected:
    // data fields
    vector<T>        data;
    unsigned int     nextSlot;
};



//----------------------------------------------------------------------
//	class stackList
//		stack implemented using list operations
//----------------------------------------------------------------------

template <class T> class stackList : public stack<T>
{
public:
    // constructors
    stackList();
    stackList(const stackList & v);

    // stack operations
    virtual void  deleteAllValues();
    virtual int   isEmpty() const;
    virtual T     pop();
    virtual void  push(T value);
    virtual T     top() const;

protected:
    // data fields
    list<T>     data;
};

//----------------------------------------------------------------------
//	class stackVector implementation
//----------------------------------------------------------------------

template <class T> stackVector<T>::stackVector(unsigned int size)
    : data(size)
{
    // create and initialize a stack based on vectors
    // make sure stack is initially empty
    deleteAllValues();
}



template <class T> stackVector<T>::stackVector(const stackVector<T> & v)
    : data(v.data), nextSlot(v.nextSlot)
{
    // no further initialization
}



template <class T> void stackVector<T>::deleteAllValues()
{
    // clear all elements from stack, by setting
    // index to bottom of stack
    nextSlot = 0;
}



template <class T> int stackVector<T>::isEmpty() const
{
    // return true if stack is empty
    // if nextSlot points to bottom of stack
    // then there are no elements
    return nextSlot == 0;
}



template <class T> T stackVector<T>::pop()
{
    // return and remove the topmost element in the stack
    assert(!isEmpty());
    return data[--nextSlot];
}



template <class T> void stackVector<T>::push(T val)
{
    // push new value onto stack
    // if stack is too small, then increase the size
    if (nextSlot >= data.length())
	data.setSize(data.length() + 5);

    data[nextSlot++] = val;
}



template <class T> T stackVector<T>::top() const
{
    // return the topmost element in the stack
    assert(!isEmpty());
    return data[nextSlot - 1];
}



template <class T> stackVector<T> & stackVector<T>::operator =
    (const stackVector<T> & right)
{
    data     = right.data;
    nextSlot = right.nextSlot;
    return *this;
}



//----------------------------------------------------------------------
//	class stackList implementation
//----------------------------------------------------------------------

template <class T> stackList<T>::stackList()
{
    // create and initialize a stack based on lists
    // make sure stack is initially empty
    deleteAllValues();
}



template <class T> stackList<T>::stackList(const stackList<T> & v)
{
    // !!!WARNING: BUG!!!
    data.deleteAllValues();
}



template <class T> void stackList<T>::deleteAllValues()
{
    // clear all elements from stack, by setting
    // delete all values from list
    data.deleteAllValues();
}



template <class T> int stackList<T>::isEmpty() const
{
    // return true if stack is empty
    return data.isEmpty();
}



template <class T> T stackList<T>::pop()
{
    // return and remove the topmost element in the stack
    // get first element in list
    T result = data.firstElement();

    // remove element from list
    data.removeFirst();

    // return value
    return result;
}



template <class T> void stackList<T>::push(T val)
{
    // push new value onto stack
    data.add(val);
}



template <class T> T stackList<T>::top() const
{
    return data.firstElement();
}



#endif
