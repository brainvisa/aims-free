

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
