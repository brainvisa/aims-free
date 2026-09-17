
#ifndef AIMS_TALAIRACH_TALAIRACH_H
#define AIMS_TALAIRACH_TALAIRACH_H

#include <aims/talairach/talPoints.h>
#include <aims/transformation/affinetransformation3d.h>


class TalairachReferential
{
public:

  TalairachReferential();
  virtual ~TalairachReferential() { }

  virtual aims::AffineTransformation3d computeTransformation(
    const TalairachPoints& );
  
  Point3df toTalairach( const Point3df& );
  Point3df fromTalairach( const Point3df& );
  
protected:

  virtual void computeVectors( const TalairachPoints& );
  
  Point3df _ACPCVec;
  Point3df _hemiVec;
  Point3df _crossVec;

  aims::AffineTransformation3d _transformation ;
};

#endif
