// -*- C++ -*-
//
// Simple support for lisp-like symbolic atoms.
//

#ifndef ATOM_H
#define ATOM_H

#include <list.h>
#include <iostream.h>

// read, write, and print atoms.
class Atom
{
public:
  Atom(char * namearg, int valarg);
  Atom(Atom &);
  Atom();
    
  // Each time we create an atom, we intern it into this list.
  // Whenever we read an atom, we search this list to lookup the atom 
  // and return a pointer to that atom.
  static list<Atom *> AllAtoms;

  char * name;
  int value;
  int operator == (Atom & other) {
    return value == other.value;
  }
};

// we read and write using pointers, so that we can intern.
ostream & operator << (ostream & str, Atom * atom);
istream & operator >> (istream & str, Atom * & atom);

#endif
