// -*- C++ -*-

#ifndef DELTATAXI_H
#define DELTATAXI_H

#include "taxi.h"
#include <iostream.h>

extern float DeltaDestinationProbability;

class DeltaTaxiState: public TaxiState
{
public:
  // constructors
  DeltaTaxiState(unsigned int seed): TaxiState(seed) { Init(); };

  // the only change is in the action routine
  virtual void Act(Action * act, ActualParameterList &);	// perform an action
  virtual DeltaTaxiState * Duplicate();
  virtual void Print(ostream & str);	// print out the state
};



class DeltaTaxiStateIterator: public iterator<DeltaTaxiState>
{
public:
  DeltaTaxiStateIterator(DeltaTaxiState & state);
  DeltaTaxiState &  state;   // the current state
  int stateIsValid;
  // initialization
  virtual int init();
  // test if there is a current element
  virtual int  operator !();
  // current element
  virtual DeltaTaxiState &  operator ()();
  // increment
  virtual int  operator ++();
  // assignment
  virtual void operator =(DeltaTaxiState newValue);

};  

#endif
