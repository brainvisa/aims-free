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
  vector_FLOAT toVector () const;

  static AimsData_FLOAT rotationaroundx( float );
  static AimsData_FLOAT rotationaroundy( float );
  static AimsData_FLOAT rotationaroundz( float );

  carto::GenericObject* header();

protected:
  virtual Point3dd transformDouble( double, double, double ) const;
  virtual Point3dd transformVectorDouble( double x, double y, double z ) const;
};

};

typedef aims::AffineTransformation3d Motion;

