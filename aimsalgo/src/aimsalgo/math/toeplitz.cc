
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/math/mathelem.h>
#include <aims/def/assert.h>
#include <cartodata/volume/volume.h>

using namespace aims;
using namespace carto;


void AimsToeplitz( const VolumeRef<float>& r,
                   const VolumeRef<float>& y,
                   VolumeRef<float>& x )
{
  ASSERT( y.getSizeX() == x.getSizeX() &&
          2 * y.getSizeX() - 1 == r.getSizeX() );

  int j, k, m, m1, m2;
  int dim = y.getSizeX();
  float sxn, sd, sgn, shn, sgd, pp, qq, pt1, pt2, qt1, qt2;
  VolumeRef<float> g( dim, 1, 1, 1, AllocatorContext::fast() ),
    h( dim, 1, 1, 1, AllocatorContext::fast() );

  x( 0 ) = y( 0 ) / r( dim - 1 );
  g( 0 ) = r( dim - 2 ) / r( dim - 1 );
  h( 0 ) = r( dim ) / r( dim - 1 );

  for ( m = 1; m <= dim; m++ )
  {
    m1 = m + 1;
    sxn = -y( m1 - 1 );
    sd = -r( dim - 1 );
    for ( j = 1; j <= m; j++ )
    {
      sxn += r( dim - 1 + m1 - j ) * x( j - 1 );
      sd += r( dim - 1 + m1 - j ) * g( m - j );
    }
    ASSERT( sd != 0.0 );
    x( m1 - 1 ) = sxn / sd;
    for ( j = 1; j <= m; j++ )
      x( j - 1 ) -= x( m1 - 1 ) * g( m - j );
    if ( m1 == dim )
      return;
    sgn = - r( dim - 1 - m1 );
    shn = - r( dim - 1 + m1 );
    sgd = - r( dim - 1 );
    for ( j = 1; j <= m; j++ )
    {
      sgn += r( dim - 1 + j - m1 ) * g( j - 1 );
      shn += r( dim - 1 + m1 - j ) * h( j - 1 );
      sgd += r( dim - 1 + j - m1 ) * h( m - j );
    }
    ASSERT( sd != 0.0 && sgd != 0.0 );
    g( m1 - 1 ) = sgn / sgd;
    h( m1 - 1 ) = shn / sd;
    k = m;
    m2 = ( m + 1 ) >> 1;
    pp = g( m1 - 1 );
    qq = h( m1 - 1 );
    for ( j = 1; j <= m2; j++ )
    {
      pt1 = g( j - 1 );
      pt2 = g( k - 1 );
      qt1 = h( j - 1 );
      qt2 = h( k - 1 );
      g( j - 1 ) = pt1 - pp * qt2;
      g( k - 1 ) = pt2 - pp * qt1;
      h( j - 1 ) = qt1 - qq * pt2;
      h( k - 1 ) = qt2 - qq * pt1;
      k--;
    }
  }
  ASSERT( 0 );
}
