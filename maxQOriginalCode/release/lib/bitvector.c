//  -*- C++ -*- 
//
// Efficient bit vector class
//
//
#include <assert.h>
#include "bitvector.h"

bitvector::bitvector(unsigned int numberElements, unsigned char initialValue)
{
  size = numberElements;
  dataSize = (size + 7) / 8;   // number of bytes
  data = new unsigned char [dataSize];
  fill(initialValue);
}

// copy constructor:

bitvector::bitvector(const bitvector & source)
{
  size = source.size;
  dataSize = source.dataSize;
  data = new unsigned char [dataSize];
  assert(data != 0);

  // copy elements
  for (int i = 0; i < dataSize; i++) data[i] = source.data[i];
}

// destructor
bitvector::~bitvector()
{
  if (data) delete [] data;
  data = 0;
}

void bitvector::fill(unsigned char value)
{
  // set to all zeros or all ones
  unsigned char fullInit = (value == 0)? 0 : 0xff;
  
  for (int i = 0; i < dataSize; i++) data[i] = fullInit;
}
    

unsigned int bitvector::setSize(unsigned int numberOfElements)
{
  // allocate new space
  unsigned int newDataSize = (numberOfElements + 7) / 8;
  unsigned char * newData = new unsigned char [newDataSize];

  assert(newData != 0);

  if (newDataSize <= dataSize) {
    // shrinking - copy as many elements as possible
    for (int i = 0; i < newDataSize; i++) {
      newData[i] = data[i];
    }
  }
  else {
    // expanding -- copy old values, remainder uninitialized
    for (int i = 0; i < dataSize; i++) {
      newData[i] = data[i];
    }
  }
  // delete old data buffer
  delete [] data;
  
  // install new values
  data = newData;
  dataSize = newDataSize;
  size = numberOfElements;

  // return new number of elements
  return size;
}

unsigned int bitvector::setSize(unsigned int numberOfElements,
				unsigned char initialValue)
{
  // allocate new space
  unsigned int newDataSize = (numberOfElements + 7) / 8;
  unsigned char * newData = new unsigned char [newDataSize];

  assert(newData != 0);

  if (newDataSize <= dataSize) {
    // shrinking - copy as many elements as possible
    for (int i = 0; i < newDataSize; i++) {
      newData[i] = data[i];
    }
  }
  else {
    // expanding -- copy old values, remainder uninitialized
    for (int i = 0; i < dataSize; i++) {
      newData[i] = data[i];
    }
    // now fill remaining space with initial Values
    // first fill the unused bytes
    unsigned char fullVal = (initialValue == 0)? 0 : 0xff;

    for (int i = dataSize; i < newDataSize; i++) {
      newData[i] = fullVal;
    }
    // we will fill the rest below

  }
  // delete old data buffer
  delete [] data;
  
  // remember old number of elements
  int oldSize = size;

  // install new values
  data = newData;
  dataSize = newDataSize;
  size = numberOfElements;

  // now figure out which elements need to be initialized
  for (int i = oldSize; i < size; i++) {
    SetBit(i, initialValue);
    if (i % 8 == 7) break;
  }

  // return new number of elements
  return size;
}
  
