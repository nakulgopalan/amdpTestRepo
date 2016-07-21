//======================================================================
//  File:        clrandom.h
//  Author:      Thomas G. Dietterich
//  Description: This file contains the interface of the clrandom
//               number generator class.
//
// The routines here are a repackaging of the random number routines
// in the GNU libg++ library
//======================================================================

#ifndef CLRANDOM_H
#define CLRANDOM_H

#include <vector.h>
#include "gnu-mlcg.h"
#include <iostream.h>


//----------------------------------------------------------------------
//	a readable and writable random state
//----------------------------------------------------------------------

class randomState
{
public:
  // constructors
  randomState(): seedOne(0), seedTwo(0) {};
  randomState(unsigned int One, unsigned int Two):
    seedOne(One),
    seedTwo(Two) {};
  randomState(const randomState & other) {
    seedOne = other.seedOne;
    seedTwo = other.seedTwo;
  };

  // data
  unsigned int  seedOne;
  unsigned int  seedTwo;
};

ostream & operator << (ostream & str, const randomState & rs);
istream & operator >> (istream & str, randomState & rs);

//----------------------------------------------------------------------
//	class clrandom
//----------------------------------------------------------------------

class clrandom
{
public:
  // constructors
  clrandom(unsigned int seed1 = 1,
	   unsigned int seed2 = 2);
  clrandom(const randomState & rs);

  // GNU RNG Multiple Linear Congruential Generator
  MLCG         Generator;

  // methods used to get values
  unsigned int randomInteger();
  double       randomReal();
  double       between(double, double);
  int          between(int, int);
  void         setseed(const randomState & rs);
  void         setseedFromClock();
  randomState  getseed();
  
  int	       weightedDiscrete(vector<int> &);
  int          Initialized;
};

#endif
