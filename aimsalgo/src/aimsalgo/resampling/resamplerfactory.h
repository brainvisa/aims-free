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


#ifndef AIMS_RESAMPLING_RESAMPLERFACTORY_H
#define AIMS_RESAMPLING_RESAMPLERFACTORY_H

#include <memory>

#include <aims/resampling/resampler.h>

namespace aims
{

  template <typename T>
  class ResamplerFactory
  {
  public:
    enum ResamplerType {
      NearestNeighbor = 0,
      Linear = 1,
      Quadratic = 2,
      Cubic = 3,
      Quartic = 4,
      Quintic = 5,
      SixthOrder = 6,
      SeventhOrder = 7,
      MajorityLabel = 101,
      Median = 201,
    };

    /** Instantiate a Resampler of the given order

    \param order order of interpolation: 0 is nearest neighbour (no
                 interpolation), 1 is linear, 3 is cubic, etc. up to 7th order.
    \return a new instance of the selected resampler type
    */
    static std::unique_ptr<Resampler<T> > getResampler( int order );

  };

} // namespace aims

#endif // !defined(AIMS_RESAMPLING_RESAMPLERFACTORY_H)
