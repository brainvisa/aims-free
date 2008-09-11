/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#include <aims/neuralnet/multilayerperceptron.h>
#include <aims/data/data.h>
#include <aims/math/random.h>
#include <aims/vector/vector.h>
#include <aims/math/mathelem.h>
#include <aims/io/writer.h>
#include <iostream>

using namespace aims;
using namespace std;

//----------------------- class of elementary neuron -------------------------
AimsMLPNeuron::AimsMLPNeuron(int ninputs)
{ _nInputs = ninputs;
  _deltaW  = new AimsData<float>(ninputs);
  _weight  = new AimsData<float>(ninputs);
  *_deltaW = 0.0;
  int x;
  ForEach1d( (*_weight), x)
    (*_weight)(x) = UniformRandom(-0.1,0.1);
}


AimsMLPNeuron::~AimsMLPNeuron()
{ delete(_weight);
  delete(_deltaW);
}


int AimsMLPNeuron::nInputs() const
{ return(_nInputs);
}


float AimsMLPNeuron::output() const
{ return(_output);
}


float AimsMLPNeuron::activation() const
{ return(_activation);
}


float AimsMLPNeuron::error() const
{ return(_error);
}


float AimsMLPNeuron::deltaW(int i) const
{ return((i<_nInputs) ? (*_deltaW)(i) : (*_deltaW)(0));
}


float AimsMLPNeuron::weight(int i) const
{ return((i<_nInputs) ? (*_weight)(i) : (*_weight)(0));
}


void AimsMLPNeuron::setWeight(const AimsData<float> &weight)
{ ASSERT(weight.dimX()==_nInputs);
  for (int k=0;k<weight.dimX();k++)
    (*_weight)(k) = weight(k);
}


void AimsMLPNeuron::forward(const AimsData<float> &input)
{ _output = 0.0;
  for (int k=0;k<input.dimX();k++)
    _output += input(k) * (*_weight)(k);
  _activation = (float)AimsSigmoid((double)_output);
}


void AimsMLPNeuron::evaluateError(float target)
{ _error = 2.0 * _activation * (target - _activation) * 
           (1.0 - _activation);
}


void AimsMLPNeuron::backPropagation(const AimsData<float> &nexterror,
                                    const AimsData<float> &nextweight)
{ _error  = 0.0;
  for (int n=0;n<nexterror.dimX();n++)
    _error += nextweight(n) * nexterror(n);
  _error *= _activation * (1.0 - _activation);
}


void AimsMLPNeuron::adjust(const AimsData<float> &input,
                           float learningrate,
                           float momentum)
{ for (int k=0;k<_nInputs;k++)
  { (*_deltaW)(k)  = learningrate * _error * input(k) + 
                     momentum     * (*_deltaW)(k);
    (*_weight)(k) += (*_deltaW)(k);
  }
}


//------------------------ class of a layer of neurons -----------------------
AimsMLPLayer::AimsMLPLayer(int nneurons,int ninputs)
{ _nNeurons = nneurons;
  _neuron   = new AimsMLPNeuron *[nneurons];
  for (int n=0;n<nneurons;n++)
    _neuron[n] = new AimsMLPNeuron(ninputs);
}


AimsMLPLayer::~AimsMLPLayer()
{ for (int n=0;n<_nNeurons;n++)
    delete(_neuron[n]);
  delete(_neuron);
}


int AimsMLPLayer::nNeurons() const
{ return(_nNeurons);
}


AimsData<float> AimsMLPLayer::activations() const
{ AimsData<float> activations(_nNeurons);

  for (int n=0;n<_nNeurons;n++)
   activations(n) = _neuron[n]->activation();

  return(activations);
}


AimsData<float> AimsMLPLayer::outputs() const
{ AimsData<float> outputs(_nNeurons);

  for (int n=0;n<_nNeurons;n++)
   outputs(n) = _neuron[n]->output();

  return(outputs);
}


AimsData<float> AimsMLPLayer::errors() const
{ AimsData<float> errors(_nNeurons);

  for (int n=0;n<_nNeurons;n++)
   errors(n) = _neuron[n]->error();

  return(errors);
}


AimsData<float> AimsMLPLayer::nextWeights(int i) const
{ AimsData<float> nextweights(_nNeurons);

  for (int n=0;n<_nNeurons;n++)
   nextweights(n) = _neuron[n]->weight(i);

  return(nextweights);
}


