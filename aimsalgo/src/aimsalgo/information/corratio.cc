
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/information/information.h>
#include <aims/math/mathelem.h>

using namespace carto;
using namespace aims;

float AimsCorrelationRatio( const rc_ptr<Volume<float> >& p1,
                            const rc_ptr<Volume<float> >& p2,
                            const rc_ptr<Volume<float> >& p12 )
{
  int levels = p1->getSizeX();

  ASSERT( p2->getSizeX() == levels && p12->getSizeX() == levels
          && p12->getSizeY() == levels );

  VolumeRef<float> mean( levels ), sigma2( levels );
  float Mean = 0.0, Sigma2 = 0.0;
  int	i;

  mean = 0.0;
  sigma2 = 0.0;

  for ( i = 0; i < levels; i++)
    if ( p1->at( i ) )
    {
      for ( int j = 0; j < levels; j++ )
      {
        mean( i ) += j * p12->at( i, j );
        sigma2( i ) += sqr( float( j ) ) * p12->at( i, j );
      }
      mean( i ) /= p1->at( i );
      sigma2( i ) = sigma2( i ) / p1->at( i ) - sqr( mean( i ) );
    }

  for ( int j = 0; j < levels; j++ )
  {
    Mean += j * p2->at( j );
    Sigma2 += sqr( float( j ) ) * p2->at( j );
  }
  Sigma2 -= sqr( Mean );

  float cr = 0.0;

  for ( i = 0; i < levels; i++)
    cr += sigma2( i ) * p1->at( i );

  cr /= Sigma2;

  return cr;
}
