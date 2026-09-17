
#ifndef AIMS_PRIMALSKETCH_PERONAMALIKSMOOTHER_H
#define AIMS_PRIMALSKETCH_PERONAMALIKSMOOTHER_H

#include <aims/primalsketch/imageSmoother.h>

namespace aims
{

  template<class T> class PeronaMalikSmoother
    : public Smoother<carto::VolumeRef<T>, carto::VolumeRef<T> >
  {

  private:

    float _K;     // control of the gradient - proportion  of gradients which are NOT edges - generally around 0.98
    float _gradK; // the actual gradient bound, computed from K and the image to be processed
    float _sigma; // regularisation parameter (i.e. smoothing of the gradient)
    float _dt;

    int _conductance; // vaut 1, ou 2, ou autrre si on rajoute des fonctions de conductance

    inline float conductance(float x)
    {
      switch (_conductance)
      {
        case 1: return(conductance1(x));
        case 2: return(conductance2(x));
        default : exit(EXIT_FAILURE);
      }
    }

    inline float conductance1(float x)
    { return (1.0/float(1.0+(x*x)/(_gradK*_gradK))); }
    inline float conductance2(float x)
    { return (exp(-(x*x)/(_gradK*_gradK))); }

    void SetDt(float dt)
    {
      if (dt<=0.25) _dt=dt;
      else
      {
        std::cerr << "Diffusion Smoother : dt must be <= 0.25" << std::endl;
        exit(EXIT_FAILURE);
      }
    }

  public:

    PeronaMalikSmoother(float dt, float K, float sigma, int cond)
            : _K(K), _sigma(sigma), _conductance(cond) {SetDt(dt);}

    carto::VolumeRef<T> doSmoothing( const carto::VolumeRef<T> & ima,
                                     int maxiter, bool verbose=false);

    float dt() {return _dt;}
    bool optimal() {return true;}

  };

}

#endif
