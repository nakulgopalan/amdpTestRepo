//======================================================================
//  File:        hash.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface and implementation
//               of the hash template classes.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef HASH_H
#define HASH_H


#include <iterator.h>

#include <vector.h>

#include <list.h>

#include <tree.h>



//----------------------------------------------------------------------
//	class hashVector
//		vector indexed using hashed key values
//              template types are type of key
//              and type of value stored in vector
//----------------------------------------------------------------------

template <class H, class T> class hashVector : public vector<T>
{
public:
    // constructors
    hashVector(int max, int (*f)(const H &));
    hashVector(int max, int (*f)(const H &), T & initialValue);
    hashVector(const hashVector<H, T> & v);

    // subscript operation
    T & operator [] (const H & idx);

private:
    // store the hash function
    int (*hashfun)(const H &);
};



//----------------------------------------------------------------------
//	class hashTable
//		collection of buckets indexed by hashed values
//              template types
//              B - bucket type
//              H - key value
//              T - element held in buckets
//----------------------------------------------------------------------

template <class B, class H, class T> class hashTableIterator;

template <class B, class H, class T> class hashTable
{
public:
    // constructor
    hashTable(unsigned int max, unsigned int (*f)(const H &));

    // test if the set is empty
    virtual int           isEmpty();

    // clear the elements of the set
    virtual void          deleteAllValues();

protected:
    friend class hashTableIterator<B, H, T>;

    // the actual table itself is a vector of buckets
    const unsigned int    tablesize;
    vector<B>             buckets;
    unsigned int          (*hashfun)(const H &);

    // convert key into unsigned integer value in range
    unsigned int          hash(const H & key) const;

    // make iterator for given position in table
    virtual iterator<T> * makeIterator(unsigned int) = 0;
};



//----------------------------------------------------------------------
//	class hashTree
//		hash table implemented using trees for buckets
//----------------------------------------------------------------------

template <class T> class hashTree : public hashTable<avlTree<T>, T, T>
{
public:
    // constructor
    hashTree(unsigned int max, unsigned int (*f)(const T &));

    // add an element to the collection
    void      add(T newele);

    // test to see if the collect includes an element
    int       includes(T ele) const;

    // remove an element
    void      remove(T ele);

protected:
    virtual iterator<T> * makeIterator(unsigned int i);
};



//----------------------------------------------------------------------
//	class hashTableIterator
//		iterator protocol for hash tables
//----------------------------------------------------------------------

template <class B, class H, class T> class hashTableIterator
    : public iterator<T>
{
public:
    // constructor
    hashTableIterator(hashTable<B, H, T> & v);
    hashTableIterator(const hashTableIterator &);

    // iterator protocol 
    virtual int     init();
    virtual T &     operator()();
    virtual int     operator !();
    virtual int     operator ++();
    virtual void    operator =(T value);

protected:
    hashTable<B, H, T> &  base;
    unsigned int          currentIndex;
    iterator<T> *         itr;

    // getNextIterator used to set internal iterator pointer
    int             getNextIterator();
};



//----------------------------------------------------------------------
//	class hashTreeIterator
//		iterator protocol for hash tree structures
//----------------------------------------------------------------------

template <class T> class hashTreeIterator
    : public hashTableIterator<avlTree<T>, T, T>
{
public:
    // constructor
    hashTreeIterator(hashTree<T> & x);
    hashTreeIterator(const hashTreeIterator &);
};



//----------------------------------------------------------------------
//	class hashVector implementation
//----------------------------------------------------------------------

template <class H, class T> hashVector<H, T>::
    hashVector(int max, int (*f)(const H &))
    : vector<T>(max), hashfun(f)
{
    // no further initialization
}



template <class H, class T> hashVector<H, T>::
    hashVector(int max, int (*f)(const H &), T & initialValue)
    : vector<T>(max, initialValue), hashfun(f)
{
    // no further initialization
}



template <class H, class T> hashVector<H, T>::
    hashVector(const hashVector<H,T> & v)
    : vector<T>(v), hashfun(v.hashfun)
{
    // no further initialization
}



template <class H, class T> T & hashVector<H, T>::
    operator [] (const H & index)
{
    // subscript a hash vector
    // hash the index value before indexing vector
    return vector<T>::operator [] ((*hashfun)(index) % size);
}



//----------------------------------------------------------------------
//	class hashTable implementation
//----------------------------------------------------------------------

template <class B, class H, class T> hashTable<B, H, T>::
    hashTable(unsigned int max, unsigned int (*f)(const H &))
    : tablesize(max), buckets(max), hashfun(f)
{
    // no further initialization
}



