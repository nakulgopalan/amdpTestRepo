//==================================================================
//	File: 		rstream.h
//	Author: 	Timothy A. Budd
//	Platform:	Cfront
//	Description: 	Template Description of random access stream ADT
//	Copyright (c) 1992 by Timothy A. Budd, All Rights Reserved
//	Permission granted for duplication if not for profit
//==================================================================

//
//	class rstream
//		randomly accessed streams
//

# ifndef rstreamH
# define rstreamH

class string;
# include <fstream.h>

template <class T> class rstream {
public:
	// constructor
	rstream(const string & name);

	// access to element
	int get(unsigned int index, T & value);

	// placement of element
	void put(unsigned int index, const T & value);

	// number of elements
	unsigned int length();

private:
	fstream 	theStream;
};

# include <iterator.h>

template <class T> class rstreamIterator : public iterator<T> {
public:
	// constructor
	rstreamIterator(rstream<T> &);
	rstreamIterator(const rstreamIterator &);

	// iterator protocol 
	virtual int 	init		();
	virtual T 	operator()	();
	virtual int 	operator ! 	();
	virtual int 	operator ++ 	();
	virtual void 	operator = 	(T value);

private:
	unsigned int index;
	rstream<T> & baseStream;
};

# endif
