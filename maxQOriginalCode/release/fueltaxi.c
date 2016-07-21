// -*- C++ -*-

#include "fueltaxi.h"
#include <stdlib.h>


// Declare reward, action, and new errors for this task
const float RewardEmpty = -20.0;    // fuel exhausted.

Action ActFillup("ActFillup", 6);

Error TENotAtFuel("TENotAtFuel", 5);
Error TEEmpty("TEEmpty", 6);

// navigation distances between sites.  Use for hand-coded policies. 
int InterSiteDistance[5][5] = {
  /*              Red   Green  Blue  Yellow  Fuel */
  /* Red    */ {   0,     8,     7,     4,    5},
  /* Green  */ {   8,     0,     5,     8,    5},
  /* Blue   */ {   7,     5,     0,     7,    4},
  /* Yellow */ {   4,     8,     7,     0,    5},
  /* Fuel   */ {   5,     5,     4,     5,    0}};

ParameterName PFuel("PFuel", 7);
ParameterName PFuelSite("PFuelSite", 8);

void FuelTaxiState::Init()
{
  TaxiState::Init();
  // Fuel is <= 5 steps from every cell, so we initialize the fuel at
  // 5 to ensure that the problem is always solvable.
  fuel = rng.between(5, 12);
}

void FuelTaxiState::Act(Action * act, ActualParameterList & apl)
{
  if (act == &ActFillup) {
    if (location == (*Sites)[Fuel]) {
      cumulativeReward += RewardAction;
      currentError = &TENone;
      fuel = 12;   // tank holds 12 units
    }
    else {
      // refueling costs one unit of reward
      cumulativeReward += RewardError;
      currentError = &TENotAtFuel;
      if (VERBOSE) cout << "Can't fillup: not a fuel location" << endl;
    }
  }
  else TaxiState::Act(act, apl);

  // we check for fuel after the action so that action models remain
  // consistent without needing to check the amount of fuel.
  if (act == &ActNorth || act == &ActEast ||
      act == &ActSouth || act == &ActWest) {
    fuel += -1;
    // It is ok to have zero fuel if we are at the destination.  We
    // don't give the penalty until an attempt is made to move with
    // zero fuel.  This attempt to move fails.
    if (fuel < 0) {
      cumulativeReward += RewardEmpty;
      currentError = &TEEmpty;
      if (VERBOSE) cout << "Out of fuel" << endl;
      return;
    }
  }
}

void FuelTaxiState::Print(ostream & str)
{
  str << "fuel=" << fuel << " ";
  TaxiState::Print(str);
}

void FuelTaxiState::Save(ostream & str)
{
  str << fuel << endl;
  TaxiState::Save(str);
}

void FuelTaxiState::Read(istream & str)
{
  str >> fuel;
  TaxiState::Read(str);
}

Point FuelTaxiState::FuelPoint()
{
  return (*Sites)[Fuel];
}

int FuelTaxiState::Terminated()
{
  return (TaxiState::Terminated() || fuel < 0);
}

int FuelTaxiState::Distance(SiteName from, SiteName to)
{
  return InterSiteDistance[from][to];
}

int FuelTaxiState::Distance(Point & p, SiteName s)
{
  // MD means Manhattan distance.  We check whether we are outside the
  // Manhattan distance rectangle (i.e., the squares where we can
  // reach by one vertical and one horizontal move).  If not, then we
  // must move to y==2, slide along x to the target location, and then
  // move up or down a fixed amount.
  switch (s) {
  case Red: {
    // MD rect is x[0,1] y[0,4]
    if (p.x > 1) return abs(p.y - 2) + p.x + 2;
    else return (4 - p.y) + p.x;  //MD
  }
  case Green: {
    // MD rect is x[2,4] y[0,4]
    if (p.x < 2) return abs(p.y - 2) + (4 - p.x) + 2;
    else return (4 - p.y) + (4 - p.x); // MD
  }
  case Blue: {
    // MD rect is x[3,4] y[0,4]
    if (p.x < 3) return abs(p.y - 2) + (3 - p.x) + 2;
    else return p.y + abs(3 - p.x);  // MD
  }
  case Yellow: {
    // MD rect is x[0,0] y[0,4]
    if (p.x > 0) return abs(p.y - 2) + p.x + 2;
    else return p.y; // MD
  }
  case Held: return 0;
  case Fuel: {
    // MD rect is x[1,2] y[0,4]
    if (p.x == 0) return abs(p.y - 2) + 2 + 1;
    else if (p.x > 0 && p.x < 3) return abs(p.y - 1) + abs(p.x - 2); // MD
    else return abs(p.y - 2) + (p.x - 2) + 1;
  }
  }
}

FuelTaxiState * FuelTaxiState::Duplicate()
{
  // use the copy constructor
  return new FuelTaxiState(*this);
}

int FuelTaxiState::featureSize(ParameterName * pname)
{
  if (pname == &PDestination || pname == &PTarget) return 5; //includes Fuel
  else if (pname == &PFuel) return 14; // -1 to 12 units of fuel
  else return TaxiState::featureSize(pname);
}

float FuelTaxiState::featureValue(ParameterName * pname)
{
  if (pname == &PFuel) return ((float) fuel + 1.0);
  else if (pname == &PFuelSite) return ((float) Fuel);
  else return TaxiState::featureValue(pname);
}

void FuelTaxiState::insertFeature(ParameterName * pname,
				  ActualParameterList & apl,
				  vector<float> & fv,
				  int & i)
{
  if (pname == &PFuel) {
    fv[i++] = featureValue(pname);
  }
  else TaxiState::insertFeature(pname, apl, fv, i);
}


//////////////////////////////////////////////////////////////////////  
//
//  Implementation of FuelTaxiStateIterator
//
//////////////////////////////////////////////////////////////////////

FuelTaxiStateIterator::FuelTaxiStateIterator(FuelTaxiState & s):
  state(s)
{
  init();
}

int FuelTaxiStateIterator::init()
{
  state.location = Point(0,0);
  state.source = Red;
  state.destination = Red;
  state.fuel = -1;
  stateIsValid = 1;
  return 1;
}

int FuelTaxiStateIterator::operator !()
{
  // return 1 if the state is still valid
  return stateIsValid;
}

FuelTaxiState & FuelTaxiStateIterator::operator ()()
{
  // return the current state
  return state;
}

int FuelTaxiStateIterator::operator ++()
{
  // increment
  state.location.x++;
  if (state.location.x >= worldSize) {
    state.location.x = 0;
    state.location.y++;
    if (state.location.y >= worldSize) {
      state.location.y = 0;
      // increment the state source up to Yellow, then skip to Held
      if (state.source < Yellow) {
	state.source = (SiteName) ((int) state.source + 1);
      }
      else if (state.source == Yellow) state.source = Held;
      else {
	// all legal values for source are used up
	state.source = Red;
	state.destination = (SiteName) ((int) state.destination + 1);
	if (state.destination > Yellow) {
	  // all legal values for destination are used up
	  state.destination = Red;
	  state.fuel += 1;
	  if (state.fuel > 12) {
	    // all legal values for fuel are used up
	    state.fuel = -1;
	    stateIsValid = 0;
	  }
	}
      }
    }
  }
  return stateIsValid;
}


void FuelTaxiStateIterator::operator = (FuelTaxiState newValue)
{
  state = newValue;
}
