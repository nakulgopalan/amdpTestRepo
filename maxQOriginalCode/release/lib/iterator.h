//======================================================================
//  File:        iterator.h
//  Author:      Timothy A. Budd
//  Description: This file contains the iterator template class
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef ITERATOR_H
#define ITERATOR_H



//----------------------------------------------------------------------
//	class iterator
//		define the protocol to be used by all iterators
//		subclasses must implement each of the five
//		iterator methods
//----------------------------------------------------------------------

template <class T> class iterator
{
public:
    // initialization
    virtual int  init() = 0;

    // test if there is a current element
    virtual int  operator !() = 0;

    // current element
    virtual T &  operator ()() = 0;

    // find next element
    virtual int  operator ++() = 0;

    // change current element
    virtual void operator =(T newValue) = 0;
};

class rangeIterator: public iterator<int>
{
public:
  rangeIterator(int from = 0, int to = -1): low(from), high(to),
    current(from) {};
  rangeIterator(const rangeIterator & old): low(old.low), high(old.high), current(old.current)
    {};
  virtual int init() { current = low; return !(*this); };
  virtual int operator !() { return (current <= high); };
  virtual int & operator () () { return current; };
  virtual int operator++() { current++; return !(*this); };
  virtual void operator = (int newvalue) { current = newvalue; };
  
protected:
  int low, high;
  int current;
};

#endif
