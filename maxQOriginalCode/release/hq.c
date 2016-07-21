 
// -*- c++ -*-
//
// Hierarchical Q learning
//
//
#include "global.h"
#include "taxi-maxq.h"
#include "state.h"
#include "hq.h"
#include "parameter.h"
#include "hq-features.h"
#include "snet.h"
#include "hq-parameters.h"
#include <list.h>
#include <table.h>
#include <iostream.h>
#include <clrandom.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <minmax.h>


#ifndef WINDOWS98
//#include <std/cmath.h>
#include <stdio.h>
#endif


int EVALVERBOSE = 0;
int LEARNVERBOSE = 0;
int TRACE = 0;
int USERCONTROL = 0;
int MAXPATHDUMP = 10;
extern int LEARN;
int LOGMAXNODES = 1; 

float TIETOLERANCE = 0.01;
QNode * QNODETOWATCH = 0;
float MINREALTEMPERATURE = 0.1;
extern int DEBUG;
int COMPARE_WITH_HAND_CODED = 0;
int debugStateValid = 0;
MaxNode * debugNode;
STATE * debugState;

extern int inheritTerminationPredicates;
extern int TraceNode(MaxNode * node, STATE * state,
	      ActualParameterList * apl);
int nonGoalTerminations = 0;

ostream & operator << (ostream & str, ValueResult & vr)
{
  str << "(" << vr.totalValue << " " << vr.currentValue << " " <<
    vr.op->name << " ";
  FormalParameterIterator fpitr(*(vr.op->formalParameters));
  for (fpitr.init(); !fpitr; ++fpitr) {
    str << "(" << fpitr()->value()->name << " . "
	<< (*vr.actualParameters)[fpitr()->key()] << ") ";
  }
  str << ")";
  return str;
}

ostream & operator << (ostream & str, MaxQPath & path)
{
  MaxQPathIterator itr(path);
  str << "(";
  for(itr.init(); !itr; ++itr) {
    str << *(itr());
  }
  str << ")";
  return str;
}

ostream & operator << (ostream & str, list<MaxQPath *> & pathlist)
{
  listIterator<MaxQPath *> itr(pathlist);
  str << "(";
  for (itr.init(); !itr; ++itr) {
    str << *(itr()) << endl;
  }
  str << ")";
  return str;
}

void DisplayPathList(ostream & str, list<MaxQPath *> & pathlist)
{
  str << pathlist;
}

ostream & operator << (ostream & str, RewardRecord & ri)
{
  str << "(" << ri.reward << " ";
  if (ri.responsibleNodeList) {
    listIterator<QNode *> itr(*ri.responsibleNodeList);
    for (itr.init(); !itr; ++itr) {
      str << itr()->name << " ";
    }
  }
  else {
    str << "(null)";
  }
  str << ")";
  return str;
}

ostream & operator << (ostream & str, RewardInfo & ri)
{
  str << "(RewardInfo:  Total reward = " << ri.TotalReward <<
    " NSteps = " << ri.NSteps << " ";
  RewardInfoIterator itr(ri);
  for (itr.init(); !itr; ++itr) {
    if (itr()) {
      str << *(itr());
    }
    else {
      str << "(null reward record)";
    }
  }
  str << ")";
  return str;
}


ostream & operator << (ostream & str, vector<float> & vec)
{
  vectorIterator<float> itr(vec);
  for (itr.init(); !itr; ++itr) {
    str << itr() << " ";
  }
  return str;
}

istream & operator >> (istream & str, vector<float> & vec)
{
  vectorIterator<float> itr(vec);
  for (itr.init(); !itr; ++itr) {
    str >> itr();
  }
  return str;
}

void indentLine(ostream & str, int n)
{
  for (int i = 0; i < n; i++) str << " ";
}

void deletePath (MaxQPath * & path)
{
  if (path) {
    listIterator<ValueResult *> itr(*path);
    for (itr.init(); !itr; ++itr) {
      delete itr();
    }
    delete path;
    path = 0;
  }
}

void deletePathList (list<MaxQPath *> * & pathList)
{
  if (pathList) {
    listIterator<MaxQPath *> itr(*pathList);
    for (itr.init(); !itr; ++itr) {
      deletePath(itr());
    }
    delete pathList;
    pathList = 0;
  }
}


TableFunctionApproximator::TableFunctionApproximator(QNode * qn,
						     STATE & statearg):
  FunctionApproximator(qn),
  state(statearg)
{
  // allocate space for this table
  int ninputs = 0;
  int ncells = 1;
  if (qnode->features) {
    listIterator<ParameterName *> itr(*qnode->features);
    for (itr.init(); !itr; ++itr) {
      int fs = state.featureSize(itr());
      if (VERBOSE) cout << "feature:" << itr() << " size " << fs << endl;
      ncells *= fs;
      ninputs += 1;
    }
    if (VERBOSE) {
      cout << "QNode: " << qn->name << " table size = " << ncells << endl;
    }
  }
  table = new vector<float>(ncells, qnode->minValue);
  counts = new vector<int>(ncells, 0);
}

int TableFunctionApproximator::index(vector<float> * fv)
{
  int answer = 0;
  // feature values
  vectorIterator<float> fvItr(*fv);
  // feature names, sizes, etc.
  listIterator<ParameterName *> itr(*qnode->features);
  for (fvItr.init(), itr.init(); !fvItr; ++fvItr, ++itr) {
    answer *= state.featureSize(itr());  // wasted the first iteration
    answer += (int) fvItr();
  }
  return answer;
}  

float & TableFunctionApproximator::lookup(vector<float> * fv)
{
  int index = 0;
  // feature values
  vectorIterator<float> fvItr(*fv);
  // feature names, sizes, etc.
  listIterator<ParameterName *> itr(*qnode->features);
  for (fvItr.init(), itr.init(); !fvItr; ++fvItr, ++itr) {
    index *= state.featureSize(itr());  // wasted the first iteration
    index += (int) fvItr();
  }
  return (*table)[index];
}

float TableFunctionApproximator::Predict(vector<float> * fv)
{
  return lookup(fv);
}

float TableFunctionApproximator::Train(vector<float> * fv,
				       float delta,
				       float learningRate)
{
  // delta is the amount by which we want to change the current value
  int i = index(fv);
  float & cell = (*table)[i];
  cell += learningRate * delta;

  // (*table)[i] = (*table)[i] + (learningRate * delta);

  /*  note: we are now using counts for something else
  if (qnode == QNODETOWATCH && delta != 0) {
    int & count = (*counts)[i];
    count++;
    if (count > 1) {
      cerr << "Index " << i << " value = " << cell << " count = " <<
	count << " delta = " << delta << endl;
    }
  }
  */
  return 0.0;
}

void TableFunctionApproximator::Store(vector<float> * fv,
				       float newvalue)
{
  int i = index(fv);
  float & cell = (*table)[i];
  cell = newvalue;
}

int TableFunctionApproximator::Visits(vector<float> * fv)
{
  int i = index(fv);
  return (*counts)[i];
}

void TableFunctionApproximator::IncrementVisits(vector<float> * fv)
{
  int i = index(fv);
  int & count = (*counts)[i];
  count++;
}

void TableFunctionApproximator::print(ostream & str)
{
  str << *table;
}

void TableFunctionApproximator::read(istream & str)
{
  str >> *table;
}

int TableFunctionApproximator::countNonZeroWeights()
{
  // all initial values are set to end in .123, so this is a
  // signature that they have not been updated yet.
  int answer = 0;
  vectorIterator<float> itr(*table);
  for (itr.init(); !itr; ++itr) {
    float remainder = itr() - ((int) itr());
    if (fabs(fabs(remainder) - 0.123) > .001) answer++;
  }
  return answer;
}

void TableFunctionApproximator::clobberZeroWeights(float val)
{
  // all initial values are set to end in .123, so this is a
  // signature that they have not been updated yet.
  vectorIterator<float> itr(*table);
  for (itr.init(); !itr; ++itr) {
    float remainder = fabs(itr() - ((int) itr()));
    // if value is -5.123, it will be rounded to -5, so we get
    // fabs(-5.123 - (-5)) = fabs(-0.123) = 0.123
    // if value is 5.123, it will be rounded to 5, so we get
    // fabs(5.123 - 5) = fabs(0.123) = 0.123
    if (fabs(remainder - 0.123) <= .001)
      itr() = val;
  }
}

NNFunctionApproximator::NNFunctionApproximator(QNode * qn,
					       clrandom & rng):
  FunctionApproximator(qn)
{
  int ninputs = 0;
  if (qnode->features) ninputs += qnode->features->length();
  // we expand the given range by 20% beyond the published min and max
  // values
  float range = qnode->maxValue - qnode->minValue;
  float increment = 0.1 * range;
  net =  new ScaledSigmoidNetwork(ninputs, qnode->nHidden, 1,
				  qnode->minValue - increment,
				  qnode->maxValue + increment);
  net->Init(rng);
}

float NNFunctionApproximator::Predict(vector<float> * fv)
{
  net->SetInputs(*fv);
  net->ForwardPass();
  return net->ScaledSigmoidNetwork::GetOutput(0);
}

float NNFunctionApproximator::Train(vector<float> * fv, float delta,
				   float learningRate)
{
  net->ScaledSigmoidNetwork::SetInputs(*fv);
  net->ForwardPass();
  float outputValue = net->ScaledSigmoidNetwork::GetOutput(0);
  net->ScaledSigmoidNetwork::SetTarget(0, outputValue + delta);
  net->ComputeErrors();
  net->BackwardPass();
  net->Step(learningRate);
  return 0.0;
}

void NNFunctionApproximator::Store(vector<float> * fv, float newvalue)
{
  cerr << "Attempt to call Store for neural network";
  abort();
}


void NNFunctionApproximator::print(ostream & str)
{
  str << *net;
}

void NNFunctionApproximator::read(istream & str)
{
  str >> *net;
}

int NNFunctionApproximator::countNonZeroWeights()
{
  // for now, this just returns zero, since the number of non-zero
  // weights is not really meaningful for a neural network and it
  // would require a tolerance in any case.
  return 0;
}

