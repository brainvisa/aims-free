/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef CARTODATA_VOLUME_VOLUMEUTIL_H
#define CARTODATA_VOLUME_VOLUMEUTIL_H

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
  template <typename T>
  class Scaler
  {
  public:
    inline Scaler( double x ) : scale( x ) {}
    inline T operator () ( const T & x ) const 
    { return (T) ( x * scale ); }
    double	scale;
  };

  template<> inline cfloat 
  Scaler<cfloat>::operator () ( const cfloat & x ) const
  {
    return x * (float) scale;
  }


  /// Volume utilities classes
  template <typename T>
  class VolumeUtil
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
  };

}

#endif

