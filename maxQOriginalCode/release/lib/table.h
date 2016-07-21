//======================================================================
//  File:        table.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface and implementation
//               of the dictionary and table template classes.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef TABLE_H
#define TABLE_H

#include <iterator.h>
#include <vector.h>
#include <list.h>
#include <hash.h>

//
//	in trying to get this to work with g++ it appears some
//	template functions must be given in the class description
//	I don't understand why.


//----------------------------------------------------------------------
//	class association
//		A single key/value pair,
//		usually maintained in a dictionary
//----------------------------------------------------------------------

template <class K, class V> class association
{
public:
    // value field is publically accessible
    V       valueField;

    // constructor
    association(K initialKey, V initialValue);
    association(const association & x);

    // assignment can be either to association or value
    V       operator =(association & anAssociation);
    V       operator =(V val);

    // accessor methods for key and value
    K       key() const;
    V       value() const;

protected:
	friend int operator == <> (association<K, V> & l, association<K, V> & r);
	friend int operator == <> (association<K, V> & l, K r);

    // key field cannot be altered
    const K keyField;

};


//----------------------------------------------------------------------
//	class cAssociation
//		a form of association that allows elements to be compared
//----------------------------------------------------------------------

template <class K, class V> class cAssociation
    : public association<K, V>
{
public:
    // constructor
    cAssociation(K initialKey, V initialValue);

	friend int operator == <> (cAssociation<K, V> & l, cAssociation<K, V> & r);
	friend int operator == <> (cAssociation<K, V> & l, K & r);

	friend int operator < <> (cAssociation<K, V> & l, cAssociation<K, V> & r);
	friend int operator < <> (cAssociation<K, V> & l, K & r);
};

//----------------------------------------------------------------------
//	class dictionary
//		A collection of key/value pairs
//		implemented as a list of associations
//----------------------------------------------------------------------

template <class K, class V> class dictionaryIterator;
template <class K, class V> class table;

template <class K, class V> class dictionary
{
public:
    // constructors
    dictionary();
    dictionary(V initialValue);
    dictionary(const dictionary & v);
    // destructor
    virtual ~dictionary();

    // find association with given key
    association<K, V> * associatedWith(K);

    // dictionary protocol
    V &     operator [](K key);
    void    deleteAllValues();
    int     includesKey(K key);
    int     isEmpty() const;
    void    removeKey(K key);
    void    setInitial(V initialValue);
    dictionary<K, V> * Duplicate();

public:
    // data is maintained in a list of associations
    list<association<K, V> *> * data;
    V                         initialValue;

    // friends
    friend class dictionaryIterator<K, V>;
    friend class table<K, V>;
};

//----------------------------------------------------------------------
//	class table
//		dictionary collection of key/value pairs
//		implemented using hash tables
//----------------------------------------------------------------------

template <class K, class V> class table
    : public hashTable<dictionary<K, V>, K, association<K, V> *>
{
public:
    // constructors
    table(unsigned int max, unsigned int (*f)(const K &));
    table(unsigned int max, unsigned int (*f)(const K &), V &);

    // new table operators 
    V &     operator [](K key);
    void    removeKey(K key);
    void    setInitial(V initialValue);
    int     includesKey(K key);  // tgd added

protected:
    iterator<association<K, V> *> * makeIterator(unsigned int i);
};

//----------------------------------------------------------------------
//	class tableIterator
//		iterator protocol for tables
//----------------------------------------------------------------------

template <class K, class V> class tableIterator
    : public hashTableIterator<dictionary<K, V>, K, association<K, V> *>
{
public:
    // constructor
    tableIterator(table<K, V> & v);
};



//----------------------------------------------------------------------
//	class orderedDictionary
//		A ordered collection of key/value pairs
//		implemented as a list of associations
//----------------------------------------------------------------------

template <class K, class V> class orderedDictionaryIterator;

template <class K, class V> class orderedDictionary
{
public:
    // constructors

    orderedDictionary();
    orderedDictionary(V initialValue);
    orderedDictionary(orderedDictionary<K, V> & v);

    // dictionary protocol
    V &     operator [](K key);
    void    deleteAllValues();
    int     includesKey(K key);
    int     isEmpty() const;
    void    removeKey(K key);
    void    setInitial(V initialValue);

private:
    // data is maintained in a list of associations
    orderedList<cAssociation<K, V> &> data;
    V                  initialValue;

    // friends
    friend class orderedDictionaryIterator<K, V>;
    friend class table<K, V>;

    // find association with given key
    cAssociation<K, V> * associatedWith	(K key);
};