void AimsMLPLayer::setWeight(const AimsData<float> &weight,
                             int neuron)
{ _neuron[neuron]->setWeight(weight);
}


void AimsMLPLayer::forward(const AimsData<float> &input)
{ for (int n=0;n<_nNeurons;n++)
    _neuron[n]->forward(input);
}


float AimsMLPLayer::evaluateError(const AimsData<float> &target)
{ float error=0.0;

  for (int n=0;n<_nNeurons;n++)
  { _neuron[n]->evaluateError(target(n));
    error += sqr(target(n) - _neuron[n]->activation());
  }
  return(error);
}


void AimsMLPLayer::backPropagation(const AimsMLPLayer &nextlayer)
{ for (int n=0;n<_nNeurons;n++)
    _neuron[n]->backPropagation(nextlayer.errors(),nextlayer.nextWeights(n));
}


void AimsMLPLayer::adjust(const AimsData<float> &input,
                          float learningrate, float momentum)
{ for (int neuron=0;neuron<_nNeurons;neuron++)
    _neuron[neuron]->adjust(input,learningrate,momentum);
}


//----------------------------  class of network ------------------------------
AimsMultilayerPerceptron::AimsMultilayerPerceptron(
                               const AimsData<int32_t> &topology,
                               int ninputs,
                               float learningrate,
                               float momentum,
                               float maxerror,
                               int stabilitytime,
                               int maxtime)
{ _topology      = new AimsData<int32_t>(topology);
  _nLayers       = topology.dimX();  //  there are *_topology[i] neurons 
                                     //  on layer i
  _nInputs       = ninputs;
  _nOutputs      = topology[_nLayers];
  _learningRate  = learningrate;
  _momentum      = momentum;
  _maxError      = maxerror;
  _stabilityTime = stabilitytime;
  _maxTime       = maxtime;

  _layer         = new AimsMLPLayer *[_nLayers];
  _layer[0]      = new AimsMLPLayer(topology[0],_nInputs);
  for (int layer=1;layer<_nLayers;layer++)
     _layer[layer] = new AimsMLPLayer(topology[layer],
                                      _layer[layer-1]->nNeurons());
}


AimsMultilayerPerceptron::AimsMultilayerPerceptron(
                               const AimsData<int32_t> &topology,
                               const AimsData<float> &weights,
                               int ninputs,
                               float learningrate,
                               float momentum,
                               float maxerror,
                               int stabilitytime,
                               int maxtime)
{ int size;

  _topology      = new AimsData<int32_t>(topology);
  _nLayers       = topology.dimX();  //  there are *_topology[i] neurons 
                                     //  on layer i
  _nInputs       = ninputs;
  _nOutputs      = topology[_nLayers];
  _learningRate  = learningrate;
  _momentum      = momentum;
  _maxError      = maxerror;
  _stabilityTime = stabilitytime;
  _maxTime       = maxtime;

  _layer         = new AimsMLPLayer *[_nLayers];
  _layer[0]      = new AimsMLPLayer(topology[0],_nInputs);
  int		layer;
  for (layer=1;layer<_nLayers;layer++)
    _layer[layer] = new AimsMLPLayer(topology[layer],
                                     _layer[layer-1]->nNeurons());

  for (layer=0;layer<_nLayers;layer++)
    for (int neuron=0;neuron<_layer[layer]->nNeurons();neuron++)
    { if (layer==0) size = _nInputs;
      else          size = _layer[layer-1]->nNeurons();
      AimsData<float> weight(size);
      for (int k=0;k<size;k++)
        weight(k) = weights(layer,neuron,k);
      _layer[layer]->setWeight(weight,neuron);
    }
}



AimsMultilayerPerceptron::~AimsMultilayerPerceptron()
{ for (int layer=0;layer<_nLayers;layer++)
    delete(_layer[layer]);
  delete(_layer);
}


AimsData<int32_t> AimsMultilayerPerceptron::topology() const
{ return(*_topology);
}


int AimsMultilayerPerceptron::nLayers() const
{ return(_nLayers);
}


int AimsMultilayerPerceptron::nInputs() const
{ return(_nInputs);
}


int AimsMultilayerPerceptron::nOutputs() const
{ return(_nOutputs);
}


float AimsMultilayerPerceptron::learningRate() const
{ return(_learningRate);
}


