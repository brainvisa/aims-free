
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/talairach/talBoxBase.h>


TalairachBoxBase::TalairachBoxBase() : TalairachReferential()
{
  _scale = Point3df( 1.0f, 1.0f, 1.0f );
}

TalairachBoxBase::~TalairachBoxBase()
{
}

Point3df TalairachBoxBase::toNormalized( const Point3df& pt )
{
  Point3df res;
  
  for ( int i=0; i<3; i++ )  res[ i ] = pt[ i ] * _scale[ i ];

  return res;
}


Point3df TalairachBoxBase::fromNormalized( const Point3df& pt )
{
  Point3df res;
  
  for ( int i=0; i<3; i++ )  res[ i ] = pt[ i ] / _scale[ i ];

  return res;
}


Point3df TalairachBoxBase::toTalairach( const Point3df& pt )
{
  return toNormalized( TalairachReferential::toTalairach( pt ) );
}


Point3df TalairachBoxBase::fromTalairach( const Point3df& pt )
{
  return TalairachReferential::fromTalairach( fromNormalized( pt ) );
}
