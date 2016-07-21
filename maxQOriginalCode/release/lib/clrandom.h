// -*- C++ -*-
//======================================================================
//  File:        random.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface of the random
//               number generator class.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef RANDOM_H
#define RANDOM_H

# include <vector.h>

class randomState
{
public:
  randomState(): seedOne(0) {};
  // ignore optional second argument
  randomState(unsigned short One, unsigned short Two = 0): seedOne(One) {};

  randomState(const randomState & other) {
    seedOne = other.seedOne;
  }



  
  // data
  unsigned short  seedOne;
};

ostream & operator << (ostream & str, const randomState & rs);
istream & operator >> (istream & str, randomState & rs);


//----------------------------------------------------------------------
//	class random
//		random numbers generated using
//              linear congruent algorithm
//              assumes 16-bit values
//----------------------------------------------------------------------

class clrandom
{
public:
    // constructors
    clrandom();
    clrandom(unsigned short);

    // methods used to get values
    unsigned int randomInteger();
    double       randomReal();
    double       between(double, double);
    int          between(int, int);
    void         setseed(const randomState &);
    void         setseedFromClock();

    int	weightedDiscrete(vector<int> &);

    // saving the seed
    randomState getseed() {return randomState(seed);}

private:
    unsigned short seed;
};

#endif
