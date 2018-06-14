%ModuleHeaderCode
#include <aims/resampling/motion.h>
%End

namespace aims
{
void transformBoundingBox( const aims::AffineTransformation3d &motion,
                const Point3df & pmin1,
                const Point3df & pmax1, Point3df & pmin2, Point3df & pmax2 );


class AffineTransformation3d : aims::Transformation3d
{
%TypeHeaderCode
#include <aims/resampling/motion.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipAimsData_FLOAT.h"
#include "sipaimssipAimsVector_FLOAT_3.h"
#include "sipaimssipvector_FLOAT.h"
#endif
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy,
    sipClass_aims_AffineTransformation3d,
    sipTransferObj, sipIsErr, sipCppPtr );
%End

public:
  AffineTransformation3d();
  AffineTransformation3d( const aims::AffineTransformation3d & );
  AffineTransformation3d( const aims::Quaternion &);
  AffineTransformation3d( const carto::Object );
  AffineTransformation3d( const vector_FLOAT & );
  virtual ~AffineTransformation3d();
  virtual bool operator == ( const aims::AffineTransformation3d & );
  aims::AffineTransformation3d *__neg__() const /Factory, ReleaseGIL/;
%MethodCode
  sipRes = new aims::AffineTransformation3d( -*sipCpp );
%End

  Point3dd transformVector( const Point3dd & vec ) const;
  Point3df transformVector( const Point3df & dir ) const;
  Point3dd transformVector( double x, double y, double z ) const;
  Point3dd transformNormal( const Point3dd & dir ) const;
  Point3df transformNormal( const Point3df & dir ) const;
  Point3dd transformNormal( double x, double y, double z ) const;
  Point3dd transformUnitNormal( const Point3dd & dir ) const;
  Point3df transformUnitNormal( const Point3df & dir ) const;
  Point3dd transformUnitNormal( double x, double y, double z ) const;

  Point3df & translation();
  // const Point3df & translation() const;
  AimsData_FLOAT & rotation();
  // const AimsData_FLOAT & rotation() const;
  bool isIdentity() const;
  virtual void setToIdentity();
  aims::AffineTransformation3d inverse() const;
  aims::AffineTransformation3d operator* (const aims::AffineTransformation3d & ) const;
  void operator*= (const aims::AffineTransformation3d & ); // SIP4.5> (*sipCpp) *= *a0))
  virtual void scale( const Point3df &, const Point3df & );
  bool isDirect() const;
  void setTranslation( Point3df );
  virtual void setRotationAffine( float, float, float,
                                  const Point3df & = Point3df(0.0,0.0,0.0) );
  vector_FLOAT toVector() const;
  vector_FLOAT toColumnVector() const;
  void fromColumnVector( const vector_FLOAT & );

  static AimsData_FLOAT rotationaroundx( float );
  static AimsData_FLOAT rotationaroundy( float );
  static AimsData_FLOAT rotationaroundz( float );

  carto::GenericObject* header();

protected:
  virtual Point3dd transformDouble( double, double, double ) const;

  virtual Point3dd transformVectorPoint3dd( const Point3dd & vec ) const;
  virtual Point3df transformVectorPoint3df( const Point3df & dir ) const;
  virtual Point3dd transformVectorDouble( double x, double y,
                                            double z ) const;
  virtual Point3df transformVectorFloat( float x, float y, float z ) const;
  virtual Point3dd transformNormalPoint3dd( const Point3dd & dir ) const;
  virtual Point3df transformNormalPoint3df( const Point3df & dir ) const;
  virtual Point3dd transformNormalDouble( double x, double y,
                                            double z ) const;
  virtual Point3df transformNormalFloat( float x, float y, float z ) const;
};

};

typedef aims::AffineTransformation3d Motion;