void NNFunctionApproximator::clobberZeroWeights(float val)
{
  // for now, this just returns zero, since the number of non-zero
  // weights is not really meaningful for a neural network and it
  // would require a tolerance in any case.
  return;
}

int NNFunctionApproximator::Visits(vector<float> * fv)
{
  return 0;
}

void NNFunctionApproximator::IncrementVisits(vector<float> * fv)
{
  return;
}

void MaxNode::Debugger(STATE & state,
		       list<ActivationRecord *> & stack,
		       ActualParameterList & apl)
{
  while (1) {
    // loop asking user what to do
    cout << "Stack backtrace (b), evaluate state (e), greedy value (g),"
	 << "show options (o) for acting, toggle single-step (s),"
	 << "toggle eval verbose (w), "
         << "toggle verbose (v), print state (p), count nonzero weights (z),"
	 << "continue execution (c), abort (q): ";
    char c;
    cin >> c;

    if (c == 'z') {
      cout << "Non-zero weights: " << MaxRoot.countNonZeroWeightsRoot() <<
	endl;
    }
    else if (c == 'w') {
      EVALVERBOSE = 1 - EVALVERBOSE;
      cout << "EVALVERBOSE = " << EVALVERBOSE << endl;
    }
    else if (c == 's') {
      DEBUG = 1 - DEBUG;
      cout << "DEBUG = " << DEBUG << endl;
    }
    else if (c == 'b') {
      listIterator<ActivationRecord *> sitr(stack);
      int i = 0;
      cout << i << "\t" << this->name << apl << endl;
      for (i = 1, sitr.init(); !sitr; ++sitr, ++i) {
	cout << i << "\t" << sitr()->node->name << *(sitr()->actualParameters) << endl;
      }
    }
    else if (c == 'e') {
      ActualParameterList apl;
      list<MaxQPath *> * result = MaxRoot.evaluateList(state, apl, 0);
      cout << "Normal evaluation: " << (*result) << endl;
      deletePathList(result);
      result = MaxRoot.evaluateList(state, apl, 1);
      cout << "Tilde evaluation: " << (*result) << endl;
      deletePathList(result);
    }
    else if (c == 'o') {
      ActualParameterList apl;
      list<MaxQPath *> * result = MaxRoot.possibleActions(state, apl);
      cout << "Possible actions: " << (*result) << endl;
      deletePathList(result);
    }
    else if (c == 'g') {
      if (state.Terminated()) {
	state.Init();
	if (TRACE || VERBOSE) {
	  cout << "Initializing the state.\n";
	  state.Print(cout);
	}
      }
      ActualParameterList apl;
      MaxQPath * result = MaxRoot.evaluate(state, apl, 0, 0);
      if (result) {
	cout << "Greedy path " << (*result) << endl;
	deletePath(result);
      }
      else cout << "Null result" << endl;
    }
    else if (c == 'v') {
      VERBOSE = 1 - VERBOSE;
      LEARNVERBOSE = 1 - LEARNVERBOSE;
      cout << "VERBOSE = " << VERBOSE << " LEARNVERBOSE = " <<
	LEARNVERBOSE << endl;
    }
    else if (c == 'p') {
      state.Print(cout);
    }
    else if (c == 'c') {
      return;
    }
    else if (c == 'q') {
      abort();
    }
  }
}


unsigned int ident (const StateHashCode & i)
{
  return i;
}


void MaxNode::init(clrandom & rng, HQParameters & params, STATE & state)
{
  if (mark == 0) {
    mark = 1;
    if (!isPrimitive) {
      // allocate moving average
      BellmanError = new MovingAverage(params.ABEspan);

      // if we are doing prioritized sweeping, then allocate data
      // structures
      if (params.prioritizedSweeping) {
	// allocate feature vector
	int nfeatures = 0;
	if (features) nfeatures = features->length();
	fv = new vector<float>(nfeatures);

	// allocate hash table.  Right now, we will choose a number of
	// buckets that is prime.  Here are some primes that are near
	// to powers of 2:
	// 67, 113, 251, 509, 1021
	// We use the identify function as the hashing function.
	StateInfo * nullStateInfo = 0;
	transitionModel =
	  new table<StateHashCode, StateInfo *>((unsigned int) 113, &ident, nullStateInfo);

	// set the size of the priority queue.  
	PSQ.setSize(100);

      }

      // traverse all of our children and initialize them
      listIterator<QNode *> itr(*children);
      for (itr.init(); !itr; ++itr) {
	itr()->init(rng, this, params, state);
      }
    }
  }
}

void MaxNode::save(ostream & str)
{
  if (!isPrimitive && (mark == 0)) {
    mark = 1;
    str << temperature << " " << coolingRate << " "
	<< minTemperature << " " 
	<< learningRate << " "
	<< endl;
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      itr()->save(str);
    }
  }
}

void MaxNode::read(istream & str)
{
  if (!isPrimitive && (mark == 0)) {
    mark = 1;
    str >> temperature >> coolingRate >> minTemperature >> learningRate;
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      itr()->read(str);
    }
  }
}

MaxNode * MaxNode::findMaxNode(string & nodeName)
{
  if (nodeName == string(name)) return this;
  else if (!isPrimitive) {
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      MaxNode * result = itr()->child.findMaxNode(nodeName);
      if (result) return result;
    }
  }
  return 0;
}    

void MaxNode::setTemperature(float temp, float cool, float minTemp)
{
  temperature = temp;
  coolingRate = cool;
  minTemperature = minTemp;
  if (!isPrimitive) {
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      itr()->child.setTemperature(temp, cool, minTemp);
    }
  }
}

void MaxNode::saveTemperature()
{
  if (!mark) {
    mark = 1;
    savedTemperature = temperature;
    savedCoolingRate = coolingRate;
    savedMinTemperature = minTemperature;
    if (!isPrimitive) {
      listIterator<QNode *> itr(*children);
      for (itr.init(); !itr; ++itr) {
	itr()->child.saveTemperature();
      }
    }
  }
}

void MaxNode::restoreTemperature()
{
  if (!mark) {
    mark = 1;
    temperature = savedTemperature;
    coolingRate = savedCoolingRate;
    minTemperature = savedMinTemperature;
    if (!isPrimitive) {
      listIterator<QNode *> itr(*children);
      for (itr.init(); !itr; ++itr) {
	itr()->child.restoreTemperature();
      }
    }
  }
}

void MaxNode::printTemperatures()
{
  if (!mark) {
    mark = 1;
    cout << name << "\t" << temperature << "\t" << policyProbability
	 << "\t" << requirePolicy << endl;
    if (!isPrimitive) {
      listIterator<QNode *> itr(*children);
      for (itr.init(); !itr; ++itr) {
	itr()->child.printTemperatures();
      }
    }
  }
}

void MaxNode::setLearningRate(float lrate)
{
  learningRate = lrate;
  if (!isPrimitive) {
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      itr()->child.setLearningRate(lrate);
    }
  }
}


int MaxNode::countNonZeroWeightsRoot()
{
  clearMarks();
  return countNonZeroWeights();
}

void MaxNode::clobberZeroWeightsRoot(float val)
{
  clearMarks();
  clobberZeroWeights(val);
}

int MaxNode::countNonZeroWeights()
{
  int answer = 0;
  if (!mark) {
    mark = 1;
    if (!isPrimitive) {
      listIterator<QNode *> itr(*children);
      for (itr.init(); !itr; ++itr) {
	answer += itr()->countNonZeroWeights();
      }
    }
  }
  return answer;
}

void MaxNode::clobberZeroWeights(float val)
{
  if (!mark) {
    mark = 1;
    if (!isPrimitive) {
      listIterator<QNode *> itr(*children);
      for (itr.init(); !itr; ++itr) {
	itr()->clobberZeroWeights(val);
      }
    }
  }
}

void MaxNode::clearMarks()
{
  mark = 0;
  if (!isPrimitive) {
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      itr()->clearMarks();
    }
  }
}


void MaxNode::setPolicyProbability(float pp)
{
  policyProbability = pp;
  if (!isPrimitive) {
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      itr()->child.setPolicyProbability(pp);
    }
  }
}

MaxQPath * MaxNode::evaluate(STATE & state,
			     ActualParameterList & apl,
			     int useTilde,
			     int indent)

{
  // if node is terminated, return 0.
  // if primitive, return empty list
  // else return the greedy path according to the policy. 

  if (EVALVERBOSE) {
    indentLine(cout, indent);
    cout << "Evaluating " << name << apl << " useTilde " << useTilde << endl;
  }

  if (isPrimitive) {
    // return empty list
    return new MaxQPath;
  }
  else if (terminationPredicate->call(state, apl)) {
    if (EVALVERBOSE) {
      indentLine(cout, indent);
      cout << name << apl
	   << " termination predicate is true"
	   << endl;
    }
    // termination predicate is true, so no actions can be performed
    // in this state.  We return NULL. 
    return 0;
  }
  // evaluate each of our children.  Choose the best one according to
  // Ctilde. And return the corresponding C() cost.
  else {
    listIterator<QNode *> itr(*children);
    float maxChildValue = -1E33;
    MaxQPath * answer = 0;

    for (itr.init(); !itr; ++itr) {
      // a Q child can return more than one possible path if it
      // has free parameters.
      list<MaxQPath *> * childAnswer;
      // we ask the child to use its costToGoTilde table.
      childAnswer = itr()->evaluate(state, apl, 1, indent + 2);
      assert(childAnswer);

      listIterator<MaxQPath *> pathItr(*childAnswer);
      for (pathItr.init(); !pathItr; ++pathItr) {
	if (pathItr()->firstElement()->totalValue > maxChildValue) {
	  maxChildValue = pathItr()->firstElement()->totalValue;
	  // remember to release previous best answers!
	  if (answer) deletePath(answer);
	  answer = pathItr();
	  pathItr() = 0;	// kill the old pointer
	}
      }
      // now release all elements in the child answer except the
      // best one.
      for (pathItr.init(); !pathItr; ++pathItr) {
	if (pathItr()) deletePath(pathItr());
      }
      // we don't call deletePathList here, because we have manually
      // deleted all of the elements except the one we wanted. 
      delete childAnswer;
      childAnswer = 0;
    }

    if (EVALVERBOSE) {
      indentLine(cout, indent);
      cout << name << " best child is ";
      if (answer) cout << *answer;
      else cout << "(none)";
      cout << endl;
    }

    if (answer) {
      if (!useTilde) {
	// replace the tilde value with the untilde value. 
	answer->firstElement()->totalValue = unTilde(answer);
      }
    }
    // note:  if answer is 0, then all of our children were
    // terminated, so we are terminated too, and we return 0. 
    return answer;
  }
}

