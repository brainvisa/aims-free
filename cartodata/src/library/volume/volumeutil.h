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

#ifndef CARTODATA_VOLUME_VOLUMEUTIL_H
#define CARTODATA_VOLUME_VOLUMEUTIL_H

#include <cartobase/type/datatypetraits.h>
#include <cartodata/volume/volume.h>


namespace carto
{

  /** Utility functor.
      Converts a regular binary functor to a unary functor using a constant
      value as second argument.
      This class may move in another location later.
  */
  template <typename T, class BinaryFunction>
  class UnaryFromConstantBinaryFunctor
  {
  public:
    inline UnaryFromConstantBinaryFunctor( const T & x, BinaryFunction func )
      : value( x ), f( func ) {}
    inline T operator () ( const T & y ) const
    { return f( y, value ); }
    T			value;
    BinaryFunction	f;
  };


  /** Utility functor.
      Same as the fist form except that the constant term is the first
      argument or the binary functor.
  */
  template <typename T, class BinaryFunction>
  class UnaryFromConstantBinaryFunctor2
  {
  public:
    inline UnaryFromConstantBinaryFunctor2( const T & x, BinaryFunction func )
      : value( x ), f( func ) {}
    inline T operator () ( const T & y ) const
    { return f( value, y ); }
    T			value;
    BinaryFunction	f;
  };


  /** Scaler functor.
      Multiplies the argument with a constant (double) value.
      This class may move in another location later.
  */
  template <typename T, typename U>
  class Scaler
  {
  public:
    inline Scaler( U x ) : scale( x ) {}
    inline T operator () ( const T & x ) const
    { return (T) ( x * scale ); }
    U	scale;
  };

  template<> inline cfloat
  Scaler<cfloat, double>::operator () ( const cfloat & x ) const
  {
    return x * (float) scale;
  }

  template<> inline cfloat
  Scaler<cfloat, long>::operator () ( const cfloat & x ) const
  {
    return x * (float) scale;
  }

  template<> inline cdouble
  Scaler<cdouble, float>::operator () ( const cdouble & x ) const
  {
    return x * (double) scale;
  }

  template<> inline cdouble
  Scaler<cdouble, long>::operator () ( const cdouble & x ) const
  {
    return x * (double) scale;
  }

  /** Divider functor.
      Divides the argument with a constant value.
      This class may move in another location later.
  */
  template <typename T, typename U>
  class Divider
  {
  public:
    inline Divider( U x ) : divisor( x ) {}
    inline T operator () ( const T & x ) const
    { return (T) ( x / divisor ); }
    U      divisor;
  };

  template<> inline cfloat
  Divider<cfloat, double>::operator () ( const cfloat & x ) const
  {
    return x * (float) ( 1. / divisor );
  }

  template<> inline cfloat
  Divider<cfloat, long>::operator () ( const cfloat & x ) const
  {
    return x * (float) ( 1. / (double) divisor );
  }

  template<> inline cdouble
  Divider<cdouble, double>::operator () ( const cdouble & x ) const
  {
    return x * ( 1. / divisor );
  }

  template<> inline cdouble
  Divider<cdouble, float>::operator () ( const cdouble & x ) const
  {
    return x * (double) ( 1. / divisor );
  }

  template<> inline cdouble
  Divider<cdouble, long>::operator () ( const cdouble & x ) const
  {
    return x * (double) ( 1. / divisor );
  }

  template <typename T, bool Scalar>
  class VolumeUtilBase {
  };

  /// Volume utilities base class for scalar types
  template <typename T>
  class VolumeUtilBase<T, true> {
    public :
      /** Get the minimum voxel value of a volume.
      */
      static T min( const Volume<T> & o );

      /** Get the maximum voxel value of a volume.
      */
      static T max( const Volume<T> & o );
  };

  /// Volume utilities base class for non scalar types
  template <typename T>
  class VolumeUtilBase<T, false> {
  };

  /// Volume utilities classes
  template <typename T>
  class VolumeUtil: public carto::VolumeUtilBase<T, carto::DataTypeTraits<T>::is_scalar>
  {
  public:
    /// applies a unary function to each voxel of a volume
    template <class UnaryFunction> static
    VolumeRef<T> apply( UnaryFunction f, const VolumeRef<T> & o );
    /// applies a binary function to each voxel of a pair of volumes
    template <class BinaryFunction> static
    VolumeRef<T> apply( BinaryFunction f, const VolumeRef<T> & o1,
                        const VolumeRef<T> & o2 );
    /** same as apply() except that the input volume is used to store the
        result */
    template <class UnaryFunction> static
    void selfApply( UnaryFunction f, VolumeRef<T> & o );
    /** same as apply() except that the first input volume is used to store
        the result */
    template <class BinaryFunction> static
    void selfApply( BinaryFunction f, VolumeRef<T> & o1,
                    const VolumeRef<T> & o2 );
    /** Apply a binary function to each voxel of the volume, with a "current"
        value as second argument. The voxel result is used for other voxels
        function calls.
        Useful for min/max for instance:
        min = VolumeUtil<T>::accumulate( std::min<T>, volume,
          std::numeric_limits<T>::max() );
        max = VolumeUtil::accumulate( std::max<T>, volume,
          -std::numeric_limits<T>::max() );
        May be used also for sum etc.
    */
    template <class BinaryFunction> static
    T accumulate( BinaryFunction f, const VolumeRef<T> & o2, T initial )
    { return accumulate( f, *o2, initial ); }
    template <class BinaryFunction> static
    T accumulate( BinaryFunction f, const Volume<T> & o2, T initial );
  };

  //--------------------------------------------------------------------------
  // Copy functions
  //--------------------------------------------------------------------------

  /// Transfer data from an allocated volume to another
  ///
  /// Allocated sizes must be equal (same number of voxels)
  template <typename T>
  void transfer( const carto::VolumeRef<T> & src,
                 carto::VolumeRef<T> & dst );


  /// Performs a copy of the data (not only a reference copy)
  ///
  /// \param copy_full_structure if true, the complete view hierarchy is copied
  ///                            if false, only visible data is copied
  template <typename T>
  carto::VolumeRef<T> deepcopy( const carto::VolumeRef<T> & src,
                                bool copy_full_structure = true );

  /// Similar to deepcopy() with copy_full_structure = false
  template <typename T>
  carto::VolumeRef<T> copy( const carto::VolumeRef<T> & src );

}

#endif

