/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#include <aims/data/fastAllocationData.h>
#include <aims/math/mathelem.h>
#include <aims/def/assert.h>

using namespace aims;


void AimsToeplitz( const AimsData<float>& r,
                   const AimsData<float>& y,
                   AimsData<float>& x )
{
  ASSERT( y.dimX() == x.dimX() &&
          2 * y.dimX() - 1 == r.dimX() );

  int j, k, m, m1, m2;
  int dim = y.dimX();
  float sxn, sd, sgn, shn, sgd, pp, qq, pt1, pt2, qt1, qt2;
  AimsFastAllocationData<float> g( dim ), h( dim );

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