list<MaxQPath *> * MaxNode::possibleActions(STATE & state,
					    ActualParameterList & apl)

{
  // compute a list of all possible child actions and their tilde values.
  // this is then used for boltzmann exploration, etc.
  if (EVALVERBOSE) 
    cout << "Evaluating possible actions for " << name << apl << endl;

  list<MaxQPath *> * answer = new list<MaxQPath *>;

  if (isPrimitive) {
    cerr << "Attempt to evaluate possible actions of primitive Max
node.  Should not happen!" << endl;
    abort();
    // return empty list
    return answer;
  }
  else if (terminationPredicate->call(state, apl)) {
    // termination predicate is true, so no actions can be performed
    // in this state.  We return an empty list.
    return answer;
  }
  // evaluate each of our children and collect them into a list.
  // A Q child may return more than one action if it had free
  // parameters (because it generates all legal values of those free
  // parameters). 
  else {
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      list<MaxQPath *> * childAnswer;
      childAnswer = itr()->evaluate(state, apl, 1, 0);
      answer->nconc(*childAnswer);
      delete childAnswer;	// we do not deletePathList, because
                                // we have copied the list elements onto answer.
      childAnswer = 0;
    }
    if (EVALVERBOSE) {
      cout << "Possible actions are:\n" << *answer << endl;
    }
    return answer;
  }
}


list<MaxQPath *> * MaxNode::evaluateList(STATE & state,
				     ActualParameterList & apl,
				     int useTilde)
{
  // compute all paths through the MAXQ tree starting in this state
  // and evaluate each of them.  If useTilde is true, our immediate
  // children use their costToGoTilde values, but below that level,
  // non-tilde values are used.

  if (EVALVERBOSE) 
    cout << "Evaluating " << name << apl << " useTilde = " << useTilde << endl;

  list<MaxQPath *> * answer = new list<MaxQPath *>;

  if (isPrimitive) {
    answer->add(new MaxQPath);
    return answer;
  }
  // evaluate each of our children and construct a list of the
  // results. 
  else if (terminationPredicate->call(state, apl)) {
    if (EVALVERBOSE) 
      cout << "Termination predicate true for "
	   << name << apl << endl;
    // termination predicate is true, so no actions can be performed
    // in this state.  We return an empty list.
    return answer;
  }
  else {
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      list<MaxQPath *> * childAnswer;
      childAnswer = itr()->evaluateList(state, apl, useTilde);
      answer->nconc(*childAnswer);
      delete childAnswer;	// we do not deletePathList, because
                                // we have copied the list elements onto answer.
      childAnswer = 0;
    }
    return answer;
  }
}
 

RewardInfo* MaxNode::execute(STATE & state,
			     ActualParameterList & apl,
			     list<ActivationRecord *> & stack,
			     list<STATE *> & upwardStates,
			     clrandom & rng,
			     HQParameters & params,
			     int & stepQuota,
			     int & explorationCount,
			     float & totalBellmanError,
			     int & childWasInterrupted)
{
  RewardInfo * answer = 0;
  if (isPrimitive){
    answer = executePrimitive(state, apl, params,
			      upwardStates, stepQuota, rng);
    // primitives can never be interrupted
    childWasInterrupted = 0;
  }
  else {
    if (params.prioritizedSweeping) {
      answer =  PSexecuteComposite(state, apl,
				   stack, upwardStates,
				   rng,
				   params, stepQuota,
				   explorationCount,
				   totalBellmanError,
				   childWasInterrupted);
    }
    else {
      answer =  executeComposite(state, apl,
				 stack, upwardStates,
				 rng,
				 params, stepQuota,
				 explorationCount,
				 totalBellmanError,
				 childWasInterrupted);
    }
  }
  return answer;
}

RewardInfo * MaxNode::executePrimitive(STATE & state,
				       ActualParameterList & apl,
				       HQParameters & params,
				       list<STATE *> & upwardStates,
				       int & stepQuota,
				       clrandom & rng)
				       
{
  // This code is a bit braindamaged.  I should re-design it to permit
  // multiple "errors" from the simulator.  For example, when there is
  // a non-trivial error, -1 should be assigned to the primitive
  // action and -10 (or whatever) assigned to the non-terminal. 
  // Right now, I'm assuming that every reward will go to exactly one
  // node, as indicated by RewardDecomposition. 

  if (VERBOSE) {
    cout << "Executing Primitive " << name << endl;
  }

  // create a list element for this state.  We copy the state at this
  // point! (ouch)
  upwardStates.add(state.Duplicate());

  float oldReward = state.cumulativeReward;
  state.Act(action, apl);

  // print the result state
  if (VERBOSE) {
    state.Print(cout);
  }

  // we remove the primitive action cost here and handle it ourselves,
  // below.  This is instead of the multiple error idea mentioned above.
  float newReward = state.cumulativeReward + 1.0;
  float reward = (newReward - oldReward);
  list<QNode *> * responsibleNodeList = 0;

  if (reward != 0) {
    // examine the current error message.
    listIterator<RewardResponsibility *> rritr(*RewardDecomposition);
    for (rritr.init(); !rritr; ++rritr) {
      if (rritr()->currentError == state.currentError) {
	responsibleNodeList = rritr()->responsibleNodeList;
	break;
      }
    }
    if (responsibleNodeList == 0) {
      cerr << "Unknown error type (" << state.currentError <<
	") in MaxNode::execute" << endl;
      abort();
    }
  }
  // now construct the answer to return up the tree
  RewardInfo * answer = new RewardInfo;
  // We have taken 1 step
  answer->NSteps = 1;

  // every primitive action costs -1.
  answer->add(new RewardRecord(-1.0, primitiveQNodes));

  // if there is another reward, add it. 
  if (responsibleNodeList) {
    answer->add(new RewardRecord(reward, responsibleNodeList));
  }
  
  // This is where interrupts would be caused.

  // If this step exceeds our quota of steps, cause an interrupt to the top
  // level. 
  stepQuota--;
  if (stepQuota <= 0) {
    // set the interrupt flag and cause a painless penalty at the top
    // level. 
    answer->add(new RewardRecord(0.0, topLevelQNodes));
  }

  // decide whether to generate a random interrupt (used to robustify
  // the learned policy)
  if (params.interruptProbability > 0 &&
      rng.between(0.0, 1.0) < params.interruptProbability) {
    cout << "Random Interrupt\n";
    // set the interrupt flag and cause a painless penalty at the top
    // level. 
    answer->add(new RewardRecord(0.0, topLevelQNodes));
  }

  return answer;
}


// This function is used to implement the hand-coded policies.  The
// hand-coded policy function returns a Q node and instantiates free
// child parameters in a cpl.  evaluate() returns a list of MaxQPath's.
// We must find one of those MaxQPath's that matches the Q node and
// parameters chosen by the Policy function.
MaxQPath * MaxNode::matchingPath(STATE & state,
				 list<MaxQPath *> * paths,
				 ActualParameterList & apl,
				 clrandom & rng,
				 list<ActivationRecord *> & stack,
				 HQParameters & params)
{

  ActualParameterList cpl;
  QNode * policyChild = (*policy)(state, apl, cpl);
  if (VERBOSE) cout << "policyChild = " << policyChild->name << cpl <<
		 endl;

  // now find a path in paths that matches the policy child and the
  // cpl.
  listIterator<MaxQPath *> itr(*paths);
  for (itr.init(); !itr; ++itr) {
    if (itr()->firstElement()->op == policyChild) {
      // Q node matches, now check for actual parameters
      ActualParameterList * papl = itr()->firstElement()->actualParameters;
      // check if every parameter in cpl has the same value in papl.
      ActualParameterIterator cplItr(cpl);
      int allMatch = 1;
      for (cplItr.init(); !cplItr; ++cplItr) {
	if ((*papl)[cplItr()->key()] != cplItr()->value()) {
	  allMatch = 0;
	  break;
	}
      }
      if (allMatch) {
	return itr();
      }
    }
  }
  cout << "No matching MaxQPath, exploring instead\n";
  Debugger(state, stack, apl);
  int dummy; // don't count this exploration
  return boltzmannExploration(paths, state, rng, dummy, params, 0);
}
	
