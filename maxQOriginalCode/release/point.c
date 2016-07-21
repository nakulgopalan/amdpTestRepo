// -*- C++ -*-
//

#include "point.h"
#include <clrandom.h>

void Point::Randomize(clrandom & rng)
{
  // set this point to a random value
  x = rng.between(0, worldSize - 1);
  y = rng.between(0, worldSize - 1);
}

#define abs(x) (((x) < 0)? -(x) : (x))

int ManhattanDistance(const Point & a, const Point & b)
{
  return abs(a.x - b.x) + abs(a.y - b.y);
}

ostream & operator << (ostream & str, Point & p)
{
  return str << "(" << p.x << "," << p.y << ")";
}

istream & operator >> (istream & str, Point & p)
{
  char c;
  str >> c;
  assert(c == '(');
  str >> p.x;
  str >> c;
  assert(c == ',');
  str >> p.y;
  str >> c;
  assert(c == ')');
  return str;
}
