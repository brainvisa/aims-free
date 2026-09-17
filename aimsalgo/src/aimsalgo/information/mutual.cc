
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/information/information.h>
#include <aims/math/mathelem.h>

using namespace carto;

float AimsMutualInformation( const rc_ptr<Volume<float> >& p1,
                             const rc_ptr<Volume<float> >& p2,
                             const rc_ptr<Volume<float> >& p12 )
{
  int levels = p1->getSizeX();

  ASSERT( p2->getSizeX() == levels && p12->getSizeX() == levels && p12->getSizeY() == levels );

  float mi=0.0;
  int x, y;
  for ( y = 0; y < levels; y++ )
    for ( x = 0; x < levels; x++ )
      if ( p2->at( y ) && p12->at( x, y ) && p1->at( x ) )
        mi += p12->at( x, y ) * log( p12->at( x, y )
          / ( p1->at( x ) * p2->at( y ) ) );

  return float( mi );
}