MaxQPath * MaxNode::boltzmannExploration(list<MaxQPath *> * paths,
					 STATE & state,
					 clrandom & rng,
					 int & explorationCount,
					 HQParameters & params,
					 int Trace)
{
  if (!paths || paths->isEmpty())  return 0;
  else if (USERCONTROL) {
    cout << *paths << endl;
    int i;
    do {
      cout << "Select path (indexed from 0): ";
      cin >> i;
    }
    while (i >= paths->length());
    return (*paths)[i];
  }
  else if (paths->length() == 1) {
    if (VERBOSE) cout << "Boltzmann had only one choice" << endl;
    return paths->firstElement();
  }
  else {
    if (VERBOSE)
      cout << name << " nondeterministic!" << endl;
  }

  int DoGreedy = 0;

  if (params.epsilonGreedy == 1) {
    // epsilon greedy execution
    float rn = rng.between(0.0,1.0);
    // cout << name << " rn = " << rn << " epsilon = " << epsilon << endl;
    if ( rn > temperature) {
      if (Trace) cout << "Epsilon greedy: greedy" << endl;
      DoGreedy = 1;
    }
    else {
      if (Trace) cout << "Epsilon greedy: random" << endl;
      // choose completely randomly
      int iwin = rng.between(0, paths->length() - 1);
      explorationCount++;
      return (*paths)[iwin];
    }
  }
  else if (params.epsilonGreedy == 2) {
    if (temperature <= 0) {
      DoGreedy = 1;
    }
    else {
      // counter based exploration
      
      // We perform balanced wandering until temperature
      // executions of each operator have been performed.
      // We use temperature so that each node can have its own
      // exploration level.
      
      // Find the operator with the lowest number of visits
      listIterator<MaxQPath *> itr(*paths);
      int firstTime = 1;
      int bestCounts = 0;
      MaxQPath * bestPath = 0;
      for (itr.init(); !itr; ++itr) {
	QNode * child = itr()->firstElement()->op;
	ActualParameterList * apl = itr()->firstElement()->actualParameters;
	computeFeatureVector(state, *apl, child->features, child->fv);
	int count = child->costToGo->Visits(child->fv);
	
	if (Trace) {
	  cout << "Counter Exploration:  "
	       << child->name << *apl
	       << " visits = " << count << endl;
	}
	// if we have already visited this node "temperature"
	// times, then we can quit.  In fact, this will cause us to
	// visit every action temperature times and to perform
	// the first action one more time, but it is an easy termination
	// condition to evaluate.  
	if (count > temperature) {
	  DoGreedy = 1;
	  if (Trace) cout << "Counter Exploration: greedy" << endl;
	  break;
	}
	
	if (firstTime) {
	  // first time through the loop, save the first element
	  bestCounts = count;
	  bestPath = itr();
	  firstTime = 0;
	}
	else {
	  // as soon as we find a count that is less than the count of
	  // the first element in the list, we can stop
	  if (count < bestCounts) {
	    bestCounts = count;
	    bestPath = itr();
	    break;
	  }
	}
      }
      if (!DoGreedy) {
	// we increment the MaxNode variable remainingExplorations to
	// count, for the selected actions, how many explorations
	// remain.  This gives us a global measure of the amount of
	// exploration remaining.
	remainingExplorations += ((int) temperature) - bestCounts;
	return bestPath;
      }
    }
  }
      
  // compute greedy action.  Then decide whether to do it.
  listIterator<MaxQPath *> itr(*paths);
  MaxQPath * bestPath = 0;
  list<MaxQPath *> winners;
  //    float bestValue = itr()->firstElement()->totalValue;
  float bestValue = -1.0E38;
  for (itr.init(); !itr; ++itr) {
    float val = itr()->firstElement()->totalValue;
    if (val > bestValue - TIETOLERANCE) {
      if (val > bestValue) bestValue = val;
      winners.add(itr());
    }
  }
  if (winners.length() == 0) {
    cout << "No winning paths: " << *paths << endl;
    assert(winners.length() > 0);
  }
  
  // winners will have some junk on it, because it has every guy
  // that was within TIETOLERANCE of the best at some point.  We go through
  // and delete such losers.
  listIterator<MaxQPath *> winnerItr(winners);
  for (winnerItr.init(); !winnerItr; ++winnerItr) {
    float val = winnerItr()->firstElement()->totalValue;
    if (val < bestValue - TIETOLERANCE) winnerItr.removeCurrent();
  }
  assert(winners.length() > 0);
  
  if (temperature < MINREALTEMPERATURE || DoGreedy) {
#if 1
    // take a greedy action, but break ties randomly.
    int iwin = rng.between(0, winners.length() - 1);
    return winners[iwin];
#else
    // always return the first action so that we have a deterministic
    // policy. 
    return winners[0];
#endif    
  }
  else {
    // Compute the softmax
    // to avoid floating overflow, we first compute the largest value
    // and subtract it from all of the others when we take the
    // exponentials.  This gives the same probabilities.
    if (Trace) cout << "Boltzmann: randomize" << endl;
    
    double normalizer = 0.0;
    listIterator<MaxQPath *> itr(*paths);
    MaxQPath * lastItem = 0;
    double bestValue = -1.0E38;
    for (itr.init(); !itr; ++itr) {
      double val = itr()->firstElement()->totalValue;
      if (val > bestValue) bestValue = val;
    }
    for (itr.init(); !itr; ++itr) {
      normalizer += exp((itr()->firstElement()->totalValue - bestValue) / 
			temperature);
    }
    double randomNumber = rng.between(0.0, normalizer);
    normalizer = 0.0;
    int ipos = 0;
    // by summing the normalizer in exactly the same order, we should
    // make the same roundoff errors, and avoid problems.
    for (itr.init(); !itr; ++itr, ++ipos) {
      normalizer += exp((itr()->firstElement()->totalValue - bestValue) / 
			temperature);
      lastItem = itr();
      if (normalizer > randomNumber) {
	if (VERBOSE) cout << "Boltzmann position " << ipos << endl;
	break;
      }
    }
    // just in case we had some roundoff problem.
    /* cerr << "Boltzmann ran off the end.  randomNumber = " <<
       randomNumber << " normalizer = " << normalizer << endl;
       */

    if (fabs(lastItem->firstElement()->totalValue - bestValue) >
	     TIETOLERANCE) {
      explorationCount++;
    }
    return lastItem;
  }
}

float unTilde(MaxQPath * tildePath)
{
  // the first element in tildePath used the tilde value tables/nets
  // to compute its contribution.  We want to undo this, so we
  // subtract off the currentValue (the one computed by the tilde
  // function approximator) and replace it with the value computed by
  // the normal function approximator.
  
  ValueResult * head = tildePath->firstElement();
  float unTildeValue = head->op->costToGo->Predict(head->features);
  return (head->totalValue - head->currentValue) + unTildeValue;
}

  
float unTildeValue(list<MaxQPath *> * tildePaths)
{
  if (tildePaths) {
    listIterator<MaxQPath *> itr(*tildePaths);
    float maxValue = -1.0e38;
    for (itr.init(); !itr; ++itr) {
      float val = unTilde(itr());
      if (val > maxValue) maxValue = val;
    }
    return maxValue;
  }
  else {
    cerr << "unTildeValue called with null tildePaths";
    abort();
  }
}


