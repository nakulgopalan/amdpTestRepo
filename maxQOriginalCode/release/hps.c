// -*- c++ -*-
//
// Additions to MaxNode and QNode for Prioritized sweeping.
//
//
#include "global.h"
#include "state.h"
#include "get-state.h"
#include "hq.h"
#include "psweep.h"
#include "parameter.h"
#include "hq-features.h"
#include "hq-parameters.h"
#include <list.h>
#include <table.h>
#include <iostream.h>
#include <clrandom.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <minmax.h>

// We should probably put these into hq.h or some other appropriate
// header file...
extern int MAXPATHDUMP;
extern int COMPARE_WITH_HAND_CODED;
extern int TRACE;
extern int TraceNode(MaxNode * node, STATE * state,
	      ActualParameterList * apl);
extern int DEBUG;
extern int inheritTerminationPredicates;
extern int nonGoalTerminations;
extern float MINREALTEMPERATURE;
extern int LOGMAXNODES;
float EPSILON = 1E-5;

// implement the details of the exploration policy.  It returns values
// for chosenChild and chosenAPL.  The storage on chosenAPL should be
// reclaimed by the caller.
void MaxNode::ExplorationPolicy(STATE & state,
				ActualParameterList & apl,
				list<ActivationRecord *> & stack,
				clrandom & rng,
				HQParameters & params,
				QNode * & chosenChild,
				ActualParameterList * & chosenAPL,
				int Trace,
				int & explorationCount)
{
  // evaluate this state (with tilde values for this max node)
  list<MaxQPath *> * tildePaths = possibleActions(state, apl);
  assert(tildePaths);
  
  if (VERBOSE || Trace) {
    int len = tildePaths->length();
    if (len < MAXPATHDUMP) {
      cout << "tildePaths:\n" << *tildePaths << endl;
    }
    else {
      cout << "tildePaths length is " << len << endl;
    }
  }
  
  if (tildePaths->isEmpty()) {
    cout << "No possible actions at node " << this->name << endl;
    Debugger(state, stack, apl);
    // We have been asked to do something impossible.  
    abort();
  }
  
  MaxQPath * chosenPath = 0;

  // follow the builtin policy if either the probability of doing
  // so is 1.0 or with probability policyProbability
  // We also permit the author to specify that the policy MUST be
  // followed. 
  if ((policyProbability == 1.0) ||
      (requirePolicy) ||
      (rng.between(0.0, 1.0) < policyProbability)) {
    chosenPath = matchingPath(state, tildePaths, apl, rng, stack, params);
    }
  else {
    chosenPath = boltzmannExploration(tildePaths, state,
				      rng, explorationCount,
				      params, Trace);
    if (COMPARE_WITH_HAND_CODED) {
      MaxQPath * policyPath = matchingPath(state, tildePaths,
					   apl, rng, stack, params);
      if (chosenPath != policyPath) {
	cout << "Disagreement at node " << name
	     << "! chosenPath:\n" << *chosenPath << endl;
	cout << " policyPath:\n" << *policyPath << endl;
	state.Print(cout);
      }
    }
  }
    
  if ((VERBOSE || TRACE || Trace) && chosenPath) {
    cout << "chosenPath: " << *chosenPath << endl;
  }
    
  // extract chosen action (child) and its parameters
  // and then reclaim the tildePaths
  ValueResult * head = chosenPath->firstElement();
  chosenChild = head->op;     //  a in the update formulas
  chosenAPL = head->actualParameters;
  head->actualParameters = 0;     // prevent premature GC
  deletePathList(tildePaths);
  chosenPath = 0;
}  
				
