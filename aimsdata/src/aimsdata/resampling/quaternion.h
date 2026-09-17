
#ifndef AIMS_RESAMPLING_QUATERNION_H
#define AIMS_RESAMPLING_QUATERNION_H


#include <aims/vector/vector.h>

namespace aims
{
  class AffineTransformation3d;


  class Quaternion
  {
  public:
    friend Quaternion operator * ( const Quaternion &, const Quaternion & );

    Quaternion();
    Quaternion( const Point4df & q );
    Quaternion( const Quaternion & q );
    Quaternion( float x, float y, float z, float t );
    Quaternion( const AffineTransformation3d & tr );
    ~Quaternion();

    Quaternion & operator = ( const Quaternion & q );
    const Point4df & vector() const { return( _vector ); }
    void setVector( const Point4df & vec );
    ///	in OpenGL style: in columns
    AimsVector<float,16> rotationMatrix() const;
    AimsVector<float,16> inverseRotationMatrix() const;
    /// 4x4 matrix in columns (OpenGL-style)
    void buildFromMatrix( const float* m );
    void buildFromTransformation( const AffineTransformation3d & m );
    /// Obsolete in aims 5.2: use buildFromTransformation() instead.
    void buildFromMotion( const AffineTransformation3d & m )
    { buildFromTransformation( m ); }
    /// Rotates vecotor (x,y,z)
    Point3df transform( float x, float y, float z ) const
    { return transform( Point3df( x, y, z ) ); }
    /// Rotates vecotor p
    Point3df transform( const Point3df & p ) const;
    Point3df transformInverse( const Point3df & p ) const;
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


  Quaternion operator * ( const Quaternion & a, const Quaternion & b );

}


#endif
