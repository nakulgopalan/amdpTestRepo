// -*- C++ -*-
//
#include "parameter.h"

ParameterName PNull("PNull", 0);

// Generic function for constructing formal parameter lists
// all arguments are optional
FormalParameterList * makeArgs(ParameterType * type1,
			       ParameterName * name1,
			       ParameterType * type2,
			       ParameterName * name2,
			       ParameterType * type3,
			       ParameterName * name3,
			       ParameterType * type4,
			       ParameterName * name4)
{
  FormalParameterList * answer = new FormalParameterList;
  if (type1) {
    assert(name1 != &PNull);
    (*answer)[name1] = type1;
  }

  if (type2) {
    assert(name2 != &PNull);
    (*answer)[name2] = type2;
  }

  if (type3) {
    assert(name3 != &PNull);
    (*answer)[name3] = type3;
  }

  if (type4) {
    assert(name4 != &PNull);
    (*answer)[name4] = type4;
  }

  return answer;
}

ParameterPairList * makePPL(ParameterName * child1,
			    ParameterName * parent1,
			    ParameterName * child2,
			    ParameterName * parent2,
			    ParameterName * child3,
			    ParameterName * parent3,
			    ParameterName * child4,
			    ParameterName * parent4)
{
  ParameterPairList * answer = new ParameterPairList;
  if (child1 != &PNull) {
    assert(parent1 != &PNull);
    (*answer)[child1] = parent1;
  }
  if (child2 != &PNull) {
    assert(parent2 != &PNull);
    (*answer)[child2] = parent2;
  }
  if (child3 != &PNull) {
    assert(parent3 != &PNull);
    (*answer)[child3] = parent3;
  }
  if (child4 != &PNull) {
    assert(parent4 != &PNull);
    (*answer)[child4] = parent4;
  }
  return answer;
}

ostream & operator << (ostream & str, ActualParameterList & apl)
{
  ActualParameterIterator itr(apl);
  str << "(";
  for (itr.init(); !itr; ++itr) {
    str << itr()->key() << "->" << itr()->value() << ";";
  }
  str << ")";
  return str;
}


int EquivalentAPL(ActualParameterList & apl1, ActualParameterList &
		  apl2)
{
  // compare two apl's and return t if they contain the same elements.
  ActualParameterIterator itr(apl1);
  if (apl1.data->length() != apl2.data->length())  return 0;
  for (itr.init(); !itr; ++itr) {
    if (apl2[itr()->key()] != itr()->value()) return 0;
  }
  return 1;
}
