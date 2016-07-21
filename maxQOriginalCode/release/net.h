// -*- C++ -*-
//
// Backpropagation neural network
//
// Not optimized.
//

#ifndef NET_H
#define NET_H

#include <iostream.h>

// Tim Budd's vectors
#include <vector.h>
#include <clrandom.h>

class SigmoidNetwork
{
public:
  // types of units
  enum UnitType {Linear, Sigmoid, Gaussian};

  // constructor and destructor
  SigmoidNetwork(int inputs, int hidden, int outputs,
		 UnitType htype = Sigmoid,
		 UnitType otype = Sigmoid);
  virtual ~SigmoidNetwork();
  virtual void AllocateSpace();

  virtual void Init(clrandom & r);		// initialize the weights
					// using random number generator
  virtual void ForwardPass();
  virtual void ComputeErrors();	
  virtual void BackwardPass();
  virtual void Step(float stepsize); // take a step in the negative
				     // gradient direction.

  virtual void SetInputs(vector<float> & v);  // set all of the inputs
  virtual void SetInput(int i, float v);      // set one of the inputs
  virtual float GetInput(int i);	      // get one of the inputs
  virtual void GetInputs(vector<float> & v);  // get all of the inputs
  virtual float SetTarget(int i, float v);     // set one of the targets
  virtual float GetOutput(int i);	      // get one of the outputs
  virtual float GetError(int i);	      // get an error value
  virtual void AdjustBiases();		      

  virtual void ClearCumGradients();        // fill cum grads with zero
  virtual void AccumulateGradients();      // add grads into cum grads
  virtual void InsertCumGradients();       // copy cum grads to grads
  
friend ostream & operator << (ostream &, SigmoidNetwork &);
friend istream & operator >> (istream &, SigmoidNetwork &);

  unsigned int NInputs;		// sizes
  unsigned int NHidden;
  unsigned int NOutputs;

  UnitType hiddenType;		// type of hidden units
  UnitType outputType;		// type of output units
protected:
  vector<float> targets;	// Output values: length NOutputs

  vector<vector<float> *> hidden; // input->hidden weights.  length NHidden
				// each element is a vector of length 
				// NInputs+1

  vector<vector<float> *> outputs; // hidden->output weights.  length NOutputs
			        // each element is a vector of length NHidden+1
  
  vector<float> activations;	// One activation value for each unit
				// including input units and output
				// units and 2 bias units (one in the
				// input layer and one in the hidden
				// layer).

  vector<float> sums;		// weighted sums of input to each
				// unit.  The activations are the
				// squashed versions of these inputs.

  vector<float> delta;		// Delta values backpropagated
				// One element for each hidden unit
				// and output unit.

  vector<vector<float> *> hiddenGradient; // input->hidden gradients.
					// 1:1 with hidden weights
  vector<vector<float> *> outputGradient; // hidden->output gradients
					// 1:1 with output weights

  vector<vector<float> *> cumHiddenGradient; // cumulative gradients
  vector<vector<float> *> cumOutputGradient; // cumulative gradients

  vector<float> errors;		// Error values to propagate backward

  float Logistic(float);	// Logistic transfer function.
  virtual void ReadWeights(istream & str);  // read in the weights
  virtual void ForwardPassHiddens();
  virtual void ForwardPassOutputs();
  virtual void BackwardPassOutputs();  // internal routines for BackwardPass
  virtual void BackwardPassHiddens();  // internal routines for BackwardPass

};

ostream & operator << (ostream &, SigmoidNetwork &);
istream & operator >> (istream &, SigmoidNetwork &);


#endif
