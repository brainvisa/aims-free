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
#include <aims/transformation/transformation.h>
#include <aims/transformation/affinetransformation3d.h>


/** Resampler resamples an input data to build or fill an output data, using an affine transformation.

Normally the input data should be set in the resampler using setRef().

Resampling is done using the doit() methods.
resample() methods also provide alternatives.
*/
template <class T>
class Resampler
{
public:

  Resampler() : _ref(), _defval( T() ) { }
  virtual ~Resampler() { }

  /** Resample the reference input data (set via setRef()) into an existing
      output data.

      Parameters:

      transform: AffineTransformation3d
          transformation to apply
      output_data: AimsData<T>
          resampled data will fill this existing output data
  */
  virtual void doit( const aims::AffineTransformation3d& transform,
                     AimsData<T>& output_data ) const;

  /** Resample the reference input data (set via setRef()) into a new output
      data.

      Parameters:

      transform: transformation to apply
      dimx, dimy, dimz: number of voxels in 3 dimensions
      voxel_size: voxel size of the output data

      Returns the output resampled volume
  */
  virtual AimsData<T> doit( const aims::AffineTransformation3d& transform,
                            int dimX, int dimY,
                            int dimZ, const Point3df& voxel_size ) const;

  /** Resample the input data into an existing output data.

      Parameters:

      input_data: data to be resampled
      transform: transformation to apply
      background: value set in output regions which are outside of the
          transformation space of the input volume
      output_data: resampled data will fill this existing output data
      verbose: print more things during resampling
  */
  virtual void resample( const AimsData< T >& input_data,
                         const aims::AffineTransformation3d& transform,
                         const T& background,
                         AimsData< T >& output_data,
                         bool verbose = false ) const;

  /** Resample a single voxel of the input data at a given specified output
      location, and set the output value.

      Parameters:

      input_data: data to be resampled
      transform: transformation to apply
      background: value set in output regions which are outside of the
          transformation space of the input volume
      output_location: position in the output space (warning, this is actually
          an input)
      output_value: resampled output value
      timestep: for 4D volume, time step to be used
  */
  virtual void resample( const AimsData< T >& input_data,
                         const aims::AffineTransformation3d& transform,
                         const T& background,
                         const Point3df& output_location,
                         T& output_value, int timestep ) const;

  /// set the input data to be resampled
  virtual void setRef( const AimsData<T>& ref )
  { _ref = carto::const_ref<AimsData<T> >(&ref); }
  const carto::const_ref<AimsData<T> >& ref() const { return _ref; }
  /// set the default background value
  void setDefaultValue( T val ) { _defval = val; }
  const T& defaultValue() const { return _defval; };

protected:

  virtual void
  doResample( const AimsData< T > &inVolume,
              const aims::Transformation3d &invTransform3d,
              const T &outBackground, const Point3df &outLocation,
              T &outValue, int time ) const = 0;

  /// called before each time step, used in spline resamplers
  virtual void updateParameters( const AimsData< T > &inVolume, int time,
                                 bool verbose ) const;

  carto::const_ref<AimsData<T> > _ref;
  T _defval;
};

#endif
