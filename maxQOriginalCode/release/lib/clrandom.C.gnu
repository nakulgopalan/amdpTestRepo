//======================================================================
//  File:        clrandom.C
//  Author:      Timothy A. Budd
//  Description: This file contains the implementation of the random
//               number generator class.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#include "clrandom.h"
#include <sys/time.h> 

// saving and restoring random states.
ostream & operator << (ostream & str, const randomState & rs)
{
  str << rs.seedOne << " " << rs.seedTwo << " " << endl;
  return str;
}

istream & operator >> (istream & str, randomState & rs)
{
  str >> rs.seedOne >> rs.seedTwo;
  return str;
}

clrandom::clrandom(unsigned int seed1, unsigned int seed2)
{
  setseed(randomState(seed1, seed2));
};

clrandom::clrandom(const randomState & rs)
{
  setseed(rs);
}

void clrandom::setseed(const randomState & rs)
{
  Generator.reseed(rs.seedOne, rs.seedTwo);
}

void clrandom::setseedFromClock()
{
   struct timeval tv;
#ifdef WINDOWS98
   Generator.seed1(81642037);
   Generator.seed2(71219124);
#endif
#ifndef WINDOWS98
   gettimeofday(&tv, 0);
   Generator.seed1(tv.tv_usec);
   Generator.seed2(1);
#endif
}  

randomState clrandom::getseed()
{
  return randomState(Generator.seed1(), Generator.seed2());
}

unsigned int clrandom::randomInteger()
{
  return Generator.asLong();
}


double clrandom::randomReal()
{
  return Generator.asDouble();
}


double clrandom::between(double low, double high)
{
    // return a random real between low and high
    return low + (high - low) * randomReal();
}



int clrandom::between(int low, int high)
{
    // return a random integer between low and high
    double dlow  = low;
    double dhigh = high + 0.99999;

    // return random real, then truncate
    double result = between(dlow, dhigh);
    return int(result);
}

int clrandom::weightedDiscrete(vector<int> & weights)
{
	int max = 0;

		// sum the wieghts
	for (int i = 0; i < weights.length(); i++)
		max += weights[i];

		// select a random number
	int rint = between(1, max);

		// find its value
	for (int i = 0; i < weights.length(); i++)
		if (rint <= weights[i])
			return i;
		else
			rint -= weights[i];
	return 0;
}

