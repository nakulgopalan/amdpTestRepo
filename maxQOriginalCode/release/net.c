//   -*-  C++  -*-
//
// Neural network implementation.
// Only three layers.
// Question:  Where does the negative sign enter into things (to force
// gradient descent)?
//

#include "net.h"
#include <math.h>

SigmoidNetwork::SigmoidNetwork(int nin, int nhid, int nout,
			       UnitType htype, UnitType otype):
     NInputs(nin),
     NHidden(nhid),
     NOutputs(nout),
     hiddenType(htype),
     outputType(otype)
{
  AllocateSpace();
}

void SigmoidNetwork::AllocateSpace()
{
  // wwe assume that nin, nhid, and nout have been set.  Now we resize
  // all of the arrays as necessary.
  targets.setSize(NOutputs);
  targets.fill(0);
  errors.setSize(NOutputs);
  errors.fill(0);
  hidden.setSize(NHidden);
  hiddenGradient.setSize(NHidden);
  cumHiddenGradient.setSize(NHidden);
  for (int i = 0; i < NHidden; i++) {
    hidden[i] = new vector<float>;
    hidden[i]->setSize(NInputs+1);
    hidden[i]->fill(0);
    hiddenGradient[i] = new vector<float>;
    hiddenGradient[i]->setSize(NInputs+1);
    hiddenGradient[i]->fill(0);
    cumHiddenGradient[i] = new vector<float>;
    cumHiddenGradient[i]->setSize(NInputs+1);
    cumHiddenGradient[i]->fill(0);
  }

  outputs.setSize(NOutputs);
  outputGradient.setSize(NOutputs);
  cumOutputGradient.setSize(NOutputs);
  for (int i = 0; i < NOutputs; i++) {
    outputs[i] = new vector<float>;
    outputs[i]->setSize(NHidden+1);
    outputs[i]->fill(0);
    outputGradient[i] = new vector<float>;
    outputGradient[i]->setSize(NHidden+1);
    outputGradient[i]->fill(0);
    cumOutputGradient[i] = new vector<float>;
    cumOutputGradient[i]->setSize(NHidden+1);
    cumOutputGradient[i]->fill(0);
  }

  activations.setSize(NInputs+1+NHidden+1+NOutputs);
  activations.fill(0);
  activations[NInputs]               = 1.0;   // bias units always outputs a 1
  activations[NInputs + 1 + NHidden] = 1.0;   
  
  sums.setSize(NHidden + NOutputs);

  delta.setSize(NHidden + NOutputs);
  delta.fill(0);
}


SigmoidNetwork::~SigmoidNetwork()
{
  // release all of the various vectors
  for (int i = 0; i < NHidden; i++) {
    delete hidden[i];
    delete hiddenGradient[i];
    delete cumHiddenGradient[i];
  }
  for (int i = 0; i < NOutputs; i++) {
    delete outputs[i];
    delete outputGradient[i];
    delete cumOutputGradient[i];
  }
}


void SigmoidNetwork::Init(clrandom & r)
{
  // initialize weights using random number generator r.

  // hidden units.
  {
    float range =  1.0/sqrt(NInputs+1.0);

    vectorIterator<vector<float> *>  itr(hidden);
    for(itr.init(); ! itr; ++itr) {
      vectorIterator<float>  jtr(*itr());
      for(jtr.init(); ! jtr; ++jtr) {
	jtr = r.between(-range, range);
      }
    }

  }

  // for output weights, the fan-in changes
  {
    float range = 1.0/sqrt(NHidden+1.0);

    vectorIterator<vector<float> *>  itr(outputs);
    for(itr.init(); ! itr; ++itr) {
      vector<float> & w = *itr();
      int j;
      float sum = 0.0;
      // 
      for(j = 0;  j < NHidden; j++) {
	w[j] = r.between(-range, range);
	sum += w[j];
      }
      // set the bias unit.  We assume that on the average, half of
      // the inputs will be on. So we set the bias weight to be -sum/2
      w[NHidden] = -sum/2.0;
    }
  }
}

inline float SigmoidNetwork::Logistic(float arg)
{
  return 1.0/(1.0 + exp(- arg));
}


void SigmoidNetwork::ForwardPass()
{
  ForwardPassHiddens();
  ForwardPassOutputs();
}

void SigmoidNetwork::ForwardPassHiddens()
{

  int i, ihidden;
  // Compute the activations vector

  // Hidden unit activations

  for (ihidden = 0; ihidden < NHidden; ihidden++) {
    vectorIterator<float> ithuw(*(hidden[ihidden]));
    float sum = 0.0;
    for (i = 0, ithuw.init();
	 i < NInputs+1 && ! ithuw;
	 i++, ++ithuw) {
      sum += ithuw()*activations[i];
    }
    sums[ihidden] = sum;
    float act = 0.0;
    switch (hiddenType)
      {
      case Linear: {act = sum; break;}
      case Sigmoid: {act = Logistic(sum); break;}
      case Gaussian: {act = exp(-(sum * sum)); break;}
      }
    activations[ihidden+NInputs+1] = act;
  }
}

