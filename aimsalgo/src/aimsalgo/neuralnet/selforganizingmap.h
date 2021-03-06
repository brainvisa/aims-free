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


#ifndef AIMS_NEURALNET_SELFORGANIZINGMAP_H
#define AIMS_NEURALNET_SELFORGANIZINGMAP_H

template <class T> class AimsData;
template <class T,int D> class AimsVector;

#include <aims/config/aimsalgo_config.h>
#include <string>

/** The class for Kohonen's self organizing maps
*/
class AIMSALGO_API AimsSelfOrganizingMap
{ protected:
    /**@name Data*/
    //@{
    /// Size of the square grid
    int        _size;
    /// Number of inputs
    int        _nInputs;
    /// Number of iterations
    int        _counter;
    /// Learning rate
    float       _learningRate;
    /// Weights data
    AimsData<float> *_weight;
    //@}

    /**@name Methods*/
    //@{
    /// Get the winner neuron for a given input
    AimsVector<short,2> winner(const AimsData<float> &input);
    /** Do a learning step.
        @param input input example
        @param learn learning rate
        @param delta width of the square neighborhood of neurons to update
               around the winner
    */
    void update(const AimsData<float> &input, float learn,int delta);
    //@}

  public:
    /**@name Constructors and Destructor*/
    //@{
    /** Constructor for virgin SOM.
        @param size size of the square grid of neurons
        @param ninputs number of inputs on each neuron
        @param learningrate learning rate
    */
    AimsSelfOrganizingMap(int size, int ninputs, float learningrate);
    /** Constructor for learned SOM.
        @param weight weight data for all the neurons
        @param learningrate learning rate
    */
    AimsSelfOrganizingMap(const AimsData<float> &weight, float learningrate);
    /// Destructor deletes the weight data
    virtual ~AimsSelfOrganizingMap();
    //@}

    /**@name Methods*/
    //@{    
    /// Return the size of the network
    int size()         const;
    /// Return the number of inputs of the network on each neuron
    int nInputs()      const;
    /// Return the number of epochs during the learning step
    int counter()      const;
    /// Return the learning rate of the network
    float learningRate() const;
    /// Set the learning rate of the network
    void  setLearningRate(float learningrate);

    /** Send a learning step from a data base.
        @param base the base is a 2D-data where the x coordinate corresponds
                    to the index of the example and the y coordinate corresponds
                    to the input of the neuron
        @param counter number of epoch to iterate the process
    */
    void learn(const AimsData<float> &base,int counter);
    /** Test a data base and return the result.
        @param base the base is a 2D-data where the x coordinate corresponds
                    to the index of the example and the y coordinate corresponds
                    to the input of the neuron
        @return a 2D-data containing the winner in the grid for each example
                of the base
    */
    AimsData< AimsVector<short,2> > test(const AimsData<float> &base);

    /// Save all the weights in a GIS file
    void save(const std::string &filename);
    //@}
};


#endif