RewardInfo * MaxNode::PSexecuteComposite(STATE & state,
					 ActualParameterList & apl,
					 list<ActivationRecord *> & stack,
					 list<STATE *> & upwardStates,
					 clrandom & rng,
					 HQParameters & params,
					 int & stepQuota,
					 int & explorationCount,
					 float & totalBellmanError,
					 int & weWereInterrupted)
{
  RewardInfo * ri = 0;		    // return result

  int terminationPredicateTrue = 0; // our termination predicate is true
  int goalPredicateTrue = 0;	    // our goal predicate is true
  int ancestorTerminated = 0;	    // our ancestor has terminated
  int ancestorTerminatedInGoalState = 0; // in his goal state!
  int weMustReturn = 0;		// we must return for any reason
  int childWasInterrupted = 0;      // child did not complete execution

  QNode * chosenChild = 0;           // child action chosen for execution
  ActualParameterList * chosenAPL;   // actual parameters for child action

  list<STATE *> downwardStates;   // The states returned by our child node.
  int nsteps;

  float startingTotalBellmanError = totalBellmanError;
  int startingExplorationCount = explorationCount;
  int startingRemainingExplorations = remainingExplorations;

  int Trace = TraceNode(this, &state, &apl);

  int NSteps = 0;
  int iterations = 0;

  if (VERBOSE || DEBUG || Trace)
    cout << "Executing Composite " << name << apl << endl;
  if (Trace) state.Print(cout);
  if (DEBUG) Debugger(state, stack, apl);

  // main loop: choose actions until interrupt or termination 
  do {
    // choose an action
    ExplorationPolicy(state, apl, stack, rng, params, 
		      chosenChild, chosenAPL, Trace,
		      explorationCount);
    
    // execute the chosen action 
    stack.add(new ActivationRecord(this, &apl));
    
    ri = chosenChild->execute(state, *chosenAPL, stack, downwardStates, rng,
			      params, stepQuota, explorationCount,
			      totalBellmanError, childWasInterrupted);
    
    if (VERBOSE) cout << "RewardInfo = " << *ri << endl;
    delete stack.pop();
    
    if (Trace) {
      cout << "Result state" << endl;
      state.Print(cout);
    }

    // compute the immediate reward
    float reward = 0.0;
    
    // accumulate the total number of steps needed to execute the
    // current Max node
    NSteps += ri->NSteps;

    // We need to compute the part of the immediate reward that is not
    // captured by our descendants.  

    // If the chosenChild is responsible, then add
    // in the reward
    RewardInfoIterator ritr(*ri);
    for (ritr.init(); !ritr; ++ritr) {
      if (ritr()->responsibleNodeList->includes(chosenChild)) {
	// take responsibility; move a reward from pending to total
	reward += ritr()->reward;
	// we delete this item from the list
	delete ritr();
	ritr.removeCurrent();
	// note that we may be responsible for multiple items
      }
    }
    
    // If our child completed its execution in a terminal state, then
    // we can update our model and perform some number of bellman
    // backups using the updated model.
    if (!childWasInterrupted) {
      // Push items onto the priority queue
      listIterator<STATE *> downItr(downwardStates);
      int n = 0;
      
      for (downItr.init(); !downItr; ++downItr) {
	StateHashCode predcode;  // index to predecessor StateInfo
	SuccessorInfo * succi;

	UpdateTransitionModel(*downItr(),
			      apl,
			      chosenChild,
			      *chosenAPL,
			      state,
			      reward,
			      predcode,
			      succi);
	n += params.prioritizedSweepingSteps;
	if (VERBOSE) {
	  cout << name << " PSQ push: " << predcode;
	  succi->Display();
	}
	// add it to PSQ with very high priority.
	PSQ.add(PSQItem(predcode, succi, 1.0E16));
      }
      
      PrioritizedSweeping(n);
    }

    delete chosenAPL;
    // Now prepend downwardStates onto the list we will return to our
    // parent.  This also empties downwardStates, thereby preparing it
    // to receive the next set of states if we haven't terminated
    // yet. 
    nsteps = downwardStates.length();
    upwardStates.prepend(downwardStates);
    

    // Now decide whether WE need to return.
    goalPredicateTrue = 0;
    terminationPredicateTrue = 0;
    weMustReturn = 0;

    // if ri->length() > 0, there are still unhandled penalties, so we
    // will need to return.
    if (ri->length() > 0) weMustReturn = 1;
    else {
      // we don't need to return this ri.
      delete ri;
      ri = 0;
    }
    
    if (stepQuota <= 0) weMustReturn = 1;

    if (VERBOSE) {
      cout << "Checking termination for " << name << endl;
    }
    
    // now check our various termination conditions
    terminationPredicateTrue = terminationPredicate->call(state, apl);
    goalPredicateTrue = goalPredicate->call(state, apl);
    
    // check if our ancestors have terminated, and if so, if they have
    // terminated in a goal state.
    ancestorTerminated = ancestorHasTerminated(state,
					       stack,
					       ancestorTerminatedInGoalState);

    if (terminationPredicateTrue || ancestorTerminated) {
      weMustReturn = 1;
    }

    // In some problems we want to inherit termination--that is, our
    // effective termination condition is the disjunction of all of
    // our ancestors' termination conditions up the tree.  Similarly,
    // our goal condition is the disjunction of the goal conditions
    // going up the tree.
    if (inheritTerminationPredicates) {
      terminationPredicateTrue =
	terminationPredicateTrue || ancestorTerminated;
      goalPredicateTrue =
	goalPredicateTrue || ancestorTerminatedInGoalState;
    }
    
    // produce debugging output
    if (VERBOSE && weMustReturn) {
      cout << name << " must return to caller" << endl;
    }
    if (VERBOSE && terminationPredicateTrue) {
      cout << name << " has terminated ";
      if (goalPredicateTrue) {
	cout << " in goal state" << endl;
      }
      else {
	cout << endl;
      }
    }
    
    // check for programming bugs.  I make a common mistake of
    // forgetting to include the goal condition in my termination
    // predicate.  Maybe I should just force the disjunction of these
    // two to be the termination predicate, but that would slow things
    // down.
    if (!terminationPredicateTrue &&
	goalPredicateTrue) {
      cout << "Goal Predicate true without Termination Predicate" <<
	endl;
      Debugger(state, stack, apl);
    }
    
    // count and announce nongoal termination
    if (terminationPredicateTrue && !goalPredicateTrue) {
      if (VERBOSE) cout << name << " non-goal termination." << endl;
      nonGoalTerminations++;
    }

    // if we terminated successfully, we will cool our
    // exploration policy.
    if (goalPredicateTrue && !childWasInterrupted) {
      // epsilonGreedy cools arithmetically; Boltzmann cools exponentially
      temperature = ((params.epsilonGreedy)?
		        temperature - coolingRate :
		        temperature * coolingRate);
      if (temperature < MINREALTEMPERATURE) temperature = 0;
      if (temperature < minTemperature) temperature = minTemperature;
    } // we can cool

  } while (! weMustReturn);

  extern int itrial;
  // print convergence information
  // Some of this has been lost...fix later.  
  if (logOutput && LOGMAXNODES || Trace)
    cout << itrial << " "
	 << explorationCount - startingExplorationCount << " "
         << remainingExplorations - startingRemainingExplorations << " " 
	 << temperature << "  "
	 << nsteps << " "
	 << goalPredicateTrue << " "
	 << PSQ.size() << " %% " << name << endl;

  // If our termination predicate is false, then we must be returning
  // because we were interrupted. 
  weWereInterrupted = !terminationPredicateTrue;

  // return the cumulative discounted reward from our children.
  // ri will be null if we terminated due to max iterations
  if (!ri) ri = new RewardInfo;
  ri->NSteps = NSteps;
  return ri;
}


