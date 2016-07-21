// -*- C++ -*-
//
// Implementation of the TAXI world
//
// This version sets source and destination to None when the customer
// has been delivered.

#include "global.h"
#include "taxi.h"
#include "point.h"
#include <list.h>
#include <clrandom.h>
#include <stdlib.h>
#include <iterator.h>

// world is 5 x 5.
const int worldSize = 5;

// reward structure
const float RewardAction = -1.0;    // every action costs 1
const float RewardDelivery = 20.0;  // successful delivery is 20.0
const float RewardError = -10.0;    // illegal actions: -10

// declare our actions
Action ActNorth("ActNorth", 0);
Action ActEast("ActEast", 1);
Action ActSouth("ActSouth", 2);
Action ActWest("ActWest", 3);
Action ActPickup("ActPickup", 4);
Action ActPutdown("ActPutdown", 5);

// declare our errors
Error TEAlreadyHolding("TEAlreadyHolding", 0);
Error TENoPassenger("TENoPassenger", 1);
Error TENotAtSource("TENotAtSource", 2);
Error TENotHolding("TENotHolding", 3);
Error TENotAtDestination("TENotAtDestination", 4);
Error TENone("TENone", 7);

// designated important locations
// Must correspond to order in taxi.h for SiteName.
list<Point> *Sites = makeList(Point(0,4),  // Red
			      Point(4,4),  // Green
			      Point(3,0),  // Blue
			      Point(0,0),  // Yellow
			      Point(2,1)); // Fuel


list<Point> *NorthWalls = makeList(Point(0,4),
				  Point(1,4),
				  Point(2,4),
				  Point(3,4),
				  Point(4,4));

list<Point> *EastWalls = makeList(Point(4,4),
				 Point(4,3),
				 Point(4,2),
				 Point(4,1),
				 Point(4,0),
				 Point(2,1),
				 Point(2,0),
				 Point(1,4),
				 Point(1,3),
				 Point(0,1),
				 Point(0,0));

list<Point> *SouthWalls = makeList(Point(0,0),
				  Point(1,0),
				  Point(2,0),
				  Point(3,0),
				  Point(4,0));

list<Point> *WestWalls = makeList(Point(3,1),
				 Point(3,0),
				 Point(2,4),
				 Point(2,3),
				 Point(1,1),
				 Point(1,0),
				 Point(0,4),
				 Point(0,3),
				 Point(0,2),
				 Point(0,1),
				 Point(0,0));

// Parameter names
ParameterName PTaxiX("PTaxiX", 1);
ParameterName PTaxiY("PTaxiY", 2);
ParameterName PSource("PSource", 3);
ParameterName PSourceOrHeld("PSourceOrHeld", 4);
ParameterName PDestination("PDestination", 5);
ParameterName PTarget("PTarget", 6);

void TaxiState::Init()
{
  location.Randomize(rng);
  source = (SiteName) rng.between(0, (int)Yellow);
  originalSource = source;
  destination = (SiteName) rng.between(0, (int)Yellow);
  cumulativeReward = 0;
  currentError = &TENone;
}

void TaxiState::Act(Action * act, ActualParameterList & apl)
{
  currentError = &TENone;
  cumulativeReward += RewardAction;

  if (act == &ActNorth) {
    if (!NorthWalls->includes(location))
      location.y += 1;
  }
  else if (act == &ActEast) {
    if (!EastWalls->includes(location))
      location.x += 1;
  }
  else if (act == &ActSouth) {
    if (!SouthWalls->includes(location))
      location.y -= 1;
  }
  else if (act == &ActWest) {
    if (!WestWalls->includes(location))
      location.x -= 1;
  }
  else if (act == &ActPickup) {
    if (source == Held) {
      cumulativeReward += RewardError;
      currentError = &TEAlreadyHolding;
      if (VERBOSE) cout << "Can't pickup: already holding passenger."
			<< endl;
    }
    else if (source == None) {
      currentError = &TENoPassenger;
      cumulativeReward += RewardError;
      if (VERBOSE) cout << "Can't pickup: no passenger to pick up." <<
		     endl;
    }
    else if (location == (*Sites)[source]) {
      source = Held;
    }
    else {
      cumulativeReward += RewardError;
      currentError = &TENotAtSource;
      if (VERBOSE) cout << "Can't pickup: not at source location" << endl;
    }
  }
  else if (act == &ActPutdown) {
    if (source != Held) {
      cumulativeReward += RewardError;
      currentError = &TENotHolding;
      if (VERBOSE) cout << "Can't putdown: not holding a passenger" <<
		     endl;
    }
    else if (location == (*Sites)[destination]) {
      cumulativeReward += RewardDelivery;
      // InitCustomer();
      source = None;
      // destination = None;
    }
    else {
      cumulativeReward += RewardError;
      currentError = &TENotAtDestination;
      if (VERBOSE) cout << "Can't putdown: not at destination" << endl;
    }
  }
  else {
    cout << "Unknown action! " << act << endl;
    abort();
  }
}
//   -----------
//  4|P  |     | 
//  3|T  |     |
//  2|         |
//  1| |  F|   |
//  0|D|   |   |
//   -----------
//    0 1 2 3 4

