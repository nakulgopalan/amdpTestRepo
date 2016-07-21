//  -*- C++ -*-
//
// Table-based value functions for HQ learning
//
//
#include "parameter.h"
#include "hq-features.h"
#include <list.h>
#include <vector.h>

float & lookup(vector<float> * table, vector<float> * fv, list<ParameterName> * features)
{
  int index = 0;
  vectorIterator<float> fvItr(*fv);
  fvItr.init();
  if (!fvItr) {
    index += (int) fvItr();
    ++fvItr;
    if (features) {
      listIterator<ParameterName> itr(*features);
      for(itr.init(); !itr; ++itr) {
	if (!fvItr) {
	  index *= featureSize(itr());
	  index += (int) fvItr();
	  ++fvItr;
	}
	else break;
      }
    }
  }
  return (*table)[index];
}
      
    
    
  
