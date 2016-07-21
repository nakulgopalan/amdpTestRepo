//  -*- C++ -*-
// 
//  class ParameterType
//
//  We define names and types for parameters.
//
//  A name is an atom of type ParameterName.  These are
//  used as the formal parameter names in predicates and nodes. 
//
//  A parameter type is a simple object that has a name and keeps track of
//  its possible values.  It can enumerate its values and it can test
//  whether a value is legal.
//

//  
//  These only need to be unique within each node, so I've defined
//  several of them.

#ifndef PARAMETER_H
#define PARAMETER_H

#include <table.h>
#include "atom.h"


class ParameterName: public Atom
{
public:
  ParameterName(char * name, int val): Atom(name, val) {};
};

extern ParameterName PNull;

class ParameterType
{
public:
  // constructors
  ParameterType(char * nm, int minv, int maxv):
    name(nm),
    minValue(minv),
    maxValue(maxv),
    nvalues(maxv - minv + 1) {};

  // operations
  int isLegalValue(int val) {
    return (minValue <= val && val <= maxValue);
  }

  // data
  char * name;
  int minValue;
  int maxValue;
  unsigned int nvalues;
};

// typedefs for parameter passing
// Parameters always take integer values.
// Every parameter has a name that is actually the value of an
// enumerated type called ParameterName *.  Defined in "enums.h".
typedef dictionary<ParameterName *, int> ActualParameterList;
typedef dictionaryIterator<ParameterName *, int> ActualParameterIterator;

// formal parameters pair a parameter name with a parameter
// type.  The parameter type information is used for type checking and
// enumeration.  The parameter name is used for binding (like keyword
// arguments). 
typedef dictionary<ParameterName *, ParameterType *> FormalParameterList;
typedef dictionaryIterator<ParameterName *, ParameterType *> FormalParameterIterator;

// a parameter pair list pairs the child's parameter names with the
// parent's parameter names.  It is used to tell which parameter
// parameter values to pass to the child.
typedef dictionary<ParameterName *, ParameterName *> ParameterPairList;
typedef association<ParameterName *, ParameterName *> ParameterPair;
typedef dictionaryIterator<ParameterName *, ParameterName *> ParameterPairIterator;

FormalParameterList * makeArgs(ParameterType * type1 = 0,
			       ParameterName * name1 = &PNull,
			       ParameterType * type2 = 0,
			       ParameterName * name2 = &PNull,
			       ParameterType * type3 = 0,
			       ParameterName * name3 = &PNull,
			       ParameterType * type4 = 0,
			       ParameterName * name4 = &PNull);

ParameterPairList * makePPL(ParameterName * child1 = &PNull,
			    ParameterName * parent1 = &PNull,
			    ParameterName * child2 = &PNull,
			    ParameterName * parent2 = &PNull,
			    ParameterName * child3 = &PNull,
			    ParameterName * parent3 = &PNull,
			    ParameterName * child4 = &PNull,
			    ParameterName * parent4 = &PNull);

ostream & operator << (ostream & str, ActualParameterList & apl);

int EquivalentAPL(ActualParameterList & apl1, ActualParameterList & apl2);

#endif
