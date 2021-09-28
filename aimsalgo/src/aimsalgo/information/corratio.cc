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
