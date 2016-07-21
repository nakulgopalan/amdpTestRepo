//======================================================================
//  File:        random.C
//  Author:      Timothy A. Budd
//  Description: This file contains the implementation of the random
//               number generator class.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#include <random.h>


random::random()
{
    // start seed with an odd prime value
    seed = 44449;
}



random::random(unsigned int s)
{
    // allow programmer to set seed
    seed = s;
}


void random::setseed(unsigned int s)
{
  seed = s;
}

unsigned int random::randomInteger()
{
    // return a new random positive value
    const unsigned int multiplier = 15625;
    const unsigned int adder      = 22221;

    // modify seed, assume overflow quietly truncated
    seed = multiplier * seed + adder;

    // return new seed (16 bits)
    return seed & 0xFFFF;
}



double random::randomReal()
{
    // return a random floating point value between 0 and 1
    double dseed = randomInteger();

    // divide by 2^16 - 1
    return dseed / 65535.0;
}



double random::between(double low, double high)
{
    // return a random real between low and high
    return low + (high - low) * randomReal();
}



int random::between(int low, int high)
{
    // return a random integer between low and high
    double dlow  = low;
    double dhigh = high + 0.99999;

    // return random real, then truncate
    double result = between(dlow, dhigh);
    return int(result);
}

int random::weightedDiscrete(vector<int> & weights)
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

// this is needed to cause Borland C++ to generate the vector<int> template
static vector<int> dummy(1);
