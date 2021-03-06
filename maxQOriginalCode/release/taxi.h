// -*- C++ -*-
//
// Declaration of the TAXI world.  
//
#ifndef TAXI_H
#define TAXI_H

#include <iostream.h>
#include <clrandom.h>
#include "point.h"
#include "state.h"

enum SiteName {Red=0, Green=1, Blue=2, Yellow=3, Fuel=4, Held=5,
	       None=6};

extern list<Point> *Sites;
extern const float RewardAction;
extern const float RewardDelivery;
extern const float RewardError;

extern Action ActNorth;
extern Action ActEast;
extern Action ActSouth;
extern Action ActWest;
extern Action ActPickup;
extern Action ActPutdown;

extern Error TEAlreadyHolding;
extern Error TENoPassenger;
extern Error TENotAtSource;
extern Error TENotHolding;
extern Error TENotAtDestination;
extern Error TENone;

ostream & operator << (ostream & str, SiteName site);
  
// Basic Taxi Simulation.  
// Taxi must pickup and putdown passengers.  

class TaxiState: public State
{
public:
  // state variables
  Point location;		// location of taxi
  SiteName source;		// location of customer
  SiteName destination;		// destination of customer
  SiteName originalSource;      // original value of source

  // constructors
  TaxiState(unsigned int seed): State(seed)
     {Init();};

  // generic methods
  virtual void Init();			// generate initial state
  virtual void Act(Action * act, ActualParameterList & apl);	// perform an action
  virtual void Print(ostream & str);	// print out the state
  virtual void Save(ostream & str); // dump state in machine-readable format
  virtual void Read(istream & str); // re-load state in
				    // machine-readable format
  virtual int Terminated();
  virtual TaxiState * Duplicate();
  virtual int featureSize(ParameterName * pname);
  virtual float featureValue(ParameterName * pname);			     
  virtual void insertFeature(ParameterName * pname,
			     ActualParameterList & apl,
			     vector<float> & fv,
			     int & i);

  Point SourcePoint();		// return source as a point
  Point DestinationPoint();	// return destination as a point
};

extern ParameterName PTaxiX;
extern ParameterName PTaxiY;
extern ParameterName PSource;
extern ParameterName PSourceOrHeld;
extern ParameterName PDestination;
extern ParameterName PTarget;

class TaxiStateIterator: public iterator<TaxiState>
{
public:
  TaxiStateIterator(TaxiState & state);
  TaxiState &  state;   // the current state
  int stateIsValid;
  // initialization
  virtual int init();
  // test if there is a current element
  virtual int  operator !();
  // current element
  virtual TaxiState &  operator ()();
  // increment
  virtual int  operator ++();
  // assignment
  virtual void operator =(TaxiState newValue);

};  



#endif
