// -*- C++ -*-
//
//
// deltaheap.h
//
// A DeltaHeap is a combination of a hash table and a heap.  It permits
// hashed access to elements of the heap.  These elements can be
// inserted, deleted, and as a subsidiary operation, they can have
// their priorities modified. 
//
//

#include "priorque.h"
#include "hash.h"
#include "table.h"

template <class T> 
class DeltaHeap 
{
public:
  // constructors
  DeltaHeap(unsigned int maxsize, unsigned int (*f)(const T &));
  // size of heap, hash function.
  DeltaHeap(const DeltaHeap<T> &);

  // modified priority queue protocol
  virtual void   add(T item, float priority);
  virtual int    isEmpty() const;
  virtual T      min() const;
  virtual void   removeMin();
  virtual T      popMin();
  virtual float  minPriority() const;  // return priority of min element
  
  // delete all values
  void           deleteAllValues();
  
  // return the size
  virtual int    size() const { return heapsize; };
  // set the size
  virtual int    setSize(unsigned int s) {
    if (s < heapsize) heapsize = s;  // silently truncate heap
    data.setSize(s);
    heapmax = s;
    return s;
  };
  
protected:
  class DeltaHeapCell
  {
  public:
    DeltaHeapCell(T c, float p): priority(p),
				 contents(c) {};

    unsigned int  index;  // index into data for the current position
    // of this cell
    float         priority;
    T             contents;
  };
  
  // cells are allocated on the C++ heap.  We are responsible for
  // cleaning them up.  They should never been returned to the caller.

  vector<DeltaHeapCell<T> *>  data;   // vector that stores the heap
  unsigned int   heapmax;    // maximize number of elements in heap
  unsigned int   heapsize;   // current number of elements in heap
  
  table<T, DeltaHeapCell<T> *>  HT;  // hash table.   Given an object, we
				// will return its DeltaHeapCell. 
  void buildHeap(unsigned int position);

};



//----------------------------------------------------------------------
//	class DeltaHeap implementation
//----------------------------------------------------------------------

template <class T> DeltaHeap<T>::DeltaHeap(unsigned int maxsize,
					   unsigned int (*f)(const T &))
    : data(maxsize), heapmax(maxsize), heapsize(0), HT(maxsize, f)

{
    // no further initialization
}



template <class T> DeltaHeap<T>::DeltaHeap(const DeltaHeap<T> & source)
	: data(source.data), heapmax(source.heapmax), heapsize(source.heapsize)
{
    data     = source.data;
    heapmax  = source.heapmax;
    heapsize = source.heapsize;
    HT       = source.HT;
}

template <class T> void DeltaHeap<T>::add(T newItem, float priority)
{
  // check if newItem is already in the heap.  If not, add it.  If so,
  // promote it if the new priority is < current priority.
  DeltaHeapCell<T> * cell = 0;
  unsigned int position = 0;

  if (HT.includesKey(newItem)) {
    cell = HT[newItem];
    // if the new priority is worse than the old, then return without
    // doing anything.
    if (priority >= cell->priority) {
      cout << " DeltaHeap::add Old priority was better, no change" << endl;
      return;
    }
    cout << "  DeltaHeap::add Promoting priority" << endl;
    // promote this cell
    cell->priority = priority;
    position = cell->index;
  }
  else {
    cout << "  DeltaHeap::add Adding new element" << endl;
    // does not already exist in heap, so add it.
    cell = new DeltaHeapCell<T>(newItem, priority);
    HT[newItem] = cell;
  
    // add a new element to the heap
    // make sure there is room for new element
    if (heapsize + 1 >= heapmax)
      {
	// grow heap by 50% each time. 
	int newSize = (int)(data.length() * 1.5);
	data.setSize(newSize);
	heapmax = newSize;
      }
    
    // value starts out in last position
    position = heapsize++;
  
  }

  // inv: position <= 0 and < heapmax
  // cell is a pointer to the item
  // priority is the priority of the item
  
  // now percolate up
  while (position > 0 && priority < data[(position-1)/2]->priority)
    {
      // item has better priority than its parent, so parent drops to
      // this level
      data[position] = data[(position-1)/2];
      data[position]->index = position;   // update position of parent
      
      // inv: tree rooted at "position" is a heap
      position = (position - 1) / 2;
    }
  
  // found location, add new element
  data[position] = cell;
  cell->index = position;
  
  // inv: tree rooted at position is a heap
  // inv: data holds a heap
  cout << "index   item  priority" << endl;
  for (int ii = 0; ii < heapsize; ii++)
    cout << ii << " " << data[ii]->contents << " " << data[ii]->priority << endl;
} 



template <class T> int DeltaHeap<T>::isEmpty() const
{
    // heap is empty if heapsize is zero
    return heapsize == 0;
}



template <class T> T DeltaHeap<T>::min() const
{
  // smallest item is first element in vector
  return data[0]->contents;
}

template <class T> float DeltaHeap<T>::minPriority() const
{
  // smallest item is first element in vector, return its priority
  return data[0]->priority;
}


template <class T> void DeltaHeap<T>::buildHeap(unsigned int position)
{
  // rebuild the heap
  DeltaHeapCell<T> * cell = data[position];

  while (position < heapsize)
    {
      // replace position with the smaller of the
      // two children, or the last element
      unsigned int childpos = position * 2 + 1;

      if (childpos < heapsize)
        {
	  if ((childpos + 1 < heapsize) &&
	      data[childpos + 1]->priority  < data[childpos]->priority)
	    childpos += 1;
	  
	  // inv: childpos indexes the smaller of two children
	  if (cell->priority < data[childpos]->priority)
            {
	      // found right location; may be a no-op
	      data[position] = cell;  
	      cell->index = position;
	      return;
            }
	  else
            {
	      data[position] = data[childpos];
	      data[position]->index = position;
	      position = childpos;
	      // recur and keep moving down
            }
        }
      else
        {
	  // childpos indexes >= heapsize.  Hence, there are no children
	  data[position] = cell;
	  cell->index = position;
	  return;
        }
    }
}



template <class T> void DeltaHeap<T>::removeMin()
{
  // remove the smallest element from a heap
  // move the last element into the first position
  DeltaHeapCell<T> * cell = data[0];
  HT.removeKey(cell->contents);
  delete cell;

  data[0] = data[--heapsize];
  data[0]->index = 0;

  // then move into position
  buildHeap(0);

  cout << "index   item  priority" << endl;
  for (int ii = 0; ii < heapsize; ii++)
    cout << ii << " " << data[ii]->contents << " " << data[ii]->priority << endl;

}

template <class T> T DeltaHeap<T>::popMin()
{
    // return and remove the smallest element
  T result = min();
  removeMin();
  return result;
}


template <class T> void DeltaHeap<T>::deleteAllValues()
{
  for (int i = 0; i < heapsize; i++) {
    DeltaHeapCell<T> * cell = data[i];
    HT.removeKey(cell->contents);
    delete cell;
  }
  // heap is empty if heapsize is zero
  heapsize = 0;
}

