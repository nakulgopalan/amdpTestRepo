// -*- C++ -*-
//
#include "ma.h"
#include <vector.h>

float MovingAverage::Add(float newValue)
{
  if (n == values.length()) {
    total -= values[index];
  }
  else {
    n += 1;
  }
  total += newValue;
  values[index] = newValue;
  index += 1;
  if (index == values.length()) index = 0;
  return (total / n);
}

float MovingAverage::Average()
{
  if (n == 0) return 0.0;
  else return total / n;
}

float MovingSS::Add(float newValue)
{
  if (n == values.length()) {
    float temp = values[index];
    sumOfSquares -= temp * temp;
  }
  sumOfSquares += newValue * newValue;
  MovingAverage::Add(newValue);
  return sumOfSquares;
}

float MovingSS::SS()
{
  if (n == 0) return 0.0;
  else return sumOfSquares;
}
