//   -*-  C++  -*-
//
//
#include "snet.h"

ScaledSigmoidNetwork::ScaledSigmoidNetwork(int inputs, int hidden, int outputs,
					   float lb = 0.0,
					   float ub = 1.0,
					   UnitType htype = Sigmoid,
					   UnitType otype = Sigmoid):
  SigmoidNetwork(inputs, hidden, outputs, htype, otype),
  lowerBound(lb),
  upperBound(ub)
{}

float ScaledSigmoidNetwork::GetOutput(int i)
{
  float result = this->SigmoidNetwork::GetOutput(i);
  return lowerBound + (result * (upperBound - lowerBound));
}

float ScaledSigmoidNetwork::SetTarget(int i, float v)
{
  this->SigmoidNetwork::SetTarget(i, (v - lowerBound) / (upperBound - lowerBound));
  return v;
}


float ScaledSigmoidNetwork::GetError(int i)	      // get an error value
{
  // errors need to be scaled by the scale factor.
  float result = this->SigmoidNetwork::GetError(i);
  return result * (upperBound - lowerBound);
}
