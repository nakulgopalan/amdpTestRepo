//======================================================================
//  File:        vector.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface and implementation
//               of the vector template classes.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef VECTOR_H
#define VECTOR_H


#include <assert.h>
#include <iterator.h>
#include <iostream.h>


//----------------------------------------------------------------------
//	class vector
//		type safe and access safe vector data type
//		permits dynamic modification of vector length
//----------------------------------------------------------------------

template <class T> class vector
{
public:
    // constructors and destructor
    vector(): data(0), size(0) {};
    vector(unsigned int numberElements);
    vector(unsigned int numberElements, T initialValue);
    vector(const vector<T> & source);
    virtual ~vector();

    // access to elements via subscript
    T &           operator [] (unsigned int index) const;

    // assignment
    vector<T> &   operator = (const vector<T> &);

    // other operations
    void          fill (const T value);

    // length of vector
    unsigned int  length() const { return size; };

    // dynamically change size
    unsigned int  setSize(unsigned intnumberOfElements);
    unsigned int  setSize(unsigned intnumberOfElements,
			 T initialValue);

    ostream &     print(ostream & str);

protected:
    // data areas
    T *           data;
    unsigned int  size;
};



//----------------------------------------------------------------------
//	class boundedVector
//		vectors with explicit upper and lower bounds
//----------------------------------------------------------------------

template <class T> class boundedVector : public vector<T>
{
public:
    // constructors
    boundedVector(int lowIndex, int highIndex);
    boundedVector(int lowIndex, int highIndex, T & initialValue);
    boundedVector(const boundedVector<T> & source);

    // element access
    T &  operator [] (int index) const;

    // structural information
    int  lowerBound() const;
    int  upperBound() const;

protected:
    int  lowbound;
};



//----------------------------------------------------------------------
//	class vectorIterator
//		iterator protocol used to loop over vector elements
//----------------------------------------------------------------------

template <class T> class vectorIterator : public iterator<T>
{
public:
    // constructor
    vectorIterator(vector<T> &);
    vectorIterator(const vectorIterator &);

    // iterator protocol
    virtual int   init();
    virtual T &   operator ()();
    virtual int   operator !();
    virtual int   operator ++();
    virtual void  operator =(T newValue);

    // new methods specific to vector iterators
    int           operator --();
    int           key();

protected:
    // data fields
    unsigned int  currentKey;
    vector<T> &   data;
};

//
//	class orderedVector
//		vector maintained in sorted order
//

template <class T> class orderedVectorIterator;

template <class T> class orderedVector {
public:
	// constructors
	orderedVector();
	orderedVector(const orderedVector<T> & v);

	// subscript operation
	T & 	operator [ ] 	   (unsigned int index) const;

	// protocol for searching
	void     add               (T value);
	void     deleteAllValues   ();
	int      includes          (T value) const;
        int      isEmpty           () const;
	void     remove            (T value);

private:
	// data area is vector for values
	vector<T> data;

	friend class orderedVectorIterator<T>;
};

template <class T>
class orderedVectorIterator : public vectorIterator<T>
{
public:
	orderedVectorIterator(orderedVector<T> &);
};

template <class T> inline
orderedVectorIterator<T>::orderedVectorIterator(orderedVector<T> & x)
	: vectorIterator<T>(x.data)
{
	// no further initialization
}


//
//	class matrix
//		two dimensional arrays
//

template <class T>
class matrix {
public:
	matrix(unsigned int numberOfRows, unsigned int numberOfColumns);
	matrix(unsigned int numberOfRows, unsigned int numberOfColumns,
			T initialValue);

	~matrix();

	vector<T> & operator [ ] (unsigned int index) const;

	int numberRows() const;
	int numberColumns() const;
	void fill(const T value);

private:
	vector<vector<T> *> rows;
};

//
//	class enumVector
//		vector abstraction index by 
//		elements of an enumerated type
//

template <class E, class T>
class enumVector : public vector<T> {
public:
	// constructors
	enumVector(E max);
	enumVector(const enumVector & v);