StateHashCode MaxNode::HashState(STATE & state, ActualParameterList & apl)
{
  // we compute the hash value as a float and then round it at the
  // end. 
  computeFeatureVector(state, apl, features, fv);
  float answer = 0.0;
  // feature values
  vectorIterator<float> fvItr(*fv);
  // feature names, sizes, etc.
  listIterator<ParameterName *> itr(*features);
  for (fvItr.init(), itr.init(); !fvItr; ++fvItr, ++itr) {
    answer *= state.featureSize(itr());  // wasted the first iteration
    answer += fvItr();
  }
  // force rounding
  StateHashCode ranswer = (StateHashCode)(answer + 0.5);
  return ranswer;
}  

StateInfo * MaxNode::LookupState(STATE & state,
				 ActualParameterList & apl)
{
  StateHashCode code = HashState(state, apl);
  StateInfo * & si = (*transitionModel)[code];
  if (!si) {
    si = new StateInfo(state.Duplicate(),
		       apl.Duplicate(),
		       code);
  }
  return si;
}

void MaxNode::UpdateTransitionModel(STATE & startState,
				    ActualParameterList & APL,
				    QNode * child,
				    ActualParameterList & childAPL,
				    STATE & resultState,
				    float reward,
				    StateHashCode & startCode,
				    SuccessorInfo * & succi)
{
  StateInfo * startSI = LookupState(startState, APL);
  StateInfo * resultSI = LookupState(resultState, APL);

  // we return the start code.
  startCode = startSI->code;
  StateHashCode resultCode = resultSI->code;

  if (VERBOSE) {
    cout << "Updating model from " << startCode << " to " <<
      resultCode << endl;
  }

  ActualParameterList * childAPLcopy =
    startSI->RecordResult(child, &childAPL, resultCode, reward, succi);
  resultSI->RecordPredecessor(startCode, child, childAPLcopy);
}

