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


//----------------------------------------------------------------------
//	class random
//		random numbers generated using
//              linear congruent algorithm
//              assumes 16-bit values
//----------------------------------------------------------------------

class random
{
public:
    // constructors
    random();
    random(unsigned int);

    // methods used to get values
    unsigned int randomInteger();
    double       randomReal();
    double       between(double, double);
    int          between(int, int);
    void         setseed(unsigned int);

    int	weightedDiscrete(vector<int> &);

private:
    unsigned int seed;
};

#endif
