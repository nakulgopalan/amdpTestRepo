// -*- C++  -*-
//
// A class for reading and printing parameters for HQ learning
//

#include <iostream.h>
#include <fstream.h>
#include "hq-parameters.h"

HQParameters::HQParameters()
{
  seed = 94857;
  learningRate = 1.0;
  discountFactor = 1.0;
  temperature = 100.0;
  policyProbability = 1.0;
  verbose = 1;
  learnverbose = 1;
  discriminate = 0;
  lambda = 0.0;
  coolingRate = 1.0;
  minTemperature = 0;
  ABEspan = 100;
  deltaBellmanThreshold = 0.0;
  interruptProbability = 0.0;
  updateThreshold = 0.5;
  epsilonGreedy = 0;
  pollingCoolingRate = 99999;
  pollingCoolingStep = 0;
  maxStepsPerTrial = 10000;
  maxStepsPerEvalTrial = 1000;
  stepsPerEvalCall = 1000;
  prioritizedSweeping = 0;
  prioritizedSweepingSteps = 0;
}

HQParameters::HQParameters(char * filename)
{
  Read(filename);
}

void skipComments(istream & str)
{
  char c;
  while (1) {
    str >> c;
    if (c == '/') {
      // this introduces a comment.  Skip to end of line
      while (c != '\n') str.get(c);
    }
    else {
      str.putback(c);
      return;
    } 
  }
}

void HQParameters::Read(char * filename)
{
  ifstream str(filename);
  skipComments(str);
  str >> seed;

  // learning controls
  skipComments(str);
  str >> prioritizedSweeping;
  skipComments(str);
  str >> learningRate;
  skipComments(str);
  str >> lrateInfo;
  skipComments(str);
  str >> prioritizedSweepingSteps;

  // exploration controls
  skipComments(str);
  str >> epsilonGreedy;
  skipComments(str);
  str >> temperature;
  skipComments(str);
  str >> coolingRate;
  skipComments(str);
  str >> minTemperature;
  skipComments(str);
  str >> tempInfo;
  skipComments(str);
  str >> nodesToRequirePolicy;

  // other stuff
  skipComments(str);
  str >> discountFactor;
  skipComments(str);
  str >> policyProbability;
  skipComments(str);
  str >> verbose;
  skipComments(str);
  str >> learnverbose;
  skipComments(str);
  str >> maxStepsPerEvalTrial;
  skipComments(str);
  str >> stepsPerEvalCall;
  skipComments(str);
  str >> nodesToLog;
  skipComments(str);
  str >> nodesToPerformValueIteration;

  // selective learning controls
  skipComments(str);
  str >> ABEspan;
  skipComments(str);
  str >> deltaBellmanThreshold;
  skipComments(str);
  str >> updateThreshold;

  // methods for introducing hierarchical greedy execution
  skipComments(str);
  str >> interruptProbability;
  skipComments(str);
  str >> pollingCoolingRate;
  skipComments(str);
  str >> pollingCoolingStep;
  skipComments(str);
  str >> maxStepsPerTrial;

  // unsupported stuff
  skipComments(str);
  str >> discriminate;
  skipComments(str);
  str >> lambda;

}  

HQParameters::HQParameters (HQParameters & other):
  seed(other.seed),
  learningRate(other.learningRate),
  discountFactor(other.discountFactor),
  temperature(other.temperature),
  policyProbability(other.policyProbability),
  verbose(other.verbose),
  learnverbose(other.learnverbose),
  discriminate(other.discriminate),
  lambda(other.lambda),
  coolingRate(other.coolingRate),
  minTemperature(other.minTemperature),
  ABEspan(other.ABEspan),
  deltaBellmanThreshold(other.deltaBellmanThreshold),
  updateThreshold(other.updateThreshold),
  interruptProbability(other.interruptProbability),
  tempInfo(other.tempInfo),
  epsilonGreedy(other.epsilonGreedy),
  pollingCoolingRate(other.pollingCoolingRate),
  pollingCoolingStep(other.pollingCoolingStep),
  maxStepsPerTrial(other.maxStepsPerTrial),
  maxStepsPerEvalTrial(other.maxStepsPerEvalTrial),
  stepsPerEvalCall(other.stepsPerEvalCall),
  lrateInfo(other.lrateInfo),
  nodesToLog(other.nodesToLog),
  nodesToRequirePolicy(other.nodesToRequirePolicy),
  nodesToPerformValueIteration(other.nodesToPerformValueIteration),
  prioritizedSweeping(other.prioritizedSweeping),
  prioritizedSweepingSteps(other.prioritizedSweepingSteps)

