// -*- C++ -*-
//
// Test whether dictionaries are properly reclaiming all of their memory. 
//

#include "table.h"
#include "clrandom.h"

clrandom rng(1234);

int main(int argc, char * argv[])
{
  dictionary<int, float> * dict;
  for (int iter=0; iter < 10000; iter++) {
    dict = new dictionary<int, float>;
    for (int i = 1; i < 10000; i++) {
      (*dict)[i] = rng.randomReal();
    }
    delete dict;
  }
}
