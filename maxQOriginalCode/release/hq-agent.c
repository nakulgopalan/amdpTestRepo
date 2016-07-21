// -*- C++ -*-
//
//  Agent using the MAXQ value function hierarchy.
//

// Budd data structure includes
#include <list.h>
#include <priorque.h>
#include <clrandom.h>

#include "get-state.h"
#include "hq.h"
#include "parameter.h"
#include "hq-parameters.h"
#include "taxi-maxq.h"

// C++ libraries
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <minmax.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>


int stopTrial = -1;

// Global variables
clrandom  RandomGenerator;               // random number generator.

int  VERBOSE = 1;		       // if 1, output verbose stuff
extern int LEARNVERBOSE;
extern int TRACE;
extern int EVALVERBOSE;
extern int nonGoalTerminations;
extern int LOGMAXNODES;

int LEARN = 1;			// turn on learning


int MESSAGES = 0;		// don't print messages

#ifdef __GNUC__
#ifndef WINDOWS98
// force explicit instantiations for g++ 2.7.2 on unix/hp
template class vector<int>;
template class list<Atom *>;
#endif
#endif

// Each domain must define currentState and stateItr. 
#ifdef TAXISTATE
TaxiState currentState(18475);
TaxiStateIterator stateItr(currentState);
#endif
#ifdef FUELTAXISTATE
FuelTaxiState currentState(18475);
FuelTaxiStateIterator stateItr(currentState);
#endif
#ifdef DELTATAXISTATE
DeltaTaxiState currentState(18475);
DeltaTaxiStateIterator stateItr(currentState);
#endif
#ifdef RYANSTATE
RyanState currentState(18475);
#endif
#ifdef PARRSTATE
ParrState currentState(18475);
ParrStateIterator stateItr(currentState);
// some states for monitoring progress
ParrState startState(0);
ActualParameterList startApl;
ParrState niceState(0);
ActualParameterList niceApl;
#endif
#ifdef HDGSTATE
HDGState currentState(18475);
HDGStateIterator stateItr(currentState);
#endif

int itrial;
int startingTrial = 0;
HQParameters params;
int DEBUG = 0;

void PrintUsage() 
{
  cout << "Usage: hq-agent [-c parameter-file][-s wtsfile][-r wtsfile][-b][-t trials][-i n][-x][-d seed]\n";
  cout << "-s save-wts-file      file for saving weights\n";
  cout << "-r read-wts-file      file for reading in weights\n";
  cout << "-c parameter-file     file of parameter values\n";
  cout << "-b                    batch train \n";
  cout << "-t trials             batch train for t trials\n";
  cout << "-i n                  evaluate every n trials\n";
  cout << "-x                    don't randomize simulation\n";
  cout << "-d seed               randomize simulation using seed\n";
}

void saveWts(char * saveFileName, MaxNode * root, STATE & state,
	     int & iteration)
{
  root->clearMarks();
  if (saveFileName) {
    ofstream saveStr(saveFileName);
    // first write out the iteration and the random number seed
    saveStr << iteration << " " << RandomGenerator.getseed() << endl;
    // then save the state
    state.Save(saveStr);
    // then save the weights
    root->save(saveStr);
  }
}

void readWts(char * readFileName, MaxNode * root, STATE & state,
	     int & iteration)
{
  if (readFileName) {
    ifstream str(readFileName);
    // read in the random number seed 
    randomState rs;
    str >> iteration >> rs;
    RandomGenerator.setseed(rs);
    // now read the state
    state.Read(str);
    // and the weights
    root->clearMarks();
    root->read(str);
  }
}

