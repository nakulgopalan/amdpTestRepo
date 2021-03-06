// -*- C++ -*-
//
// State abstraction.  This is a pure virtual class.
//
//

#ifndef STATE_H
#define STATE_H

#include <iostream.h>
#include <clrandom.h>
#include "atom.h"
#include "parameter.h"
#include <stdlib.h>

class Action: public Atom
{
public:
  Action(char * name, int val): Atom(name, val) {};
};

class Error: public Atom
{
public:
  Error(char * name, int val): Atom(name, val) {};
};

class State
{
public:
  // constructors
  State(unsigned int seed): rng(seed), cumulativeReward(0) { Init(); };

  // methods

  virtual void Init() { cumulativeReward = 0; }	      // generate initial state
  virtual void Act(Action * act, ActualParameterList & apl);  // perform an action
  virtual void Print(ostream & str);		      // print out the state
  virtual void Save(ostream & str); // dump state in machine-readable format
  virtual void Read(istream & str); // re-load state in
				    // machine-readable format
  virtual int Terminated();
  virtual State * Duplicate();			      // make a copy
  virtual int featureSize(ParameterName * pname);
  virtual float featureValue(ParameterName * pname);			     
  virtual void insertFeature(ParameterName * pname,
			     ActualParameterList & apl,
			     vector<float> & fv,
			     int & i);
  virtual int operator == (State & other) {
    return 1;
  }

  // data fields
  clrandom rng;					      // random number generator
  float cumulativeReward;			      // total reward in simulation
  Error * currentError;				      // cause of current error
};
  
#endif
