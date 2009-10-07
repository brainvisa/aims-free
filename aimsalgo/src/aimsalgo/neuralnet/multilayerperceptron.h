/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
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
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#ifndef AIMS_NEURALNET_MULTILAYERPERCEPTRON_H
#define AIMS_NEURALNET_MULTILAYERPERCEPTRON_H

#include <aims/config/aimsalgo_config.h>
#include <string>

template <class T> class AimsData;
template <class T,int D> class AimsVector;


/** The class that describes MLP neuron.
*/
class AIMSALGO_API AimsMLPNeuron
{ protected:
    /**@name Data*/
    //@{
    /// Number of inputs
    int _nInputs;
    /// Floating output
    float _output;
    /// Activation value (before entering sigmoid function)
    float _activation;
    /// Error between desired and actual value
    float _error;
    /// Weight variation data
    AimsData<float> *_deltaW;
    /// Weight data
    AimsData<float> *_weight;
    //@}

  public:
    /**@name Constructor and Destructor*/
    //@{
    /** Constructor
        @param ninputs number of inputs of the neuron
    */
    AimsMLPNeuron(int ninputs);
    /// Destructor deletes allocated data
    virtual ~AimsMLPNeuron();
    //@}

    /**@name Methods*/
    //@{
    /// Return the number of inputs
    int nInputs()    const;
    /// Return the actual output
    float output()     const;
    /// Return the activation
    float activation() const;
    /// Return the error
    float error()      const;
    /// Return the variation of weights
    float deltaW(int i) const;
    /// Return the ieme weight
    float weight(int i) const;
    /// Set the weights from weight vector
    void  setWeight(const AimsData<float> &weight);

    /// Forward propagation
    void forward(const AimsData<float> &input);
    /// Error evaluation
    void evaluateError(float target);
    /// Gradient back propagation
    void backPropagation(const AimsData<float> &nexterror,
                         const AimsData<float> &nextweight);
    /// Modification of the weights
    void adjust(const AimsData<float> &input,
                float learningrate,
                float momentum);
    //@}
};


/** The class to manage MLP layers of neurons
*/
class AIMSALGO_API AimsMLPLayer
{ protected:
    /**@name Data*/
    //@{
    /// Number of neurons
    int           _nNeurons;
    /// Pointer to neuron pointers
    AimsMLPNeuron **_neuron;
    //@}

  public:
    /**@name Constructor and Destructor*/
    //@{
    /** Two arguments must be given to the constructor
        @param nneurons number of neurons in the layer
        @param ninputs number of inputs on each neuron
    */
    AimsMLPLayer(int nneurons,int ninputs);
    /// Destructor deletes the pointer of neuron pointers
    virtual ~AimsMLPLayer();
    //@}

    /**@name Methods*/
    //@{
    /// Return the number of neurons in the layer
    int nNeurons() const;
    /// Return the vector of activations
    AimsData<float> activations() const;
    /// Return the vector of outputs
    AimsData<float> outputs() const;
    /// Return the vector of errors
    AimsData<float> errors() const;
    /// Return the weights coming from input n
    AimsData<float> nextWeights(int n) const;
    /// Set the weights of a specific neuron of the layer
    void setWeight(const AimsData<float> &weight, int neuron);

    /// Forward propagation on the layer
    void  forward(const AimsData<float> &input);
    /// Evaluation of the error of the layer for a given target
    float evaluateError(const AimsData<float> &target);
    /// Gradient back propagation on the layer
    void  backPropagation(const AimsMLPLayer &nextlayer);
    /// Modification of the weights
    void adjust(const AimsData<float> &input,
                float learningrate, float momentum);
    //@}
};


/** The class for a complete MultiLayer Perceptron.
*/
class AIMSALGO_API AimsMultilayerPerceptron
{ protected:
    /**@name Data*/
    //@{
    /// Number of layers
    int        _nLayers;
    /// Number of inputs for each neuron
    int        _nInputs;
    /// Number of outputs of the network
    int        _nOutputs;
    /// Learning rate
    float        _learningRate;
    /// Momentum coefficient
    float        _momentum;
    /// Maximum error to consider the network learned
    float        _maxError;
    /// Minimum number of epochs for the network to be considered stable
    int        _stabilityTime;
    /// Maximum of number of epochs for the learning step
    int        _maxTime;
    /// Pointer to layer pointers
    AimsMLPLayer **_layer;
    /// Topology of the network, i.e. number of neurons for each layer
    AimsData<int32_t> *_topology;
    //@}

  public:
    /**@name Constructors and Destructor*/
    //@{
    /** The constructor needs 7 parameters
        @param topology data containing the number of neurons for each layer
        @param ninputs number of inputs per neuron
        @param learningrate learning rate
        @param momentum momentum coefficient
        @param maxerror maximum error to consider the network learned
        @param stabilitytime minimum number of epochs for the network 
                             to be considered stable
        @param maxtime maximum of number of epochs for the learning step
    */
    AimsMultilayerPerceptron(const AimsData<int32_t> &topology, int ninputs,
                             float learningrate, float momentum,
                             float maxerror, int stabilitytime, int maxtime);
    /** The constructor needs 8 parameters
        @param topology data containing the number of neurons for each layer
        @param weights neuron weights
        @param ninputs number of inputs per neuron
        @param learningrate learning rate [default=0]
        @param momentum momentum coefficient [default=0]
        @param maxerror max error to consider the network learned [default=0]
        @param stabilitytime minimum number of epochs for the network 
                             to be considered stable [defaul=0]
        @param maxtime maximum of number of epochs for the learning step
                       [default=0]
    */
    AimsMultilayerPerceptron(const AimsData<int32_t> &topology,
                             const AimsData<float> &weights,
                             int ninputs, float learningrate=0,
                             float momentum=0, float maxerror=0,
                             int stabilitytime=0, int maxtime=0);
    /// Destructor deletes all the layers
    virtual ~AimsMultilayerPerceptron();
    //@}

    /**@name Methods*/
    //@{
    /// Return the topology of the MLP network
    AimsData<int32_t> topology() const;
    /// Return the number of layers of the network
    int nLayers() const;
    /// Return the number of inputs of the network
    int nInputs() const;
    /// Return the number of outputs of the network
    int nOutputs() const;
    /// Return the learning rate of the network
    float learningRate() const;
    /// Return the momentum of the network
    float momentum() const;
    /// Return the maximum allowed error for convergence
    float maxError() const;
    /// Return the minimum time for the net to be stable
    int stabilityTime() const;
    /// Return the maximum time for learning step
    int maxTime() const;
    /// Set the learning rate of the network
    void setLearningRate(float learningrate);
    /// Set the momentum of the network
    void setMomentum(float momentum);
    /// Set the maximum allowed error for convergence
    void setMaxError(float maxerror);
    /// Set the minimum time for the net to be stable
    void setStabilityTime(int stabilitytime);
    /// Set the maximum time for learning step
    void setMaxTime(int maxtime);

    /// Send a learning operation on a data base with a target
    void learn(const AimsData<float> &base,
               const AimsData<float> &target,
               int counter);
    /// Forward propagation
    void forward(const AimsData<float> &input);
    /// Gradient backward propagation
    void backPropagation();
    /// Modification of the weights
    void adjust(const AimsData<float> &input);
    /// Test a data base and return the result
    AimsData<float> test(const AimsData<float> &base);

    /// Return all the weights of all the neurons in a volume
    AimsData<float> weights() const;

    /// Save the network to both G.I.S. files
    void save(const std::string &filetopology,
              const std::string &fileweight);
    //@}
};


#endif