int Step(STATE & state, HQParameters & params, int stepQuota,
	 MaxNode * node, ActualParameterList & apl)
{
  list<ActivationRecord *> stack;  int start = stepQuota;

  if (state.Terminated()) {
    // if the state is terminated, then we initialize a new trial
    state.Init();
    if (VERBOSE || TRACE) {
      cout << "Initializing State:\n";
      state.Print(cout);
    }
  }

  nonGoalTerminations = 0;

  while (stepQuota > 0 && !state.Terminated()) {
    float totalBellmanError = 0.0;
    int explorationCount = 0;
    int childWasInterrupted = 0;

    list<STATE *> upwardStates;

    RewardInfo * ri = node->execute(state,
				    apl,
				    stack,
				    upwardStates,
				    RandomGenerator,
				    params,
				    stepQuota,
				    explorationCount,
				    totalBellmanError,
				    childWasInterrupted); 
    
    // release the returned list of states
    listIterator<STATE *> itr(upwardStates);
    for (itr.init(); !itr; ++itr) delete itr();
    upwardStates.deleteAllValues();

    if(!stack.isEmpty()) cerr << "Stack returned nonempty" << endl;
  
    if (ri) {
      if (VERBOSE) cout << "Reward info: " << *ri << endl;
      delete ri;
      // a non-null ri indicates that something was actually
    }
    else {
      cout << "Reward info: (null)" << endl;
    }
  }

  // return number of actual steps taken
  return start - stepQuota;
}

void EvaluationTrial(MaxNode * Root, int totalSteps)
{
  Root->clearMarks();
  Root->saveTemperature();
  float saveLambda = params.lambda;
  Root->setTemperature(0.0, 0.0, 0.0);  // go greedy
  params.lambda = 0.0;        // no learning
  LEARN = 0;
  int saveVERBOSE = VERBOSE;
  VERBOSE = 0;
  int saveLEARNVERBOSE = LEARNVERBOSE;
  LEARNVERBOSE = 0;
  int saveLOGMAXNODES = LOGMAXNODES;
  //  LOGMAXNODES = 0;
  ActualParameterList emptyApl;

  nonGoalTerminations = 0;
  currentState.Init();
  
  // Should we do our evaluation trials in a non-greedy way, in
  // a totally greedy way, or what?  Perhaps this should be yet
  // another parameter. 
  int remainingStepsThisTrial = params.maxStepsPerEvalTrial;
  int rootQuota = params.stepsPerEvalCall;
  int stepsThisTrial = 0;
  int nonGoalTerminationsThisTrial = 0;
  int actualSteps;

  while (remainingStepsThisTrial > 0) {
    if (rootQuota > remainingStepsThisTrial) {
      rootQuota = remainingStepsThisTrial;
    }
    actualSteps = Step(currentState, params, rootQuota, Root, emptyApl);
    stepsThisTrial += actualSteps;
    nonGoalTerminationsThisTrial += nonGoalTerminations;
    remainingStepsThisTrial -= actualSteps;
    if (currentState.Terminated()) remainingStepsThisTrial = 0;
  }
  
  cout << itrial << "\t"
       << Root->countNonZeroWeightsRoot() << "\t"
       << currentState.cumulativeReward << "\t"
       << stepsThisTrial << "\t"
       << nonGoalTerminationsThisTrial << "\t"
       << totalSteps
       << "  ## Eval" << endl;
  Root->clearMarks();
  Root->restoreTemperature();
  params.lambda = saveLambda;
  Root->clearMarks();
  cout << "Nodename\tTemperature\tpolicyProbability\trequirePolicy\n";
  Root->printTemperatures();
  LEARN = 1;
  LEARNVERBOSE = saveLEARNVERBOSE;
  VERBOSE = saveVERBOSE;
  LOGMAXNODES = saveLOGMAXNODES;
}

void setLearningRates(HQParameters & params, MaxNode * root, int trial)
{
  // we have just completed trial trial, change any learning rates,
  // as specified by params.
  
  listIterator<LearningRateInfo> itr(params.lrateInfo);
  for (itr.init(); !itr; ++itr) {
    if (itr().trial == trial) {
      // process it
      MaxNode * node = root->findMaxNode(itr().nodeName);
      if (!node) {
	cerr << "Unable to find Max Node " << itr().nodeName
	     << " to set learning rate." << endl;
	abort();
      }
      node->learningRate = itr().learningRate;
      cout << "Setting learning rate of " << itr().nodeName
	   << " to " << itr().learningRate << endl;
    }
  }
}

