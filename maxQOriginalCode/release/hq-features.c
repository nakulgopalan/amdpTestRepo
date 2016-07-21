//  -*- c++ -*-
//
#include "hq-features.h"

void RepresentOneOfN(vector<float> & fv, int & i, int val, int n)
{
  // val can range from 0 to n - 1, we need n units, using a 1-of-n
  // representation. 
  fv[i + val] = 1.0;
  i += n;
}



