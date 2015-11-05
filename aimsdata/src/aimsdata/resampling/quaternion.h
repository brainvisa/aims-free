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


#ifndef AIMS_RESAMPLING_QUATERNION_H
#define AIMS_RESAMPLING_QUATERNION_H


#include <aims/vector/vector.h>
#include <aims/data/data.h>

namespace aims
{
  class AffineTransformation3d;
}
typedef aims::AffineTransformation3d Motion;

namespace aims
{

  class Quaternion
  {
  public:
    friend Quaternion operator * ( const Quaternion &, const Quaternion & );

    Quaternion();
    Quaternion( const Point4df & q );
    Quaternion( const Quaternion & q );
    Quaternion( float x, float y, float z, float t );
    ~Quaternion();

    Quaternion & operator = ( const Quaternion & q );
    const Point4df & vector() const { return( _vector ); }
    void setVector( const Point4df & vec );
    ///	in OpenGL style: in columns
    AimsVector<float,16> rotationMatrix() const;
    AimsVector<float,16> inverseRotationMatrix() const;
    /// 4x4 matrix in columns (OpenGL-style)
    void buildFromMatrix( const float* m );
    void buildFromMotion( const Motion & m );
    /// Rotates vecotor (x,y,z)
    Point3df transform( float x, float y, float z ) const
    { return transform( Point3df( x, y, z ) ); }
    /// Rotates vecotor p
    Point3df transform( const Point3df & p ) const;
    Point3df transformInverse( const Point3df & p ) const;
    /// OBSOLETE BUGGY FUNCTION - Don't use it ever.
    Point3df apply( const Point3df & p ) const
      __attribute__((__deprecated__("OBSOLETE BUGGY FUNCTION.")))
    { return transformInverse( p ); }
    /// OBSOLETE BUGGY FUNCTION - Don't use it ever.
    Point3df applyInverse( const Point3df & p ) const
      __attribute__((__deprecated__("OBSOLETE BUGGY FUNCTION.")))
    { return transform( p ); }

    Quaternion & operator += ( const Quaternion & q );
    Quaternion & operator *= ( const Quaternion & q );
    void norm();
    Quaternion normalized() const;
    void fromAxis( const Point3df & c, float phi );
    Quaternion inverse() const;
    Point3df axis() const;
    float angle() const;

    static float dot( const Quaternion & a, const Quaternion & b )
    { return( dot( a._vector, b._vector ) ); }
    static float dot( const Point4df & a, const Point4df & b );
    static Point4df cross( const Point4df & a, const Point4df & b );
    static Point3df norm( const Point3df & a );

  protected:
    Point4df	_vector;
  };


  Quaternion operator + ( const Quaternion & a, const Quaternion & b );
  Quaternion operator * ( const Quaternion & a, const Quaternion & b );

}


#endif
