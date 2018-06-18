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


/** Resampling of data from a volume, applying a transformation.

The doit() methods work on input data passed to the setRef() method.
setDefaultValue() can also be called to set the background value.

The resample() methods provide stateless alternatives.
*/
template <class T>
class Resampler
{
public:

  Resampler() : _ref(), _defval( T() ) { }
  virtual ~Resampler() { }

  /** Resample the input volume set with setRef() into an existing volume.

      \param[in]     transform   transformation from coordinates of the input
                                 volume (unit: mm), to coordinates of the
                                 output volume (unit: mm) (its inverse is used
                                 for resampling)
      \param[in,out] output_data existing volume to be filled with resampled
                                 data (its pre-existing dimensions and voxel
                                 size are used)

      \throws std::runtime_error if no input volume has been set with setRef.

      The background value (to be used for regions that are outside of the
      input volume) can be set with setDefaultValue().

      The \c transformations, \c referentials, and \c referential header
      attributes of \p output_data are not touched; it is up to the calling
      code to update them accordingly.

      The level of verbosity is taken from carto::verbose (i.e. the
      `--verbose` command-line argument is honoured).
  */
  virtual void doit( const aims::AffineTransformation3d& transform,
                     AimsData<T>& output_data ) const;

  /** Resample the input volume set with setRef() in a newly allocated volume.

      \param[in] transform        transformation from coordinates of the input
                                  volume (unit: mm), to coordinates of the
                                  output volume (unit: mm) (its inverse is used
                                  for resampling)
      \param[in] dimX, dimY, dimZ dimensions of the newly allocated volume
      \param[in] voxel_size       voxel size of the newly allocated volume
                                  (unit: mm)

      \returns a newly allocated volume containing the resampled data (its size
               along the \em t axis is the same as the input volume).

      \throws std::runtime_error if no input volume has been set with setRef.

      The background value (to be used for regions that are outside of the
      input volume) can be set with setDefaultValue().

      The level of verbosity is taken from carto::verbose (i.e. the
      `--verbose` command-line argument is honoured).

      The \c transformations, \c referentials, and \c referential header
      attributes of the new volume are reset if `transform.isIdentity()` is
      false:
      \li the \c referential attribute is removed
      \li each transformation in \c transformations is composed with
          \p transform so that the output volume still points to the original
          space. If that is not possible (e.g. the \c transformations attribute
          is missing or invalid), then a new transformation is added that
          points to the input volume.
  */
  virtual AimsData<T> doit( const aims::AffineTransformation3d& transform,
                            int dimX, int dimY,
                            int dimZ, const Point3df& voxel_size ) const;

  /** Resample a volume into an existing output volume.

      \param[in]     input_data  data to be resampled (its voxel size is taken
                                 into account)
      \param[in]     transform   transformation from coordinates of the input
                                 volume (unit: mm), to coordinates of the
                                 output volume (unit: mm) (its inverse is used
                                 for resampling)
      \param[in]     background  value set in output regions that are outside
                                 of the transformed input volume
      \param[in,out] output_data existing volume to be filled with resampled
                                 data (its pre-existing dimensions and voxel
                                 size are used)
      \param[in]     verbose     print progress to stdout

      The \c transformations, \c referentials, and \c referential header
      attributes of \p output_data are not touched; it is up to the calling
      code to update them accordingly.

      This method does \b not use the instance state set by setRef() or
      setDefaultValue().
  */
  virtual void resample( const AimsData< T >& input_data,
                         const aims::AffineTransformation3d& transform,
                         const T& background,
                         AimsData< T >& output_data,
                         bool verbose = false ) const;

  /** Resample a volume at a single location.

      \param[in]     input_data   data to be resampled (its voxel size is taken
                                  into account)
      \param[in]     transform    transformation from coordinates of the input
                                  volume (unit: mm), to output coordinates
                                  (its inverse is used for resampling)
      \param[in]     background   value set in output regions that are outside
                                  of the transformed input volume
      \param[in,out] output_location coordinates in output space (destination
                                  space of \p transform)
      \param[out]    output_value variable to be filled with resampled data
      \param[in]     timestep     for 4D volume, time step to be used

      This method does \b not use the instance state set by setRef() or
      setDefaultValue().
  */
  virtual void resample( const AimsData< T >& input_data,
                         const aims::AffineTransformation3d& transform,
                         const T& background,
                         const Point3df& output_location,
                         T& output_value, int timestep ) const;

  /// Set the input data to be resampled by the doit() methods
  virtual void setRef( const AimsData<T>& ref );

  /// Input data to be resampled by the doit() methods
  const carto::const_ref<AimsData<T> >& ref() const { return _ref; }

  /// Set the background value to be used by the doit() methods
  void setDefaultValue( T val ) { _defval = val; }

  /// Background value used by the doit() methods
  const T& defaultValue() const { return _defval; };

protected:

  /** Resample a volume at a single location.

      \param[in]     input_data   data to be resampled (its voxel size is
                                  \b not taken into account)
      \param[in]     inverse_transform transformation from output coordinates
                                  to coordinates of the input volume
                                  (<b>unit: voxels</b>)
      \param[in]     background   value set if the transformed point is outside
                                  of the input volume
      \param[in,out] output_location coordinates in output space (source space
                                  of \p transform)
      \param[out]    output_value variable to be filled with resampled data
      \param[in]     timestep     for 4D volume, time step to be used
  */
  virtual void
  doResample( const AimsData< T > &input_data,
              const aims::Transformation3d &inverse_transform,
              const T &background, const Point3df &output_location,
              T &output_value, int timestep ) const = 0;

  /** Method called before each time step (used in derived classes).

      This method is called by all the resampling methods (resample() and
      doit()) before they call doResample.

      The base class version of this method does nothing.
   */
  virtual void updateParameters( const AimsData< T > &inVolume, int time,
                                 bool verbose ) const {};

  carto::const_ref<AimsData<T> > _ref;
  T _defval;
};

#endif
