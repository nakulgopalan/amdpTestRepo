//  -*- c++  -*-
//
//
//  Scaled sigmoid unit.  Target and output values are scaled
//  according to the instructions given in the constructor.
//
#ifndef SNET_H
#define SNET_H

#include "net.h"

class ScaledSigmoidNetwork: public SigmoidNetwork
{
public:
  ScaledSigmoidNetwork(int inputs, int hidden, int outputs,
		       float lb = 0.0,
		       float ub = 1.0,
		       UnitType htype = Sigmoid,
		       UnitType otype = Sigmoid);

  virtual float GetOutput(int i);             // get an output value
  virtual float SetTarget(int i, float v);    // set target value
  virtual float GetError(int i);	      // get an error value

protected:
  float lowerBound;
  float upperBound;
};

#endif