// This is the most important function in the whole program.
RewardInfo* MaxNode::executeComposite(STATE & state,
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

  list<STATE *> downwardStates;   // The states returned by our child node.

  float startingTotalBellmanError = totalBellmanError;
  int startingExplorationCount = explorationCount;
  int startingRemainingExplorations = remainingExplorations;

  int Trace = TraceNode(this, &state, &apl);

  // We collect a sequence of situation-action pairs until we reach a
  // terminating condition.  At each step, we only use the most recent
  // two elements on this last (actually, only the last element), but
  // we keep the others for debugging and then release them at the end.
  //
  // Formulas:
  //
  // Let p be the parent MAX node.  a is an action.  s is a state.
  // Q_p(s,a) = C_p(s,a) + Max_a(s)
  // Qtilde_p(s,a) = Ctilde_p(s,a) + Max_a(s)
  // Max_a(s) = max_b C_a(s,b)
  //
  // The greedy policy is defined by
  //  pi_p(s) = argmax_a Qtilde_p(s,a)
  //
  // The learning rules are
  // Ctilde_p(s,a) += alpha * (gamma * [R(s'|s,a) + max_a' Qtilde_p(s',a')] - Ctilde_p(s,a))
  //   if s' is a terminal state of p, then Qtilde_p(s',a') is -100. 
  //   
  // C_p(s,a) += alpha * (gamma * [R(s'|s,a) + Q_p(s',a')] - C_p(s,a))
  //   where a' is the next action taken in the current policy.
  //   Q_p(s',a') = 0 if s' is a terminal state of p. 
  //
  // Technically, if a took k time steps, then replace gamma with
  // gamma^k in the above formulas.  However, the code with
  // discounting has not been tested. 
  //

  int NSteps = 0;
  list<SAP *> saps;		// sequence of situation-action pairs
  int iterations = 0;
  float TotalReward = 0.0;	// cumulative discounted reward during
				// this operator.
  float gamma = 1.0;            // effective discount factor


  MaxQPath * chosenPath;

  if (VERBOSE || DEBUG || Trace)
    cout << "Executing Composite " << name << apl << endl;

  if (Trace) state.Print(cout);

  if (DEBUG) Debugger(state, stack, apl);

  // Debugger(state, stack, apl);

  // outer loop: choose actions until interrupt or termination 
  do {
    
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
      // We have been asked to do something impossible.  The operator
      // becomes a no-op, and we return a null reward info record
      return new RewardInfo;
    }
    
    // choose an action
    
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
    
    // extract child and sibling items for sap
    // and then reclaim the tildePaths
    ValueResult * head = chosenPath->firstElement();
    QNode * chosenChild = head->op;     //  a' in the update formulas
    float currentStateTildeValue = value(tildePaths);   // max_a' Qtilde_p(s',a')
    // compute normal (non-tilde) value of the chosen child
    float chosenChildValue = unTilde(chosenPath);       // Q_p(s',a')
    // make a copy of the state before we act.
    
    ActualParameterList * chosenAPL = head->actualParameters;
    head->actualParameters = 0;     // prevent premature GC
    
    deletePathList(tildePaths);
    chosenPath = 0;
    
    // we can now perform the learning step for the previous state,
    // because we have currentStateTildeValue and chosenChildValue,
    // which we need in the update formulas.
    
    if (!saps.isEmpty()) {
      if (LEARNVERBOSE || Trace) {
	cout << "Learning steps for " << name << endl;
      }
      // The first element on saps describes the previous action
      // (prior to our selecting chosenChild.
      // The elements on downwardStates list all of the states in which
      // primitive actions were performed (in reverse order) in order
      // to execute that previous action.
      // The presumption is that in each state of downwardStates, if we
      // had executed the same (composite) action, then it would have
      // resulted in the same final state that we are currently
      // occupying. 
      
      SAP * lastSap = saps[0];
      
      float discount = 1.0;
      
      // iterate over the states, most recent first
      listIterator<STATE *> downItr(downwardStates);
      
      STATE * lastState = 0;
      // the following code defeats all states updating
      for (downItr.init(); !downItr; ++downItr) {
#if 0
	lastState = downItr();
      }

      for (downItr.init(); !downItr; ++downItr) {
	if (downItr() != lastState) continue;
#endif

	// discount this by one step.
	discount *= params.discountFactor;

	float childValue;
	float childTildeValue;
	
	// compute the current value of this potential start state
	// NOTE: also computes the feature vector for lastSap->child,
	// which we use below.
	lastSap->child->computeValues(*(downItr()),
				      *(lastSap->apl),
				      childValue,
				      childTildeValue);
	
	// compute the deltas.
	float tildeTDError = (lastSap->rewardTilde + currentStateTildeValue) 
	  - childTildeValue;
	float sarsaTDError = (lastSap->reward + chosenChildValue)
	  - childValue;
	
	if (LEARNVERBOSE || Trace) {
	  cout << name << " tilde: "
	       << " result=" << currentStateTildeValue 
	       << " reward=" << lastSap->rewardTilde
	       << " start="  << childTildeValue
	       << " BellmanErr="  << tildeTDError << endl;
	  cout << "      sarsa " 
	       << " result=" << chosenChildValue 
	       << " reward=" << lastSap->reward 
	       << " start=" << childValue 
	       << " BellmanErr=" << sarsaTDError << endl;
	}
	
	BellmanError->Add(fabs(tildeTDError));
	totalBellmanError += fabs(tildeTDError);
	
	// these are unused.  The errors printed above are the bellman errors!
	float bellmanError = 0;
	float bellmanErrorTilde = 0;
	
	if (lastSap->child->OkToUpdate(params)) {
	  // do the learning!
	  // the feature vector of child was set by computeValues()
	  // above. 
	  float lrate = learningRate;
	  if (params.epsilonGreedy == 2) {
	    // counter-based learning rates.  lrate = 1/nvisits
	    // Because of all states updating, we may never have
	    // visited this state before, in which case, we set the
	    // learning rate to 0.
	    int visits = lastSap->child->costToGo->Visits(lastSap->child->fv);
	    if (visits > 0) {
	      lrate = 1.0 / visits;
	      if (lrate < params.learningRate) lrate =
						 params.learningRate;
	    }
	    else {
	      lrate = 1.0;
	      // all states updating.  Increment this visit
	      lastSap->child->costToGo->IncrementVisits(lastSap->child->fv);
	    }
	    if (LEARNVERBOSE) cout << "lrate = " << lrate << endl;
	    // we set this for commentary
	    learningRate = lrate;
	  }

	  lastSap->child->update(*(lastSap->child->fv),
				 sarsaTDError,
				 tildeTDError,
				 lrate,
				 bellmanError,
				 bellmanErrorTilde);
	  if (Trace) cout << "Updated" << endl;

	} // oktoupdate
      } // all states updating loop
    } // saps is nonempty
    
    // Now prepend these states onto the list we will return to our
    // parent.  This also empties downwardStates, thereby preparing it
    // to receive the next set of values.
    upwardStates.prepend(downwardStates);
    
    // execute the chosen action (finally!)
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
    float rewardTilde = 0.0;
    
    // update the discount factor
    gamma *= pow(params.discountFactor, ri->NSteps);
    // accumulate the discounted reward
    // we do this because our predecessor steps (at this level, i.e.,
    // our uncles) will need to predict the cumulative discounted
    // expected reward, so we need to know the "total value" of the
    // resulting state and the "total reward" obtained by executing
    // the chosenChild.  
    TotalReward += gamma * ri->TotalReward;
    
    // accumulate the total number of steps needed to execute the
    // current Max node
    NSteps += ri->NSteps;
    int childNSteps = ri->NSteps;
    
    // Decide whether we need to do a final update and return to our
    // parent.  There are four ways we can terminate:
    //
    // interrupt Termination (child action interrupted)
    //   If our child action did not complete, then we do NOT do a
    //   final update.  We just return.  

    // interrupt Termination (child action completed)
    //   This occurs when we are NOT terminating but either there are
    //   unhandled interrupts, the step quota is exhausted, or one of
    //   our ancestors terminates.  In this case, we do a one-step
    //   lookahead to get a resultState value, and then perform the Q
    //   and sarsa backups.
    
    // goal Termination
    //   This occurs when we have terminated in a goal state.  The
    //   result state value is zero.  We do Q and sarsa backups.

    // nonGoal termination
    //   This occurs when we have terminated, but not in a goal
    //   state.  The result state value for sarsa is zero, but for the
    //   Q backups, we must take the nonGoalTerminationPenalty. 

    goalPredicateTrue = 0;
    terminationPredicateTrue = 0;
    weMustReturn = 0;

    // If the chosenChild is responsible, then add
    // in the reward
    RewardInfoIterator ritr(*ri);
    for (ritr.init(); !ritr; ++ritr) {
      if (ritr()->responsibleNodeList->includes(chosenChild)) {
	// take responsibility; move a reward from pending to total
	TotalReward += gamma * ritr()->reward;
	reward += ritr()->reward;
	rewardTilde += ritr()->reward;
	// we delete this item from the list
	delete ritr();
	ritr.removeCurrent();
	// note that we may be responsible for multiple items
      }
    }
    
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
    
    // implemented nongoal termination: adjust terminal reward
    if (terminationPredicateTrue && !goalPredicateTrue) {
      if (VERBOSE) cout << name << " non-goal termination." << endl;
      rewardTilde += nonGoalTerminationPenalty;
      nonGoalTerminations++;
    }

    saps.add(new SAP(chosenChild,  // operator
		     chosenAPL,    // parameter values
		     reward,       // reward
		     rewardTilde,  // rewardTilde
		     childNSteps));   // number of steps
    
    chosenAPL = 0;		// ~sap will reclaim it

    // if we are terminating and our child was not interrupted, then
    // we need to do final learning updates
    if (weMustReturn) {
      if (!childWasInterrupted) {
	// we cool epsilonGreedy with each goal termination
	if (goalPredicateTrue && params.epsilonGreedy == 1) {
	  temperature -= coolingRate;
	  if (temperature < minTemperature) temperature = minTemperature;
	}
	
	// The first element on saps describes the state prior to our
	// final action
	SAP * lastSap = saps[0];
	
	// Now we must determine the resultStateValue in order to do our
	// final backups
	
	float resultStateValue = 0.0;
	float resultStateTildeValue = 0.0;
	if (!terminationPredicateTrue) {
	  // we are returning in a non-terminal state.  This means we
	  // must do a lookahead to compute the resultStateValue
	  // (In a terminal state, the result state is always worth 0.
	  // The termination penalty is part of the last reward
	  // transition.)
	  
	  // We face a quandary about what to do for the sarsa backup.
	  // Some possible options:  
	  // (a) We could take the unTildeValue of the greedy action
	  // according to the tilde values. (*chosen*)
	  // (b) We could just not do a sarsa backup.
	  
	  MaxQPath * bestTildePath = evaluate(state, apl, 1, 0);
	  // evaluate could return NULL if we or our children are
	  // terminated, but this should never be true.
	  assert(bestTildePath);
	  resultStateTildeValue = bestTildePath->firstElement()->totalValue;
	  resultStateValue = unTilde(bestTildePath);
	  deletePath(bestTildePath); 
	}
	
	// Do the backups!
	float discount = 1.0;
	listIterator<STATE *> downItr(downwardStates);
	
	STATE * lastState = 0;
	// the following code defeats all states updating
	for (downItr.init(); !downItr; ++downItr) {
#if 0
	  lastState = downItr();
	}

	for (downItr.init(); !downItr; ++downItr) {
	  if (downItr() != lastState) continue;
#endif
	  // discount this by one step.
	  discount *= params.discountFactor;
	  
	  float childValue;
	  float childTildeValue;
	  
	  // compute the current value of this potential start state
	  lastSap->child->computeValues(*(downItr()),
					*(lastSap->apl),
					childValue,
					childTildeValue);
	  
	  // compute the deltas.
	  float tildeTDError = (lastSap->rewardTilde + resultStateTildeValue)
	    - childTildeValue;
	  float sarsaTDError = (lastSap->reward + resultStateValue)
	    - childValue;
	  
	  BellmanError->Add(fabs(tildeTDError));
	  totalBellmanError += fabs(tildeTDError);
	  
	  if (LEARNVERBOSE || Trace) {
	    cout << name << " tilde: "
		 << " result=" << resultStateTildeValue 
		 << " reward=" << lastSap->rewardTilde
		 << " start="  << childTildeValue
		 << " BellmanErr="  << tildeTDError << endl;
	    cout << "      sarsa " 
		 << " result=" << resultStateValue 
		 << " reward=" << lastSap->reward 
		 << " start=" << childValue 
		 << " BellmanErr=" << sarsaTDError << endl;
	  }
	  
	  
	  // these are unused
	  float bellmanError = 0;
	  float bellmanErrorTilde = 0;
	  
	  if (lastSap->child->OkToUpdate(params)) {
	    // do the learning!
	    // The feature vector of child was computed by computeValues()
	    // above. 
	    float lrate = learningRate;
	    if (params.epsilonGreedy == 2) {
	      // counter-based learning rates.  lrate = 1/nvisits
	      // Because of all states updating, we may never have
	      // visited this state before, in which case, we set the
	      // learning rate to 0.
	      int visits = lastSap->child->costToGo->Visits(lastSap->child->fv);
	      if (visits > 0) {
		lrate = 1.0 / visits;
		if (lrate < params.learningRate) lrate = params.learningRate;
	      }
	      else {
		lrate = 1.0;
		// all states updating.  Simulate a visit.
		lastSap->child->costToGo->IncrementVisits(lastSap->child->fv);
	      }

	      if (LEARNVERBOSE) cout << "lrate = " << lrate << endl;
	      // we set this for commentary
	      learningRate = lrate;
	    }

	    lastSap->child->update(*(lastSap->child->fv),
				   sarsaTDError,
				   tildeTDError,
				   lrate,
				   bellmanError,
				   bellmanErrorTilde);

	  if (Trace) cout << "Updated" << endl;

	  }
	} // all states updating loop
      } // we must do terminal updates

      // pass the downward states up to the parent if we are
      // returning, otherwise parent won't see this last set of
      // states. 
      upwardStates.prepend(downwardStates);
    } // we must return

  } while (! weMustReturn);

  extern int itrial;
  float deltaBellmanError = totalBellmanError - startingTotalBellmanError;
  int nsteps = saps.length();
  float deltaBEPerMacroStep = (deltaBellmanError / nsteps);
  // print convergence information
  if (logOutput && LOGMAXNODES || Trace)
    cout << itrial << " "
	 << deltaBellmanError << " "
	 << explorationCount - startingExplorationCount << " "
         << remainingExplorations - startingRemainingExplorations << " " 
	 << temperature << "  "
	 << TotalReward << "  "
	 << nsteps << " "
	 << goalPredicateTrue << " %% " << name << endl;

  // If we successfully completed the call with little or no tilde
  // Bellman Error, then we cool the temperature one step.
  // In noisy domains, this turns out to not be useful, however, and
  // we usually set deltaBellmanThreshold to be very big.
  if (terminationPredicateTrue &&
      goalPredicateTrue &&
      deltaBEPerMacroStep < params.deltaBellmanThreshold &&
      params.epsilonGreedy == 0) {
    temperature *= coolingRate;
    if (temperature < MINREALTEMPERATURE) temperature = 0;
    if (temperature < minTemperature) temperature = minTemperature;
  }
    
  // reclaim the SAPs
  listIterator<SAP *> itr(saps);
  for (itr.init(); !itr; ++itr) delete itr();
  saps.deleteAllValues();
  
  // If our termination predicate is false, then we must be returning
  // because we were interrupted. 
  weWereInterrupted = !terminationPredicateTrue;

  // return the cumulative discounted reward from our children.
  // ri will be null if we terminated due to max iterations
  if (!ri) ri = new RewardInfo;
  ri->TotalReward = TotalReward;
  ri->NSteps = NSteps;
  return ri;
}

  
int MaxNode::ancestorHasTerminated(STATE & state,
				   list<ActivationRecord *> & stack,
				   int & inGoalState)
{
  listIterator<ActivationRecord *> itr(stack);
  for (itr.init(); !itr; ++itr) {
    if (itr()->node->terminationPredicate->call(state,
						*(itr()->actualParameters))){
      // check the goal predicate of this ancestor.  If we terminated
      // in a non-goal state for an ancestor (and we are doing
      // inheritance, then we must inherit this non-goal termination
      // too). 
      inGoalState =
	(itr()->node->goalPredicate->call(state,
					  *(itr()->actualParameters)));
      if (VERBOSE) {
	cout << itr()->node->name << " has terminated";
	if (inGoalState) {
	  cout << " in goal state." << endl;
	}
	else cout << endl;
      }
      return 1;
    }
  }
  return 0;
}

