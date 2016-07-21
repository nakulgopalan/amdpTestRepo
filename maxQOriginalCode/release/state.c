// -*- C++ -*- 

#include "state.h"

ostream & operator << (ostream & str, Action & act)
{
  return str << act.name;
}

void State::Act(Action * act, ActualParameterList & apl)
{
  cerr << "Attempt to perform action " << act << apl << " on State class." <<
    endl;
  abort();
}

void State::Print(ostream & str)
{
  cerr << "Attempt to print State class." <<
    endl;
  abort();
}

void State::Save(ostream & str)
{
  str << rng.getseed() << endl;
}

void State::Read(istream & str)
{
  randomState rs;
  str >> rs;
  rng.setseed(rs);
}

int State::Terminated()
{
  cerr << "Attempt to test whether State class is terminated" << endl;
  abort();
  return 0;  // to please compiler
}

State * State::Duplicate()
{
  cerr << "Attempt to make a copy of the abstract State class" <<
    endl;
  return 0;  // to please compiler
}


int State::featureSize(ParameterName * pname)
{
  cerr << "Attempt to compute size of unknown parameter name " <<
    pname << endl;
  abort();
  return 0;
}

float State::featureValue(ParameterName * pname)
{
  cerr << "Attempt to compute value of unknown parameter name " <<
    pname << endl;
  abort();
  return 0;
}

void State::insertFeature(ParameterName * pname, 
			  ActualParameterList & apl,
			  vector<float> & fv,
			  int & i)
{
  cerr << "Attempt to insert feature value of parameter name " <<
    pname << endl;
  abort();
}
