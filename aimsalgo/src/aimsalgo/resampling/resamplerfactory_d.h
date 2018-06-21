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


#ifndef AIMS_RESAMPLING_RESAMPLERFACTORY_D_H
#define AIMS_RESAMPLING_RESAMPLERFACTORY_D_H

#include <aims/resampling/resamplerfactory.h>

#include <aims/resampling/nearestneighborresampler.h>
#include <aims/resampling/linearresampler.h>
#include <aims/resampling/quadraticresampler.h>
#include <aims/resampling/cubicresampler.h>
#include <aims/resampling/quarticresampler.h>
#include <aims/resampling/quinticresampler.h>
#include <aims/resampling/sixthorderresampler.h>
#include <aims/resampling/seventhorderresampler.h>
#include <iostream>


namespace {
  template <class T, template<typename U> class ResamplerType>
  carto::rc_ptr<aims::Resampler<T> > make_rc_ptr(ResamplerType<T>* obj) {
    return carto::rc_ptr<aims::Resampler<T> >(obj);
  }
} // anonymous namespace

namespace aims
{

  template <typename T>
  const carto::rc_ptr<Resampler<T> >
  ResamplerFactory<T>::newResampler( ResamplerType order )
  {
    switch( order )
      {
      case 0:
        return make_rc_ptr(new NearestNeighborResampler<T>);
      case 1:
        return make_rc_ptr(new LinearResampler<T>);
      case 2:
        return make_rc_ptr(new QuadraticResampler<T>);
      case 3:
        return make_rc_ptr(new CubicResampler<T>);
      case 4:
        return make_rc_ptr(new QuarticResampler<T>);
      case 5:
        return make_rc_ptr(new QuinticResampler<T>);
      case 6:
        return make_rc_ptr(new SixthOrderResampler<T>);
      case 7:
        return make_rc_ptr(new SeventhOrderResampler<T>);
      default:
        std::cerr << "no resampler for order " << order
                  << ", taking order 3 (cubic) instead" << std::endl;
      }
    return make_rc_ptr(new CubicResampler<T>);
  }

  template <typename T>
  Resampler<T>* ResamplerFactory<T>::getResampler( int order )
  {
    carto::rc_ptr<Resampler<T> > r = newResampler(static_cast<ResamplerType>(order));
    return r.release();
  }

} // namespace aims

#endif
