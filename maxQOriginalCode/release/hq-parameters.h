// -*- C++  -*-
//
// A class for reading and printing parameters for hierarchical Q learning
//

#ifndef HQ_PARAMETERS_H
#define HQ_PARAMETERS_H

#include <iostream.h>
#include <clstring.h>
#include <list.h>

class TemperatureInfo
{
public:
  string nodeName;
  float temperature;
  float coolingRate;
  float minTemperature;
  int operator == (TemperatureInfo & other);
  // basic constructor
  TemperatureInfo(): nodeName(""), temperature(0.0), minTemperature(0.0), coolingRate(1.0) {};
  // copy constructor.  make sure that the string gets copied properly
  TemperatureInfo (const TemperatureInfo & other);
};

istream & operator >> (istream & str, TemperatureInfo & info);
ostream & operator << (ostream & str, TemperatureInfo & info);

class LearningRateInfo
{
public:
  LearningRateInfo(): nodeName(""), learningRate(0.0), trial(0) {};
  string nodeName;
  float learningRate; 
  int trial;
  // needed in order to be in a list
  int operator == (LearningRateInfo & other) {
    return (nodeName == other.nodeName && learningRate ==
	    other.learningRate && trial == other.trial);
  };
  // copy constructor.  make sure that the string gets copied properly
  LearningRateInfo (const LearningRateInfo & other) {
    nodeName = other.nodeName;
    learningRate = other.learningRate;
    trial = other.trial;
  };

};

istream & operator >> (istream & str, LearningRateInfo & info);
ostream & operator << (ostream & str, LearningRateInfo & info);

class HQParameters
{
public:
  HQParameters();
  HQParameters(char *);
  HQParameters(HQParameters & other);
  int seed;
  float learningRate;
  float discountFactor;
  float temperature;
  float policyProbability;
  int verbose;
  int learnverbose;
  int discriminate;
  float lambda;
  float coolingRate;
  float minTemperature;
  int ABEspan;
  float deltaBellmanThreshold;
  float updateThreshold;
  float interruptProbability;
  list<TemperatureInfo> tempInfo;
  int epsilonGreedy;
  int pollingCoolingRate;   // reduce RootQuota every N trials
  int pollingCoolingStep;   // amount to reduce RootQuota
  int maxStepsPerTrial;     // maximum number of steps per trial
  int maxStepsPerEvalTrial; // maximum number of steps per evaluation trial
  int stepsPerEvalCall;     // set to infty for hierarchical execution
                            // set to 1 for hierarchical greedy execution
  
  list<LearningRateInfo> lrateInfo;
  list<string> nodesToLog;
  list<string> nodesToRequirePolicy;
  list<string> nodesToPerformValueIteration;
  int prioritizedSweeping;  // set to 1 for prioritized sweeping; 0
                            // for MAXQ-Q learning
  int prioritizedSweepingSteps;  // number of steps of prioritized
                            // sweeping per primitive action
  void Display(ostream & str);
  void Read(char * filename);
};

#endif