//----------------------------------------------------------------------
//	class dictionaryIterator
//		implementation of iterator protocol for dictionaries
//  
//----------------------------------------------------------------------

template <class K, class V> class dictionaryIterator
    : public listIterator<association<K, V> *>
{
public:
    // constructor
    dictionaryIterator(dictionary<K, V> & dict);
};



//----------------------------------------------------------------------
//	class orderedDictionaryIterator
//		implementation of iterator protocol for
//              ordered dictionaries
//----------------------------------------------------------------------

template <class K, class V> class orderedDictionaryIterator
    : public listIterator<cAssociation<K, V> &>
{
public:
    // constructor
    orderedDictionaryIterator(orderedDictionary<K, V> & dict);
};



//----------------------------------------------------------------------
//	class sparseMatrix
//		two dimensional matrix using dictionaries to store rows
//----------------------------------------------------------------------

template <class T> class sparseMatrix
{
public:
    // constructors
    sparseMatrix(unsigned int n, unsigned int m);
    sparseMatrix(unsigned int n, unsigned int m, T initial);

    // only operation is subscript
    dictionary<int, T> & operator [](unsigned int n);

private:
    vector< dictionary<int,T> > data;
};



//----------------------------------------------------------------------
//	class sparseMatrix2
//		second form of sparse matrix, 
//		uses dictionaries for both rows and columns
//----------------------------------------------------------------------

template <class T> class sparseMatrix2
{
public:
    // constructors
    sparseMatrix2(unsigned int n, unsigned int m);
    sparseMatrix2(unsigned int n, unsigned int m, T init);

    dictionary<int,T> & operator [] (unsigned int n);

private:
    // data areas
    T initialValue;
    dictionary< int, dictionary<int, T> > data;
};



//----------------------------------------------------------------------
//	class association implementation
//----------------------------------------------------------------------

template <class K, class V> association<K, V>::
    association(K initialKey, V initialValue)
    : keyField(initialKey), valueField(initialValue)
{
    // no further initialization
}

template <class K, class V> association<K, V>::
	association(const association<K, V> & x)
	: keyField(x.keyField), valueField(x.valueField)
{
	// no further initialization
}

template <class K, class V> V   association<K, V>::
    operator = (association<K, V> & anAssociation)
{
    // assignment from an association
    valueField = anAssociation.valueField;
    return valueField;
}




template <class K, class V> V   association<K, V>::
    operator = (V val)
{
    // assignment from a value
    valueField = val;
    return valueField;
}



template <class K, class V> K association<K, V>::key() const
{
    // return key
    return keyField;
}

template <class K, class V> V association<K, V>::value() const
{
    // return value
    return valueField;
}



template <class K, class V> int operator ==
    (association<K, V> & left, association<K, V> & right)
{
    // compare the key values for two associations
    return left.key() == right.key();
}



template <class K, class V> int operator ==
    (association<K, V> & left, K   rightKey)
{
    // compare the key values for two associations
    return left.key() == rightKey;
}



//----------------------------------------------------------------------
//	class dictionary implementation
//----------------------------------------------------------------------

template <class K, class V> dictionary<K, V>::dictionary():
  initialValue((V) 0)
{
  data = new list<association<K, V> *>;
}



template <class K, class V> dictionary<K, V>::dictionary(V val)
{
  data = new list<association<K, V> *>;
  // set initial value
  initialValue = val;
}

template <class K, class V> dictionary<K, V>::
    dictionary(const dictionary<K, V> & v)
	: data(v.data->duplicate()), initialValue (v.initialValue)
{
  // the above duplicate() of data causes a copy of the BuddLink's in
  // the list.  Now we need to copy the association records too.
  listIterator<association<K, V> *> itr(*data);
  for(itr.init(); !itr; ++itr) {
    // copy each association too
    itr() = new association<K,V>(*itr());
  }
}

// Tue Dec 21 22:20:02 1999 tgd: Why did this have an argument? (V val)
// Perhaps to get the template to instantiate properly?
// egcs objects to this.  The arg was not used, so I have deleted it.
template <class K, class V> dictionary<K, V>::~dictionary()
{
  // we do deep deletion of the assocations, because list delete just
  // deletes the buddlink's but not their contents.
  listIterator<association<K, V> *> itr(*data);
  for (itr.init(); !itr; ++itr) {
    delete itr();
  }
  // now delete the list itself and its buddlinks.
  delete data;
}


template <class K, class V> association<K, V> * dictionary<K, V>::
    associatedWith(K key)
{
    // return the association with the given key
    // or the null pointer if no association yet exists
    listIterator<association<K, V> *> itr(*data);
	
    // loop over the elements, looking for a match
    for (itr.init(); ! itr; ++itr)
        if (itr()->key() == key)
        {
	    // address of a reference is a pointer
	    return itr();
        }

    // not found, return null pointer
    return 0;
}