void SigmoidNetwork::ForwardPassOutputs()
{
  int i, iout;

  // Output unit activations

  for (iout = 0; iout < NOutputs; iout++) {
    vectorIterator<float> itout(*(outputs[iout]));
    float sum = 0.0;
    for (i = NInputs+1, itout.init();
	 i < NInputs+1+NHidden+1 && ! itout;
	 i++, ++itout) {
      sum += itout()*activations[i];
    }
    sums[NHidden + iout] = sum;
    float act = 0.0;
    switch (outputType)
      {
      case Linear: {act = sum; break;}
      case Sigmoid: {act = Logistic(sum); break;}
      case Gaussian: {act = exp(-(sum * sum)); break;}
      }
    activations[iout+NInputs+1+NHidden+1] = act;
  }
}

void SigmoidNetwork::SetInputs(vector<float> & v)
{
  for (int i=0; i < NInputs; i++) {
    activations[i] = v[i];
  }
}

void SigmoidNetwork::SetInput(int i, float v)
{
  assert(i >= 0);
  assert(i < NInputs);

  activations[i] = v;
}

float SigmoidNetwork::GetInput(int i)
{
  assert(i >= 0);
  assert(i < NInputs);
  return activations[i];
}

void SigmoidNetwork::GetInputs(vector<float> & v)
{
  for (int i = 0; i < NInputs; i++) {
    v[i] = activations[i];
  }
}

float SigmoidNetwork::GetOutput(int i)
{
  assert(i >= 0);
  assert(i < NOutputs);

  return activations[i+NInputs+1+NHidden+1];
}

float SigmoidNetwork::SetTarget(int i, float v)
{
  return targets[i] = v;
}


void SigmoidNetwork::ComputeErrors()
{
  for(int iout = 0; iout < NOutputs; iout++) {
    errors[iout] = targets[iout] - activations[NInputs+1+NHidden+1+iout];
  }
}


void SigmoidNetwork::BackwardPass()
{
  delta.fill(0);
  BackwardPassOutputs();
  BackwardPassHiddens();
}

void SigmoidNetwork::BackwardPassOutputs()
{
  // output layer
  int iout;
  int ihidden;
  float thisdelta;

  for(iout = 0; iout < NOutputs; iout++) {
    float act = activations[NInputs+1+NHidden+1+iout];
    float sum = sums[NHidden + iout];

    switch (outputType)
      {
      case Linear:
	{
	  thisdelta = errors[iout];
	  break;
	}
      case Sigmoid:
	{
	  thisdelta = errors[iout] * act * (1.0 - act);
	  break;
	}
      case Gaussian:
	{
	  thisdelta = - errors[iout] * act * sum;
	  break;
	}
      }

    delta[NHidden+iout] = thisdelta;

    // now propagate this delta value backward through each of the
    // incoming weights and compute hidden->output gradients
    vector<float> * w = outputs[iout];
    vector<float> * g = outputGradient[iout];

    // add the contribution of this output unit to the deltas of each
    // of the hidden units feeding to us.
    for (ihidden = 0; ihidden < NHidden + 1; ihidden++) {
      delta[ihidden] += (*w)[ihidden] * thisdelta;
      (*g)[ihidden] = thisdelta * activations[NInputs+1+ihidden];
    }
  }

}

void SigmoidNetwork::BackwardPassHiddens()
{
  int ihidden;
  int iinput;
  float thisdelta;

  // hidden layer.  Just compute thisdelta and gradient.

  for(ihidden = 0; ihidden < NHidden; ihidden++)  {
    float act = activations[NInputs+1+ihidden];
    float sum = sums[ihidden];

    switch (hiddenType)
      {
      case Linear:
	{
	  thisdelta = delta[ihidden];
	  break;
	}
      case Sigmoid:
	{
	  thisdelta = delta[ihidden] * act * (1.0 - act);
	  break;
	}
      case Gaussian:
	{
	  thisdelta = - delta[ihidden] * act * sum;
	  break;
	}
      }

    delta[ihidden] = thisdelta;

    vector<float> * w = hidden[ihidden];
    vector<float> * g = hiddenGradient[ihidden];

    for (iinput = 0; iinput < NInputs + 1; iinput++) {
      (*g)[iinput] = thisdelta * activations[iinput];
    }

  }
}
	 

void SigmoidNetwork::Step(float stepsize)
{
  // update the weights using the gradient
  // input->hidden weights
  for (int ihidden = 0; ihidden < NHidden; ihidden++) {
    vector<float> * h = hidden[ihidden];
    vector<float> * g = hiddenGradient[ihidden];
    for (int iinput = 0; iinput < NInputs + 1; iinput++) {
      (*h)[iinput] += stepsize * (*g)[iinput];
    }
  }
  for (int ioutput = 0; ioutput < NOutputs; ioutput++) {
    vector<float> * w = outputs[ioutput];
    vector<float> * g = outputGradient[ioutput];
    for (int ihidden = 0; ihidden < NHidden + 1; ihidden++) {
      (*w)[ihidden] += stepsize * (*g)[ihidden];
    }
  }
}

