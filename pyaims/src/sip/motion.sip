%ModuleHeaderCode
#include <aims/resampling/motion.h>
%End

namespace aims
{
void transformBoundingBox( const Motion &motion, const Point3df & pmin1,
                const Point3df & pmax1, Point3df & pmin2, Point3df & pmax2 );
};


class Motion : carto::RCObject
{
%TypeHeaderCode
#include <aims/resampling/motion.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipAimsData_FLOAT.h"
#include "sipaimssipAimsVector_FLOAT_3.h"
#include "sipaimssipvector_FLOAT.h"
#endif
%End

public:
  Motion();
  Motion( const Motion & );
  Motion( const aims::Quaternion &);
  Motion( const carto::Object );
  Motion( const vector_FLOAT & );
  virtual ~Motion();
  virtual bool operator == ( const Motion & );
  Motion *__neg__() const /Factory, ReleaseGIL/;
%MethodCode
  sipRes = new Motion( -*sipCpp );
%End
  Point3df & translation();
  // const Point3df & translation() const;
  AimsData_FLOAT & rotation();
  // const AimsData_FLOAT & rotation() const;
  Point3df transform( float, float, float) const;
  Point3df transform( const Point3df ) const;
  Point3df transform_normal( const Point3df ) const;
  Point3df transform_normal( float, float, float) const;
  bool isIdentity() const;
  virtual void setToIdentity();
  Motion inverse() const;
  Motion operator* (const Motion & );
  // Motion operator*= (const Motion & ); // SIP4.5> (*sipCpp) *= *a0))
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
};

