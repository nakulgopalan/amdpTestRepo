//  -*- C++ -*- 
//
//   Hierarchical Q learning maxq hierarchy for taxi task.
//
//
#include "predicate.h"
#include "taxi.h"
#include "fueltaxi.h"
#include "hq.h"
#include "parameter.h"
#include "hq-features.h"
#include "global.h"
#include <minmax.h>
#include <stdlib.h>

// ----------------------------------------------------------------------
//
// inherit termination
//
// ----------------------------------------------------------------------
// In this domain, we do not inherit termination of parent nodes.
int inheritTerminationPredicates = 0;

int impTFalse(FuelTaxiState & state, ActualParameterList & apl)
{
  return 0;
}

Predicate TFalse(
  /* name       */ "TFalse",
  /* function   */ &impTFalse,
  /* parameters */ 0);

int impTTrue(FuelTaxiState  & state, ActualParameterList & apl)
{
  return 1;
}
Predicate TTrue(
  /* name       */ "TTrue",
  /* function   */ &impTTrue,
  /* parameters */ makeArgs());


// ----------------------------------------------------------------------
//
// Parameter Types
//
// ----------------------------------------------------------------------
ParameterType * AnySite = new ParameterType("FuelSite",
					     (int) Red, (int) Fuel);

// hack warning: state.source is an enumerated type with values {Red,
// Green, Blue, Yellow, Fuel, Held, None}.  We use its value directly
// for PTarget, which is of type AnySite, and for PSource.  However,
// sometimes we need a variable whose value can be {Red, Green, Blue,
// Yellow, Held}.  We have defined another feature, PSourceOrHeld, for
// this purpose.  PSource takes on values {Red, Green, Blue, Yellow},
// so it should only be used in cases where we know we are not holding
// a passenger.


// ----------------------------------------------------------------------
//
// Primitive nodes
//
// ----------------------------------------------------------------------
MaxNode MaxNorth   ("MaxNorth",   &ActNorth);
MaxNode MaxEast    ("MaxEast",    &ActEast);
MaxNode MaxSouth   ("MaxSouth",   &ActSouth);
MaxNode MaxWest    ("MaxWest",    &ActWest);
MaxNode MaxPickup  ("MaxPickup",  &ActPickup);
MaxNode MaxPutdown ("MaxPutdown", &ActPutdown);
MaxNode MaxFillup  ("MaxFillup",  &ActFillup);

list<ParameterName *> * allFeatures = makeList(&PTaxiX,
					       &PTaxiY,
					       &PSourceOrHeld,
					       &PDestination,
					       &PFuel);

// ----------------------------------------------------------------------
//
// Navigation
//
// ----------------------------------------------------------------------
//
// Each navigation node must estimate the cost-to-go of completing the
// navigation.  Hence, it needs to know its current location (PTaxiX
// and PTaxiY) and also the target.

