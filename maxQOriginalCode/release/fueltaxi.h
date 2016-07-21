// -*- C++ -*-

#ifndef FUELTAXI_H
#define FUELTAXI_H

#include "taxi.h"
#include <iostream.h>

extern Action ActFillup;
extern Error TENotAtFuel;
extern Error TEEmpty;

// Taxi must avoid running out of fuel.
class FuelTaxiState: public TaxiState
{
public:
  // state variables
  int fuel;			// amount of fuel

  // constructors
  FuelTaxiState(unsigned int seed): TaxiState(seed) { Init(); };

  // generic methods
  virtual void Init();			// generate initial state
  virtual void Act(Action * act, ActualParameterList & apl);	// perform an action
  virtual void Print(ostream & str);	// print out the state
  virtual void Save(ostream & str); // dump state in machine-readable format
  virtual void Read(istream & str); // re-load state in
				    // machine-readable format
  virtual int Terminated();
  virtual FuelTaxiState * Duplicate();
  virtual int featureSize(ParameterName * pname);
  virtual float featureValue(ParameterName * pname);			     
  virtual void insertFeature(ParameterName * pname,
			     ActualParameterList & apl,
			     vector<float> & fv,
			     int & i);

  Point FuelPoint();		// return fuel as a point
  int Distance(Point &, SiteName); // compute navigation distance
  int Distance(SiteName, SiteName);
};

extern ParameterName PFuel;
extern ParameterName PFuelSite;

class FuelTaxiStateIterator: public iterator<FuelTaxiState>
{
public:
  FuelTaxiStateIterator(FuelTaxiState & state);
  FuelTaxiState &  state;   // the current state
  int stateIsValid;
  // initialization
  virtual int init();
  // test if there is a current element
  virtual int  operator !();
  // current element
  virtual FuelTaxiState &  operator ()();
  // increment
  virtual int  operator ++();
  // assignment
  virtual void operator =(FuelTaxiState newValue);

};  




#endif