template <class K, class V> V & dictionary<K, V>::operator [] (K key)
{
    // return value of association specified by key
    // first look to see if association is there already
    association<K, V> * newassoc = associatedWith(key);

    // if not there, make a new one
    if (!newassoc)
    {
        newassoc = new association<K, V>(key, initialValue);
	assert(newassoc != 0);
	data->add(newassoc);
    }
	
    // return reference to value field
    return newassoc->valueField;
}



template <class K, class V> void dictionary<K, V>::deleteAllValues()
{
    data->deleteAllValues();
}



template <class K, class V> int dictionary<K, V>::includesKey(K key)
{
    // if there is an association, then element is in the dictionary 
    return associatedWith(key) != 0; 
}



template <class K, class V> int dictionary<K, V>::isEmpty() const
{
    return data->isEmpty();
}



template <class K, class V> void dictionary<K, V>::removeKey(K key)
{
    // loop over the elements looking for key
    listIterator<association<K, V> *> itr(*data);

    for (itr.init(); ! itr; ++itr) {
      if (itr()->key() == key) {
	delete itr();         // release the association*
	itr.removeCurrent();  // remove the BuddLink
      }
    }
}



template <class K, class V> void dictionary<K, V>::setInitial(V val)
{
    initialValue = val;
}


template <class K, class V> 
dictionary<K, V> * dictionary<K, V>::Duplicate()
{
  dictionary<K, V> * result = new dictionary<K,V>(initialValue);
  result->data = data->duplicate();
  // now copy the association records too.
  listIterator<association<K, V> *> itr(*data);
  for(itr.init(); !itr; ++itr) {
    // copy each association too
    itr() = new association<K,V>(*itr());
  }
  return result;
}



//----------------------------------------------------------------------
//	class orderedDictionary implementation
//----------------------------------------------------------------------

template <class K, class V> orderedDictionary<K, V>::orderedDictionary()
{
    // nothing to initialize
}



template <class K, class V> orderedDictionary<K, V>::
    orderedDictionary(V val)
{
    // set initial value
    initialValue = val;
}



template <class K, class V> orderedDictionary<K, V>::
    orderedDictionary(orderedDictionary<K, V> & v)
{
    initialValue = v.initialValue;

    data.deleteAllValues();
    listIterator<cAssociation<K, V> &> itr(v.data);

    for (itr.init(); !itr; ++itr)
        data.add(itr());
}



template <class K, class V> V & orderedDictionary<K, V>::
    operator [] (K key)
{
    // return value of association specified by key
    // first look to see if association is there already
    cAssociation<K, V> * newassoc = associatedWith(key);

    // if not there, make a new one
    if (!newassoc)
    {
        newassoc = new cAssociation<K, V>(key, initialValue);
	assert(newassoc != 0);
	data.add(*newassoc);
    }
	
    // return reference to value field
    return newassoc->valueField;
}



template <class K, class V> void orderedDictionary<K, V>::deleteAllValues()
{
    data.deleteAllValues();
}



template <class K, class V> int orderedDictionary<K, V>::includesKey(K key)
{
    // if there is an association, then element is in the dictionary 
    return associatedWith(key) != 0; 
}



template <class K, class V> int orderedDictionary<K, V>::isEmpty() const
{
    return data.isEmpty();
}



template <class K, class V> void orderedDictionary<K, V>::removeKey(K key)
{
    // loop over the elements looking for key
    listIterator<cAssociation<K, V> &> itr(data);

    for (itr.init(); ! itr; ++itr)
        if (itr().key() == key)
	    itr.removeCurrent();
}



template <class K, class V> void orderedDictionary<K, V>::setInitial(V val)
{
    initialValue = val;
}



template <class K, class V> cAssociation<K, V> * orderedDictionary<K, V>::
    associatedWith(K key)
{
    // return the association with the given key
    // or the null pointer if no association yet exists
    listIterator<cAssociation<K, V> &> itr(data);
	
    // loop over the elements, looking for a match
    for (itr.init(); ! itr; ++itr)
        if (itr().key() == key)
        {
	    // address of a reference is a pointer
            cAssociation<K, V> & theAssociation = itr();
	    return &theAssociation;
        }

    // not found, return null pointer
    return 0;
}



//----------------------------------------------------------------------
//	class dictionaryIterator implementation
//----------------------------------------------------------------------

template <class K, class V> dictionaryIterator<K, V>::
    dictionaryIterator(dictionary<K, V> & dict)
    : listIterator<association<K, V> *>(*dict.data)
{
    // no further initialization
}



