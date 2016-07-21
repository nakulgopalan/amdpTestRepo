//==================================================================
//	File: 		queue.h
//	Author: 	Timothy A. Budd
//	Platform:	Cfront
//	Description: 	Template Description of Abstract Queue ADT
//	Copyright (c) 1992 by Timothy A. Budd, All Rights Reserved
//	Permission granted for duplication if not for profit
//==================================================================

# ifndef queueH
# define queueH

template <class T>
class queue {
public:
	// protocol for queue operations
	virtual void 	deleteAllValues	() = 0;
	virtual T 	dequeue	() = 0;
	virtual void 	enqueue	(T value) = 0;
	virtual T 	front	() const = 0;
	virtual int 	isEmpty	() const = 0;
};

# include <vector.h>

template <class T> class queueVector : public queue<T> {
public:
	// constructor requires a starting size
	queueVector(unsigned int max);
	queueVector(const queueVector & x);

	// implement queue protocol
	virtual void 	deleteAllValues	();
	virtual T 	dequeue	();
	virtual void 	enqueue	(T value);
	virtual T 	front	() const;
	virtual int 	isEmpty	() const;

private:
	vector<T> 		data;
	const unsigned int 	max;
	unsigned int 		nextSlot;
	unsigned int 		nextUse;
};

# include <list.h>

//
//	class queueList
//		queue implemented using list operations
//

template <class T> class queueList : public queue<T> {
public:
	// constructors
	queueList();
	queueList(const queueList & v);

	// protocol for queue operations
	virtual void 	deleteAllValues	();
	virtual T 	dequeue	();
	virtual void 	enqueue	(T value);
	virtual T 	front	() const;
	virtual int 	isEmpty	() const;

private:
	doubleEndedList<T> data;
};

//==================================================================

# include <assert.h>

template <class T> queueVector<T>::queueVector(unsigned int size)
	: max(size), data(size)
{
	// make sure queue is initially empty
	deleteAllValues();
}

template <class T> queueVector<T>::queueVector(const queueVector<T> & v) :
	data(v.data), max(v.max), nextSlot(v.nextSlot), nextUse(v.nextUse)
{
	// no further initialization
}

template <class T> void queueVector<T>::deleteAllValues()
{
	nextSlot = 0;
	nextUse = 0;
}

template <class T> void queueVector<T>::enqueue(T val) 
{ 
	data[nextSlot++] = val;
	if (nextSlot >= max) 
		nextSlot = 0;
	// make sure queue has not overflowed
	assert(nextSlot != nextUse); 
}

template <class T> T queueVector<T>::front() const
{
	// can not return a value from an empty queue
	assert(! isEmpty());
	return data[nextUse]; 
}

template <class T> T queueVector<T>::dequeue() 
{
	// can not dequeue from an empty queue
	assert(! isEmpty());
	int dataloc = nextUse;
	nextUse++;
	if (nextUse >= max) 
		nextUse = 0;
	return data[dataloc]; 
}

template <class T> int queueVector<T>::isEmpty() const
{
	// queue is empty if next slot is
	// pointing to same location as next use
	return nextSlot == nextUse;
}

//==================================================================

template <class T>  queueList<T>::queueList() : data()
{
	// no further initialization
}

template <class T>  queueList<T>::queueList(const queueList<T> & v) 
	: data(v.data)
{
	// no further initialization
}

template <class T>  void queueList<T>::deleteAllValues()
{
	data.deleteAllValues();
}


template <class T>  void queueList<T>::enqueue(T value)
{
	data.addToEnd(value);
}

template <class T>  T queueList<T>::front() const
{
	return data.firstElement();
}

template <class T>  int queueList<T>::isEmpty() const
{
	return data.isEmpty();
}

template <class T> T queueList<T>::dequeue()
{ 
	T result = data.firstElement();
	data.removeFirst();
	return result; 
}

//
//	class queueRingBuffer -
//		implementation of queue
//		using ring buffers
//

template <class T> class queueRingBuffer : public queue<T> {

public:
	// constructor takes initial size
	queueRingBuffer(unsigned int);
	~queueRingBuffer();

	// protocol for queue operations
	virtual void 	deleteAllValues	();
	virtual T 	dequeue	();
	virtual void 	enqueue	(T value);
	virtual T 	front	() /* const */;
	virtual int	isEmpty	() /* const */;

private:
	BuddLink<T> * lastFilled;
	BuddLink<T> * lastFree;
};

template <class T> inline void queueRingBuffer<T>::deleteAllValues()
{
	lastFree = lastFilled;
}

template <class T> inline int queueRingBuffer<T>::isEmpty() /* const */
{
	// queue is empty if pointers are equal
	return lastFree == lastFilled;
}

template <class T> queueRingBuffer<T>::~queueRingBuffer()
{	BuddLink<T> * p = lastFree;
	BuddLink<T> * next;

	// walk around the circle deleting nodes
	while (p->ptrToNextLink != lastFree) {
		next = p->ptrToNextLink;
		delete p;
		p = next;
		}
}

template <class T> queueRingBuffer<T>::queueRingBuffer(unsigned int max) 
{
	// create the first link
	T initialvalue;
	lastFree = new BuddLink<T>(initialvalue, 0);
	assert(lastFree != 0);
	lastFilled = lastFree;
	// make value point to itself
	lastFilled->ptrToNextLink = lastFilled;
	// now add the remainder of the elements
	while (max-- > 0) 
		lastFilled->insert(initialvalue);
}

template <class T> T queueRingBuffer<T>::dequeue()
{
	// cannot dequeue from empty queue
	assert(! isEmpty());
	// advance last free position
	lastFree = lastFree->ptrToNextLink;
	// return value stored in last free position
	return lastFree->value;
}

template <class T> T queueRingBuffer<T>::front() /* const */
{
	// cannot take front of empty queue
	assert(! isEmpty());
	BuddLink<T> * ptrToFront = lastFree->ptrToNextLink;
	return ptrToFront->value;
}

template <class T> void queueRingBuffer<T>::enqueue(T val)
{
	// first check for potential overflow
	if (lastFilled->ptrToNextLink == lastFree)
		lastFilled = lastFilled->insert(val);
	else {
		// simply advance the last filled pointer
		lastFilled = lastFilled->ptrToNextLink;
		lastFilled->value = val;
		}
}

# endif

