
namespace aims
{

class Quaternion
{
%TypeHeaderCode
#include <aims/resampling/quaternion.h>
%End

  public:
  Quaternion();
  Quaternion( const Point4df &q );
  Quaternion( const aims::Quaternion &q );
  Quaternion( float x, float y, float z, float t );
  ~Quaternion();

  const Point4df & vector() const;
  void setVector( const Point4df &vec );
  // AimsVector_FLOAT_S16 rotationMatrix() const;
  // AimsVector_FLOAT_S16 inverseRotationMatrix() const;
  void buildFromMatrix( const float *m );
  void buildFromMotion( const Motion & m );
  Point3df transform( const Point3df &p ) const;
  Point3df transform( float x, float y, float z ) const;
  Point3df transformInverse( const Point3df &p ) const;
  aims::Quaternion & operator+= ( const aims::Quaternion &q );
  aims::Quaternion & operator *= ( const aims::Quaternion &q );
  static aims::Quaternion operator * ( const aims::Quaternion &b ) const;
  void norm();
  aims::Quaternion normalized() const;
  void fromAxis( const Point3df &c, float phi );
  aims::Quaternion inverse() const;
  Point3df axis() const;
  float angle() const;
};

};

