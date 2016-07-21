//======================================================================
//  File:        bitvec.cpp
//  Author:      Timothy A. Budd
//  Description: This file contains the implementation of the bit
//               vector classes.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

# include <bitvec.h>
# include <iostream.h>

//----------------------------------------------------------------------
//	class bitVector implementation
//----------------------------------------------------------------------

bitVector::bitVector(unsigned int numberOfElements)
    : bitValues((numberOfElements + 7)/8, 0)
{
    // no additional initialization
}




bitVector::bitVector(const bitVector & source)
    : bitValues(source.bitValues)
{
    // no additional initialization
}



unsigned int bitVector::length() const
{
    // eight bits stored in every vector element
    return 8 * bitValues.length();
}



void bitVector::set(unsigned int index)
{
    // set the indicated bit in the data array
    bitValues[byteNumber(index)] |= mask(index);
}



void bitVector::clear(unsigned int index)
{
    // clear the indicated bit in the bitValues array
    bitValues[byteNumber(index)] &= ~ mask(index);
}



int bitVector::test(unsigned int index) const
{
    // test the indicated bit in the bitValues array
    return 0 != (bitValues[byteNumber(index)] & mask(index));
}



void bitVector::flip(unsigned int index)
{
    // flip the indicated bit in the bitValues array
    bitValues[byteNumber(index)] ^= mask(index);
}



unsigned int bitVector::byteNumber(unsigned int index) const
{
    // return index of byte containing specified value
    // byte number is index value divided by 8
    return index >> 3;
}



unsigned int bitVector::mask(unsigned int index) const
{
    // compute the amount to shift by examining
    // the low order 3 bits of the index
    const unsigned int shiftAmount = index & 07;

    // make a mask by shifting the value ``1''
    // left by the given amount
    return 1 << shiftAmount;
}



void bitVector::unionWith(bitVector& right)
{
    // form the union of set with argument set
    // only works if we're both same size
    assert(length() == right.length());

    const int asize = bitValues.length();

    for (int i = 0; i < asize; i++)
        bitValues[i] |= right.bitValues[i];
}



void bitVector::intersectWith(bitVector& right)
{
    // form the intersection of set with argument set
    // only works if we're both same size
    assert(length() == right.length());

    const int asize = bitValues.length();

    for (int i = 0; i < asize; i++)
        bitValues[i] &= right.bitValues[i];
}



void bitVector::differenceWith(bitVector& right)
{
    // form the difference of set from argument set
    // only works if we're both same size
    assert(length() == right.length());

    const int asize = bitValues.length();

    for (int i = 0; i < asize; i++)
        bitValues[i] &= ~ right.bitValues[i];
}



int bitVector::operator == (bitVector& right)
{
    // test to see if two sets are the same
    // only works if we're both same size
    assert(length() == right.length());

    // test to see if every position is equal to the argument
    const int asize = bitValues.length();

    for (int i = 0; i < asize; i++)
        if (bitValues[i] != right.bitValues[i])
            return 0;

    // all equal, two sets the same
    return 1;
}



int bitVector::subset(bitVector& right)
{
    // return true if set is subset of argument
    // only works if we're both same size
    assert(length() == right.length());

    // test to see if every position of the argument
    // is a subset of the corresponding receiver position
    const int asize = bitValues.length();

    for (int i = 0; i < asize; i++)
        if (right.bitValues[i] != (bitValues[i] & right.bitValues[i]))
            return 0;

    return 1;
}



//----------------------------------------------------------------------
//	class charSet implementation
//----------------------------------------------------------------------

charSet::charSet(char * initstr) : bitVector(1024)
{
    // initialize the set with character from string
    while (*initstr)
        add(*initstr++);
}



void charSet::add(char ele)
{
    // simply invoke method from bit vector class
    set((unsigned int) ele); 
}



void charSet::deleteAllValues()
{
    for (int i = bitValues.length() - 1; i >= 0; i--)
        bitValues[i] = 0;
}



int charSet::includes(char ele) const
{
    // return true if set includes argument character
    return test((unsigned int) ele); 
}



void charSet::remove(char ele)
{
    // remove argument character from set
    clear((unsigned int) ele);
}
