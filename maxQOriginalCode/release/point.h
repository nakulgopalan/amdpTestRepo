// -*- c++ -*-
//
// Simple 2-D point
//

#ifndef POINT_H
#define POINT_H

#include <clrandom.h>
#include <iostream.h>
#include "global.h"

class Point
{
public:
  // constructors
  Point(): x(0), y(0) {};
  Point(int xarg, int yarg): x(xarg), y(yarg) {};

  // variables
  int x;
  int y;

  // methods
  int operator == (const Point & other) {
    return (x == other.x && y == other.y);
  };

  void Randomize(clrandom & );
};

int ManhattanDistance(const Point & a, const Point & b);

ostream & operator << (ostream & str, Point & p);
istream & operator >> (istream & str, Point & p);

#endif