template <class B, class H, class T> int hashTable<B, H, T>::isEmpty()
{
    // if any table is non-empty, return 0
    for (int i = 0; i < tablesize; i++)
        if (!buckets[i].isEmpty())
            return 0;

    // all empty
    return 1;
}



template <class B, class H, class T> void hashTable<B, H, T>::
    deleteAllValues()
{
    // delete all values from a hash table
    // clear the elements from each of teh buckets
    for (int i = 0; i < tablesize; i++)
        buckets[i].deleteAllValues();
}



template <class B, class H, class T> unsigned int hashTable<B, H, T>::
    hash(const H & key) const
{
    // return hashed value of key
    return (*hashfun)(key) % tablesize;
}



//----------------------------------------------------------------------
//	class hashTree implementation
//----------------------------------------------------------------------

template <class T> hashTree<T>::
    hashTree(unsigned int max, unsigned int (*f)(const T &))
    : hashTable<avlTree<T>, T, T>(max, f)
{
    // no further initialization
}



template <class T> void hashTree<T>::add(T newele)
{
    // find the right bucket, then add the element
    buckets[hash(newele)].add(newele); 
}



template <class T> int hashTree<T>::includes(T ele) const
{
    // test to see if the collection includes an element
    return buckets[hash(ele)].includes(newele);
}



template <class T> void hashTree<T>::remove(T ele)
{
    // remove an element
    buckets[hash(ele)].remove(newele);
}



template <class T> iterator<T> * hashTree<T>::
    makeIterator(unsigned int i)
{
    return new inorderTreeTraversal<T>(buckets[i]);
}



//----------------------------------------------------------------------
//	class hashTableIterator implementation
//----------------------------------------------------------------------

template <class B, class H, class T> hashTableIterator<B, H, T>::
    hashTableIterator(hashTable<B, H, T> & v)
    : base(v), currentIndex(0), itr(0)
{
    // no further initialization
}

template <class B, class H, class T> hashTableIterator<B, H, T>::
    hashTableIterator(const hashTableIterator<B, H, T> & x)
	: base(x.base), currentIndex(x.currentIndex), itr(x.itr)
{
	// no further initialization
}


template <class B, class H, class T> int hashTableIterator<B, H, T>::
    init()
{
    // initialize iterator, 
    // start search with first bucket
    currentIndex = 0; 
    itr = 0;
    return getNextIterator(); 
}



template <class B, class H, class T> T & hashTableIterator<B, H, T>::
    operator()()
{
    // return current element
    return (*itr)();
}



template <class B, class H, class T> int hashTableIterator<B, H, T>::
    operator !()
{
    // test if there is a current element
    return itr != 0;
}



template <class B, class H, class T> int hashTableIterator<B, H, T>::
    operator ++()
{
    // see if current iterator can be advanced
    if (itr && ++(*itr)) 
        return 1;

    // if not, get next iterator
    currentIndex++;
    return getNextIterator();
}



template <class B, class H, class T> void hashTableIterator<B, H, T>::
    operator=(T val)
{
    // change the current value
    (*itr) = val;
}



template <class B, class H, class T> int hashTableIterator<B, H, T>::
    getNextIterator()
{
    // if there is an old iterator, delete it
    if (itr != 0) 
        delete itr;

    // now search for a new one
    for (; currentIndex < base.tablesize; currentIndex++)
    {
        // generate a new iterator at the current point
        itr = base.makeIterator(currentIndex);
        assert(itr != 0);

        // if it has at least one element, we're done
        if (itr->init())
            return 1;

        // otherwise delete it, try again
        delete itr;
    }

    // all out of iterators, can quit
    itr = 0;
    return 0;
}



//----------------------------------------------------------------------
//	class hashTreeIterator implementation
//----------------------------------------------------------------------

template <class T> hashTreeIterator<T>::hashTreeIterator(hashTree<T> & x)
    : hashTableIterator<avlTree<T>, T, T>(x)
{
    // no further initialization
}

template <class T> hashTreeIterator<T>::
	hashTreeIterator(const hashTreeIterator<T> & x)
		: hashTableIterator<avlTree<T>, T, T>(x)
{
	// no further initialization
}


//----------------------------------------------------------------------
//	miscellaneous template functions
//----------------------------------------------------------------------

// bucket sort (version #1)

# ifdef __GNUG__

# include <hash.c>

# endif

# ifndef __GNUG__

template <class T> void bucketSort(vector<T> & data,
    unsigned int tablesize, unsigned int (*f)(const T &));

template <class T> void bucketSort(vector<T> & data,
    unsigned int (*f)(const T &));

# endif

#endif
