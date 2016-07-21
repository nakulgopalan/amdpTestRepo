//==================================================================
//	File: 		openHashTable.h
//	Author: 	Timothy A. Budd
//	Platform:	Cfront
//	Description: 	Template Description of open addressing hash table ADT
//	Copyright (c) 1992 by Timothy A. Budd, All Rights Reserved
//	Permission granted for duplication if not for profit
//==================================================================

# ifndef openHashTableH
# define openHashTableH

//
//	class openHashTable
//		external hash table
//		type T must supply function ``isEmpty'' and equality test
//

# include <rstream.h>

template <class T> class openHashTableIterator;

template <class T> class openHashTable {
public:
	// constructor
	openHashTable(const string & name, unsigned int (*f)(const T &));

	// operations
	void create(unsigned int size);
	int add(const T &);
	int  includes(T &);
	void remove(const T &);

private:
	friend class openHashTableIterator<T>;
	// data values - hash function, random access stream and size of file
	unsigned int  (*hashfun)(const T &);
	rstream<T>    rfile;
	unsigned int  size;
};

# endif
