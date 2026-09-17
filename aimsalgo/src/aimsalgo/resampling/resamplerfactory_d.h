
#ifndef AIMS_RESAMPLING_RESAMPLERFACTORY_D_H
#define AIMS_RESAMPLING_RESAMPLERFACTORY_D_H

#include <aims/resampling/resamplerfactory.h>

#include <aims/resampling/nearestneighborresampler.h>
#include <aims/resampling/linearresampler.h>
#include <aims/resampling/majoritylabelresampler.h>
#include <aims/resampling/medianresampler.h>
#include <aims/resampling/quadraticresampler.h>
#include <aims/resampling/cubicresampler.h>
#include <aims/resampling/quarticresampler.h>
#include <aims/resampling/quinticresampler.h>
#include <aims/resampling/sixthorderresampler.h>
#include <aims/resampling/seventhorderresampler.h>
#include <iostream>


namespace {
  template <class T, template<typename U> class ResamplerType>
  std::unique_ptr<aims::Resampler<T> > make_ptr(ResamplerType<T>* obj) {
    return std::unique_ptr<aims::Resampler<T> >(obj);
  }
} // anonymous namespace

namespace aims
{

  template <typename T>
  std::unique_ptr<Resampler<T> >
  ResamplerFactory<T>::getResampler( int order )
  {
    switch( order )
      {
      case 0:
        return make_ptr(new NearestNeighborResampler<T>);
      case 1:
        return make_ptr(new LinearResampler<T>);
      case 2:
        return make_ptr(new QuadraticResampler<T>);
      case 3:
        return make_ptr(new CubicResampler<T>);
      case 4:
        return make_ptr(new QuarticResampler<T>);
      case 5:
        return make_ptr(new QuinticResampler<T>);
      case 6:
        return make_ptr(new SixthOrderResampler<T>);
      case 7:
        return make_ptr(new SeventhOrderResampler<T>);
      case 101:
        return make_ptr(new MajorityLabelResampler<T>);
      case 201:
        return make_ptr(new MedianResampler<T>);
      default:
        std::cerr << "no resampler for order " << order
                  << ", taking order 3 (cubic) instead" << std::endl;
      }
    return make_ptr(new CubicResampler<T>);
  }

} // namespace aims

#endif
