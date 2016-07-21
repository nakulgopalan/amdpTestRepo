//  -*- C++ -*- 
//
//   Hierarchical Q learning maxq hierarchy for taxi task.
//
//
#include "predicate.h"
#include "taxi.h"
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

int impTFalse(TaxiState & state, ActualParameterList & apl)
{
  return 0;
}

Predicate TFalse(
  /* name       */ "TFalse",
  /* function   */ &impTFalse,
  /* parameters */ 0);

int impTTrue(TaxiState  & state, ActualParameterList & apl)
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
ParameterType * AnySite = new ParameterType("AnySite",
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

list<ParameterName *> * allFeatures = makeList(&PTaxiX,
					     &PTaxiY,
					     &PSourceOrHeld,
					     &PDestination);

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
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QEast(
  /* name       */ "QEast",
  /* parameters */ makeArgs(AnySite, &PTarget),
  /* child      */ MaxEast,
  /* child args */ makePPL(),
  /* features   */ makeList(&PTarget, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QSouth(
  /* name       */ "QSouth",
  /* parameters */ makeArgs(AnySite, &PTarget),
  /* child      */ MaxSouth,
  /* child args */ makePPL(),
  /* features   */ makeList(&PTarget, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QWest(
  /* name       */ "QWest",
  /* parameters */ makeArgs(AnySite, &PTarget),
  /* child      */ MaxWest,
  /* child args */ makePPL(),
  /* features   */ makeList(&PTarget, &PTaxiX, &PTaxiY),
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

int impTAtTarget(TaxiState  & state, ActualParameterList & apl)
{
  int target = apl[&PTarget];
  return state.location  == (*Sites)[target];
}
Predicate TAtTarget(
  /* name       */ "AtTarget",
  /* function   */ &impTAtTarget, 
  /* parameters */ makeArgs(AnySite, &PTarget));

QNode * PolicyNavigate(TaxiState & state,
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
  /* minValue   */ 0.123,
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
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

int impTHolding(TaxiState  & state, ActualParameterList & apl)
{
  int result = (state.source == Held);
  return result;
}
Predicate THolding(
  /* name       */ "Holding",
  /* function   */ &impTHolding, 
  /* parameters */ makeArgs());

QNode * PolicyGet(TaxiState & state,
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
  /* minValue   */ 0.123,
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
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

int impTNotHolding(TaxiState  & state, ActualParameterList & apl)
{
  return state.source != Held;
}
Predicate TNotHolding(
  /* name       */ "NotHolding",
  /* function   */ &impTNotHolding, 
  /* parameters */ makeArgs());

int impTNotHoldingAtDestination(TaxiState  & state, ActualParameterList & apl)
{
  // If we succeed, then the source is set to None and the
  // simulation is terminated.
  return (state.source == None);
}
Predicate TNotHoldingAtDestination(
  /* name       */ "NotHoldingAtDestination",
  /* function   */ &impTNotHoldingAtDestination, 
  /* parameters */ makeArgs());

QNode * PolicyPut(TaxiState & state,
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
  /* features              */ makeList(&PTaxiX, &PTaxiY, &PSourceOrHeld, &PDestination),
  /* policy                */ &PolicyPut,
  /* nonGoal penalty       */ -100.0);

// ----------------------------------------------------------------------
//
// Top Level
//
// ----------------------------------------------------------------------


// We need to estimate the cost of completing the Put after completing
// the Get.  This means we need to know the source (because this will
// be where we end up after the Get) and the destination.
QNode QGet(
  /* name       */ "QGet",
  /* parameters */ makeArgs(),
  /* child      */ MaxGet,
  /* child args */ makePPL(),
  /* features   */ makeList(&PSource, &PDestination),
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

// This node will always be terminal.
QNode QPut(
  /* name       */ "QPut",
  /* parameters */ makeArgs(),
  /* child      */ MaxPut,
  /* child args */ makePPL(),
  /* features   */ 0,
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode * PolicyRoot(TaxiState & state,
		   ActualParameterList & apl,
		   ActualParameterList & cpl)
{

  if (state.source != Held) {
      return &QGet;
  }
  else return &QPut;
}


int impTSimulationTerminatedAndPassengerDelivered(TaxiState  & state, ActualParameterList & apl)
{
  return (state.Terminated() && state.source == None);
}
Predicate TSimulationTerminatedAndPassengerDelivered(
  /* name       */ "TSimulationTerminatedAndPassengerDelivered",
  /* function   */ &impTSimulationTerminatedAndPassengerDelivered,
  /* parameters */ makeArgs());


int impTSimulationTerminated(TaxiState  & state, ActualParameterList & apl)
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
  /* children              */ makeList(&QGet, &QPut),
  /* features              */ makeList(&PTaxiX, &PTaxiY,
				       &PSourceOrHeld, &PDestination),
  /* policy                */ &PolicyRoot,
  /* nonGoal penalty       */ -100.0);


// ----------------------------------------------------------------------
//
// Reward Decomposition
//
// ----------------------------------------------------------------------


list<QNode *> * primitiveQNodes = makeList(&QNorth, &QEast, &QSouth, &QWest,
					   &QPickup, &QPutdown);

list<QNode *> * topLevelQNodes = makeList(&QGet, &QPut);

list<RewardResponsibility *> * RewardDecomposition = makeList(
  new RewardResponsibility(&TENone,           // no error 
			   primitiveQNodes),
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


