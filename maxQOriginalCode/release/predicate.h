//  -*- C++ -*- 
//
//  Functions for invoking predicates
//


#ifndef PREDICATE_H
#define PREDICATE_H

#include <table.h>
#include "parameter.h"
#include "state.h"
#include "get-state.h"

typedef int (*PredicateType)(STATE &, dictionary<ParameterName *, int>&);

class Predicate
{
public:
  // constructors and destructors
  Predicate(char * nm, PredicateType fn, FormalParameterList * fpl):
    name(nm), 
    func(fn),
    formalParameters(fpl) {};

  // operators
  // invoke the function, passing the parameters
  int call(STATE &, ActualParameterList &);		
  void checkParameters(FormalParameterList * fpl);

  // data
  PredicateType  func;		// function to call
  FormalParameterList * formalParameters;  
  char * name;			// name for error msgs
};

#endif

