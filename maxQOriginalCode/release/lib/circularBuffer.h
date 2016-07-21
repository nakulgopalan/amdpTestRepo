//  -*- C++  -*-
//
// Implementation of a circular buffer including iterator support.
//
//

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <assert.h>
#include <iterator.h>
#include <vector.h>

template <class T> class CBiterator;

template <class T>
class circularBuffer
{
public:
  circularBuffer(int n = 0): data(n + 1), last(0), first(0) {};
  // we allocate an extra entry, because we need a dividing line
  // between the start and end cells.

  // no destructor, since we maintain no pointers.  The destructor for
  // data will handle releasing the vector data items.

  // add a new element to the buffer
  void       push(T value);
  // delete an element from the buffer
  T          pop();
  // test if the buffer is full
  int        isFull();
  // test if buffer is empty
  int        isEmpty();
  // access by subscript
  T &        operator [] (unsigned int index);
  // access the last element
  T &        lastElement();

  // return number of elements
  int        length();

  friend class CBiterator<T>;

protected:
  int         nextIndex(int i);
  int         lastIndex(int i);
  vector<T>   data;
  int         last;		// index of last element in the buffer
  int         first;		// index of first element in the
				// buffer
  // invariant:  last == first implies empty buffer
  //   last + 1  == first implies full buffer
  // this means that we can't use all of the cells in the array to
  // store data, because otherwise we wouldn't know whether
  // last==first meant the buffer was full or empty.
};

template <class T>
void circularBuffer<T>::push(T value)
{
  assert(!isFull());
  data[last] = value;
  last = nextIndex(last);
}

template <class T>
T circularBuffer<T>::pop()
{
  assert(!isEmpty());
  T result = data[first];
  first = nextIndex(first);
  return result;
}

template<class T>
int circularBuffer<T>::isFull()
{
  return nextIndex(last) == first;
}

template<class T>
int circularBuffer<T>::isEmpty()
{
  return first == last;
}

template<class T>
int circularBuffer<T>::nextIndex(int i)
{
  int len = data.length();
  i++;
  if (i >= len) i = 0;
  return i;
}

template<class T>
int circularBuffer<T>::lastIndex(int i)
{
  i--;
  if (i < 0) i = data.length() - 1;
  return i;
}

template <class T>
T & circularBuffer<T>::operator [] (unsigned int index)
{
  int i = index + first;
  if (first <= last) {
    assert (i < last);
  }
  else if (i >= data.length()) {
    i = i % data.length();
    assert (i < last);
  }
  return data[i];
}

template <class T>
T & circularBuffer<T>::lastElement ()
{
  assert(!isEmpty());
  int i = lastIndex(last);
  return data[i];
}


template <class T>
int circularBuffer<T>::length ()
{
  // return number of elements currently stored in the buffer
  int top = last;
  if (top < first) top += data.length();
  return top - first;
}


/*
template <class T>
class CBiterator : public iterator<T>
{
public:
  CBiterator(circularBuffer<T> & thecb);

  int init();
  // used with -- to reverse enumerate the buffer
  int initReverse();

  // change current element
  void operator =(T newValue);

  // return T if there is a current element
  int operator !();
  
  // return reference to current element
  T & operator ()();

  // advance to next element
  int  operator ++();


  // backup to previous element
  int  operator --();


protected:
  int index;
  circularBuffer<T> & cb;
};

template <class T>
CBiterator<T>::CBiterator (circularBuffer<T> & thecb)
: cb(thecb), index(thecb.first)
{
  init();
};


template <class T>
int CBiterator<T>::init()
{
  index = cb.first;
  return !(*this);
}

template <class T>
int CBiterator<T>::initReverse()
{
  index = cb.lastIndex(cb.last);
  return !(*this);
}

template <class T>
T & CBiterator<T>::operator ()()
{
  assert(!(*this));
  return cb.data[index];
}

template <class T>
int CBiterator<T>::operator !()
{
  if (cb.first <= cb.last) {
    return (cb.first <= index) && (index < cb.last);
  }
  else {
    return (cb.first <= index) || (index < cb.last);
  }
}

template <class T>
int CBiterator<T>::operator ++()
{
  index = cb.nextIndex(index);
  return !(*this);
}

template <class T>
int CBiterator<T>::operator --()
{
  index = cb.lastIndex(index);
  return !(*this);
}

template <class T>
void CBiterator<T>::operator =(T newValue)
{
  assert(!(*this));
  cb.data[index] = newValue;
}

*/
#endif
