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



#ifndef AIMS_FINITE_ELEMENT_SMOOTHER_H
#define AIMS_FINITE_ELEMENT_SMOOTHER_H

#include <aims/primalsketch/surfaceSmoother.h>
#include <aims/mesh/curv.h>
#include <map>
#include <set>
#include <utility>

namespace aims
{

  template<int D, typename T> class FiniteElementSmoother 
    : public Smoother<AimsSurface<D, Void>, Texture<T> >
  {

  private:

    std::map<unsigned, std::set< std::pair<unsigned,float> > >  weightLapl; // map (index, ensemble des pair(voisins,poids))

    void ComputeWeights( float threshold = 0.98) { 
        weightLapl = AimsMeshWeightFiniteElementLaplacian( * this->_mesh , threshold ); 
        }
    float _dt;

  public:

    FiniteElementSmoother( float dt, AimsSurface<D, Void> *mesh )
      : _dt(dt) { 
        this->_mesh = mesh; 
        ComputeWeights(); 
        }

    FiniteElementSmoother ( float dt,
                            AimsSurface<D, Void> *mesh,
                            std::map<unsigned, std::set< std::pair<unsigned,float> > >
                                 &previouslyComputedWeights )
      : _dt(dt) {
        this->_mesh=mesh;
        this->weightLapl = previouslyComputedWeights;
    }
    std::map<unsigned, std::set< std::pair<unsigned,float> > > getWeights () { return weightLapl;}
    virtual ~FiniteElementSmoother() {}
    void SetDt(float dt) { _dt = dt; }

    float dt() { return _dt; }                                                                 //smooooth operatooor..

    Texture<T> doSmoothing(const Texture<T> & ima, int maxiter, bool verbose=true);

    bool optimal() { return true; }
  };


}

#endif