	// operations
	T & operator [ ] (E index);
};

//----------------------------------------------------------------------
//	class vector implementation
//----------------------------------------------------------------------

template <class T> vector<T>::vector(unsigned int numberElements)
    : size(numberElements)
{
    // create and initialize a new vector
    // allocate the space for the elements
    data = new T[size];

    // check that allocation was successful
    assert(data != 0);
}



template <class T> vector<T>::vector(unsigned int numberElements,
    T initialValue) : size(numberElements)
{
    // create and initialize a new vector
    // allocate the space for the elements
    data = new T[size];
    assert(data != 0);

    // set each element to the initial value
    for (int i = 0; i < size; i++)
	data[i] = initialValue;
}



template <class T> vector<T>::vector(const vector<T> & source)
    : size(source.size)
{
    // create and initialize a new vector
    // allocate the space for the elements
    data = new T[size];
    assert(data != 0);

    // copy values from old vector
    for (int i = 0; i < size; i++)
	data[i] = source.data[i];
}



template <class T> vector<T>::~vector()
{
    // free teh dynamic memory buffer
    delete [] data;
    data = 0;
}



// inline this routine
template <class T> inline T & vector<T>::operator [] (unsigned int index) const
{
    // subscript a vector value
    // check that index is valid
    assert(index < size);

    // return requested element
    return data[index];
}



template <class T> vector<T> & vector<T>::operator =
    (const vector<T> & right)
{
    // match sizes
    if (size != right.size)
	setSize(right.size);

    // copy the elements
    for (int i = 0; i < right.size; i++)
	data[i] = right.data[i];

    // return current value
    return *this;
}


// fill the vector with a constant
template <class T> void vector<T>::fill(const T value)
{
  for (int i = 0; i < size; i++) {
    data[i] = value;
  }
}


template <class T> unsigned int vector<T>::setSize
    (unsigned int numberOfElements)
{
    // dynamically alter the size of the vector

    // first create the new data area
    T * newData = new T[numberOfElements];
    assert(newData != 0);

    if (data) {
      if (numberOfElements <= size)
	{
	  // data area is shrinking - copy as many elements as possible
	       for (int i = 0; i < numberOfElements; i++)
		 newData[i] = data[i];
	}
      else
	{
	  // data area is growing - first copy old values
	       for (int i = 0; i < size; i++)
		 newData[i] = data[i];
	}
    
      // delete the old data buffer
      delete [] data;
    }

    // update the data member fields
    size = numberOfElements;
    data = newData;

    // return new size
    return size;
}



template <class T> unsigned int vector<T>::setSize
    (unsigned int numberOfElements, T initialValue)
{
    // dynamically alter the size of the vector

    // first create the new data area
    T * newData = new T[numberOfElements];
    assert(newData != 0);

    if (numberOfElements <= size)
    {
	// data area is shrinking - copy as many elements as possible
	for (int i = 0; i < numberOfElements; i++) {
	  newData[i] = data[i];
	}
    }
    else
    {
	// data area is growing - first copy old values
	for (int i = 0; i < size; i++) {
	  newData[i] = data[i];
	}

	// then initialize new values
	for (int i = size; i < numberOfElements; i++)
	  newData[i] = initialValue;
    }

    // delete the old data buffer
    delete [] data;

    // update the data member fields
    size = numberOfElements;
    data = newData;

    // return new size
    return size;
}

// dot product
template <class T> T dot (const vector<T> & left,
			  const vector<T> & right)
{
  assert(left.length() == right.length());

  T  answer = 0;

  for (int i = 0; i<size; i++) {
    answer += left[i] * right[i];
  }
  return answer;
}


template <class T> ostream & vector<T>::print (ostream & str)
{
  str << "[";
  for (int i = 0; i < size; i++) {
    str << data[i];
    if (i + 1 < size) str << " ";
  }
  return str << "]";
}
  
template <class T> ostream & operator << (ostream & str, vector<T> & v)
{
  return v.print(str);
}


