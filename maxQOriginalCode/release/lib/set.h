//======================================================================
//  File:        set.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface and implementation
//               of the set and bag template classes.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef SET_H
#define SET_H

#include <iterator.h>
#include <vector.h>
#include <list.h>
#include <hash.h>


//----------------------------------------------------------------------
//	class setList
//		set protocol built on top of list implementation
//----------------------------------------------------------------------

template <class T> class setList : public list<T>
{
public:
    // add an item to the set
    virtual void add(T val);

    // removal test
    virtual void remove(T);

    // union formation
    void unionWith(setList<T> &);

    // intersection
    void intersectWith(setList<T> &);

    // difference between sets
    void differenceFrom(setList<T> &);

    // subset test
    int subset(setList<T> &);
};



//----------------------------------------------------------------------
//	class setTable
//		set protocol built out of hash tables
//----------------------------------------------------------------------

template <class T> class setTable : public hashTable<setList<T>, T, T>
{
public:
    // constructor
    setTable(unsigned int max, unsigned int (*f)(const T &));

    // implement set protocol
    void     add(T val);
    void     differenceFrom(setTable<T> &);
    int      includes(T val) const;
    void     intersectWith(setTable<T> &);
    void     remove(T val);
    void     unionWith(setTable<T> &);
    int      subset(setTable<T> &) const;

protected:
    // make an iterator
    iterator<T> * makeIterator(unsigned int i);
};



//----------------------------------------------------------------------
//	set template functions (for class iterator)
//----------------------------------------------------------------------

template <class ST, class T> void setUnionWith(ST & set,
    iterator<T> & itr)
{
    // simply add every element from the argument iterator
    for (itr.init(); ! itr; ++itr)
        set.add(itr());
}



template <class ST, class T> void setDifferenceFrom(ST & set,
    iterator<T> & itr)
{
    // test every element to see if it is in the receiver
    for (itr.init(); ! itr; ++itr)
        // if current set has it
        if (set.includes(itr()))
            // then remove it
            set.remove(itr());
} 



template <class ST, class T> int setSubset(ST & set,
    iterator<T> & itr)
{
    // every element of right must also be in receiver
    for (itr.init(); ! itr; ++itr)
        // if set does not have it
        if (! set.includes(itr()))
            // then can't be a subset
            return 0;

    // everything checks out, must be a subset
    return 1;
}



template <class ST, class T> void setIntersectWith(ST & set,
    iterator<T> & itr)
{
    // make a list of elements in the difference
    list<T> difference;

    // for each element in iterator
    for (itr.init(); ! itr; ++itr)
        // if it is not in set
        if (! set.includes(itr()))
            // then save it
            difference.add(itr());

    // now remove elements from the difference
    listIterator<T> litr(difference);

    for (litr.init(); ! litr; ++litr)
        set.remove(litr());
}



//----------------------------------------------------------------------
//	set template functions (for class listIterator)
//----------------------------------------------------------------------

template <class ST, class T> void setUnionWith(ST & set,
    listIterator<T> & itr)
{
    // simply add every element from the argument iterator
    for (itr.init(); ! itr; ++itr)
        set.add(itr());
}



template <class ST, class T> void setDifferenceFrom(ST & set,
    listIterator<T> & itr)
{
    // test every element to see if it is in the receiver
    for (itr.init(); ! itr; ++itr)
        // if current set has it
        if (set.includes(itr()))
            // then remove it
            set.remove(itr());
} 



template <class ST, class T> int setSubset(ST & set,
    listIterator<T> & itr)
{
    // every element of right must also be in receiver
    for (itr.init(); ! itr; ++itr)
        // if set does not have it
        if (! set.includes(itr()))
            // then can't be a subset
            return 0;

    // everything checks out, must be a subset
    return 1;
}



template <class ST, class T> void setIntersectWith(ST & set,
    listIterator<T> & itr)
{
    // make a list of elements in the difference
    list<T> difference;

    // for each element in iterator
    for (itr.init(); ! itr; ++itr)
        // if it is not in set
        if (! set.includes(itr()))
            // then save it
            difference.add(itr());

    // now remove elements from the difference
    listIterator<T> litr(difference);

    for (litr.init(); ! litr; ++litr)
        set.remove(litr());
}



//----------------------------------------------------------------------
//	class setList implementation
//----------------------------------------------------------------------

template <class T> void setList<T>::add(T val)
{
    // only add if it is not already in the set
    if (! includes(val))
        list<T>::add(val);
}



template <class T> void setList<T>::remove(T val)
{
    listIterator<T> itr(*this);

    for (itr.init(); ! itr; ++itr)
        if (itr() == val)
        {
            itr.removeCurrent();
            return;
        }
}



template <class T> void setList<T>::unionWith(setList<T> & right)
{
    // simply make an iterator and then use the generic method
    listIterator<T> itr(right);
    setUnionWith(*this, itr);
}



template <class T> void setList<T>::intersectWith(setList<T> & right)
{
    // simply make an iterator and then use the generic method
    listIterator<T> itr(right);
    setIntersectWith(*this, itr);
}



template <class T> void setList<T>::differenceFrom(setList<T> & right)
{
    // simply make an iterator and then use the generic method
    listIterator<T> itr(right);
    setDifferenceFrom(*this, itr);
}



template <class T> int setList<T>::subset(setList<T> & right)
{ 
    // simply make an iterator and then use the generic method
    listIterator<T> itr(right);
    return setSubset(*this, itr);
}



template <class T> int operator == (setList<T> & left, setList<T> & right)
{
    // two sets are equal if both are subsets of each other
    return left.subset(right) && right.subset(left);
}



//----------------------------------------------------------------------
//	class setTable implementation
//----------------------------------------------------------------------

template <class T> setTable<T>::
    setTable(unsigned int max, unsigned int (*f)(const T &))
    : hashTable<setList<T>, T, T>(max, f)
{
    // no further initialization
}



template <class T> void setTable<T>::add(T val)
{
    // compute bucket, then add element
    buckets[hash(val)].add(val);
}



template <class T> void setTable<T>::differenceFrom(setTable<T> & right)
{
    assert(tablesize == right.tablesize);

    // form the difference of each bucket independently
    for (int i = 0; i < tablesize; i++)
        buckets[i].differenceFrom(right.buckets[i]);
}



template <class T> int setTable<T>::includes(T val) const
{
    // compute bucket, then see if it includes element
    return buckets[hash(val)].includes(val);
}



template <class T> void setTable<T>::intersectWith(setTable<T> & right)
{
    assert(tablesize == right.tablesize);

    // form the intersection of each bucket independently
    for (int i = 0; i < tablesize; i++)
        buckets[i].intersectWith(right.buckets[i]);
}



template <class T> void setTable<T>::remove(T val)
{
    // compute bucket, then remove element
    buckets[hash(val)].remove(val);
}



template <class T> void setTable<T>::unionWith(setTable<T> & right)
{
    assert(tablesize == right.tablesize);

    // form the union of each bucket independently
    for (int i = 0; i < tablesize; i++)
        buckets[i].unionWith(right.buckets[i]);
}



template <class T> int setTable<T>::subset(setTable<T> & right) const
{ 
    assert(tablesize == right.tablesize);

    // test subset of each bucket independently
    for (int i = 0; i < tablesize; i++)
        if (! buckets[i].subset(right.buckets[i]))
            return 0;

    // all bucket subsets smaller than corresponding buckets
    return 1;
}



template <class T> iterator<T> * setTable<T>::
    makeIterator(unsigned int i)
{
    return new listIterator<T>(buckets[i]);
}



#endif
