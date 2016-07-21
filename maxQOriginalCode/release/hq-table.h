// -*- C++  -*-

#ifndef HQ_TABLE_H
#define HQ_TABLE_H

#include "parameter.h"
#include <vector.h>
#include <list.h>

float & lookup(vector<float> * table, vector<float> * fv, list<ParameterName> * features);

#endif