float MaxNode::value(list<MaxQPath *> * paths)
{
  // compute the MaxQPath with the highest value, and return its value
  if (!paths) {
    cerr << "Attempt to take value of empty path list; returning 0.0"
	 << endl;
    return 0.0;
  }
  else if (paths->length() == 0) {
    // this occurs when we are in a terminal state
    return 0.0;
  }
  else {
    float answer = paths->firstElement()->firstElement()->totalValue;
    listIterator<MaxQPath *> itr(*paths);
    for (itr.init(); !itr; ++itr) {
      if (itr()->firstElement()->totalValue > answer) {
	answer = itr()->firstElement()->totalValue;
      }
    }
    return answer;
  }
}
    
void MaxNode::CheckDownwardParameters()
{
  // for each child Q node, check that its formal parameters are
  // contained in the parent's parameters.  Report any "free"
  // parameters.
  if (!isPrimitive) {
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      itr()->CheckUpwardParameters(formalParameters);
    }
  }
}
  
void MaxNode::CheckPredicates()
{
  // check whether each parameter used by a predicate appears in the
  // formal parameter list of the max node
  if (!isPrimitive) {
    terminationPredicate->checkParameters(formalParameters);
    goalPredicate->checkParameters(formalParameters);
    listIterator<QNode *> itr(*children);
    for (itr.init(); !itr; ++itr) {
      itr()->child.CheckPredicates();
    }
  }
}


// ----------------------------------------------------------------------
// 
// QNode implementation
// 
// ----------------------------------------------------------------------


void QNode::init(clrandom & rng, MaxNode * parentNode, HQParameters &
		 params, STATE & state)
{
  parent = parentNode;

  if (useTable) {
    costToGo = new TableFunctionApproximator(this, state);
    costToGoTilde = new TableFunctionApproximator(this, state);
  }
  else {
    costToGo = new NNFunctionApproximator(this, rng);
    costToGoTilde = new NNFunctionApproximator(this, rng);
  }

  int nfeatures = 0;
  if (features) nfeatures = features->length();
  fv = new vector<float>(nfeatures);

  child.init(rng, params, state);
}


void QNode::save(ostream & str)
{
  costToGo->print(str);
  str << endl;
  costToGoTilde->print(str);
  str << endl;
  // invoke child
  child.save(str);
}

void QNode::read(istream & str)
{
  cout << "Reading weights for " << name << endl;
  costToGo->read(str);
  costToGoTilde->read(str);
  // invoke child
  child.read(str);
}

list<MaxQPath *> * QNode::evaluate(STATE & state,
				   ActualParameterList & apl,
				   int useTilde,
				   int indent)
{
  // Compute Q(i,s,j) where 
  //  i = this->parent  is the parent node
  //  s = state is the state
  //  j = this->child is the child node
  //
  //  if j has free parameters, then this routine computes a list of
  //  Q(i,s,j) elements for each possible instantiation of the free
  //  parameters. 

  // Always returns a list, possible empty if all subtasks are
  // terminated. 

  if (EVALVERBOSE) {
    indentLine(cout, indent);
    cout << "Evaluating " << name << apl << " useTilde = " << useTilde
	 << endl;
  }
  // generate values for unbound parameters
  // once all parameters are bound, then evaluate our child max node
  // add in the value from our cost-to-go table.

  // find first unbound variable
  FormalParameterIterator  itr(*formalParameters);
  for (itr.init(); !itr; ++itr) {
    if (! apl.includesKey(itr()->key())) {
      // create iterator for the legal values of the parameter
      rangeIterator vitr(itr()->value()->minValue,
			 itr()->value()->maxValue);
      // collect the returned values into a list
      list<MaxQPath *> * answer = new list<MaxQPath *>;
      // invoke ourselves recursively for each legal value of the
      // unbound parameter. 
      for (vitr.init(); !vitr; ++vitr) {
	apl[itr()->key()] = vitr();
	list<MaxQPath *> * childAnswers;
	childAnswers = evaluate(state, apl, useTilde, indent + 2);
	answer->nconc(*childAnswers);
	delete childAnswers; // just deletes list header, list is null
	                     // after nconc	     
	childAnswers = 0;
      }
      // remove the binding for this parameter so that it isn't
      // accidentally passed to siblings of the same Max node. 
      apl.removeKey(itr()->key());
      return answer;
    }
  }
  // allocate answer
  list<MaxQPath *> * answer = new list<MaxQPath *>;

  // all parameters are bound
  ActualParameterList * childApl = mapActualParameters(apl, state);
  MaxQPath * childValue = child.evaluate(state, *childApl, 0,
					 indent + 2);
  delete childApl;

  // childValue is the best MaxQPath from our child.
  // Our job is to add ourself to this path.
  // To add ourself, we need to
  // compute our cost-to-go function.

  if (childValue) {
    // the child is not terminated.  Compute our cost-to-go function
    // compute the feature vector for this state
    computeFeatureVector(state, apl, features, fv);
    float ctg;

    if (useTilde) {
      ctg = costToGoTilde->Predict(fv);
    }
    else {
      ctg = costToGo->Predict(fv);
    }
    
    if (childValue->isEmpty()) {
      // if the MaxQPath is empty, then our child was a primitive Max
      // node, so we just initialize a result.
      // copy the actual parameter list.  This does a deep copy,
      // which is important since we are modifying the apl above. 
      ActualParameterList * aplCopy = new ActualParameterList(apl);
      // copy the full feature vector
      vector<float> * mfCopy = new vector<float>(*fv);
      ValueResult * us = new ValueResult(ctg, ctg, this, aplCopy, mfCopy);
      childValue->add(us);
    }
    else {
      // examine the first element in the MaxQPath and compute a new
      // element to add to the path.
      float oldTotal = childValue->firstElement()->totalValue;
      ActualParameterList * aplCopy = new ActualParameterList(apl);
      vector<float> * mfCopy = new vector<float>(*fv);
      ValueResult * us = new ValueResult(ctg + oldTotal,
					 ctg,
					 this,
					 aplCopy,
					 mfCopy);
      childValue->add(us);
    }
    // stick this one path onto the answer.
    answer->add(childValue);
  }
  if (EVALVERBOSE) {
    indentLine(cout, indent);
    cout << "Finished evaluating " << name << apl
	 << " = " << *answer << endl;
  }
  return answer;
}

