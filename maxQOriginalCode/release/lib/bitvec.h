//======================================================================
//  File:        bitvec.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface of the bit vector
//               class.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef BITVEC_H
#define BITVEC_H

#include <assert.h>
#include <vector.h>

//----------------------------------------------------------------------
//	class bitVector
//		a vector of binary (0 or 1) values
//              individual bits can be set or cleared,
//              tested or flipped
//----------------------------------------------------------------------

class bitVector
{
public:
    // constructors
    bitVector(unsigned int numberOfElements);
    bitVector(const bitVector & source);
    
    // number of legal index positions
    unsigned int    length() const;

    // bit set, change operations
    void            set(unsigned int index);
    void            clear(unsigned int index);
    int             test(unsigned int index) const;
    void            flip(unsigned int index);

    // set operations
    void            unionWith(bitVector & right);
    void            intersectWith(bitVector & right);
    void            differenceWith(bitVector & right);
    int             operator == (bitVector & right);
    int             subset(bitVector & right);

protected:
    vector<unsigned char> bitValues;    

    // position decoding functions
    unsigned int    byteNumber(unsigned int indx) const;
    unsigned int    mask(unsigned int indx) const;
};


//----------------------------------------------------------------------
//	class charSet
//		set indexed by character values
//----------------------------------------------------------------------

class charSet : public bitVector
{
public:
    // constructors
    charSet();
    charSet(char *);
    charSet(const charSet &);

    // set protocol
    void    add(char ele);
    void    deleteAllValues();
    int     includes(char ele) const;
    void    remove(char ele);
};


#endif
