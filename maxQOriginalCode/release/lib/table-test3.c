// -*- C++ -*-

#include <iostream.h>
#include "table.h"
#include "clrandom.h"

clrandom rng(10, 20);

// test hash tables

void print(table<int, float> & t1)
{
  tableIterator<int, float> itr(t1);
  cout << "Key\tValue" << endl;
  for (itr.init(); !itr; ++itr) {
    cout << itr()->key() << "\t" << itr()->value() << endl;
  }
}

unsigned int ident (const int & key)
{
  return key;
}

int main(int argc, char * argv[])
{
  table<int, float> t1(11, &ident);
  for (int i = 0; i < 100; i++) {
    t1[i] = rng.between(0.0, 1.0);
  }
  print(t1);
}