// This is the old evaluate routine, which I have renamed
list<MaxQPath *> * QNode::evaluateList(STATE & state,
				   ActualParameterList & apl,
				   int useTilde)
{
  if (EVALVERBOSE)
    cout << "Evaluating " << name << apl << endl;
  // generate values for unbound parameters
  // once all parameters are bound, then evaluate our child max node
  // add in the value from our cost-to-go table.

  // find first unbound variable
  FormalParameterIterator  itr(*formalParameters);
  for (itr.init(); !itr; ++itr) {
    if (! apl.includesKey(itr()->key())) {
      // create iterator for the legal values of the parameter
      rangeIterator vitr(itr()->value()->minValue,
			 itr()->value()->maxValue);
      // collect the returned values into a list
      list<MaxQPath *> * answer = new list<MaxQPath *>;
      // invoke ourselves recursively for each legal value of the
      // unbound parameter. 
      for (vitr.init(); !vitr; ++vitr) {
	apl[itr()->key()] = vitr();
	list<MaxQPath *> * childAnswers;
	childAnswers = evaluateList(state, apl, useTilde);
	answer->nconc(*childAnswers);
	delete childAnswers; // just deletes list header, list is null
	                     // after nconc	     
	childAnswers = 0;
      }
      // remove the binding for this parameter so that it isn't
      // accidentally passed to siblings of the same Max node. 
      apl.removeKey(itr()->key());
      return answer;
    }
  }
  // all parameters are bound
  ActualParameterList * childApl = mapActualParameters(apl, state);
  list<MaxQPath *> * childValues = 
     child.evaluateList(state, *childApl, 0);
  delete childApl;

  // childValues is the list of MAXQPath's from our children.
  // Our job is to add ourselves to each of those
  // MaxQPath's and return the result.  To add ourselves, we need to
  // compute our cost-to-go function.

  if (childValues && !childValues->isEmpty()) {
    // the child was ok.  Compute our cost-to-go function
    // compute the feature vector for this state
    computeFeatureVector(state, apl, features, fv);
    float ctg;

    if (useTilde) {
      ctg = costToGoTilde->Predict(fv);
    }
    else {
      ctg = costToGo->Predict(fv);
    }

    listIterator<MaxQPath *> cvItr(*childValues);
    for (cvItr.init(); !cvItr; ++cvItr) {
      if (cvItr()->isEmpty()) {
	// if the MaxQPath is empty, then our child was a primitive Max
	// node, so we just initialize a result.
	// copy the actual parameter list.  This does a deep copy,
	// which is important since we are modifying the apl above. 
	ActualParameterList * aplCopy = new ActualParameterList(apl);
	// copy the full feature vector
	vector<float> * mfCopy = new vector<float>(*fv);
	ValueResult * us = new ValueResult(ctg, ctg, this, aplCopy, mfCopy);
	cvItr()->add(us);
      }
      else {
	// examine the first element in the MaxQPath and compute a new
	// element to add to the path.
	float oldTotal = cvItr()->firstElement()->totalValue;
	ActualParameterList * aplCopy = new ActualParameterList(apl);
	vector<float> * mfCopy = new vector<float>(*fv);
	ValueResult * us = new ValueResult(ctg + oldTotal,
					   ctg,
					   this,
					   aplCopy,
					   mfCopy);
	cvItr()->add(us);
      }
    }
  }
  return childValues;
}


// this function computes the feature vector for a given state.  It is
// passed the actual parameter list, a list defining the features, and
// an empty feature vector of the proper size, which it updates. 
void computeFeatureVector(STATE & state,
			  ActualParameterList & apl,
			  list<ParameterName *> * features,
			  vector<float> * fv)
{
  int i = 0;
  fv->fill(0.0);

  // now install node features
  if (features) {
    listIterator<ParameterName *> itr(*features);
    for (itr.init(); !itr; ++itr) {
      state.insertFeature(itr(), apl, *fv, i);
    }
  }
}

void QNode::generateBindingsAndFeatures(STATE & state,
					ActualParameterList & apl,
					list<MaxQPath *> * tildePaths)
{
  // for each possible value of free parameters (if any), generate an
  // apl and a feature vector 

  // We want to ensure, however, that this qnode appears in tildePaths
  // with the indicated features.

  // check to see that this Qnode appears in tildePaths.
  int thisAppearsInPaths = 0;
  listIterator<MaxQPath *> pathItr(*tildePaths);
  for (pathItr.init(); !pathItr; ++pathItr) {
    if (pathItr()->firstElement()->op == this) {
      thisAppearsInPaths = 1;
      break;
    }
  }
  if (!thisAppearsInPaths) return;

  // find first unbound variable
  FormalParameterIterator  itr(*formalParameters);
  for (itr.init(); !itr; ++itr) {
    if (! apl.includesKey(itr()->key())) {
      // create iterator for the legal values of the parameter
      rangeIterator vitr(itr()->value()->minValue,
			 itr()->value()->maxValue);
      // invoke ourselves recursively for each legal value of the
      // unbound parameter. 
      for (vitr.init(); !vitr; ++vitr) {
	apl[itr()->key()] = vitr();
	generateBindingsAndFeatures(state, apl, tildePaths);
	apl.removeKey(itr()->key());
      }
      return;
    }
  }
  // all parameters are bound


}
	  


ActualParameterList * QNode::mapActualParameters(ActualParameterList & apl,
						 STATE & state)


{
  ActualParameterList * answer = new ActualParameterList;
  ParameterPairIterator ppitr(*childParameters);

  for (ppitr.init(); !ppitr; ++ppitr) {
    // each element has the form 
    // association(ourParameterName, childParameterName)
    //
    // ourParameterName may be either a parameter pass to us or a
    // global feature. 

    if (apl.includesKey(ppitr()->key())) {
      (*answer)[ppitr()->value()] = apl[ppitr()->key()];
    }
    else {
      (*answer)[ppitr()->value()] = (int) state.featureValue(ppitr()->key());

    }
  }
  return answer;
  // note: receiver is responsible for deleting
}

RewardInfo * QNode::execute(STATE & state,
			    ActualParameterList & apl,
			    list<ActivationRecord *> & stack,
			    list<STATE *> & upwardStates,
			    clrandom & rng,
			    HQParameters & params,
			    int & stepQuota,
			    int & explorationCount,
			    float & totalBellmanError,
			    int & childWasInterrupted)
{
  // we have been chosen for execution.
  if (params.epsilonGreedy == 2) {
    // if we are doing counter-based exploration and learning, count
    // this execution.
    computeFeatureVector(state, apl, features, fv);
    costToGo->IncrementVisits(fv);
  }

  ActualParameterList * childApl = mapActualParameters(apl, state);
  // now execute child node
  RewardInfo * ri =  child.execute(state, *childApl, stack, 
				   upwardStates,
				   rng, params, stepQuota,
				   explorationCount,
				   totalBellmanError,
				   childWasInterrupted);
  delete childApl;
  return ri;
}

void QNode::update(vector<float> & lastFeatures,
		   float backedUpValue,
		   float backedUpValueTilde,
		   float learningRate,
		   float & bellmanError,
		   float & bellmanErrorTilde)
{
  if (!useTable) {
    // ensure that the target values are within the legal range
    backedUpValue = min(backedUpValue, maxValue);
    backedUpValue = max(backedUpValue, minValue);
    backedUpValueTilde = min(backedUpValueTilde, maxValue);
    backedUpValueTilde = max(backedUpValueTilde, minValue);
  }

  if (LEARN) {
    // to disable learning, set LEARN to 0
    bellmanErrorTilde = costToGoTilde->Train(&lastFeatures,
					     backedUpValueTilde,
					     learningRate);
    bellmanError = costToGo->Train(&lastFeatures,
				   backedUpValue,
				   learningRate);
  }
}


void QNode::store(vector<float> & lastFeatures,
		  float backedUpValue,
		  float backedUpValueTilde)
{
  // like update, except that we simply store these new values!
  costToGoTilde->Store(&lastFeatures,
		       backedUpValueTilde);
  costToGo->Store(&lastFeatures,
		  backedUpValue);
}

void QNode::CheckUpwardParameters(FormalParameterList * fpl)
{
  // fpl is the formal parameter list of our parent.
  // Check each of our formal parameters to see if it is in that list
  // and report free parameters

  if (formalParameters && !formalParameters->isEmpty()) {
    FormalParameterIterator myItr(*formalParameters);
    for (myItr.init(); !myItr; ++myItr) {
      // see if this is contained in the other list
      if (fpl && fpl->includesKey(myItr()->key())) {
	// a-ok
      }
      else {
	cout << "QNode " << name << " parameter " << myItr()->key() <<
	  " is free" << endl;
      }
    }
  }
  child.CheckDownwardParameters();
}
      
int QNode::countNonZeroWeights ()
{
  int nzw = costToGoTilde->countNonZeroWeights();
  if (VERBOSE) cout << name << " " << nzw << " non-zero weights." << endl;
  nzw += child.countNonZeroWeights();
  return nzw;
}

void QNode::clobberZeroWeights (float val)
{
  cout << "Setting all zero weights in " << name << " to " << val << endl;
  costToGo->clobberZeroWeights(val);
  costToGoTilde->clobberZeroWeights(val);
  child.clobberZeroWeights(val);
}

void QNode::clearMarks ()
{
  mark = 0;
  child.clearMarks();
}

void QNode::computeValues(STATE & state, ActualParameterList & apl,
			  float & value, float & tildeValue)
{
  computeFeatureVector(state, apl, features, fv);
  value = costToGo->Predict(fv);
  tildeValue = costToGoTilde->Predict(fv);
}

int QNode::OkToUpdate(HQParameters & params)
{
  return child.isPrimitive ||
    (child.BellmanError->Average() < params.updateThreshold) ||
    child.requirePolicy || 
    child.policyProbability == 1;
}


//////////////////////////////////////////////////////////////////////
//
// Empirical value iteration for MAXQ graphs
//
//////////////////////////////////////////////////////////////////////

