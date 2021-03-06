// -*- C++ -*-
//
// This file ensures that the proper includes for the state-specific
// classes have been performed.  It also defines a global symbol
// STATE, which can be used wherever the task-specific state type is
// needed.
//
// The files that currently use this are predicate.h, hq.h, and all of
// the main programs.

#ifndef GET_STATE_H
#define GET_STATE_H

#ifdef TAXISTATE
#include "taxi.h"
#define STATE TaxiState
#endif
#ifdef FUELTAXISTATE
#include "fueltaxi.h"
#define STATE  FuelTaxiState
#endif
#ifdef DELTATAXISTATE
#include "delta-taxi.h"
#define STATE   DeltaTaxiState
#endif
#ifdef RYANSTATE
#include "ryan.h"
#define STATE   RyanState
#endif
#ifdef PARRSTATE
#include "parr.h"
#define STATE   ParrState
#endif
#ifdef HDGSTATE
#include "hdg.h"
#define STATE   HDGState
#endif
 
#endif