QNode QNorth(
  /* name       */ "QNorth",
  /* parameters */ makeArgs(AnySite, &PTarget),
  /* child      */ MaxNorth,
  /* child args */ makePPL(),
  /* features   */ makeList(&PTarget, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QEast(
  /* name       */ "QEast",
  /* parameters */ makeArgs(AnySite, &PTarget),
  /* child      */ MaxEast,
  /* child args */ makePPL(),
  /* features   */ makeList(&PTarget, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QSouth(
  /* name       */ "QSouth",
  /* parameters */ makeArgs(AnySite, &PTarget),
  /* child      */ MaxSouth,
  /* child args */ makePPL(),
  /* features   */ makeList(&PTarget, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QWest(
  /* name       */ "QWest",
  /* parameters */ makeArgs(AnySite, &PTarget),
  /* child      */ MaxWest,
  /* child args */ makePPL(),
  /* features   */ makeList(&PTarget, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

int impTAtTarget(FuelTaxiState  & state, ActualParameterList & apl)
{
  int target = apl[&PTarget];
  return state.location  == (*Sites)[target];
}
Predicate TAtTarget(
  /* name       */ "AtTarget",
  /* function   */ &impTAtTarget, 
  /* parameters */ makeArgs(AnySite, &PTarget));

QNode * PolicyNavigate(FuelTaxiState & state,
		   ActualParameterList & apl,
		   ActualParameterList & cpl)
{
  int target = apl[&PTarget];

  if (target == Red) {
    if (state.location.x < 2) {
      if (state.location.y < 4) return &QNorth;
      else return &QWest;
    }
    else if (state.location.y == 2) return &QWest;
    else if (state.location.y > 2) return &QSouth;
    else return &QNorth;
  }
  else if (target == Yellow) {
    if (state.location.x == 0) return &QSouth;
    else if (state.location.y == 2) return &QWest;
    else if (state.location.y > 2) return &QSouth;
    else return &QNorth;
  }
  else if (target == Green) {
    if (state.location.x > 1) {
      if (state.location.y < 4) return &QNorth;
      else return &QEast;
    }
    else if (state.location.y == 2) return &QEast;
    else if (state.location.y > 2) return &QSouth;
    else return &QNorth;
  }
  else if (target == Blue) {
    if (state.location.x > 2) {
      if (state.location.y > 0) return &QSouth;
      else return &QWest;
    }
    else if (state.location.y == 2) return &QEast;
    else if (state.location.y > 2) return &QSouth;
    else return &QNorth;
  }
#if 0
  else if (target == Fuel) {
    if (state.location.x < 1) {
      if (state.location.y == 2) return &QEast;
      else if (state.location.y > 2) return &QSouth;
      else return &QNorth;
    }
    else if (state.location.x > 2) {
      if (state.location.y == 2) return &QWest;
      else if (state.location.y > 2) return &QSouth;
      else return &QNorth;
    }
    // in the MD region.  Get y right first.
    else if (state.location.y == 1) return &QEast;
    else if (state.location.y > 1) return &QSouth;
    else return &QNorth;
  }
#endif
  else if (target == Fuel) {
    if (state.location.x == 2) {
      if (state.location.y < 1) return &QNorth;
      else return &QSouth;
    }
    else if (state.location.x == 0) {
      if (state.location.y > 2) return &QSouth;
      else if (state.location.y < 2) return &QNorth;
      else return &QEast;
    }
    else if (state.location.x == 1) {
      if (state.location.y > 2) return &QSouth;
      else return &QEast;
    }
    else if (state.location.x > 2) {
      if (state.location.y > 2) return &QSouth;
      else if (state.location.y < 2) return &QNorth;
      else return &QWest;
    }
  }

  else {
    cout << "Unknown Target " << target << endl;
    abort();
  }
}

MaxNode MaxNavigate(
  /* name                  */ "MaxNavigate",
  /* parameters            */ makeArgs(AnySite, &PTarget),
  /* precondition pred     */ &TTrue,
  /* termination predicate */ &TAtTarget,
  /* goal predicate        */ &TAtTarget,
  /* children              */ makeList(&QNorth, &QEast, &QSouth, &QWest),
  /* features              */ makeList(&PTaxiX, &PTaxiY, &PTarget),
  /* policy                */ &PolicyNavigate,
  /* nonGoal penalty       */ -100.0);

// ----------------------------------------------------------------------
//
// Get
//
// ----------------------------------------------------------------------

// We only need to estimate the cost of finishing the parent task
// (Get) after completing the navigation.  This is just a single
// action, so we don't need any features here.
// Sun Mar 21 11:51:36 1999 I have added a feature to make this
// consistent with the paper.  Because of our representation of the
// state space, the four source states are in different equivalence
// classes, and must therefore have separate values. 
QNode QNavigateForGet(
  /* name       */ "QNavigateForGet",
  /* parameters */ makeArgs(),
  /* child      */ MaxNavigate,
  /* child args */ makePPL(&PSource, &PTarget),
  /* features   */ makeList(&PSource),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

// We need to model the cost of completing the Navigate after
// inappropriately executing a QPickup.  For now, I will not do any
// feature engineering.
QNode QPickup(
  /* name       */ "QPickup",
  /* parameters */ makeArgs(),
  /* child      */ MaxPickup,
  /* child args */ makePPL(),
  /* features   */ makeList(&PSource, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

extern int EVALVERBOSE;

int impTHolding(FuelTaxiState  & state, ActualParameterList & apl)
{
  int result = (state.source == Held);
  if (EVALVERBOSE) cout << "impTHolding = " << result << endl;
  return result;
}
Predicate THolding(
  /* name       */ "Holding",
  /* function   */ &impTHolding, 
  /* parameters */ makeArgs());

QNode * PolicyGet(FuelTaxiState & state,
		  ActualParameterList & apl,
		  ActualParameterList & cpl)
{
  if (state.location == state.SourcePoint()) return &QPickup;
  else return &QNavigateForGet;
}


// Goal is to be holding the customer at the source location. 
MaxNode MaxGet(
  /* name                  */ "MaxGet",
  /* parameters            */ makeArgs(),
  /* precondition pred     */ &TTrue,
  /* termination predicate */ &THolding,
  /* goal predicate        */ &THolding,
  /* children              */ makeList(&QNavigateForGet, &QPickup),
  /* features              */ makeList(&PTaxiX, &PTaxiY, &PSourceOrHeld),
  /* policy                */ &PolicyGet,
  /* nonGoal penalty       */ -100.0);


// ----------------------------------------------------------------------
//
// Put
//
// ----------------------------------------------------------------------

// We only need to estimate the cost of finishing the parent task
// (Put) after completing the nagivation.  This is just a single
// action, so we don't need any features here either.
// We are protected from inappropriate invocation by the termination
// condition of MaxNavigate. 
// Sun Mar 21 11:53:19 1999 added Destination feature for consistency
// with paper
QNode QNavigateForPut(
  /* name       */ "QNavigateForPut",
  /* parameters */ makeArgs(),
  /* child      */ MaxNavigate,
  /* child args */ makePPL(&PDestination, &PTarget),
  /* features   */ makeList(&PDestination),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

// This needs to handle the case where we attempt to Putdown before we
// are at the target position.  
QNode QPutdown(
  /* name       */ "QPutdown",
  /* parameters */ makeArgs(),
  /* child      */ MaxPutdown,
  /* child args */ makePPL(),
  /* features   */ makeList(&PDestination, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

int impTNotHolding(FuelTaxiState  & state, ActualParameterList & apl)
{
  return state.source != Held;
}
Predicate TNotHolding(
  /* name       */ "NotHolding",
  /* function   */ &impTNotHolding, 
  /* parameters */ makeArgs());

int impTNotHoldingAtDestination(FuelTaxiState  & state, ActualParameterList & apl)
{
  // If we succeed, then the source is set to None and the
  // simulation is terminated.
  return (state.source == None);
}
Predicate TNotHoldingAtDestination(
  /* name       */ "NotHoldingAtDestination",
  /* function   */ &impTNotHoldingAtDestination, 
  /* parameters */ makeArgs());

QNode * PolicyPut(FuelTaxiState & state,
		  ActualParameterList & apl,
		  ActualParameterList & cpl)
{
  if (state.location == state.DestinationPoint()) return &QPutdown;
  else return &QNavigateForPut;
}

MaxNode MaxPut(
  /* name                  */ "MaxPut",
  /* parameters            */ makeArgs(),
  /* precondition pred     */ &TTrue,
  /* termination predicate */ &TNotHolding,
  /* goal predicate        */ &TNotHoldingAtDestination,
  /* children              */ makeList(&QNavigateForPut, &QPutdown),
  /* features              */ makeList(&PTaxiX, &PTaxiY, 
				       &PSourceOrHeld, &PDestination, 
				       &PFuel),
  /* policy                */ &PolicyPut,
  /* nonGoal penalty       */ -100.0);

// ----------------------------------------------------------------------
//
// Refuel
//
// ----------------------------------------------------------------------

// We need to estimate the cost of completing the refueling after we
// have reached the Fuel location.  This will always be trivial.  So
// we have no features.  We pass the Fuel Site name as a parameter to
// MaxNavigate.
QNode QNavigateForRefuel(
  /* name       */ "QNavigateForRefuel",
  /* parameters */ makeArgs(),
  /* child      */ MaxNavigate,
  /* child args */ makePPL(&PFuelSite, &PTarget),
  /* features   */ 0,
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

// This is a primitive.  But we need to determine whether we are at
// the fuel location or not.  Hence, we need our current location.
QNode QFillup(
  /* name       */ "QFillup",
  /* parameters */ makeArgs(),
  /* child      */ MaxFillup,
  /* child args */ makePPL(),
  /* features   */ makeList(&PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

int impTFuelFull(FuelTaxiState  & state, ActualParameterList & apl)
{
  return (state.fuel == 12 && state.location == state.FuelPoint());
}
Predicate TFuelFull(
  /* name       */ "TFuelFull",
  /* function   */ &impTFuelFull,
  /* parameters */ makeArgs());

QNode * PolicyRefuel(FuelTaxiState & state,
		     ActualParameterList & apl,
		     ActualParameterList & cpl)
{
  if (state.location == state.FuelPoint()) return &QFillup;
  else return &QNavigateForRefuel;
}

//
// I'm not sure what to put for the features here.  My guess is that I
// only need to know taxi location and amount of fuel, everything else
// remains unchanged by refueling
//
MaxNode MaxRefuel(
  /* name                  */ "MaxRefuel",
  /* parameters            */ makeArgs(),
  /* precondition pred     */ &TTrue,
  /* termination predicate */ &TFuelFull,
  /* goal predicate        */ &TFuelFull,
  /* children              */ makeList(&QNavigateForRefuel, &QFillup),
  /* features              */ makeList(&PTaxiX, &PTaxiY, &PFuel),
  /* policy                */ &PolicyRefuel,
  /* nonGoal penalty       */ -100.0);


// ----------------------------------------------------------------------
//
// Top Level
//
// ----------------------------------------------------------------------


// We need to estimate the cost of completing the Put (and possible
// refueling) after completing the Get.  This means we need to know
// the source, the destination, and the amount of fuel.  It turns out
// we also need to know our location to cover the cases where we will
// run out of fuel before we get to the source or after we get to the
// source but before we can put or refuel.  This is unfortunate. 
QNode QGet(
  /* name       */ "QGet",
  /* parameters */ makeArgs(),
  /* child      */ MaxGet,
  /* child args */ makePPL(),
  /* features   */ makeList(&PSource, &PDestination, &PFuel, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

// This node will always be terminal. (Either we will succeed, in which
// case, it is trivial to estimate the resulting value.  It will
// always be zero.  Or we fail because we exhaust fuel.)  So we need to
// know the destination, the amount of fuel, and our current location.
QNode QPut(
  /* name       */ "QPut",
  /* parameters */ makeArgs(),
  /* child      */ MaxPut,
  /* child args */ makePPL(),
  /* features   */ makeList(&PDestination, &PFuel, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);


// This node is interesting.  Because of the hierarchical credit
// assignment, all top level nodes must predict whether we will run
// out of fuel during their execution.  Hence, this node needs the
// full state.  If we are willing to assume that we won't run out of
// fuel while moving toward the F location, then this node could just
// use the source and destination locations, since after successful
// execution, the location and fuel of the taxi will be known
// implicitly. 
QNode QRefuel(
  /* name       */ "QRefuel",
  /* parameters */ makeArgs(),
  /* child      */ MaxRefuel,
  /* child args */ makePPL(),
  /* features   */ makeList(&PSourceOrHeld, &PDestination, &PFuel, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ -40.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);


QNode * PolicyRoot(FuelTaxiState & state,
		   ActualParameterList & apl,
		   ActualParameterList & cpl)
{
  // There are four policies in general:
  // 1. pickup, putdown
  // 2. pickup, fillup, putdown
  // 3. fillup, pickup, putdown
  // 4. fillup, pickup, fillup, putdown

  // Determine current goal:  source, destination, or fuel.

  if (state.source != Held) {
    // Goal is either Fuel or Source.
    if (state.Distance(state.location, state.source) +
	state.Distance(state.source, state.destination) <= state.fuel) {
      // we can do everything without refueling.  Case 1.
      return &QGet;
    }
    else {
      // is case 2 possible?
      int case2 = state.Distance(state.location, state.source) +
	state.Distance(state.source, Fuel);
      if (case2 <= state.fuel) {
	// Yes, case 2 is possible, so we must compare against case 3.
	// Is case 3 possible?  After fueling, goto source and dest.
	int case3 = state.Distance(Fuel, state.source) +
	  state.Distance(state.source, state.destination);
	if (case3 <= 12) {
	  // Yes, case3 is possible, so we must compare case2 and
	  // case3.
	  case2 += state.Distance(Fuel, state.destination);
	  case3 += state.Distance(state.location, Fuel);
	  if (case2 < case3) return &QGet;
	  else return &QRefuel;
	}
	else {
	  // Only case 2 is possible.
	  return &QGet;
	}
      }
      else return &QRefuel;
    }
  }
  else {
    // Goal is either Fuel or Destination.
    if (state.Distance(state.location, state.destination) <= state.fuel) {
      // we can make it without refueling
      return &QPut;
    }
    else return &QRefuel;
  }
}


int impTSimulationTerminatedAndPassengerDelivered(FuelTaxiState  & state, ActualParameterList & apl)
{
  return (state.Terminated() && state.source == None);
}
Predicate TSimulationTerminatedAndPassengerDelivered(
  /* name       */ "TSimulationTerminatedAndPassengerDelivered",
  /* function   */ &impTSimulationTerminatedAndPassengerDelivered,
  /* parameters */ makeArgs());


int impTSimulationTerminated(FuelTaxiState  & state, ActualParameterList & apl)
{
  return state.Terminated();
}
Predicate TSimulationTerminated(
  /* name       */ "TSimulationTerminated",
  /* function   */ &impTSimulationTerminated,
  /* parameters */ makeArgs());



MaxNode MaxRoot(
  /* name                  */ "MaxRoot",
  /* parameters            */ makeArgs(),
  /* precondition pred     */ &TTrue,
  /* termination predicate */ &TSimulationTerminated,
  /* goal predicate        */ &TSimulationTerminatedAndPassengerDelivered,
  /* children              */ makeList(&QGet, &QPut, &QRefuel),
  /* features              */ makeList(&PTaxiX, &PTaxiY,
				       &PSourceOrHeld, &PDestination,
				       &PFuel),
  /* policy                */ &PolicyRoot,
  /* nonGoal penalty       */ -100.0);



// ----------------------------------------------------------------------
//
// Reward Decomposition
//
// ----------------------------------------------------------------------


list<QNode *> * primitiveQNodes = makeList(&QNorth, &QEast, &QSouth, &QWest,
					   &QPickup, &QPutdown, &QFillup);

list<QNode *> * topLevelQNodes = makeList(&QGet, &QPut, &QRefuel);

list<RewardResponsibility *> * RewardDecomposition = makeList(
  new RewardResponsibility(&TENone,           // no error 
			   primitiveQNodes),
  new RewardResponsibility(&TENotAtFuel,      // attempt to fillup not at Fuel
			   makeList(&QFillup)),
  new RewardResponsibility(&TEEmpty,          // no fuel left
			   topLevelQNodes),
  new RewardResponsibility(&TEAlreadyHolding, // already holding
                           makeList(&QPickup)),
  new RewardResponsibility(&TENoPassenger,    // bad putdown
	    	           makeList(&QPutdown)),
  new RewardResponsibility(&TENotAtSource,    // can't pickup
		           makeList(&QPickup)),
  new RewardResponsibility(&TENotHolding,     // bad putdown
		           makeList(&QPutdown)),
  new RewardResponsibility(&TENotAtDestination, // bad putdown
		           makeList(&QPutdown)));


