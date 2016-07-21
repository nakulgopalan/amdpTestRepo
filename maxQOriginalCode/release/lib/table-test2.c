// -*- C++ -*-

#include <iostream.h>
#include <table.h>


void print(dictionary<int, int> * d1)
{
  dictionaryIterator<int, int> itr(*d1);
  cout << "Key\tValue" << endl;
  for (itr.init(); !itr; ++itr) {
    cout << itr()->key() << "\t" << itr()->value() << endl;
  }
}

int main(int argc, char * argv[])
{
  dictionary<int, int> * d1 = new dictionary<int, int>;
  for (int i=0; i < 10; i++) (*d1)[i] = 2 * i;
  cout << "Dictionary d1:\n";
  print(d1);
  dictionary<int, int> * d2 = d1->Duplicate();
  cout << "Dictionary d2:\n";
  print(d2);
  delete d1;
  cout << "Dictionary d2 again, after deleting d1:\n";
  print(d2);
}
