
#ifndef AIMS_TALAIRACH_TALBOXBASE_H
#define AIMS_TALAIRACH_TALBOXBASE_H

#include <aims/talairach/talairach.h>


class TalairachBoxBase : public TalairachReferential
{
public:

  TalairachBoxBase() ;
  virtual ~TalairachBoxBase() ;

  Point3df toTalairach( const Point3df& );
  Point3df fromTalairach( const Point3df& );

protected:

  Point3df _scale;

private:

  Point3df toNormalized( const Point3df& );
  Point3df fromNormalized( const Point3df& );
};


#endif