int TraceNode(MaxNode * node, STATE * state,
	      ActualParameterList * apl)
{
  // decide whether to trace this node
#if 0
#ifdef PARRSTATE
  extern MaxNode MaxExitInter;
  if (((ParrState *) state)->location == Point(36,60) &&
      node == &MaxExitInter &&
      (*apl)[&PDir] == South) return 1;
#endif
#endif
  return 0;
}


int main(int argc, char * argv[])
{
  int optc;
  extern char *optarg;
  extern int optind;
  char * paramFileName = 0;
  char * readFileName = 0;
  char * saveFileName = 0;
  int iteration = 0;
  // agent state:
  Action * chosenAction;
  int batch = 0;        
  int ntrials = 1;
  long int neval = 9999999;
  int dontRandomize = 0;
  extern char getopt(int, char *[], char *);
  extern char * optarg;
  int valueIteration = 0;
  ActualParameterList emptyApl;

  while (( optc = getopt(argc, argv, "s:r:c:bt:n:i:xd:v:1")) != -1 ) {
    switch (optc) {
    case 'c':
      paramFileName = optarg;
      printf("Read parameters from %s.\n", paramFileName);
      break;
    case 's':
      saveFileName = optarg;
      printf("Write weights on %s.\n", saveFileName);
      break;
    case 'r':
      readFileName = optarg;
      printf("Read weights from %s.\n", readFileName);
      break;
    case 'b':
      batch = 1;
      printf("Batch mode.\n");
      break;
    case 't':
      ntrials = atoi(optarg);
      printf("Perform %d trials.\n", ntrials);
      break;
    case 'i':
      neval = atoi(optarg);
      printf("Perform non-learning evaluation trial every %d trials.\n", neval);
      break;
    case 'x':
      dontRandomize = 1;
      printf("Random number generator for simulation is not randomized.\n");
      break;
    case 'd': {
      int seed = atoi(optarg);
      randomState rs(seed, 1);
      currentState.rng.setseed(rs);
      dontRandomize = 1;
      printf("Set simulator seed to %d.\n", seed);
      break;
    }
    case 'v':
      valueIteration = atoi(optarg);
      cout << "Perform value iteration for " << valueIteration << " iterations.\n";
      break;
    case '1':
      DEBUG = 1;
      cout << "Turning on single stepping" << endl;
      break;
    default:
      PrintUsage();
      exit(-1);
    }
  }

#ifdef PARRSTATE
  // dump the maze in gnuplot usable format
  {
    ofstream mazeout("maze.data");
    for (int x = 0; x < worldSize; x++) {
      for (int y = 0; y < worldSize; y++) {
	if (MazeAt(x, y) == '*' ||
	    MazeAt(x, y) == '#') {
	  mazeout << x + 0.5 << " " << worldSize - (y + 0.5) << "\n";
	}
      }
    }
  }

  // initialize the cache
  InitializeLegalRoomCache();
#endif  

  if (!dontRandomize) {
    // note: on pc, this does not work
    currentState.rng.setseedFromClock();
    cout << "Taxi random number seed set to "<< currentState.rng.getseed()
	 << " from clock" << endl;
  }

  if (paramFileName) {
    params.Read(paramFileName);
  }

  {
    unsigned short temp = params.seed & 0xffff;
    RandomGenerator.setseed(randomState(temp ,1));
  }

  MaxNode * Root = &MaxRoot;
  // initialize the MAXQ hierarchy
  Root->init(RandomGenerator, params, currentState);

  // do parameter checks
  Root->CheckDownwardParameters();
  Root->CheckPredicates();

  // install default parameter values.  Most of these can be
  // overridden by further options in params and/or interactively. 
  VERBOSE = params.verbose;
  LEARNVERBOSE = params.learnverbose;
  Root->setTemperature(params.temperature, params.coolingRate,
		       params.minTemperature);
  Root->setPolicyProbability(params.policyProbability);
  Root->setLearningRate(params.learningRate);

  {
    // install temperature controls for specified nodes.
    listIterator<TemperatureInfo> itr(params.tempInfo);
    int temperror = 0;
    for (itr.init(); !itr; ++itr) {
      MaxNode * node = MaxRoot.findMaxNode(itr().nodeName);
      if (!node) {
	cerr << "Unable to find MaxNode " << itr().nodeName << endl;
	temperror = 1;
      }
      node->temperature = itr().temperature;
      node->coolingRate = itr().coolingRate;
      node->minTemperature = itr().minTemperature;
    }
    if (temperror) return 2;  // exit    
  }

  setLearningRates(params, Root, 0);

  {
    // mark nodes to include in log file
    listIterator<string> itr(params.nodesToLog);
    int logerror = 0;
    for (itr.init(); !itr; ++itr) {
      MaxNode * node = MaxRoot.findMaxNode(itr());
      if (!node) {
	cout << "Unable to find MaxNode to log " << itr() << endl;
	logerror = 1;
      }
      node->logOutput = 1;
    }
    if (logerror) return 2;
  }

  {
    // mark nodes to require hand-coded policy
    listIterator<string> itr(params.nodesToRequirePolicy);
    int logerror = 0;
    for (itr.init(); !itr; ++itr) {
      MaxNode * node = MaxRoot.findMaxNode(itr());
      if (!node) {
	cout << "Unable to find MaxNode for requirePolicy " << itr() << endl;
	logerror = 1;
      }
      node->requirePolicy = 1;
    }
    if (logerror) return 2;
  }

  {
    // mark nodes to perform value iteration
    listIterator<string> itr(params.nodesToPerformValueIteration);
    int logerror = 0;
    for (itr.init(); !itr; ++itr) {
      MaxNode * node = MaxRoot.findMaxNode(itr());
      if (!node) {
	cout << "Unable to find MaxNode for performValueIteration " << itr() << endl;
	logerror = 1;
      }
      node->performValueIteration = 1;
    }
    if (logerror) return 2;
  }

  // if a saved state/weight set was given, then read it.  It will
  // overwrite the temperature information from the config file. Which
  // is why this is done at this point.
  if (readFileName) {
    readWts(readFileName, Root, currentState, startingTrial);
  }

  char c;
  
  // Now that the simulation is episodic, we can change the meaning of
  // batch mode.  The value of batch is now simply non-zero if we are
  // in batch mode.  The value of ntrials tells how many total
  // episodes we will do, and the value of neval tells how often we
  // will do an evaluation episode. 

  int actualSteps;

#ifdef PARRSTATE
  startState.Init();
  startApl[&PDir] = South;
  // niceState.Init();
  // niceState.location.x = 48;
  // niceState.location.y = 84;
  // niceApl[&PDir] = East;
  // cout << "niceState: " << endl;
  //  niceState.Print(cout);
#endif

  if (valueIteration > 0) {
    params.Display(cout);
    // perform value iteration instead of Q learning

    int explorationCount = 0;
    for (int i=0; i < valueIteration; i++) {
      ActualParameterList apl;
      list<ActivationRecord *> stack;  
      float totalBellmanError = 0.0;
      int bellmanCount = 0;

      // visit every possible state, and perform a VI update for that
      // state at all levels of the MAXQ graph
      for (stateItr.init(); !stateItr; ++stateItr) {
	if (VERBOSE) {
	  cout << "Starting Value Iteration for State\n";
	  currentState.Print(cout);
	}
	// we misuse two parameters to control value iteration
	// maxStepsPerTrial is used to control the number of steps that
	// each action is permitted to run during VI.
	// pollingCoolingStep is used to control the number of
	// iterations of each action that is performed, in order to get
	// good statistics on the result state expected value.
	float tbe = 0.0;
	Root->ValueIteration(currentState,
			     apl,
			     stack,
			     RandomGenerator,
			     params,
			     params.maxStepsPerTrial,
			     explorationCount,
			     tbe,
			     bellmanCount,
			     params.pollingCoolingStep);

	totalBellmanError += tbe;

	/*
	if (tbe > 0.0) {
	  cout << "BellmanError = " << tbe << " in state: " << endl;
	  currentState.Print(cout);
	}
	*/
      }

      cout << i << "\t" << totalBellmanError
	   << "\t" << bellmanCount
	   << "\t" << explorationCount
	   << "  VI" << endl;
      for (int itrial = 0; itrial < neval; itrial++) {
	EvaluationTrial(Root, explorationCount);
      }
      totalBellmanError = 0;
    }
    cout << "Saving weights on ";
    if (saveFileName) {
      cout << saveFileName << endl;
    }
    else {
      cout << " [null file name -- no save]" << endl;
    }
    saveWts(saveFileName, Root, currentState, itrial);	    
    return 0;
  }

  if (batch > 0) {
    // batch mode.  In this mode, we perform a total of
    // ntrials.  In each trial, we perform one episode.
    // Every neval trials, we perform an evaluation trial (during
    // which, we turn off learning and exploration). 
    // neval is initialized to a very big number, so we normally don't
    // do any evaluations.
    cout << "Batch mode" << endl;
    params.Display(cout);
    cout << "Trial TotalReward  actualSteps  nonGoalTerminations totalSteps restarts" << endl;

    int coolingInterval = params.pollingCoolingRate;
    int stepsPerTrial = params.maxStepsPerTrial;
    int totalSteps = 0;

    for (itrial = startingTrial; itrial < startingTrial + ntrials; itrial++) {
      setLearningRates(params, Root, itrial);
      coolingInterval--;
      if (coolingInterval <= 0) {
	coolingInterval = params.pollingCoolingRate;
	stepsPerTrial -= params.pollingCoolingStep;
	if (stepsPerTrial < 1) stepsPerTrial = 1;
	if (DEBUG) cout << itrial << " " << stepsPerTrial << " stepsPerTrial" << endl;
      }
      int remainingStepsThisTrial = params.maxStepsPerTrial;
      int rootQuota = stepsPerTrial;
      int stepsThisTrial = 0;
      int nonGoalTerminationsThisTrial = 0;
      int restarts = 0;
      currentState.Init();

      // Invoke Step until termination or until we have performed
      // maxStepsPerTrial (i.e., until there are no remaining steps)
      // In each invocation, we perform rootQuota steps. 
      while (remainingStepsThisTrial > 0) {
	if (rootQuota > remainingStepsThisTrial) {
	  rootQuota = remainingStepsThisTrial; 
	}
	restarts++;
	actualSteps = Step(currentState, params, rootQuota, Root, emptyApl);
	stepsThisTrial += actualSteps;
	totalSteps += actualSteps;
	nonGoalTerminationsThisTrial += nonGoalTerminations;
	remainingStepsThisTrial -= actualSteps;
	if (currentState.Terminated()) remainingStepsThisTrial = 0;
      }

      cout << itrial << "\t"
	   << currentState.cumulativeReward << "\t"
	   << stepsThisTrial << "\t"
	   << nonGoalTerminationsThisTrial << "\t"
	   << totalSteps << "\t"
	   << restarts
	   << "  ##" << endl;
      
      if (itrial == stopTrial) {
	cout << "At trial stopTrial!" << endl;
      }

      if (itrial % neval == (neval - 1)) EvaluationTrial(Root, totalSteps);
    }
    cout << "Saving weights on ";
    if (saveFileName) {
      cout << saveFileName << endl;
    }
    else {
      cout << " [null file name -- no save]" << endl;
    }
    saveWts(saveFileName, Root, currentState, itrial);	    
    return 0;
  }
  else {
    // interactive mode
    
    currentState.Init();
    while (1) {
      // loop asking user what to do
      // Ask user what to do:
      cout << "Evaluate state (e), follow policy (f), primitive action (a)," << endl;
      cout << "  greedy value (g), set policyProbability (x), root quota (r), " << endl;
      cout << "  show options (o) for acting, clobber zero weights (b)," << endl;
      cout << "  show params (c) toggle verbose (v), print state (p), single step (s)"
	   << endl;
      cout << "  invoke value iteration (i), toggle EVALVERBOSE (w)," << endl;
      cout << "  show transition model (m), show PSQ sizes (#)," << endl;
      cout << "  set temp (t), count nonzero weights (z), run tests ([), quit (q): ";
      cin >> c;
      
      if (c == 'z') {
	cout << "Non-zero weights: " << Root->countNonZeroWeightsRoot() <<
	  endl;
      }
      else if (c == 'm') {
	string s;
	cout << "Enter name of MaxNode: ";
	cin >> s;
	MaxNode * node = Root->findMaxNode(s);
	if (node) node->DisplayTransitionModel();
	else cout << "Can't find MaxNode " << s << endl;
      }
      else if (c == '#') {
	Root->clearMarks();
	Root->PrintPQSize();
      }
      else if (c == 'w') {
	EVALVERBOSE = 1 - EVALVERBOSE;
	cout << "EVALVERBOSE = " << EVALVERBOSE << endl;
      }
      else if (c == 's') {
	DEBUG = 1 - DEBUG;
	cout << "DEBUG = " << DEBUG << endl;
      }
      else if (c == '[') {
	cout << "Choose test to run: " << endl;
	cout << "  0 - dump temperature information " << endl;
	cout << "  1 - perform 10000 evaluateLists " << endl;
	cout << "  2 - perform 10000 possibleActions" << endl;
	cout << "  3 - perform 10000 evaluations " << endl;
#ifdef HDGSTATE
	cout << "  4 - dump nearest landmark cache " << endl;
	cout << "  5 - test all combinations of states and landmarks "
	     << endl;
	cout << "  6 - set state and goal " << endl;
#endif	
#ifdef PARRSTATE
	cout << "  4 - dump legal direction cache " << endl;
	cout << "  5 - set state " << endl;
#ifdef HPARRSTATE
        cout << "  6 - rollouts of MaxExitInter " << endl;
#endif
#endif	
	int testnum;
	cout << "? ";
	cin >> testnum;
	switch (testnum) {
	case 0: { 
	  Root->clearMarks();
	  cout << "Nodename\tTemperature\tpolicyProbability\trequirePolicy\n";
	  Root->printTemperatures();
	  break;
	}
	case 1: {
	  for (int itest = 0; itest < 10000; itest++) {
	    ActualParameterList apl;
	    list<MaxQPath *> * result = Root->evaluateList(currentState, apl, 0);
	    deletePathList(result);
	  }
	  break;
	}
	case 2: {
	  for (int itest = 0; itest < 10000; itest++) {
	    ActualParameterList apl;
	    list<MaxQPath *> * result = Root->possibleActions(currentState, apl);
	    deletePathList(result);
	  }
	  break;
	}
	case 3: {
	  for (int itest = 0; itest < 10000; itest++) {
	    ActualParameterList apl;
	    MaxQPath  * result = Root->evaluate(currentState, apl, 0);
	    deletePath(result);
	  }
	  break;
	}
#ifdef HDGSTATE
	case 4: {
	  for (int y = 0; y < worldSize; y++) {
	    for (int x = 0; x < worldSize; x++) {
	      cout << x << "," << y << " nearest landmark = " <<
		(int) currentState.NearestLandmark(Point(x,y)) << endl;
	    }
	  }
	  break;
	}
	case 5: {
	  cout << "x  y  starting-cell  goal-cell  nsteps" << endl;
	  for (int y = 0; y < worldSize; y++) {
	    for (int x = 0; x < worldSize; x++) {
	      Point loc(x,y);
	      currentState.location = loc;
	      HDGLandmark nearest = currentState.NearestLandmark(loc);
	      listIterator<HDGLandmark> itr(*(currentState.LandmarkNeighborList(nearest)));
	      for (itr.init(); !itr; ++itr) {
		currentState.location = loc;
		currentState.goal = currentState.LandmarkAsPoint(itr());
		int n = Step(currentState, params,
			     params.maxStepsPerTrial, Root, emptyApl);
		cout << x << " "
		     << y << " "
		     << (int) nearest << " "
		     << itr() << " "
		     << n << endl;
	      }
	    }
	  }
	  break;
	}
	case 6: {
	  cout << "Enter current location as a Point: (x,y):";
	  cin >> currentState.location;
	  cout << "Enter goal location as a Point: (x,y):";
	  cin >> currentState.goal;
	  break;
	}
#endif
#ifdef PARRSTATE
	case 4: {
	  currentState.dumpLegalRoomCache();
	  break;
	}
	case 5: {
	  cout << "Enter location (x,y): ";
	  cin >> currentState.location;
	  currentState.Print(cout);
	  break;
	}
#ifdef HPARRSTATE
	case 6: {
	  cout << "Performing 10000 rollouts of MaxExitInter" << endl;
	  extern MaxNode MaxExitInter;
	  ActualParameterList apl;
	  apl[&PDir] = South;
	  apl[&PSnapRoom] = (int) currentState.featureValue(&PSnapRoom);
	  for (int i = 0; i < 10000; i++) {
	    STATE * s = currentState.Duplicate();

	    float totalBellmanError = 0.0;
	    int explorationCount = 0;
	    int childWasInterrupted = 0;
	    int stepQuota = 1000;
	    list <ActivationRecord *> stack;
	    
	    list<STATE *> upwardStates;
	    
	    RewardInfo * ri = MaxExitInter.execute(*s,
					    apl,
					    stack,
					    upwardStates,
					    RandomGenerator,
					    params,
					    stepQuota,
					    explorationCount,
					    totalBellmanError,
					    childWasInterrupted); 
	    
	    // release the returned list of states
	    listIterator<STATE *> itr(upwardStates);
	    for (itr.init(); !itr; ++itr) delete itr();
	    upwardStates.deleteAllValues();
	    
	    if(!stack.isEmpty()) cerr << "Stack returned nonempty" << endl;
	    
	    if (ri) {
	      if (VERBOSE) cout << "Reward info: " << *ri << endl;
	      delete ri;
	      // a non-null ri indicates that something was actually
	    }
	    else {
	      cout << "Reward info: (null)" << endl;
	    }
	    // keep the random numbers evolving
	    currentState.rng = s->rng;

	    delete s;
	  }
	  break;
	}
#endif
#endif
	}
      }
      else if (c == 'r') {
	cout << "Current value of root quota = " << params.maxStepsPerTrial << endl;
	cout << "Enter root quota: ";
	cin >> params.maxStepsPerTrial;
      }
      else if (c == 'c') {
	params.Display(cout);
	cout << "\tVERBOSE\t" << VERBOSE << endl;
	cout << "\tLEARNVERBOSE\t" << LEARNVERBOSE << endl;
	cout << "\tDEBUG\t" << DEBUG << endl;
      }
      else if (c == 'e') {
	ActualParameterList apl;
	list<MaxQPath *> * result = Root->evaluateList(currentState, apl, 0);
	cout << "Normal evaluation: " << (*result) << endl;
	deletePathList(result);
	result = Root->evaluateList(currentState, apl, 1);
	cout << "Tilde evaluation: " << (*result) << endl;
	deletePathList(result);
      }
      else if (c == 'o') {
	ActualParameterList apl;
	list<MaxQPath *> * result = Root->possibleActions(currentState, apl);
	cout << "Possible actions: " << (*result) << endl;
	deletePathList(result);
      }
#ifdef PARRSTATE
      else if (c == '%') {
	// display sonar readings
	cout << "Dir\tLong\tShort" << endl;
	for (int d = North; d <= West; ++d) {
	  cout << "\t" << (int) d << "\t"
	       << currentState.LongSonar((Direction)d)
	       << "\t"
	       << currentState.ShortSonar((Direction)d)
	       << endl;
	}
      }
#endif      
      else if (c == 'g') {
	if (currentState.Terminated()) {
	  currentState.Init();
	  if (TRACE || VERBOSE) {
	    cout << "Initializing the state.\n";
	    currentState.Print(cout);
	  }
	}
	ActualParameterList apl;
	MaxQPath * result = Root->evaluate(currentState, apl, 1);
	if (result) {
	  cout << "Greedy path " << (*result) << endl;
	  deletePath(result);
	}
	else cout << "Null result" << endl;
      }
      else if (c == 'x') {
	cout << "Enter policyProbability: ";
	float pp;
	cin >> pp;
	Root->setPolicyProbability(pp);
	cout << "policyProbability set to " << pp <<
	  " in all max nodes" << endl;
      }
      else if (c == 'v') {
	VERBOSE = 1 - VERBOSE;
	LEARNVERBOSE = 1 - LEARNVERBOSE;
	cout << "VERBOSE = " << VERBOSE << " LEARNVERBOSE = " <<
	  LEARNVERBOSE << endl;
      }
      else if (c == 'p') {
	currentState.Print(cout);
      }
      else if (c == 't') {
	cout << "Enter temperature: ";
	float temp;
	cin >> temp;
	cout << "Enter cooling rate: ";
	float cool;
	cin >> cool;
	cout << "Enter minimum temperature: ";
	float minTemp;
	cin >> minTemp;
	Root->setTemperature(temp, cool, minTemp);
      }
      else if (c == 'b') {
	float val;
	cout << "Enter value to provide to all unmodified weights: ";
	cin >> val;
	MaxRoot.clobberZeroWeightsRoot(val);
      }
      else if (c == 'f') {
	Step(currentState, params, params.maxStepsPerTrial, Root, emptyApl);
#if PARRSTATE
	{
	  // dump trajectory on cout
	  // we transform the coordinates by subtracting the y
	  // coordinates from worldSize, so that increasing y goes downward
	  ParrState::trajectory.reverse();
	  listIterator<Point> itr(ParrState::trajectory);
	  cout << 12 << " " << worldSize - 12 << "\n";
	  for (itr.init(); !itr; ++itr) {
	    cout << itr().x << " " << worldSize - itr().y << "\n";
	  }
	  cout << endl;
	  ParrState::trajectory.deleteAllValues();
	}
#endif
      }
      else if (c == 'a') {
	// choose primitive action to perform
#if TAXISTATE || DELTATAXISTATE
	cout << "(n)orth (s)outh (e)ast (w)est pick(u)p put(d)own? ";
#endif
#ifdef FUELTAXISTATE
	cout << "(n)orth (s)outh (e)ast (w)est pick(u)p put(d)own (f)illup? ";
#endif
#ifdef RYANSTATE
	cout << "(n)orth (s)outh (e)ast (w)est\n";
	cout << "(1)northeast (2)southeast (3)southwest (4)northwest? ";
#endif
#if PARRSTATE || HDGSTATE
	cout << "(n)orth (s)outh (e)ast (w)est\n? ";
#endif	
	cin >> c;
	if (c == 'n') chosenAction = &ActNorth;
	else if (c == 'e') chosenAction = &ActEast;
	else if (c == 's') chosenAction = &ActSouth;
	else if (c == 'w') chosenAction = &ActWest;
#ifdef RYANSTATE
	else if (c == '1') chosenAction = &ActNorthEast;
	else if (c == '2') chosenAction = &ActSouthEast;
	else if (c == '3') chosenAction = &ActSouthWest;
	else if (c == '4') chosenAction = &ActNorthWest;
#endif
#if TAXISTATE || FUELTAXISTATE || DELTATAXISTATE
	else if (c == 'u') chosenAction = &ActPickup;
	else if (c == 'd') chosenAction = &ActPutdown;
#endif
#ifdef FUELTAXISTATE
	else if (c == 'f') chosenAction = &ActFillup;
#endif
	else {
	  cout << "Unrecognized action; defaulting to ActNorth" << endl;
	  chosenAction = &ActNorth;
	}
	if (currentState.Terminated()) {
	  currentState.Init();
	}
	{
	  ActualParameterList apl;
	  currentState.Act(chosenAction, apl);
	}
      }
      else if (c == 'i')  {
	cout << "Invoking value iteration in this state" << endl;
	{
	  ActualParameterList apl;
	  list<ActivationRecord *> stack;  
	  float totalBellmanError = 0.0;
	  int explorationCount = 0;
	  int bellmanCount = 0;
	  Root->ValueIteration(currentState,
			       apl,
			       stack,
			       RandomGenerator,
			       params,
			       params.maxStepsPerTrial,
			       explorationCount,
			       totalBellmanError,
			       bellmanCount,
			       params.pollingCoolingStep);
	  cout << "explorationCount = " << explorationCount
	       << "totalBellmanError = " << totalBellmanError
	       << "bellmanCount = " << bellmanCount
	       << endl;
	}
      }
      else if (c == 'q') {
	saveWts(saveFileName, Root, currentState, iteration);
	break;
      }
    }
    iteration++;
  }
  return 0;
}