void MaxNode::DisplayTransitionModel()
{
  cout << "Transition model browser for node " << name << endl;
  while (1) {
    cout << "Enter command (c show codes; n view node; q quit): ";
    char command;
    cin >> command;
    if (command == 'q') return;
    else if (command == 'c') {
      // print a list of all of the nodes and their codes
      // display the transition model for this MaxNode
      tableIterator<StateHashCode, StateInfo *> itr(*transitionModel);
      for (itr.init(); !itr; ++itr) {
	cout << "code = " << itr()->value()->code << " "
	     << itr()->value()->successors.length() << " successors, "
	     << itr()->value()->predecessors.length() << " predecessors, "
	     << "command (d display; q quit; n next): ";
	cin >> command;
	if (command == 'q') break;
	else if (command == 'n') continue;
	else {
	  itr()->value()->Display();
	}
      }
    }
    else if (command == 'n') {
      cout << "Enter hash code for node: ";
      StateHashCode code;
      cin >> code;
      StateInfo * & si = (*transitionModel)[code];
      if (!si) {
	cout << "No such code" << endl;
      }
      else {
	si->Display();
      }
    }
  }
}

void MaxNode::PrioritizedSweeping(int nsteps)
{
  // Perform prioritized sweeping for n steps
  while (nsteps > 0 && !PSQ.isEmpty()) {
    nsteps--;
    PSQItem top = PSQ.deleteMin();

    if (VERBOSE) {
      cout << name << " PSQ Pop: " << top.code << " priority = " <<
	top.priority;
      top.successorInfo->Display();
    }

    // Now compute an updated Q value for this state transition.
    // Extract start state info
    StateInfo * startSI = (*transitionModel)[top.code];
    STATE * startState = startSI->state;
    ActualParameterList * startAPL = startSI->apl;

    // extract the action info
    QNode * chosenChild = top.successorInfo->action;
    ActualParameterList * childAPL = top.successorInfo->apl;
    
    // Now update the C values for this action according to these
    // equations: 
    //
    // ~C(i,s,a) = sum_s' P(s'|s,a) [~V(i,s')+ ~R(s') + R_i(s'|s,a)]
    //           = sum_s' P(s'|s,a) [max_a' V(a',s') + ~C(i,s',a') +
    //                                       ~R(s') + R_i(s'|s,a)]
    //
    // C(i,s,a) = sum_s' P(s'|s,a)  [V(a'*,s') + C(i,s',a'*) +
    //                                R_i(s'|s,a)]
    //
    // and a*' is the a' chosen in the previous equation

    // iterator for the sum over s'
    listIterator<ResultStateInfo *> itr(top.successorInfo->resultStates);

    float tildeCvalue = 0.0;
    float Cvalue = 0.0;

    // sum over s'
    for (itr.init(); !itr; ++itr) {
      float resultStateTildeValue = 0.0;
      float resultStateValue = 0.0;
      int terminationPredicateTrue = 0;

      StateInfo * resultSI = (*transitionModel)[itr()->code];
      STATE * resultState = resultSI->state;
      ActualParameterList * resultAPL = resultSI->apl; // should be
                                                       // same as startAPL

      MaxQPath * bestTildePath = evaluate(*resultState, *resultAPL, 1, 0);

      // if bestTildePath is empty, this means the result state is
      // terminated, so the result state values are 0 by definition. 
      if (bestTildePath) {
	terminationPredicateTrue = 0;
	// resultStateTildeValue: max_a' V(a',s') + ~C(i,s',a')
	resultStateTildeValue = bestTildePath->firstElement()->totalValue;
	// resultStateValue:      V(a'*,s') + C(i,s',a'*)
	resultStateValue = unTilde(bestTildePath);
	deletePath(bestTildePath);
      }
      else {
	terminationPredicateTrue = 1;
      }

      // reward:                R_i(s'|s,a)
      float reward = itr()->totalReward / (float) top.successorInfo->N;
      // rewardTilde:           ~R(s')
      // tricky issue: we cannot check for parent task termination, so
      // we cannot do termination inheritance.
      // we might gain a speedup by caching the termination info
      int goalPredicateTrue =
	goalPredicate->call(*resultState, *resultAPL);
      float rewardTilde = 0.0;
      if (terminationPredicateTrue && !goalPredicateTrue) {
	rewardTilde += nonGoalTerminationPenalty;
      }
      // probability:           P(s'|s,a)
      float probability = ((float) itr()->N /
			   (float) top.successorInfo->N);
      
      if (VERBOSE) {
	cout << "  Result state " << itr()->code
	     << " P(s'|s,a) = " << probability
	     << " ~V(i,s') = " << resultStateTildeValue
	     << " ~R(s') = " << rewardTilde
	     << " R_i(s'|s,a) = " << reward << endl;
      }

      // accumulate the sums
      tildeCvalue += probability * (resultStateTildeValue + reward + rewardTilde);
      Cvalue += probability * (resultStateValue + reward);
    }
    
    if (VERBOSE) {
      cout << "     ~C(i,s,a) = " << tildeCvalue
	   << " C(i,s,a) = " << Cvalue << endl;
    }
    // Now we will recompute the value of this state:
    // ~V(i,s) = max_a [~C(i,s,a) + V(a,s)]
    // 
    // When we reach the a whose value we just recomputed above, 
    // we will update the C and ~C values.  We will also keep track of
    // whether this new ~C value is the "winner" for this state.  If
    // so, then we push new entries onto the PSQ.
    //

    SuccessorInfo * bestOldSuccessor = 0; 
    SuccessorInfo * bestNewSuccessor = 0; 
    float bestOldSuccessorValue = -999999.0;
    float bestNewSuccessorValue = -999999.0;

    float tildeValue = 0.0;
    float newTildeValue = 0.0;

    if (VERBOSE) {
      cout << "Preparing to compute new best a" << endl;
    }
    listIterator<SuccessorInfo *> sitr(startSI->successors);
    for (sitr.init(); !sitr; ++sitr) {
      QNode * a = sitr()->action;
      ActualParameterList * aAPL = sitr()->apl;
      list<MaxQPath *> * bestPath = a->evaluate(*startState, *aAPL, 1, 0);
      // Because aAPL should be fully bound, QNode::evaluate should 
      // return exactly one answer.
      assert(bestPath);
      assert(bestPath->length() == 1);
      ValueResult * vr = bestPath->firstElement()->firstElement();

      tildeValue = vr->totalValue;

      if (VERBOSE) {
	cout << "  successor:  tildeValue = " << tildeValue;
	sitr()->Display();
      }

      if (tildeValue > bestOldSuccessorValue) {
	if (VERBOSE) {
	  cout << "Found better old successor" << endl;
	}
	bestOldSuccessor = sitr();
	bestOldSuccessorValue = tildeValue;
      }

      // now check to see if we should be updating this guy
      float unused;
      if (sitr() == top.successorInfo) {
	if (VERBOSE) {
	  cout << "Found ourselves! " << endl;
	}
	a->store(*(a->fv),
		 Cvalue,
		 tildeCvalue);

	// now see if these revised values make any difference.
	// Take ~V(a,s) - old~C(i,s,a) + new~C(i,s,a)

	newTildeValue = vr->totalValue - vr->currentValue + tildeCvalue;
	if (newTildeValue > bestNewSuccessorValue) {
	  if (VERBOSE) {
	    cout << "We are a better new successor with value " <<
	      newTildeValue << endl;
	  }
	  bestNewSuccessor = sitr();
	  bestNewSuccessorValue = newTildeValue;
	}
      }
      else {
	// not the one to update.  Hence, his "New" value is his "Old"
	// value.
	if (tildeValue > bestNewSuccessorValue) {
	  if (VERBOSE) {
	    cout << "Found better new successor with value " <<
	      tildeValue << endl;
	  }
	  bestNewSuccessor = sitr();
	  bestNewSuccessorValue = tildeValue;
	}
      }
      deletePathList(bestPath);
    }
    
    if (VERBOSE) {
      cout << "Best old ~Q(i,s,a) " << bestOldSuccessorValue;
      bestOldSuccessor->Display();
      cout << "Best new ~Q(i,s,a) " << bestNewSuccessorValue;
      bestNewSuccessor->Display();
    }

    float delta = fabs(bestNewSuccessorValue - bestOldSuccessorValue);
    if (delta > EPSILON) {
      pushNewPSQs(startState,
		  startAPL,
		  delta,
		  startSI);
    }
  }
}

