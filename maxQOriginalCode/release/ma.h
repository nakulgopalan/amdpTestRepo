// -*- C++ -*-
//
// Moving Average
//
//


#ifndef MA_H
#define MA_H

#include <vector.h>

class MovingAverage
{
public:
  MovingAverage(int len): values(len), total(0.0), n(0), index(0) {};

  float Add(float newValue);	// add a new value into the average
  float Average();		// compute the average

  vector<float> values;		// the most recent set of values
  float total;			// the current running total
  int n;			// the number of real elements in values
  int index;			// index of the oldest value in
				// values.  This is the value that
				// will be replaced by the next Add operation.
};

class MovingSS: public MovingAverage
{
public:
  MovingSS(int len): MovingAverage(len), sumOfSquares(0.0) {};
  float Add(float newValue);    // add new value
  float SS();                   // sum of squares

  float sumOfSquares;
};

#endif
