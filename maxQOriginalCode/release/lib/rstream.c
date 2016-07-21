//==================================================================
//	File: 		rstream.c
//	Author: 	Timothy A. Budd
//	Platform:	Cfront
//	Description: 	Template implementation of random access stream ADT
//	Copyright (c) 1992 by Timothy A. Budd, All Rights Reserved
//	Permission granted for duplication if not for profit
//==================================================================

//
//	implementation of rstream -- randomly accessed streams
//

# include "string.h"
# include <stream.h>
# include <fstream.h>

template <class T> rstream<T>::rstream(const string & name)
{
	// convert string to C style pointer variable
	const char * cname = name;
	// open the file for both input and output
	theStream.open(cname, ios::in | ios::out);
}

template <class T> int rstream<T>::get(unsigned int index, T & value)
{
	// first position the stream
	theStream.seekg(sizeof( T ) * index);
	// then read the value
	char * valuePtr = (char *) & value;
	theStream.read(valuePtr, sizeof( T ));
	// return the number of characters read
	return theStream.gcount();
}

template <class T> void rstream<T>::put(unsigned int index, const T & value)
{
	// first position the stream
	theStream.seekg(sizeof( T ) * index);
	// then write the value
	const char * valuePtr = (const char *) & value;
	theStream.write(valuePtr, sizeof( T ));
}

template <class T> unsigned int rstream<T>::length()
{
	if (! theStream)
		return 0;
	else {
		theStream.seekg(0, ios::end);
		return theStream.tellg() / sizeof( T );
		}
}

//
//	stream iterators
//

template <class T> rstreamIterator<T>::rstreamIterator (rstream<T> & bs)
	: baseStream(bs)
{
	index = 0;
}

template <class T> rstreamIterator<T>::
	rstreamIterator (const rstreamIterator & x)
		: baseStream(x,baseStream), index(x.index)
{
	// no further initialization
}

template <class T> T rstreamIterator<T>::operator() ()
{
	T result;
	baseStream.get(index, result);
	return result;
}


template <class T> int rstreamIterator<T>::init()
{
	index = 0;
	return operator ! ();
}

template <class T> int rstreamIterator<T>::operator ! ()
{
	T temp;
	// if we can read a value, we aren't at the end
	if (baseStream.get(index, temp) != 0)
		return 1;
		// this resets the stream pointer
	baseStream.get(0, temp);
	return 0;
}

template <class T> int rstreamIterator<T>::operator ++ ()
{
	index++;
	return operator ! ();
}

template <class T> void rstreamIterator<T>::operator = (T value)
{	// simply assign the value the current position
	baseStream.put(index, value);
}

