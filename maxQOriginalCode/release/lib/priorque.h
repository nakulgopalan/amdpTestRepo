//======================================================================
//  File:        priorque.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface of the priorityQueue
//               classes.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef PRIORQUE_H
#define PRIORQUE_H

#include <assert.h>
#include <list.h>
#include <vector.h>
#include <tree.h>

// 
// tgd: This code assumes that the objects to be placed in the heap
// can be compared using operator <.
//



//----------------------------------------------------------------------
//	class priorityQueue
//		ordered collection of values permitting
//              rapid removal of smallest value
//----------------------------------------------------------------------

template <class T> class priorityQueue
{
public:
    // add a value to the collection
    virtual void   add(T value) = 0;

    // return and remove the smallest element from the collection
    T              deleteMin();

    // indicate whether there are elements in the collection
    virtual int    isEmpty() const = 0;

    // return the smallest element in the collection
    virtual T      min() const = 0;
     
    // remove the smallest element from the collection
    virtual void   removeMin() = 0;
};



//----------------------------------------------------------------------
//	class priorityQueueList
//		implement priority queue protocol
//              using a linked list structure
//----------------------------------------------------------------------

template <class T> class priorityQueueList : public priorityQueue<T>
{
public:
    // constructors
    priorityQueueList();
    priorityQueueList(const priorityQueueList<T> & v);

    // priority queue protocol
    virtual void   add(T value);
    virtual int    isEmpty() const;
    virtual T      min() const;
    virtual void   removeMin();

private:
    // data area
    orderedList<T> lst;
};



//----------------------------------------------------------------------
//	class heap
//		a priority queue managed as a vector heap
//----------------------------------------------------------------------

template <class T> class heap : public priorityQueue<T>
{
public:
    // constructors
    heap(unsigned int maxsize);
    heap(const heap<T> &);

    // priority queue protocol
    virtual void   add(T value);
    virtual int    isEmpty() const;
    virtual T      min() const;
    virtual void   removeMin();

    // delete all values
    void           deleteAllValues();

    // return the size
    virtual int    size() const { return heapsize; };
    // set the size
    virtual int    setSize(unsigned int s) {
      if (s < heapsize) heapsize = s;  // silently truncate heap
      data.setSize(s);
      heapmax = s;
    };

    // protected: temporarily make them visible.
    // data areas
    vector<T>      data;       // vector that stores the heap
    unsigned int   heapmax;    // maximize number of elements in heap
    unsigned int   heapsize;   // current number of elements in heap
};



//----------------------------------------------------------------------
//	class skewHeap
//		heap priority implemented using skew heap merge
//              operations
//----------------------------------------------------------------------

template <class T> class skewHeap : public priorityQueue<T>
{
public:
    // constructors and destructor
    skewHeap();
    skewHeap(const skewHeap<T> & v);
    virtual ~skewHeap();

    // priority queue protocol
    virtual void   add(T value);
    virtual int    isEmpty() const;
    virtual T      min() const;
    virtual void   removeMin();

    // delete all values
    void           deleteAllValues();

    // merge two heaps together
    void           add(skewHeap<T> & secondHeap); 

    // root of heap
    node<T> *      root;

    // private method to merge two heaps
    node<T> *      merge(node<T> *, node<T> *);
};



//----------------------------------------------------------------------
//	class prioityQueue implementation
//----------------------------------------------------------------------

template <class T> T priorityQueue<T>::deleteMin()
{
    // return and remove the smallest element
    T result = min();
    removeMin();
    return result;
}



//----------------------------------------------------------------------
//	class prioityQueueList implementation
//----------------------------------------------------------------------

template <class T> priorityQueueList<T>::priorityQueueList()
{
    // no initialization required
}



template <class T> priorityQueueList<T>::priorityQueueList(
    const priorityQueueList<T> & source) : lst(source.lst)
{
# if 0
    // remove all values from the priority queue
    lst.deleteAllValues();

    // duplicate elements from source list
    listIterator<T> itr(source.lst);

    for (itr.init(); !itr; ++itr)
        add(itr());
# endif
}



template <class T> void priorityQueueList<T>::add(T value)
{
    // add item to queue by adding to list
    lst.add(value);
}



template <class T> int priorityQueueList<T>::isEmpty() const
{
    // queue is empty if list is empty
    return lst.isEmpty();
}



template <class T> T priorityQueueList<T>::min() const
{
    // smallest item is first element in list
    return lst.firstElement();
}



