//======================================================================
//  File:        split.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface to the split
//               function, which splits a sentence string into words
//               and returns a vector of these words.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef SPLIT_H
#define SPLIT_H


#include <cstring.h>
#include <vector.h>
#include <bitvec.h>


// Function Prototype

void split(string &, const charSet &, vector<string> &);


#endif