float AimsMultilayerPerceptron::momentum() const
{ return(_momentum);
}


float AimsMultilayerPerceptron::maxError() const
{ return(_maxError);
}


int AimsMultilayerPerceptron::stabilityTime() const
{ return(_stabilityTime);
}


int AimsMultilayerPerceptron::maxTime() const
{ return(_maxTime);
}


void AimsMultilayerPerceptron::setLearningRate(float learningrate)
{ ASSERT(learningrate>(float)0);
  _learningRate = learningrate;
}


void AimsMultilayerPerceptron::setMomentum(float momentum)
{ ASSERT(momentum>(float)0);
  _momentum = momentum;
}


void AimsMultilayerPerceptron::setMaxError(float maxerror)
{ ASSERT(maxerror>(float)0);
  _maxError = maxerror;
}


void AimsMultilayerPerceptron::setStabilityTime(int stabilitytime)
{ _stabilityTime = (stabilitytime==0) ? 1 : stabilitytime;
}


void AimsMultilayerPerceptron::setMaxTime(int maxtime)
{ _maxTime = (maxtime<=_stabilityTime) ? _stabilityTime : maxtime;
}


void AimsMultilayerPerceptron::learn(const AimsData<float> &base,
                                     const AimsData<float> &target,
                                     int  counter)
{ int         time=0,q;
  float        error;
  AimsData<float> input(_nInputs);
  AimsData<float> targ (_nOutputs);
  int		k;

  do
  { error = 0.0;
    for (int i=0;i<counter;i++)
    { q     = (uint)UniformRandom((double)0,(double)base.dimY()-1);
      for (k=0;k<_nInputs;k++)
        input(k) = base(k,q);
      for (k=0;k<_nOutputs;k++)
        targ(k)  = target(k,q);
    
      forward(input);
      error += _layer[_nLayers-1]->evaluateError(targ);
      backPropagation();
      adjust(input);
    }
    error /= (float)counter * (float)_nOutputs;
    if (error<=_maxError) time++;
    else                  time = 0;
    cout << "error = " << error << "\n";
  }
  while (error>_maxError || (error<=_maxError && time<_stabilityTime));
}


void AimsMultilayerPerceptron::forward(const AimsData<float> &input)
{ _layer[0]->forward(input);
  for (int layer=1;layer<_nLayers;layer++)
    _layer[layer]->forward(_layer[layer-1]->activations());
}


void AimsMultilayerPerceptron::backPropagation()
{ for (int layer=_nLayers-1;layer--;)
    _layer[layer]->backPropagation(*_layer[layer+1]);
}


void AimsMultilayerPerceptron::adjust(const AimsData<float> &input)
{ _layer[0]->adjust(input,_learningRate,_momentum);
  for (int layer=1;layer<_nLayers;layer++)
    _layer[layer]->adjust(_layer[layer-1]->activations(),
                          _learningRate,_momentum);
}


AimsData<float> AimsMultilayerPerceptron::test(const AimsData<float> &base)
{ ASSERT(base.dimX() == _nInputs);
  AimsData<float> result(_nOutputs,base.dimY());
  AimsData<float> input(base.dimX());
  int		k;

  for (int i=0;i<base.dimY();i++)
  { for (k=0;k<_nInputs;k++)
      input(k) = base(k,i);
    forward(input);
    AimsData<float> act(_layer[_nOutputs-1]->activations());
    for (k=0;k<_nOutputs;k++)
      result(k,i) = act(k);
  }  
  return(result);
}


AimsData<float> AimsMultilayerPerceptron::weights() const
{ int tmp = _topology->maximum();
  AimsData<float> weights(_topology->dimX(),
                          tmp,
                          max(_nInputs,tmp));
  AimsData<float> nextweights(1);

  for (int x=0;x<_nLayers;x++)
    for (int y=0;y<_layer[x]->nNeurons();y++)
    { nextweights = _layer[x]->nextWeights(y);
      for (int z=0;z<nextweights.dimX();z++)
        weights(x,y,z) = nextweights(z);
    }
  return(weights);
}


void AimsMultilayerPerceptron::save(const string &filetopology,
                                    const string &fileweight)
{ AimsData<float> weights(weights());

  Writer<AimsData<int32_t> > writer1(filetopology);
  writer1 << *_topology;

  Writer<AimsData<float> > writer2(fileweight);
  writer2 << weights;
}
