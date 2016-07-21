//======================================================================
//  File:        sim.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface of the simulation
//               classes.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef SIM_H
#define SIM_H

#include <assert.h>
#include <priorque.h>



//----------------------------------------------------------------------
//	class event
//		execution event in a discrete event driven
//              simulation
//----------------------------------------------------------------------

class event
{
public:
    // constructor uses time of event
    event(unsigned int t);

    // time is a public data field
    const int unsigned time;

    // execute event by invoking this method
    virtual void processEvent() = 0;
};



//----------------------------------------------------------------------
//	class simulation
//		framework for discrete event-driven simulations
//----------------------------------------------------------------------

class simulation
{
public:
    // constructor
    simulation();

    // start and run simulation
    void              run();

    // return current time
    unsigned int      currentTime();

    // schedule a new future event
    void              scheduleEvent(event &);

    // flag to set when simulation is finished
    int               done;

protected:
    skewHeap<event &> eventQueue;
    unsigned int      time;
};



// Function Prototypes

int operator < (const event & left, const event & right);



#endif
