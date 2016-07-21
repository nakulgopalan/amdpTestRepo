#include "deltaheap.h"
#include <iostream.h>
#include "clrandom.h"

unsigned int hashfun(const unsigned int & val)
{
  return val;
}

int main()
{
  DeltaHeap<unsigned int> DH(100, &hashfun);

  for (unsigned int i = 10; i > 0; i--) {
    cout << "Adding item " << i << " with priority " << (float) i << endl;
    DH.add(i, (float) i);
  }

  cout << "Promote 9 to have value 3.0" << endl;
  DH.add(9, 3.0);

  cout << "Demote 6 to have value 8.0" << endl;
  DH.add(6, 8.0);

  unsigned int u = DH.min();
  float p = DH.minPriority();
  cout << "The minimum element is " << u << " with priority " << p << endl;

  p = DH.minPriority();
  u = DH.popMin();
  cout << "Popped the minimum element " << u << " with priority " << p << endl;
  p = DH.minPriority();
  u = DH.popMin();
  cout << "Popped the minimum element " << u << " with priority " << p << endl;
  p = DH.minPriority();
  u = DH.popMin();
  cout << "Popped the minimum element " << u << " with priority " << p << endl;

  cout << "Adding element " << u << " with priority 0.0" << endl;
  DH.add(u, 0.0);

  DH.deleteAllValues();
  cout << "DH size is " << DH.size() << endl;

  clrandom rng;
  DH.add(10, -1E16);
  cout << "top priority = " << -DH.minPriority() << endl;


  return 0;
}