void SigmoidNetwork::ClearCumGradients()
{
  for (int ihidden = 0; ihidden < NHidden; ihidden++) {
    cumHiddenGradient[ihidden]->fill(0);
  }
  for (int ioutput = 0; ioutput < NOutputs; ioutput++) {
    cumOutputGradient[ioutput]->fill(0);
  }
}

void SigmoidNetwork::AccumulateGradients()
{
  for (int ihidden = 0; ihidden < NHidden; ihidden++) {
    vector<float> & g = *(hiddenGradient[ihidden]);
    vector<float> & cg = *(cumHiddenGradient[ihidden]);
    for (int iinput = 0; iinput < NInputs + 1; iinput++) {
      cg[iinput] += g[iinput];
    }
  }
  for (int ioutput = 0; ioutput < NOutputs; ioutput++) {
    vector<float> & g = *(outputGradient[ioutput]);
    vector<float> & cg = *(cumOutputGradient[ioutput]);
    for (int ihidden = 0; ihidden < NHidden + 1; ihidden++) {
      cg[ihidden] += g[ihidden];
    }
  }
}

void SigmoidNetwork::InsertCumGradients()
{
  for (int ihidden = 0; ihidden < NHidden; ihidden++) {
    vector<float> & g = *(hiddenGradient[ihidden]);
    vector<float> & cg = *(cumHiddenGradient[ihidden]);
    g = cg;
  }
  for (int ioutput = 0; ioutput < NOutputs; ioutput++) {
    vector<float> & g = *(outputGradient[ioutput]);
    vector<float> & cg = *(cumOutputGradient[ioutput]);
    g = cg;
  }
}

float SigmoidNetwork::GetError(int i)
{
  return errors[i];
}

void SigmoidNetwork::AdjustBiases()
{
  // assuming that each hidden unit outputs 0.5, adjust the output
  // biases to match the values in the targets array.
  // The activation going into the output unit is
  // b * 1 + (W * 0.5), where W is the hidden-to-output weight vector
  // and 0.5 is a corresponding vector of 0.5's.  b is the "bias" weight
  // that connects to activations[NInputs + 1 + NHidden].
  // Let T be the target.  For sigmoid units, the desired value for b is
  //  ln(T/(1-T)) - (W * 0.5) = b
  // For gaussian units, the desired value for b is 
  //  +/- sqrt(- ln T) - (W * 0.5)
  // We have ambiguity (unless T = 1).  We will always choose the low
  // side (the - sign). 
  // When the target is 0 or 1, we adjust it to 0.01 or 0.99.

  if (outputType == Linear) return;

  for (int iout = 0; iout < NOutputs; iout++) {
    float tar = targets[iout];
    if (tar < 0.01) tar = 0.01;
    else if (tar > 0.99) tar = 0.99;

    if (outputType == Sigmoid) tar = log(tar / (1.0 - tar));
    else if (outputType == Gaussian) tar = -sqrt(-log(tar));

    vectorIterator<float> itout(*(outputs[iout]));
    float sum = 0.0;
    int i;
    for (i = NInputs + 1, itout.init();
	 i < NInputs + 1 + NHidden && ! itout;
	 i++, ++itout) {
      sum += itout() * 0.5;
    }
    
    float bias = tar - sum;
    itout() = bias;
  }
}


ostream & operator << (ostream & str, SigmoidNetwork & net)
{
  str << net.NInputs << " " << net.NHidden << " " << net.NOutputs << "\n";
  // write out the weights from the network
  for (int ihidden = 0; ihidden < net.NHidden; ihidden++) {
    for (int iinput = 0; iinput < net.NInputs + 1; iinput++) {
      str << (*(net.hidden[ihidden]))[iinput] << " ";
    }
    str << "\n";
  }
  str << "\n";
  for (int ioutput = 0; ioutput < net.NOutputs; ioutput++) {
    for (int ihidden = 0; ihidden < net.NHidden + 1; ihidden++) {
      str << (*(net.outputs[ioutput]))[ihidden] << " ";
    }
    str << "\n";
  }
  return str;
}

void SigmoidNetwork::ReadWeights(istream & str)
{
  // read in the weights from the network
  for (int ihidden = 0; ihidden < NHidden; ihidden++) {
    for (int iinput = 0; iinput < NInputs + 1; iinput++) {
      str >> (*(hidden[ihidden]))[iinput];
    }
  }
  for (int ioutput = 0; ioutput < NOutputs; ioutput++) {
    for (int ihidden = 0; ihidden < NHidden + 1; ihidden++) {
      str >> (*(outputs[ioutput]))[ihidden];;
    }
  }
}  

istream & operator >> (istream & str, SigmoidNetwork & net)
{

  str >> net.NInputs >> net.NHidden >> net.NOutputs;
  net.hiddenType = net.outputType = SigmoidNetwork::Sigmoid;

  net.AllocateSpace();
  net.ReadWeights(str);

  return str;
}

