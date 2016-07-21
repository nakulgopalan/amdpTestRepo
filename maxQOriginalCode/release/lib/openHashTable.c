//==================================================================
//	File: 		openHashTable.c
//	Author: 	Timothy A. Budd
//	Platform:	Cfront
//	Description:  Template implementation of open addressing hash table ADT
//	Copyright (c) 1992 by Timothy A. Budd, All Rights Reserved
//	Permission granted for duplication if not for profit
//==================================================================

//
//	implementation file for open hash tables
//

template <class T> 
openHashTable<T>::openHashTable(const string & name, unsigned int (*f)(const T &))
	: hashfun(f), rfile(name)
{
	// determine size of file
	size = rfile.length();
}

template <class T> void openHashTable<T>::create(unsigned int sz)
{	// create a new open Hash Table with sz records
	size = sz;

	// assume default constructor sets known value
	T initialValue;
	for (unsigned int i = 0; i < sz; i++)
		rfile.put(i, initialValue);
}

template <class T> int openHashTable<T>::add(const T & value)
{	// add record to an open hash table

	// find initial hash value
	unsigned int hashValue = (*hashfun)(value) % size;
	unsigned int index = hashValue;
	
	// see if occupied
	T currentValue;
	rfile.get(index, currentValue);
	while (! isEmpty(currentValue)) {
		// see if next record is empty
		index = index + 1;
		if (index >= size) 
			index = 0;
		if (index == hashValue)
			return 0;
		rfile.get(index, currentValue);
		}
	// found an empty position
	rfile.put(index, value);
	return 1;
}


template <class T> int openHashTable<T>::includes(T & value)
{
	unsigned int hashValue = (*hashfun)(value) % size;
	unsigned int index = hashValue;
	T currentValue;
	rfile.get(index, currentValue);
	// loop as long as we don't see value
	while (! (currentValue == value)) {
		if (isEmpty(currentValue))
			return 0;
		index = index + 1;
		if (index >= size)
			index = 0;
		if (index == hashValue)
			return 0;
		rfile.get(index, currentValue);
		}

	
	// found it, set value
	value = currentValue;
	return 1;
}

template <class T> void openHashTable<T>::remove(const T & value)
{	// add record to an open hash table

	// find initial hash value
	unsigned int hashValue = (*hashfun)(value) % size;
	unsigned int index = hashValue;
	
	// see if occupied
	T currentValue;
	rfile.get(index, currentValue);
	// loop as long as we don't see value
	while (! (currentValue == value)) {
		// see if next record is empty
		index = index + 1;
		if (index >= size) 
			index = 0;
		if (index == hashValue)
			return;
		rfile.get(index, currentValue);
		}
	// found it, empty it out
	T empty;
	rfile.put(index, empty);
	return;
}


