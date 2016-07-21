//  -*- C++ -*- 
//
//   Flat Q learning maxq hierarchy for taxi task.
//
//
#include "predicate.h"
#include "taxi.h"
#include "hq.h"
#include "parameter.h"
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
float nonGoalTerminationPenalty = -100.0;

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

int impTSimulationTerminatedAndPassengerDelivered(TaxiState  & state,
						  ActualParameterList & apl)
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
// Primitive Actions
//
// ----------------------------------------------------------------------
QNode QNorth(
  /* name       */ "QNorth",
  /* parameters */ makeArgs(),
  /* child      */ MaxNorth,
  /* child args */ makePPL(),
  /* features   */ allFeatures,
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QEast(
  /* name       */ "QEast",
  /* parameters */ makeArgs(),
  /* child      */ MaxEast,
  /* child args */ makePPL(),
  /* features   */ allFeatures,
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QSouth(
  /* name       */ "QSouth",
  /* parameters */ makeArgs(),
  /* child      */ MaxSouth,
  /* child args */ makePPL(),
  /* features   */ allFeatures,
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QWest(
  /* name       */ "QWest",
  /* parameters */ makeArgs(),
  /* child      */ MaxWest,
  /* child args */ makePPL(),
  /* features   */ allFeatures,
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QPickup(
  /* name       */ "QPickup",
  /* parameters */ makeArgs(),
  /* child      */ MaxPickup,
  /* child args */ makePPL(),
  /* features   */ allFeatures,
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode QPutdown(
  /* name       */ "QPutdown",
  /* parameters */ makeArgs(),
  /* child      */ MaxPutdown,
  /* child args */ makePPL(),
  /* features   */ allFeatures,
  /* nhidden    */ 4,
  /* minValue   */ 0.123,
  /* maxvalue   */ 0.0,
  /* useTable   */ 1);

QNode * PolicyRoot(TaxiState & state,
		   ActualParameterList & apl,
		   ActualParameterList & cpl)
{

  if (state.source != Held) {
    // go to source location
    if (state.location == state.SourcePoint()) {
      // we are there, so pickup
      return &QPickup;
    }
    else {
      if (state.source == Red) {
	if (state.location.x < 2) {
	  if (state.location.y < 4) return &QNorth;
	  else return &QWest;
	}
	else if (state.location.y == 2) return &QWest;
	else if (state.location.y > 2) return &QSouth;
	else return &QNorth;
      }
      else if (state.source == Yellow) {
	if (state.location.x == 0) return &QSouth;
	else if (state.location.y == 2) return &QWest;
	else if (state.location.y > 2) return &QSouth;
	else return &QNorth;
      }
      else if (state.source == Green) {
	if (state.location.x > 1) {
	  if (state.location.y < 4) return &QNorth;
	  else return &QEast;
	}
	else if (state.location.y == 2) return &QEast;
	else if (state.location.y > 2) return &QSouth;
	else return &QNorth;
      }
      else if (state.source == Blue) {
	if (state.location.x > 2) {
	  if (state.location.y > 0) return &QSouth;
	  else return &QWest;
	}
	else if (state.location.y == 2) return &QEast;
	else if (state.location.y > 2) return &QSouth;
	else return &QNorth;
      }
    }
  }
  else {
    // go to destination
    if (state.location == state.DestinationPoint()) {
      // we are there so putdown
      return &QPutdown;
    }
    else {
      if (state.destination == Red) {
	if (state.location.x < 2) {
	  if (state.location.y < 4) return &QNorth;
	  else return &QWest;
	}
	else if (state.location.y == 2) return &QWest;
	else if (state.location.y > 2) return &QSouth;
	else return &QNorth;
      }
      else if (state.destination == Yellow) {
	if (state.location.x == 0) return &QSouth;
	else if (state.location.y == 2) return &QWest;
	else if (state.location.y > 2) return &QSouth;
	else return &QNorth;
      }
      else if (state.destination == Green) {
	if (state.location.x > 1) {
	  if (state.location.y < 4) return &QNorth;
	  else return &QEast;
	}
	else if (state.location.y == 2) return &QEast;
	else if (state.location.y > 2) return &QSouth;
	else return &QNorth;
      }
      else if (state.destination == Blue) {
	if (state.location.x > 2) {
	  if (state.location.y > 0) return &QSouth;
	  else return &QWest;
	}
	else if (state.location.y == 2) return &QEast;
	else if (state.location.y > 2) return &QSouth;
	else return &QNorth;
      }
    }
  }
}

MaxNode MaxRoot(
  /* name                  */ "MaxRoot",
  /* parameters            */ makeArgs(),
  /* precondition pred     */ &TTrue,
  /* termination predicate */ &TSimulationTerminated,
  /* goal predicate        */ &TSimulationTerminatedAndPassengerDelivered,
  /* children              */ makeList(&QNorth, &QEast, &QSouth, &QWest,
				       &QPickup, &QPutdown),
  /* features              */ allFeatures, 
  /* policy                */ &PolicyRoot,
  /* nonGoal penalty       */ -100.0);


// ----------------------------------------------------------------------
//
// Reward Decomposition
//
// ----------------------------------------------------------------------


list<QNode *> * primitiveQNodes = makeList(&QNorth, &QEast, &QSouth, &QWest,
					   &QPickup, &QPutdown);

list<QNode *> * topLevelQNodes = makeList(&QNorth, &QEast, &QSouth, &QWest,
					  &QPickup, &QPutdown);

list<RewardResponsibility *> * RewardDecomposition = makeList(
  new RewardResponsibility(&TENone,           // no error 
			   primitiveQNodes),
  new RewardResponsibility(&TEAlreadyHolding, // already holding
                           primitiveQNodes),
  new RewardResponsibility(&TENoPassenger,    // bad putdown
	    	           primitiveQNodes),
  new RewardResponsibility(&TENotAtSource,    // can't pickup
		           primitiveQNodes),
  new RewardResponsibility(&TENotHolding,     // bad putdown
		           primitiveQNodes),
  new RewardResponsibility(&TENotAtDestination, // bad putdown
		           primitiveQNodes));


