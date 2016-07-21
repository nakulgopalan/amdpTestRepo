// -*- C++ -*-
//
//  Efficient bit vector class
//
//  Has much of the functionality of the vector<T> class, but no
//  indexed access or indexed assignment (this could have been done
//  using a helper class, but that would have been very expensive)
//

#ifndef BITVECTOR_H
#define BITVECTOR_H

#include <assert.h>
#include <iostream.h>

class bitvector
{
public:
  // constructors and destructor
  bitvector(): data(0), size(0), dataSize(0) {};
  bitvector(unsigned int numberElements, unsigned char initialValue = 0);
  bitvector(const bitvector & source);
  virtual ~bitvector();

  unsigned char GetBit(unsigned int index)
  {
    return ((data[byteNumber(index)] & mask(index)) == 0)? 0 : 1;
  }
    
  void SetBit(unsigned int index, unsigned char val)
  {
    if (val == 0) {
      data[byteNumber(index)] &= ~ mask(index);
    }
    else {
      data[byteNumber(index)] |= mask(index);
    }
  }

  // other operations
  void          fill (unsigned char value);

  // length of bitvector
  unsigned int  length() const { return size; };

  // dynamically change size
  unsigned int  setSize(unsigned int  numberOfElements);
  unsigned int  setSize(unsigned int  numberOfElements,
			unsigned char initialValue);

protected:
  // data areas
  unsigned char * data;
  unsigned int dataSize;  // length of data array
  unsigned int  size;     // number of bits in the bitvector
  // position decoding functions
  unsigned int    byteNumber(unsigned int indx) const
  {
    assert(indx < size);
    // return index of byte containing specified value
    // byte number is index value divided by 8
    return indx >> 3;
  };
	     
  unsigned int    mask(unsigned int indx) const
  {
    // compute the amount to shift by examining
    // the low order 3 bits of the index
    const unsigned int shiftAmount = indx & 07;
    
    // make a mask by shifting the value ``1''
    // left by the given amount
    return 1 << shiftAmount;
  }


};

#endif
