// -*- C++ -*-

#include "delta-taxi.h"

float DeltaDestinationProbability = 0.3;

void DeltaTaxiState::Act(Action * act, ActualParameterList & apl)
{
  // if we leave the source location while carrying the passenger, the
  // passenger can probabilistically change destinations.
#if 1  
  if (source == Held &&
      location == (*Sites)[originalSource] &&
      act != &ActPickup &&
      act != &ActPutdown) {
    // we are moving away from the original source while holding the
    // passenger. 
    if (rng.between(0.0, 1.0) < DeltaDestinationProbability) {
      SiteName oldDestination = destination;
      destination = (SiteName) rng.between(0, (int)Yellow);
      if (oldDestination != destination) {
	cout << "Destination changed to " << destination << endl;
      }
    }
  }
#else
  // the following code is temporary:  to make this an MDP, because
  // the above code is non-markovian.
  if (source == Held &&
      act != &ActPickup &&
      act != &ActPutdown &&
      (rng.between(0.0, 1.0) < 0.25)) {
    if (rng.between(0.0, 1.0) < DeltaDestinationProbability) {
      SiteName oldDestination = destination;
      destination = (SiteName) rng.between(0, (int)Yellow);
      if (oldDestination != destination) {
	cout << "Destination changed to " << destination << endl;
      }
    }
  }
  // end temporary code
#endif

  // Add noise to the four nagivation actions
  float val = rng.between(0.0, 1.0);
  if (val < 0.1) {
    // switch to the action on the "right"
    if (act == &ActNorth) act = &ActEast;
    else if (act == &ActEast) act = &ActSouth;
    else if (act == &ActSouth) act = &ActWest;
    else if (act == &ActWest) act = &ActNorth;
  }
  else if (val < 0.2) {
    // switch to the action on the "left"
    if (act == &ActNorth) act = &ActWest;
    else if (act == &ActEast) act = &ActNorth;
    else if (act == &ActSouth) act = &ActEast;
    else if (act == &ActWest) act = &ActSouth;
  }

  TaxiState::Act(act, apl);
}

DeltaTaxiState * DeltaTaxiState::Duplicate()
{
  // use the copy constructor
  return new DeltaTaxiState(*this);
}

void DeltaTaxiState::Print(ostream & str)
{
  str << "originalSource = " << originalSource << " ";
  TaxiState::Print(str);
}


//////////////////////////////////////////////////////////////////////  
//
//  Implementation of DeltaTaxiStateIterator
//
//////////////////////////////////////////////////////////////////////

DeltaTaxiStateIterator::DeltaTaxiStateIterator(DeltaTaxiState & s):
  state(s)
{
  init();
}

int DeltaTaxiStateIterator::init()
{
  state.location = Point(0,0);
  state.source = Red;
  state.originalSource = state.source;
  state.destination = Red;
  stateIsValid = 1;
  return 1;
}

int DeltaTaxiStateIterator::operator !()
{
  // return 1 if the state is still valid
  return stateIsValid;
}

DeltaTaxiState & DeltaTaxiStateIterator::operator ()()
{
  // return the current state
  return state;
}

int DeltaTaxiStateIterator::operator ++()
{
  // we do not change originalSource here.
  // increment
  state.location.x++;
  if (state.location.x >= worldSize) {
    state.location.x = 0;
    state.location.y++;
    if (state.location.y >= worldSize) {
      state.location.y = 0;
      state.destination = (SiteName) ((int) state.destination + 1);
      if (state.destination > Yellow) {
	state.destination = Red;
	state.source = (SiteName) ((int) state.source + 1);
	if (state.source == Fuel) {
	  // skip fuel
	  state.source = Held;
	}
	else if (state.source > Held) {
	  state.source = Red;
	  stateIsValid = 0;
	}
      }
    }
  }

#if 0
  // When we are Held, we need to treat originalSource as one of the
  // state variables.  
  // increment
  state.location.x++;
  if (state.location.x >= worldSize) {
    state.location.x = 0;
    state.location.y++;
    if (state.location.y >= worldSize) {
      state.location.y = 0;
      state.destination = (SiteName) ((int) state.destination + 1);
      if (state.destination > Yellow) {
	state.destination = Red;
	if (state.source == Held) {
	  // try incrementing the originalSource
	  if (state.originalSource < Yellow) {
	    state.originalSource =
	      (SiteName) ((int) state.originalSource + 1);
	  }
	  else {
	    // Held is the last legal value for state.source, so when
	    // we have exhausted values for originalSource, we are done.
	    state.originalSource = Red;
	    stateIsValid = 0;
	  }
	}
	else {
	  // source is not Held
	  // increment the state source up to Yellow, then skip to Held
	  if (state.source < Yellow) {
	    state.source = (SiteName) ((int) state.source + 1);
	    state.originalSource = state.source;
	  }
	  else if (state.source == Yellow) {
	    state.source = Held;
	    state.originalSource = Red;
	  }
	}
      }
    }
  }
#endif

  return stateIsValid;

}

void DeltaTaxiStateIterator::operator = (DeltaTaxiState newValue)
{
  state = newValue;
}
