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



#include <cstdlib>
#include <aims/perfusion/perfSkip.h>
#include <aims/perfusion/perfSplitVol.h>
#include <aims/utility/utility_g.h>
#include <map>
#include <math.h>

using namespace aims;
using namespace std;


int PerfusionSkip::value( AimsData< short >& d, PerfusionParameters& pp )
{
  int x, y, z, t;
  int dx = d.dimX(), dy = d.dimY(), dz = d.dimZ(), dt = d.dimT();
  PerfusionSplitVolume< short > vol;
  multimap< short, Point3d > lst;
  short val = 0;

  // get the maximum intensities
  AimsData< short > vol1 = vol.first( d );
  short max = vol1.maximum();
  short thres = (short)( 0.95f * max );
  AimsThreshold< short, short > tLV( AIMS_GREATER_THAN, thres );
  AimsData< short > vmax = tLV( vol1 );

  for ( z=0; z<dz; z++ )
    for ( y=0; y<dy; y++ )
      for ( x=0; x<dx; x++ )
	if ( ( val = vmax( x, y, z ) ) )
	  lst.insert( pair< short, Point3d >( val, Point3d( x, y, z ) ) );

  int limit = ( lst.size() < 20 ) ? lst.size() : 20 ;

  AimsData< int32_t > dsum( dt );
  multimap< short, Point3d >::iterator it = lst.begin();
  while ( limit-- && it != lst.end() )
    {
      Point3d p = it->second;
      for ( t=0; t<dt; t++ )  dsum( t ) += d( p[ 0 ], p[ 1 ], p[ 2 ], t );
      ++it;
    }

  int maxS = dsum.maximum();
  int seuil = (int)abs( maxS * pp.skipThres() );
  int delta = (int)abs( (int)( maxS - dsum( 1 ) ) );

  t = 1;
  while ( delta > seuil && t < dt )
    {
      t++;
      delta = (int)abs( (int)( dsum( t - 1 ) - dsum( t ) ) );
    }

  return ( t > dt / 2 ) ? 5 : t;
}