{
  // copy constructor.  Needed because of the lists.
}


void HQParameters::Display(ostream & str)
{
  str << "\tRandom number seed\t" << seed << endl;

  // learning rate controls
  str << "Learning controls:" << endl;
  str << "\tDo Prioritized Sweeping?\t" << prioritizedSweeping << endl;
  str << "\tLearning rate\t" << learningRate << endl;
  str << "\tLearning Rate Information\t" << lrateInfo << endl;
  str << "\tNumber of Prioritized Sweeping Steps per action\t" <<
    prioritizedSweepingSteps << endl;

  // exploration controls
  str << "Exploration controls:" << endl;
  str << "\tUse Epsilon Greedy Exploration?\t" << epsilonGreedy << endl;
  str << "\tInitial temperature\t" << temperature << endl;
  str << "\tCooling Rate\t" << coolingRate << endl;
  str << "\tMinimum Temperature\t" << minTemperature << endl;
  str << "\tTemperature info\t" << tempInfo << endl;
  str << "\tExecute hand-coded policy at nodes\t"
      << nodesToRequirePolicy << endl;

  // other stuff
  str << "Other stuff:" << endl;
  str << "\tDiscount factor\t" << discountFactor << endl;
  str << "\tInitial policyProbability\t" << policyProbability << endl;
  str << "\tVerbose\t" << verbose << endl;
  str << "\tLearnVerbose\t" << learnverbose << endl;
  str << "\tMaximum number of steps per evaluation trial\t" << maxStepsPerEvalTrial << endl;
  str << "\tSteps per MaxRoot call during evaluation trials\t" << stepsPerEvalCall << endl;
  str << "\tNodes to monitor in log file\t" << nodesToLog << endl;
  str << "\tNodes on which to Perform Value Iteration\t"
      << nodesToPerformValueIteration << endl;

  // selective learning controls
  str << "Selective learning controls:" << endl;
  str << "\tBellman Error moving average span\t" << ABEspan << endl;
  str << "\tDelta Bellman threshold for cooling\t" << deltaBellmanThreshold << endl;
  str << "\tMax child moving average for update\t" << updateThreshold << endl;

  // methods for introducing hierarchical greedy execution
  str << "Hierarchical greedy execution:" << endl;
  str << "\tInterrupt Probability\t" << interruptProbability << endl;
  str << "\tReduce Root Quota after n trials\t" << pollingCoolingRate << endl;
  str << "\tAmount to reduce Root Quota\t" << pollingCoolingStep << endl;
  str << "\tMaximum number of steps per trial\t" << maxStepsPerTrial << endl;


  str << "Unsupported options:" << endl;
  str << "\tDiscriminate\t" << discriminate << endl;
  str << "\tLambda\t" << lambda << endl;


}

int TemperatureInfo::operator == (TemperatureInfo & other)
{
  return (nodeName == other.nodeName &&
	  temperature == other.temperature &&
	  minTemperature == other.minTemperature &&
	  coolingRate == other.coolingRate);
}

TemperatureInfo::TemperatureInfo (const TemperatureInfo & other)
{
  nodeName = other.nodeName;
  temperature = other.temperature;
  minTemperature = other.minTemperature;
  coolingRate = other.coolingRate;
}

istream & operator >> (istream & str, TemperatureInfo & info)
{
  str >> info.nodeName >> info.temperature
      >> info.coolingRate >> info.minTemperature;
  return str;
}

ostream & operator << (ostream & str, TemperatureInfo & info)
{
  str << info.nodeName << " " << info.temperature << " " <<
    info.coolingRate << " " << info.minTemperature << endl; 
  return str;
}

istream & operator >> (istream & str, LearningRateInfo & info)
{
  str >> info.trial >> info.nodeName >> info.learningRate;
  return str;
}

ostream & operator << (ostream & str, LearningRateInfo & info)
{
  str << info.trial << " " << info.nodeName << " "
      << info.learningRate << endl;
  return str;
}

