// -*- C++ -*-

#include "psweep.h"
#include "hq.h"

ActualParameterList * StateInfo::RecordResult(QNode * act,
					      ActualParameterList * apl,
					      StateHashCode resultId,
					      float reward,
					      SuccessorInfo * & succi)
{
  // starting in the state corresponding to this instance, action act
  // was performed and resulted in state resultId.
  // We return a pointer to the childAPL, which can then be used to
  // record the successor information for our predecessors without
  // allocating new memory.
  ActualParameterList * answer = 0;

  // find the action that was performed
  listIterator<SuccessorInfo *> itr(successors);
  for (itr.init(); !itr; ++itr) {
    if (itr()->MatchingSuccessorInfo(act, apl)) {
      succi = itr();
      answer = itr()->apl;
      itr()->N++;
      // find the resulting state
      listIterator<ResultStateInfo *> ritr(itr()->resultStates);
      for (ritr.init(); !ritr; ++ritr) {
	if (ritr()->code == resultId) {
	  ritr()->N++;
	  ritr()->totalReward += reward;
	  return answer;
	}
      }
      // loop failed to find the entry, so add it
      itr()->resultStates.add(new ResultStateInfo(resultId, reward));
      return answer;
    }
  }
  // loop failed to find this action, so add it.  We make a copy of
  // the apl here, since the original will be modified/deleted by the
  // caller later. 
  answer = apl->Duplicate();
  succi = new SuccessorInfo(act, answer, resultId, reward);
  successors.add(succi);
  return answer;
}

void StateInfo::RecordPredecessor(StateHashCode startCode,
				  QNode * act,
				  ActualParameterList * apl)
{
  // see if we can find a record for this one already
  listIterator<PredecessorInfo *> itr(predecessors);
  for (itr.init(); !itr; ++itr) {
    if (itr()->MatchingPredecessorInfo(startCode, act, apl)) return;
  }
  // loop did not find this state/action pair
  predecessors.add(new PredecessorInfo(startCode, act, apl->Duplicate()));
}
      

void StateInfo::Display()
{
  // display this StateInfo record on cout
  cout << "code = " << code << " parameters = " << (*apl) << endl;
  state->Print(cout);
  // now display successors and predecessors numerically
  cout << "Successors: " << endl;
  listIterator<SuccessorInfo *> sitr(successors);
  for (sitr.init(); !sitr; ++sitr) sitr()->Display();
  cout << "Predecessors: " << endl;
  listIterator<PredecessorInfo *> pitr(predecessors);
  for (pitr.init(); !pitr; ++pitr) pitr()->Display();
}

void SuccessorInfo::Display()
{
  cout << "  " << action->name << (*apl) << " " << N << endl;
  listIterator<ResultStateInfo *> itr(resultStates);
  for (itr.init(); !itr; ++itr) {
    cout << "    ";
    itr()->Display();
    cout << endl;
  }
}

void ResultStateInfo::Display()
{
  cout << "code = " << code << " N = " << N << " reward = " <<
    totalReward;
}

void PredecessorInfo::Display()
{
  cout << "code = " << idnum << " " << action->name << (*apl) << endl;
}

float SuccessorInfo::findResultProbability(StateHashCode code)
{
  // The action indicated by our ResultInfo record resulted in
  // arriving at the state indicated by code.
  int resultCount = 0;   // number of times we arrived at state/apl
  
  listIterator<ResultStateInfo *> itr(resultStates);
  for (itr.init(); !itr; ++itr) {
    if (itr()->code == code) {
      return (float) itr()->N / (float) N;
    }
  }
  cerr << "findResultProbability: Unable to find desired resulting state";
  abort();
}
    