void MaxNode::pushNewPSQs(STATE * state,
			  ActualParameterList * apl,
			  float deltaValue,
			  StateInfo * si)
{
  // the computed value of state/apl has just been changed by an
  // amount deltaValue.  Push each predecessor onto the priority
  // queue.
  StateHashCode code = HashState(*state, *apl);
  
  listIterator<PredecessorInfo *> itr(si->predecessors);
  for (itr.init(); !itr; ++itr) {
    SuccessorInfo * succi = findMatchingSuccessorInfo(itr()->idnum,
						      itr()->action,
						      itr()->apl);
    // now compute probability of reaching state/action pair
    float arrivalProbability =
      succi->findResultProbability(code);

    if (VERBOSE) {
      cout << name << " PSQ push*: " << itr()->idnum
	   << " priority = " << arrivalProbability * deltaValue;
      succi->Display();
    }

    PSQ.add(PSQItem(itr()->idnum, succi, arrivalProbability * deltaValue));
  }
}


SuccessorInfo * MaxNode::findMatchingSuccessorInfo(StateHashCode code,
						   QNode * action,
						   ActualParameterList * apl)
{
  // Given the hash code CODE of a predecessor state, look up that
  // state, and search its successor information to find the entry for
  // action/apl. 

  StateInfo * si = (*transitionModel)[code];
  listIterator<SuccessorInfo *> itr(si->successors);
  for (itr.init(); !itr; ++itr) {
    if (itr()->MatchingSuccessorInfo(action, apl)) {
      return itr();
    }
  }
  cerr << "findMatchingSuccessorInfo: unable to find matching  successor";
  abort();
}

void MaxNode::PrintPQSize()
{
  if (!mark) {
    mark = 1;
    if (!isPrimitive) {
      cout << "MaxNode " << name << " PQ size " << PSQ.size() << endl;
      listIterator<QNode *> itr(*children);
      for (itr.init(); !itr; ++itr) {
	itr()->child.PrintPQSize();
      }
    }
  }
}
