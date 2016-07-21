// -*- c++ -*-
//

#include "parameter.h"
#include "predicate.h"
#include <iostream.h>
#include <stdlib.h>
#include <table.h>

int Predicate::call(STATE & state, ActualParameterList & apl)
{
  // check that every formal parameter has an actual parameter with
  // a legal value
  if (formalParameters && !formalParameters->isEmpty()) {
    FormalParameterIterator itr(*formalParameters);
    for (itr.init(); !itr; ++itr) {
      // is this parameter supplied?
      if (!(apl.includesKey(itr()->key()))) {
	cerr << "Attempt to call " << name
	     << "missing argument for formal parameter "
	     << itr()->value()->name << endl;
	abort();
      }
      // does this parameter have a legal value?
      if (!(itr()->value()->isLegalValue(apl[itr()->key()]))) {
	cerr << "Argument " << itr()->value()->name
	  << " of call to " << name << " has illegal value "
	  << apl[itr()->key()] << endl;
	abort();
      }
    }
  }
  return (*func)(state, apl);
}


void Predicate::checkParameters(FormalParameterList * fpl)
{
  if (formalParameters) {
    FormalParameterIterator itr(*formalParameters);
    for (itr.init(); !itr; ++itr) {
      if (!(fpl && fpl->includesKey(itr()->key()))) {
	cout << "Predicate " << name << " parameter " << itr()->key()
	  << " does not appear in calling max node" << endl;
      }
    }
  }
}
