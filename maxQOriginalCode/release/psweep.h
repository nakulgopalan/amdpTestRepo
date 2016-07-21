// -*- C++ -*-
//
// Data structures needed for implementing prioritized sweeping
//

#ifndef PSWEEP_H
#define PSWEEP_H

#include "get-state.h"
#include "parameter.h"

//
// The key data structure for prioritized sweeping is a hash table
// that associates equivalence classes of (state, parentapl)
//
//        class(x) = {(s,apl) |\chi(s,apl) = x}
//
// with a single instance of the StateInfo class which stores the
// forward model of the transition probabilities and a backward model
// of potential predecessor states. 
//
// The purpose of creating the equivalence classes is to be able to
// share transition experiences (for estimating the forward model) and
// to make the prioritized sweeping process more efficient by reducing
// the size of the state space.  
//
// A difficult issue is how to define the \chi mapping.  The approach
// we have taken is to let the programmer specify which features
// should be included and which should be ignored in forming the
// equivalence class.  But what features should these be?  The
// features must be sufficient to perform a bellman backup of the C()
// values according to the equations
//
// ~C(i,s,a) = sum_s' P(s'|s,a) ~V(i,s')
//           = max_a sum_s' P(s'|s,a) [max_a' V(a',s') + ~C(i,s',a') +
//                                            ~R(s') + R_i(s'|s,a')]
//
// C(i,s,a) = sum_s' P(s'|s,a*)  [V(a'*,s') + C(i,s',a'*) +
//                                R_i(s'|s,a'*)]
//
// where a* is the action chosen in the previous equation
// and a*' is the a' chosen in the previous equation
//
// To compute the value of V(a',s') correctly, we need to distinguish
// between all pairs of states that will give different answers, and
// these will be all pairs of states that differ in any feature used
// by any descendant of i.  For ~C(i,s',a'), we would only need to
// distinguish among features used by one of the immediate children of
// i. 

// Technically, we would only need to distinguish among states (in the
// forward model for action a) whose descendants make distinctions.
// But this would be even trickier to compute.  So in our current
// implementation, we will just make a fixed set of distinctions as
// defined by MaxNode::features.  To do this, we will compute a hash
// code based on the feature vector computed from each state/parentapl
// combination.  This will be a unique identifier for each state/apl
// combination.  So if two hash values are equal, then we can view
// them as being the same state for this Max node.  We can also use
// these hash codes to access hash tables.  There will be one hash
// table for each Max node that maps from a hash code to a record
// containing all of the necessary information StateInfo.  

// Type defs and forward declarations

class QNode;
// We are using unsigned integers as hash codes.
typedef unsigned int StateHashCode;



// Class ResultStateInfo
// 
// This stores information about a result state and the number of
// times it was visited.
//
class ResultStateInfo
{
public:
  StateHashCode code; // hash code of the resulting state
  unsigned int  N;    // number of times this resulting state was
                      // visited. 
  double        totalReward;  // total reward R_i(s'|s,a) received
                              // upon transitions to this state.
  ResultStateInfo(StateHashCode i, float reward):
    code(i), N(1), totalReward(reward) {};

  void Display();

};

// 
// Class SuccessorInfo
//
// This stores information about the states that will result from
// executing a particular action
//

class SuccessorInfo
{
public:
  QNode *  action;            // action to perform
  ActualParameterList * apl;  // parameters to pass
  int N;                      // number of times this action was executed
  list<ResultStateInfo *> resultStates;

  // constructor
  SuccessorInfo(QNode * act,
		ActualParameterList * params,
		StateHashCode resultId,
		float reward):
    action(act),
    apl(params),
    N(1)
  {
    resultStates.add(new ResultStateInfo(resultId, reward));
  };

  // use to find the element that matches
  int MatchingSuccessorInfo(QNode * otheract, ActualParameterList * otherapl)
  {
    return (action == otheract &&
	    EquivalentAPL(*apl, *otherapl));
  };

  void Display();

  float findResultProbability(StateHashCode code);

};

class PredecessorInfo
{
public:
  StateHashCode idnum;              // hash code of predecessor state
  QNode *  action;                  // action performed
  ActualParameterList * apl;        // actual parameters
  PredecessorInfo(StateHashCode i,
		  QNode * a,
		  ActualParameterList * l): idnum(i), action(a), apl(l) {};
  int MatchingPredecessorInfo(StateHashCode code,
			      QNode * a,
			      ActualParameterList * l)
  {
    return (idnum == code &&
	    action == a &&
	    EquivalentAPL(*apl, *l));
  };

  void Display();

};


//
// StateInfo
//
// This data structure stores information about a state for a
// particular MaxNode to support Prioritized Sweeping
//

class StateInfo
{
public:
  STATE *  state;                  // pointer to the state
  ActualParameterList * apl;       // pointer to the parameters
  StateHashCode  code;		   // hashed value of state/apl combo
  list<SuccessorInfo *> successors;  // information about result states
  list<PredecessorInfo *> predecessors;  // indexes to predecessor states

  // constructor
  StateInfo(STATE * s, ActualParameterList * l, StateHashCode c):
    state(s), apl(l), code(c) {};
  ActualParameterList * RecordResult(QNode * act,
				     ActualParameterList * apl,
				     StateHashCode resultid,
				     float reward,
				     SuccessorInfo * & succi);
  void RecordPredecessor(StateHashCode startCode,
			 QNode * act,
			 ActualParameterList * apl);
  void Display();

};

// 
// class PSQItem  (prioritized sweeping queue item)
//

class PSQItem
{
public:
  StateHashCode    code;            // code of starting state
  SuccessorInfo *  successorInfo;   // operator and apl to apply
  float            priority;        // expected change in value

  // constructor
  PSQItem(StateHashCode c, SuccessorInfo * s, float p):
    code(c), successorInfo(s), priority(p) {};
  // We need a default constructor also
  PSQItem(): code(0), successorInfo(0), priority(-1E16) {};

  // comparator
  int operator < (PSQItem & other) {
    return (priority < other.priority);
  }
};



#endif
