// -*- C++ -*-
//  This is a test
//
#include "atom.h"
#include <list.h>
#include <string.h>
#include <stdlib.h>

list<Atom *> Atom::AllAtoms;


Atom::Atom(char * namearg, int valarg):
  name(namearg), value(valarg)
{
  // check for duplicates before adding
  listIterator<Atom *> itr(Atom::AllAtoms);
  for (itr.init(); !itr; ++itr) {
    if (::strcmp(name, itr()->name) == 0) {
      cerr << "Attempt to create duplicate Atom " << name
	   << " old atom has value " << itr()->value
	     << " new atom has value " << value << endl;
      abort();
    }
  }
  Atom::AllAtoms.add(this);
}

Atom::Atom()
{
  cerr << "Attempt to create null atom using default constructor" <<
    endl;
  abort();
}

Atom::Atom(Atom & other)
{
  cerr << "Attempt to copy an atom using the copy constructor" <<
    endl;
  abort();
}

ostream & operator << (ostream & str, Atom * atom)
{
  if (atom) str << atom->name;
  else str << "(null atom)";
  return str;
}

istream & operator >> (istream & str, Atom * & atom)
{
  char buf[10000];
  str >> buf;
  listIterator<Atom *> itr(Atom::AllAtoms);
  for (itr.init(); !itr; ++itr) {
    if (::strcmp(itr()->name , buf) == 0) {
      atom = itr();
      return str;
    }
  }
  cerr << "Attempt to read unknown atom with name " << buf << endl;
  abort();
  return str;
}