//----------------------------------------------------------------------
//	class boundedVector implementation
//----------------------------------------------------------------------

template <class T> boundedVector<T>::boundedVector
    (int lowIndex, int highIndex)
    : lowbound(lowIndex), vector<T>(1 + highIndex - lowIndex)
{
    // create and initialize a new bounded vector
    assert(lowIndex <= highIndex);
}



template <class T> boundedVector<T>::boundedVector
    (int lowIndex, int highIndex, T & initialValue)
    : lowbound(lowIndex), vector<T>(1 + highIndex - lowIndex, initialValue)
{
    // create and initialize a new bounded vector
    assert(lowIndex <= highIndex);
}



template <class T> boundedVector<T>::boundedVector
    (const boundedVector<T> & source)
    : lowbound(source.lowbound), vector<T>(source)
{
    // no further initialization necessary
}



template <class T> T & boundedVector<T>::operator [](int index) const
{
    // subscript operator for bounded vectors
    // subtract off lower bound
    // yielding value between 0 and size of vector
    // then use subscript from parent class
    return vector<T>::operator [](index - lowbound);
}



template <class T> int boundedVector<T>::lowerBound() const
{
    // return smallest legal index for bounded vector
    return lowbound;
}



template <class T> int boundedVector<T>::upperBound() const
{
    // return largest legal index for bounded vector
    return lowerBound() + length() - 1;
}



//----------------------------------------------------------------------
//	class vectorIterator implementation
//----------------------------------------------------------------------

template <class T> vectorIterator<T>::vectorIterator(vector<T> & v)
    : data(v)
{
    // create and initialize a vector iterator
    init();
}

template <class T> vectorIterator<T>::
	vectorIterator(const vectorIterator<T> & x)
		: data(x.data), currentKey(x.currentKey)
{
	// no further initialization
}


template <class T> int vectorIterator<T>::init()
{
    // set current key to firstelement
    currentKey = 0;
    return operator !();
}



template <class T> T & vectorIterator<T>::operator ()()
{
    // return current value
    return data[currentKey];
}



template <class T> int vectorIterator<T>::operator !()
{
    // return true if there is a current element
    return currentKey < data.length();
}



template <class T> int vectorIterator<T>::operator ++()
{
    // advance to the next element
    currentKey++;
    return operator !();
}



template <class T> void vectorIterator<T>::operator = (T newValue)
{
    // change the current index value
    data[currentKey] = newValue;
}



template <class T> int vectorIterator<T>::operator --()
{
    // move current key back one element
    if (currentKey > 0)
	currentKey--;

    return operator !();
}



template <class T> int vectorIterator<T>::key()
{
    // return index of current value
    return currentKey;
}

//----------------------------------------------------------------------
//	class matrix implementation
//----------------------------------------------------------------------

template <class T>
matrix<T>::matrix(unsigned int numberOfRows, unsigned int numberOfColumns) 
	: rows(numberOfRows)
{
	// create and initialize a new matrix value 
	// now allocate each row of data
	for (unsigned int i = 0; i < numberOfRows; i++) {
		rows[i] = new vector<T>(numberOfColumns);
		// check that allocation was successful
		assert(rows[i] != 0);
		}
}

template <class T>
matrix<T>::matrix(unsigned int numberOfRows, unsigned int numberOfColumns, 
	T initialValue) : rows(numberOfRows)
{
	// create and initialize a new matrix value 
	// now allocate each row of data
	for (unsigned int i = 0; i < numberOfRows; i++) {
		rows[i] = new vector<T>(numberOfColumns, initialValue);
		// check that allocation was successful
		assert(rows[i] != 0);
		}
}

template <class T>
matrix<T>::~matrix()
{
	// free up dynamic storage allocated by matrix object
	unsigned int max = rows.length();
	for (unsigned int i = 0; i < max; i++) {
		// some compilers do not allow 
		// subscripted variables in delete 
		//vector<T> * p = rows[i];
		//delete p;
		rows[i] = 0;
		}
}