void TaxiState::Print(ostream & str)
{
  // T = taxi, P = passenger, D = destination, F = fuel station
  // top wall
  str << "source=" << source << " destination=" << destination <<
    " location=" << location <<  endl;
  str << "Cumulative reward = " << cumulativeReward;
  str << " CurrentError = " << currentError;
  if (Terminated()) str << " TERMINATED!";
  str << endl;

  str << " -";
  for (int i = 0; i < worldSize; i++) str << "--";
  str << endl;
  for (int y = worldSize - 1; y >= 0; y--) {
    str << y << "|";
    for (int x = 0; x < worldSize; x++) {
      Point current(x,y);
      int taxiHere = 0;
      int sourceHere = 0;
      int destinationHere = 0;

      if (location == current) taxiHere = 1;
      if ((destination != None) &&
	  (current == (*Sites)[destination])) destinationHere = 1;
      if (source < Held && current == (*Sites)[source]) sourceHere = 1;

      int totalHere = taxiHere + sourceHere + destinationHere;
      if (totalHere > 1) str << totalHere;
      else if (taxiHere) str << "T";
      else if (sourceHere) str << "S";
      else if (destinationHere) str << "D";
      else if (current == (*Sites)[Fuel]) str << "F";
      else str << " ";

      if (EastWalls->includes(current)) {
	str << "|";
      }
      else {
	str << " ";
      }
    }
    str << endl;
  }
  str << " -";
  for (int i = 0; i < worldSize; i++) str << "--";
  str << endl;
  str << " ";
  for (int i = 0; i < worldSize; i++) str << " " << i;
  str << endl;
}
      
void TaxiState::Save(ostream & str)
{
  str << location << " " << (int) source << " " 
      << (int) originalSource << " "
      << (int) destination << endl;
  State::Save(str);
}

void TaxiState::Read(istream & str)
{
  str >> location;
  int temp;
  str >> temp;  source = (SiteName) temp;
  str >> temp;  originalSource = (SiteName) temp;
  str >> temp;  destination = (SiteName) temp;
  State::Read(str);
}

Point TaxiState::SourcePoint()
{
  if (source < Held) {
    return (*Sites)[source];
  }
  else return Point(-1,-1);
}

Point TaxiState::DestinationPoint()
{
  return (*Sites)[destination];
}

int TaxiState::Terminated()
{
  return (source == None);
}
	  

ostream & operator << (ostream & str, SiteName site)
{
  if (site == None) return str << "None";
  else if (site == Held) return str << "Held";
  else return str << (*Sites)[site];
}

TaxiState * TaxiState::Duplicate()
{
  // use the copy constructor
  return new TaxiState(*this);
}

int TaxiState::featureSize(ParameterName * pname)
{
  if (pname == &PTaxiX || pname == &PTaxiY) return worldSize;
  else if (pname == &PSource) return 4;
  else if (pname == &PSourceOrHeld) return 6;
  else if (pname == &PDestination || pname == &PTarget) return 4;
  else return State::featureSize(pname);
}

float TaxiState::featureValue(ParameterName * pname)
{
  if (pname == &PTaxiX) return location.x;
  else if (pname == &PTaxiY) return location.y;
  else if (pname == &PSource) return source;
  else if (pname == &PSourceOrHeld) {
    if (source >= Held) return (source - 1.0);
    else return ((float) source);
  }
  else if (pname == &PDestination) return (float) destination;
  else return State::featureValue(pname);
}


void TaxiState::insertFeature(ParameterName * pname,
			      ActualParameterList & apl,
			      vector<float> & fv,
			      int & i)
{
  if (pname == &PTaxiX ||
      pname == &PTaxiY ||
      pname == &PSource ||
      pname == &PSourceOrHeld ||
      pname == &PDestination) {
    fv[i++] = featureValue(pname);
  }
  else if (pname == &PTarget) {
    fv[i++] = (float) apl[pname];
  }
  else
    State::insertFeature(pname, apl, fv, i);
}
    
    
//////////////////////////////////////////////////////////////////////  
//
//  Implementation of TaxiStateIterator
//
//////////////////////////////////////////////////////////////////////

TaxiStateIterator::TaxiStateIterator(TaxiState & s):
  state(s)
{
  init();
}

int TaxiStateIterator::init()
{
  state.location = Point(0,0);
  state.source = Red;
  state.destination = Red;
  stateIsValid = 1;
  return 1;
}

int TaxiStateIterator::operator !()
{
  // return 1 if the state is still valid
  return stateIsValid;
}

TaxiState & TaxiStateIterator::operator ()()
{
  // return the current state
  return state;
}

int TaxiStateIterator::operator ++()
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
	  state.destination = Red;
	  stateIsValid = 0;
	}
      }
      //      stateIsValid = 0; 
    }
  }
  return stateIsValid;
}


void TaxiStateIterator::operator = (TaxiState newValue)
{
  state = newValue;
}
