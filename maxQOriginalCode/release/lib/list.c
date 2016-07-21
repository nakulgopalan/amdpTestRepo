#include "vector.h"
#include "list.h"
#include <iostream.h>

//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose

template <class T> void listInsertionSort(vector<T> & v)
{
	// declare an ordered list of the correct type
	orderedList<T> sorter;

	// copy the entire vector into the ordered list
	// the following generates a string g++ error
	vectorIterator<T> vitr(v);
	for (vitr.init(); ! vitr; ++vitr) {
		sorter.add(vitr()); 
		}

	// now copy the values back into the array
	int i = 0;
	listIterator<T> itr(sorter);
	for (itr.init(); ! itr; ++itr)
		v[i++] = itr();
}

template <class T>
ostream & operator << (ostream & ostr, list<T> & target)
{
  // write out a list of objects surrounded by parentheses and separated by spaces
  ostr << "(";
  listIterator<T>  itr(target);

  itr.init();
  while (!itr) {
    ostr << itr();
    ++itr;
    if(!itr) ostr << " ";
  }
  ostr << ")";
  return ostr;
}

template <class T>
istream & operator >> (istream & istr, list<T> & target)
{
  // read in a list of objects surrounded by parentheses and separated by spaces
  // we push them onto the target list and then reverse the list
  assert(target.isEmpty()); // target must be empty

  char delim;
  istr >> delim;
  if (delim != '(') {
    cerr << "Found `" << delim << "' where `(' was expected." << endl;
  }
  assert(delim == '(');
  do {
    istr >> delim;
    if (delim == ')') break;
    istr.putback(delim);
    T val;
    istr >> val;
    target.add(val);
  } while (istr);
  target.reverse();
  return istr;
}