template <class T>
vector<T> & matrix<T>::operator [ ] (unsigned int index) const
{	// return elemement (vector) index by value
	// check that reference is valid
	assert(rows[index] != 0 );

	// return vector value
	return * rows[index];
}


template <class T>
int matrix<T>::numberRows() const
{
	// return number of rows in vector
	return rows.length();
}

template <class T>
int matrix<T>::numberColumns() const
{
	// return number of columns in vector
	return rows[0]->length();
}

template <class T>
void matrix<T>::fill (const T value)
{
  for (int i = 0; i < rows.length(); i++) {
    rows[i]->fill(value);
  }
}

//----------------------------------------------------------------------
//	class enumVector implementation
//----------------------------------------------------------------------

template <class E, class T> 
enumVector<E, T>::enumVector(E max) : vector<T>(1 + int(max))
{
	// create and initialize an instance of enumerated vector
}

template <class E, class T> 
enumVector<E, T>::enumVector(const enumVector<E, T> & source) : vector<T>(source)
{
	// create and initialize an instance of enumerated vector
}

template <class E, class T> 
T & enumVector<E, T>::operator [ ] (E index)
{
	// subscript an enumerated vector
	// simply convert enumerated index value into integer
	return vector<T>::operator [ ] (int(index));
}

// *******************************************************
//	template functions 
//	Cfront demands these are in a different file,
//	g++ and borland want them in this file.
// *******************************************************

# ifdef __GNUG__

# include <vector.c>

# endif

# ifndef __GNUG__

template <class VecType, class EleType>
int binarySearch(VecType data, EleType ele, unsigned int max);

template <class T> void swap( vector<T> & data, int i, int j);

template <class T> void bubbleSort(vector<T> & data);

template <class T> void selectionSort(vector<T> & data);

template <class T> void insertionSort(vector<T> & data);

template <class T>
int partition(vector<T> & v, int low, int high, int pivotIndex);

template <class T>
T findElement(vector<T> & v, int N, int low, int high);

template <class T>
void quackSort(vector<T> & v, int low, int high);

template <class T>
void quackSort(vector<T> & v);

# endif

//
//	implementation of ordered vectors
//

template <class T>  orderedVector<T>::orderedVector() : data(0)
{
	// no further initialization required
}

template <class T>
T & 	orderedVector<T>::operator [ ] (unsigned int index) const
{
	return data[index];
}

template <class T> 
orderedVector<T>::orderedVector(const orderedVector<T> & v) : data(v.data)
{
	// no further initialization required
}

template <class T>  void orderedVector<T>::deleteAllValues()
{	// set the size of the vector to zero
	// thereby deleting all values
	data.setSize(0);
}

template <class T>  int orderedVector<T>::isEmpty() const
{	// collection is empty if vector is length zero
	return data.length() == 0;
}

template <class T> void orderedVector<T>::add(T value)
{
	// get the current length of the vector
	unsigned int max = data.length();

	// perform binary search to find proper location for element
	unsigned int index = binarySearch(data, value, max);

	// grow vector to new size
	data.setSize(max + 1);

	// move everything up to make space 
	for (unsigned int i = max; i > index; i--)
		data[i] = data[i-1];

	// place element in location
	data[index] = value;
}

template <class T> int orderedVector<T>::includes(T value) const
{	// see if vector includes given element
	unsigned int max = data.length();
	unsigned int index = binarySearch(data, value, max);
	// if value is the right element, return 1
	if (index < max)
		if (value == data[index])
			return 1;
	// not found
	return 0;
}

template <class T> void orderedVector<T>::remove(T value)
{	// remove a value from collection, if present
	unsigned int max = data.length();
	unsigned int index = binarySearch(data, value, max);
	// if value is the right element, return 1
	if (index < max)
		if (value == data[index]) {
			// found element, remove it
			max = max - 1;
			for (unsigned int i = index; i < max; i++)
				data[i] = data[i+1];
			// then change the size of the vector
			data.setSize(max);
			}
}

#endif
