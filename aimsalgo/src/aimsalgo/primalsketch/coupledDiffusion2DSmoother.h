
#ifndef AIMS_PRIMALSKETCH_COUPLEDDIFFUSION2DSMOOTHER_H
#define AIMS_PRIMALSKETCH_COUPLEDDIFFUSION2DSMOOTHER_H

#include <aims/primalsketch/imageCoupledSmoother.h>
#include <aims/signalfilter/g2dsmooth.h>

namespace aims
{

  template<typename T> class CoupledDiffusion2DSmoother
    : public CoupledSmoother<carto::VolumeRef<T>, carto::VolumeRef<T> >
  {
  private:
    float _dt;
    float _alpha;
    float _beta;
    float _gamma;

  public:

    CoupledDiffusion2DSmoother( float delta_t, float alpha=1.0f,
                                float beta=1.0f, float gamma=1.0f )
      : _alpha(alpha), _beta(beta), _gamma(gamma)
    { SetDt(delta_t); }
    void SetDt(float Delta_t)
    {
      if (Delta_t<=0.25) _dt=Delta_t;
      else
      {
        std::cerr << "Diffusion Smoother : dt must be <= 0.25" << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    float dt() {return _dt;}   //smooooth operatooor...

    std::pair<carto::VolumeRef<T>, carto::VolumeRef<T> > doSmoothing(
      const std::pair<carto::VolumeRef<T>, carto::VolumeRef<T> > & ima,
      const std::pair<carto::VolumeRef<T>, carto::VolumeRef<T> > & constraint,
      int maxiter, bool verbose=false );

    bool optimal() {return true;}

  };
}

#endif