//----------------------------------------------------------------------
//	class orderedDictionaryIterator implementation
//----------------------------------------------------------------------

template <class K, class V> orderedDictionaryIterator<K, V>::
    orderedDictionaryIterator(orderedDictionary<K, V> & dict)
    : listIterator<cAssociation<K, V> &>(dict.data)
{
    // no further initialization
}



//----------------------------------------------------------------------
//	class sparseMatrix implementation
//----------------------------------------------------------------------

template <class T> sparseMatrix<T>::
    sparseMatrix(unsigned int n, unsigned int m)
    : data(n) 
{
    // no further initialization
}



template <class T> sparseMatrix<T>::
    sparseMatrix(unsigned int n, unsigned int m, T initial)
    : data(n) 
{
    // set the initial value for each dictionary
    for (unsigned int i = 0; i < n; i++)
        data[i].setInitial(initial);
}



template <class T> dictionary<int,T> & sparseMatrix<T>
    ::operator [](unsigned int n)
{
    // simply return the appropriate dictionary
    return data[n];
}



//----------------------------------------------------------------------
//	class sparseMatrix2 implementation
//----------------------------------------------------------------------

template <class T> sparseMatrix2<T>::
    sparseMatrix2(unsigned int n, unsigned int m)
    : data(n) 
{
    // no further initialization
}



template <class T> sparseMatrix2<T>::
    sparseMatrix2(unsigned int n, unsigned int m, T initial)
    : data(n) 
{
    // set the initial value for each dictionary
    for (unsigned int i = 0; i < n; i++)
        data[i].setInitial(initial);
}



template <class T> dictionary<int, T> & sparseMatrix2<T>::
    operator [] (unsigned int n)
{
    // subscript operator for sparse matrices
    // if we already have a row, just return it
    if (data.includesKey(n))
        return data[n];

    // else make a new entry and set initial value
    data[n].setInitial(initialValue);
    return data[n];
}



//----------------------------------------------------------------------
//	class cAssociation implementation
//----------------------------------------------------------------------

template <class K, class V> cAssociation<K, V>::cAssociation(K n, V m)
    : association<K, V>(n, m)
{
    // no further initialization
}



template <class K, class V> 
	int operator == (cAssociation<K, V> & left, cAssociation<K, V> & right)
{
    // compare the key values for two associations
    return left.key() == right.key();
}



template <class K, class V> 
	int operator == (cAssociation<K, V> & left, K & rightKey)
{
    // compare the key values for two associations
    return left.key() == rightKey;
}



template <class K, class V> 
	int operator < (cAssociation<K, V> & left, cAssociation<K, V> & right)
{
    // compare the key values for two associations
    return left.key() < right.key();
}



template <class K, class V> 
	int operator < (cAssociation<K, V> & left, K & rightKey)
{
    // compare the key value of an associations to a key value
    return left.key() < rightKey;
}



//----------------------------------------------------------------------
//	class table implementation
//----------------------------------------------------------------------

template <class K, class V> table<K, V>::
    table(unsigned int max, unsigned int (*f)(const K &))
    : hashTable<dictionary<K, V>, K, association<K, V> *>(max, f) 
{
    // no further initialization
}



template <class K, class V> table<K, V>::
    table(unsigned int max, unsigned int (*f)(const K &), V & v)
    : hashTable<dictionary<K, V>, K, association<K, V> *>(max, f) 
{
    // set the initial value in each bucket
    setInitial(v);
}


// tgd added
template <class K, class V> int table<K, V>::includesKey(K key)
{
  return buckets[hash(key)].includesKey(key);
}

template <class K, class V> V & table<K, V>::operator [](K key)
{
    // find right dictionary, then subscript dictionary
    return buckets[hash(key)][key];
}



template <class K, class V> void table<K, V>::removeKey(K key)
{
    // find the right bucket, then remove the key
    buckets[hash(key)].removeKey(key); 
}



template <class K, class V> void table<K, V>::setInitial(V val)
{
    // set the initial value in each bucket
    for (int i = 0; i < tablesize; i++)
        buckets[i].initialValue = val;
}



template <class K, class V> iterator<association<K, V> *> * table<K, V>::
    makeIterator(unsigned int i)
{
    return new dictionaryIterator<K, V>(buckets[i]);
}



//----------------------------------------------------------------------
//	class tableIterator implementation
//----------------------------------------------------------------------

template <class K, class V> tableIterator<K, V>::
    tableIterator(table<K, V> & v)
    : hashTableIterator<dictionary<K, V>, K, association<K, V> *>(v)
{ 
    // no further initialization
}



#endif
