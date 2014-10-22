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


#ifndef AIMS_RESAMPLING_RESAMPLER_H
#define AIMS_RESAMPLING_RESAMPLER_H


#include <aims/data/data.h>
#include <aims/vector/vector.h>
#include <aims/resampling/motion.h>


template <class T>
class Resampler
{
public:

  Resampler() : _ref( 0 ), _defval( 0 ) { }
  virtual ~Resampler() { }

  virtual void doit( const Motion& motion, AimsData<T>& thing );
  virtual AimsData<T> doit( const Motion& motion, int dimX, int dimY, 
                            int dimZ, const Point3df& resolution );

  virtual void resample( const AimsData< T >& inVolume,
                         const Motion& transform3d,
                         const T& outBackground,
                         AimsData< T >& outVolume,
                         bool verbose = false );

  virtual void resample( const AimsData< T >& inVolume,
                         const Motion& transform3d,
                         const T& outBackground,
                         const Point3df& outLocation,
                         T& outValue, int time );

  // seems to be the volume to be resampled... (what a crappy API...)
  void setRef( const AimsData<T>& ref ) { _ref = &ref; }
  void setDefaultValue( T val ) { _defval = val; }

protected:

  virtual void 
  doResample( const AimsData< T > &inVolume, 
              const Motion &invTransform3d, 
              const T &outBackground, const Point3df &outLocation, 
              T &outValue, int time ) = 0;

  /// called before each time step, used in spline resamplers
  virtual void updateParameters( const AimsData< T > &inVolume, int time, 
                                 bool verbose );

  const AimsData<T>* _ref;
  T _defval;
};



#endif