void MaxNode::ValueIteration(STATE & state,
			     ActualParameterList & apl,
			     list<ActivationRecord *> & stack,
			     clrandom & rng,
			     HQParameters & params,
			     int stepsPerChild,
			     int & explorationCount,
			     float & totalBellmanError,
			     int & bellmanCount,
			     int iterations)
{
  // recursively call each action at this level to perform value
  // iteration.  When they return, then perform value iteration at
  // this level. Note that nodes with multiple parents will be updated
  // multiple times.  To avoid this, we would need to detect identical
  // values in apl, but this is hard.  So we won't bother.  In any case,
  // the multiple updates will not cause any problem, since VI is
  // idempotent. 

  if (isPrimitive) return;
  
  if (VERBOSE) cout << "Call ValueIteration " << name << apl << endl;

  if (DEBUG) Debugger(state, stack, apl);

  // we operate on a copy of the current state so that we don't
  // clobber it

  list<MaxQPath *> * tildePaths = possibleActions(state, apl);
  assert(tildePaths);
    
  if (VERBOSE) cout << "Possible Actions for VI:\n" << *tildePaths << endl;
  // there may be no possible actions here, in which case, we just
  // return.
  if (tildePaths->isEmpty()) {
    delete tildePaths;
    return;
  }
    
  // now iterate over each of the actions and compute an updated
  // estimate of its Q value and its Q-tilde value
  listIterator<MaxQPath *> actItr(*tildePaths);
  for (actItr.init(); !actItr; ++actItr) {  
    // Consider each child action.

    ValueResult * head = actItr()->firstElement();
    QNode * chosenChild = head->op;
    ActualParameterList * chosenAPL = head->actualParameters;

    // First, invoke ValueIteration for this action:
    stack.add(new ActivationRecord(this, &apl));
    chosenChild->ValueIteration(state, *chosenAPL, stack, rng, params,
				stepsPerChild, explorationCount,
				totalBellmanError, bellmanCount, iterations);
    delete stack.pop();

    // Now perform ValueIteration at this level.  Given the current
    // state, we perform "iterations" number of executions of the
    // action compute the values of the result states, back them up,
    // and update the Q values at this level.

    if (performValueIteration) {
      // if we are not performing VI at this level, then we don't
      // measure the children at this level.  Presumably after we save
      // weights from this run, the bit will be cleared, and we will
      // measure these actions (after they have been learned better?).

      // Now measure its behavior in this state
      if (VERBOSE) cout << "Measuring " << chosenChild->name <<
		     *chosenAPL << endl;
      
      float totalResultValue = 0.0;
      float totalResultValueTilde = 0.0;
      int goodIterations = 0;
      // turn off learning during child execution
      int savedLEARN = LEARN;
      LEARN = 0;
      
      for (int i = 0; i < iterations; i++) {
	int childWasInterrupted = 0;
	int terminationPredicateTrue = 0;
	int goalPredicateTrue = 0;
	int ancestorTerminated = 0;
	int ancestorTerminatedInGoalState = 0;
	list<STATE *> downwardStates;   // The states returned by our child node.
	int stepQuota = stepsPerChild;
	int unused;
	
	// copy the state.  Maybe this is the problem!
	STATE * s = state.Duplicate();
	
	stack.add(new ActivationRecord(this, &apl));
	// now execute one action
	float childBellmanError = 0.0;
	RewardInfo * ri = chosenChild->execute(*s, *chosenAPL, stack, downwardStates, rng,
					       params, stepQuota, unused,
					       childBellmanError, childWasInterrupted);
	delete stack.pop();
	
	// count the total number of primitive actions performed
	explorationCount += stepsPerChild - stepQuota;
	
	if (VERBOSE) cout << "RewardInfo = " << *ri << endl;
	
	// release list of visited states.
	listIterator<STATE *> sitr(downwardStates);
	for (sitr.init(); !sitr; ++sitr) delete sitr();
	downwardStates.deleteAllValues();
	
	// compute the immediate reward
	// This is typically zero except for maxq nodes whose children
	// are all primitives.
	float reward = 0.0;
	float rewardTilde = 0.0;
	
	RewardInfoIterator ritr(*ri);
	for (ritr.init(); !ritr; ++ritr) {
	  if (ritr()->responsibleNodeList->includes(chosenChild)) {
	    // take responsibility; move a reward from pending to total
	    reward += ritr()->reward;
	    rewardTilde += ritr()->reward;
	  }
	  // reclaim it, even if it isn't ours
	  delete ritr();
	  ritr.removeCurrent();
	}
	delete ri;
	ri = 0;
	
	// now compute value of resulting state
	float resultStateValue = 0.0;
	float resultStateTildeValue = 0.0;
	
	// now check our various termination conditions
	terminationPredicateTrue = terminationPredicate->call(*s, apl);
	goalPredicateTrue = goalPredicate->call(*s, apl);
	
	// check if our ancestors have terminated, and if so, if they have
	// terminated in a goal state.
	ancestorTerminated = ancestorHasTerminated(*s,
						   stack,
						   ancestorTerminatedInGoalState);
	if (inheritTerminationPredicates) {
	  terminationPredicateTrue =
	    terminationPredicateTrue || ancestorTerminated;
	  goalPredicateTrue =
	    goalPredicateTrue || ancestorTerminatedInGoalState;
	}
	
	// implement nongoal termination: adjust terminal reward
	if (terminationPredicateTrue && !goalPredicateTrue) {
	  if (VERBOSE) cout << name << " non-goal termination." << endl;
	  rewardTilde += nonGoalTerminationPenalty;
	  nonGoalTerminations++;
	}
	
	// if the child was interrupted, then it didn't complete its
	// execution, and the result state is not valid. 
	if (!childWasInterrupted) {
	  // this is a usable result state
	  if (!terminationPredicateTrue) {
	    // we are returning in a non-terminal state.  This means we
	    // must do a lookahead to compute the resultStateValue
	    // (In a terminal state, the result state is always worth 0.
	    // The termination penalty is part of the last reward
	    // transition.)
	    
	    // We face a quandary about what to do for the sarsa backup.
	    // Some possible options:  
	    // (a) We could take the unTildeValue of the greedy action
	    // according to the tilde values. (*chosen*)
	    // (b) We could just not do a sarsa backup.
	    
	    MaxQPath * bestTildePath = evaluate(*s, apl, 1, 0);
	    // evaluate could return NULL if we or our children are
	    // terminated, but this should never be true.
	    assert(bestTildePath);
	    resultStateTildeValue = bestTildePath->firstElement()->totalValue;
	    resultStateValue = unTilde(bestTildePath);
	    deletePath(bestTildePath); 
	  } // lookahead Max of Q values in next state
	  goodIterations++;
	  totalResultValue += resultStateValue + reward;
	  totalResultValueTilde += resultStateTildeValue + rewardTilde;
	  if (VERBOSE) {
	    cout << "reward = " << reward
		 << "resultStateValue = " << resultStateValue
		 << endl;
	    cout << "rewardTilde = " << rewardTilde
		 << "resultStateTildeValue = " << resultStateTildeValue
		 << endl;
	  }
	} // end this was a usable result state
	
	// before GCing the state, we will copy the seed of the RNG back
	// into state, so that the random numbers continue to evolve. 
	state.rng = s->rng;
	
	// garbage collection
	delete s;
	s = 0;
      } // repeated trials of same action

      LEARN = savedLEARN;

      if (goodIterations > 0) {
	// if we had good iterations, we can update the Q function
	totalResultValue /= goodIterations;
	totalResultValueTilde /= goodIterations;
	
	// now save this computed value in the function approximator.
	// To do this, we must first ask the Q node to compute its value
	// one more time.
	float value;
	float tildeValue;
	chosenChild->computeValues(state, *chosenAPL, value, tildeValue);
	float sarsaTDError = totalResultValue - value;
	float tildeTDError = totalResultValueTilde - tildeValue;

	if (VERBOSE) {
	  cout << name
	       << " previous value " << value
	       << " previous tildeValue " << tildeValue
	       << endl;
	  cout << " new value " << totalResultValue
	       << " new tildeValue " << totalResultValueTilde
	       << endl;
	  cout << name
	       << " value iteration tildeTDError = " << tildeTDError
	       << "sarsaTDError = " << sarsaTDError
	       << endl;
	}

#if 0
	if (fabs(tildeTDError) != 0.0) {
	  if (debugStateValid) {
	    // see if we have visited this state before.
	    if (debugNode == this &&
		state == *debugState) {
	      cout << "Visiting previously-visited debug state!" << endl;
	      cout << name
		   << " previous value " << value
		   << " previous tildeValue " << tildeValue
		   << endl;
	      cout << " new value " << totalResultValue
		   << " new tildeValue " << totalResultValueTilde
		   << endl;
	      cout << name
		   << " value iteration tildeTDError = " << tildeTDError
		   << "sarsaTDError = " << sarsaTDError
		   << endl;
	      cout << "Possible Actions for VI:\n" << *tildePaths << endl;
	      Debugger(state, stack, apl);
	    }
	  }
	  else {
	    // this is the first non-zero error state.  Save it.
	    debugState = state.Duplicate();
	    debugNode = this;
	    debugStateValid = 1;
	    cout << "Saving this state for debugging" << endl;
	    state.Print(cout);
	    cout << name
		 << " previous value " << value
		 << " previous tildeValue " << tildeValue
		 << endl;
	    cout << " new value " << totalResultValue
		 << " new tildeValue " << totalResultValueTilde
		 << endl;
	    cout << name
		 << " value iteration tildeTDError = " << tildeTDError
		 << "sarsaTDError = " << sarsaTDError
		 << endl;
	  }
	}
#endif

	/*
	if (VERBOSE && (sarsaTDError != 0 || tildeTDError != 0)) {
	  cout << name << " value iteration tildeTDError = " << tildeTDError <<
	    "sarsaTDError = " << sarsaTDError << endl;
	  cout << name << " executing " << chosenChild->name << *chosenAPL
	       << " non-zero TDERROR: " << endl;
	  state.Print(cout);
	}

	if (fabs(tildeTDError) > 1.0) {
	  cout << name << " value iteration tildeTDError = " << tildeTDError <<
	    "sarsaTDError = " << sarsaTDError << endl;
	  cout << name << " executing " << chosenChild->name << *chosenAPL
	       << " non-zero TDERROR: " << endl;
	  state.Print(cout);
	}
	*/

	
	// these two variables are unused
	float bellmanError = 0;
	float bellmanErrorTilde = 0;
	chosenChild->update(*(chosenChild->fv),
			    sarsaTDError,
			    tildeTDError,
			    1.0,           // force learning rate to 1.0
			    bellmanError,
			    bellmanErrorTilde);
	totalBellmanError += fabs(tildeTDError);
	bellmanCount++;
      }
      else {
	cout << name << " executing " << chosenChild->name << *chosenAPL
	     << " no good iterations in state: " << endl;
	state.Print(cout);
      }
    }
  } // for each child action

  // GC
  deletePathList(tildePaths);
}

void QNode::ValueIteration(STATE & state,
			   ActualParameterList & apl,
			   list<ActivationRecord *> & stack,
			   clrandom & rng,
			   HQParameters & params,
			   int stepsPerChild,
			   int & explorationCount,
			   float & totalBellmanError,
			   int & bellmanCount,
			   int iterations)
{
  // Now create the actual parameters of the child and invoke it.
  ActualParameterList * childApl = mapActualParameters(apl, state);
  child.ValueIteration(state, *childApl, stack, rng, params, stepsPerChild,
		       explorationCount, totalBellmanError,
		       bellmanCount, iterations);
  delete childApl;
}
