// -*- C++ -*-
//
// 

#ifndef TAXI_MAXQ_H
#define TAXI_MAXQ_H

#include "hq.h"
#include <list.h>

// list all Max nodes here.

extern MaxNode MaxRoot;

// decomposition of the error function
extern list<RewardResponsibility *> * RewardDecomposition;
extern list<QNode *> * primitiveQNodes;
extern list<QNode *> * topLevelQNodes;

#endif