template <class T> void priorityQueueList<T>::removeMin()
{
    // smallest item is first element in list
    lst.removeFirst();
}



//----------------------------------------------------------------------
//	class heap implementation
//----------------------------------------------------------------------

template <class T> heap<T>::heap(unsigned int maxsize)
    : data(maxsize), heapmax(maxsize), heapsize(0)
{
    // no further initialization
}



template <class T> heap<T>::heap(const heap<T> & source)
	: data(source.data), heapmax(source.heapmax), heapsize(source.heapsize)
{
    //data     = source.data;
    //heapmax  = source.heapmax;
    //heapsize = source.heapsize;
}



template <class T> void heap<T>::add(T newele)
{
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
    unsigned int position = heapsize++;

    // inv: position <= 0 and < heapmax

    // now percolate up
    while (position > 0 && newele < data[(position-1)/2])
    {
        data[position] = data[(position-1)/2];

        // inv: tree rooted at "position" is a heap
        position = (position - 1) / 2;
    }

    // found location, add new element
    data[position] = newele;

    // inv: tree rooted at position is a heap
    // inv: data holds a heap
//	for (int ii = 0; ii < heapsize; ii++)
//		cout << data[ii] << " & ";
//	cout << "\\\\\n";
} 



template <class T> int heap<T>::isEmpty() const
{
    // heap is empty if heapsize is zero
    return heapsize == 0;
}



template <class T> T heap<T>::min() const
{
    // smallest item is first element in vector
    return data[0];
}


// g++ and CFRONT handle template functions differently
# ifndef __GNUG__ 

template <class T> void buildHeap(
    vector<T> & data, int heapsize, int position);

# endif 


template <class T> void heap<T>::removeMin()
{
    // remove the smallest element from a heap
    // move the last element into the first position
    data[0] = data[--heapsize];

    // then move into position
    buildHeap(data, (int) heapsize, 0);
}



template <class T> void heap<T>::deleteAllValues()
{
    // heap is empty if heapsize is zero
    heapsize = 0;
}



//----------------------------------------------------------------------
//	class skewHeap implementation
//----------------------------------------------------------------------

template <class T> skewHeap<T>::skewHeap()
{
    root = 0;
}



template <class T> skewHeap<T>::skewHeap(const skewHeap<T> & source)
{
    // duplicate tree from source
    root = source.root->copy();
}



template <class T> skewHeap<T>::~skewHeap()
{
    deleteAllValues();
}



template <class T> void skewHeap<T>::add(T val)
{
    // to add a new value, simply merge with an empty tree
    // current tree
    node<T> * newnode = new node<T>(val);
    assert(newnode != 0);
    root = merge(root, newnode);
}



template <class T> int skewHeap<T>::isEmpty() const
{
    // heap is empty if root is zero
    return root == 0;
}



template <class T> T skewHeap<T>::min() const
{
    // smallest item is first at the root
    assert(! isEmpty());
    return root->value;
}


template <class T> void skewHeap<T>::removeMin()
{
    // remove the smallest element from a skew heap
    assert(!isEmpty());

    node<T> * top = root;
    root = merge(root->right(), root->left());
    delete top;
}



template <class T> void skewHeap<T>::deleteAllValues()
{
    // releasing the tree at the root deletes all the values
    if (root)
    {
        root->release();
        root = 0;
    }
}



template <class T> void skewHeap<T>::add(skewHeap<T> & secondHeap)
{
    // merge elements from a second heap into current heap
    root = merge(root, secondHeap.root);

    // empty values from second heap
    secondHeap.root = 0;
}



template <class T> node<T> * skewHeap<T>::merge(
    node<T> * h1, node<T> * h2)
{
    // merge two skew heaps to form a new heap
    // if either tree is empty, return the other
    if (!h1)  return h2;
    if (!h2)  return h1;

    // assume smallest is root h1
    if (h2->value < h1->value)
        return merge(h2, h1);

    // reverse children and recur
    node<T> * lchild = h1->left();

    if (lchild)
    {
        h1->left(merge(h1->right(), h2));
        h1->right(lchild);
    }
    else    // no left child
        h1->left(h2);

    return h1;
}


//----------------------------------------------------------------------
//	function: heapSort
//----------------------------------------------------------------------

# ifdef __GNUG__

# include <priorque.c>


# endif

# ifndef __GNUG__

template <class T> void heapSort(vector<T> & data);

# endif

#endif
