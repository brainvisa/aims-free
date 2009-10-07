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


#ifndef AIMS_RESAMPLING_SPLINERESAMPLER_H
#define AIMS_RESAMPLING_SPLINERESAMPLER_H

#include <aims/resampling/resampler.h>

template <class T>
class SplineResampler : public Resampler< T >
{
public:

  SplineResampler();
  ~SplineResampler() { }

  virtual int getOrder() const = 0;
  AimsData<double> getSplineCoef( const AimsData< T >& inVolume,
				  int t = 0,
				  bool verbose = false );

  /** be carreful; spline resampler need to pre-process spline coefficients;
      in order to be more efficient, they are calculated with a recursive
      scheme from input reference volume; this recursive algorithm is much 
      faster when you have to resample a whole volume, but it is slower
      when you only need to resample one point; so a cache mechanism has been
      implemented in order to process those coefficients only once,
      provided the input inVolume has not changed, or a call to reset() 
      function has not been sent.
      This reset() function force recalculation of the spline coefficient set.
  */
  void reset();

protected:

  void 
  doResample( const AimsData< T > &inVolume, 
              const Motion &transform3d, 
              const T &outBackground, const Point3df &outLocation, 
              T &outValue, int t );

  void updateParameters( const AimsData< T >& inVolume, int t, 
                         bool verbose );
  void iirConvolveMirror( std::vector< double >& data ) const;
  int getFold( int i, int size ) const;
  virtual double getBSplineWeight( int i, double x ) const = 0;

  std::vector< double >	_poles;
  double		_gain;
  AimsData< double >	_splineCoefficients;
  const AimsData<T>	*_lastvolume;
  int			_lasttime;
};


#endif

