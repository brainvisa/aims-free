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
#include <aims/perfusion/perfSplitVol.h>
#include <aims/perfusion/perfAifPoints.h>
#include <aims/utility/utility_g.h>
#include <aims/connectivity/connectivity_g.h>
#include <map>
#include <math.h>

using namespace aims;
using namespace std;


list< Point4dl > PerfusionAifPoints::search( AimsData< short >& d, 
                                            BucketMap< Void >& mask,
                                            PerfusionParameters& params )
{
  int t, dt = d.dimT(), skip = params.skip(), nAif = params.nAif();
  float athres = params.aifThreshold();

  BucketMap<Void>::Bucket::iterator it = mask[ 0 ].begin(), et = mask[0].end();

  // Get the initial values (at skip time)
  PerfusionSplitVolume< short > psv;
  AimsData< short > initVal = psv.volume( d, skip );

  // Look for the maximum signal loss on the brain mask
  Point3d pt;
  short delta, best = 0;
  while( it != et )
    {
      pt = it->first;

      for ( t=skip; t<dt; t++ )
	{
	  delta = (short)abs( initVal( pt ) - d( pt[0], pt[1], pt[2], t ) );
	  if ( delta > best ) best = delta;
	}

      ++it;
    }

  // Compute the threshold value
  int seuil = (int)( ( 1.0f - athres ) * best );
  int dx = d.dimX();
  int dy = d.dimY();
  int dz = d.dimZ();
  float sx = d.sizeX();
  float sy = d.sizeY();
  float sz = d.sizeZ();
  AimsData< short > newVol( dx, dy, dz );
  newVol.setSizeXYZT( sx, sy, sz );
  newVol = 0;

  for ( it=mask[ 0 ].begin(); it!=et; ++it )
    {
      pt = it->first;

      for ( t=skip; t<dt; t++ )
	if ( (int)abs( initVal( pt ) - d( pt[0], pt[1], pt[2], t ) ) > seuil )
	  newVol( pt ) = 32767;
    }

  AimsBucket< Void > Vcompo;
  AimsConnectedComponent( Vcompo, newVol, 
			  Connectivity::CONNECTIVITY_26_XYZ );

  // Look for the maximum amplitude variations of each connected component
  multimap< short, Point4dl, greater< short > > md;
  int bestT;
  BucketMap<Void>	bmap = Vcompo;
  BucketMap<Void>::Bucket::iterator itb;
  BucketMap<Void>::Bucket::iterator itf;
  BucketMap< Void >::iterator itcc;

  for ( itcc=bmap.begin(); itcc!=bmap.end(); ++itcc ) 
    {
      best = 0;
      bestT = skip;

      itb = itcc->second.begin();
      itf = itcc->second.end();

      Point3d bPt;

      while( itb != itf )
	{
	  pt = itb->first;

	  for ( t=skip; t<dt; t++ )
	    {
	      delta = (short)abs( initVal( pt )-d( pt[0], pt[1], pt[2], t ) );

	      if ( delta > best )
		{
		  best = delta;
		  bestT = t;
		  bPt = pt;
		}
	    }

	  ++itb;
	}

      if ( bestT > 0 ) //< dt / 2 )
	{
	  Point4dl pt4( bPt[0], bPt[1], bPt[2], bestT );
	  md.insert( pair< short, Point4dl >( best, pt4 ) );
	}
    }

  // Get the nAIF best values and sort them in ascending time
  multimap< short, Point4dl > ml;
  multimap< short, Point4dl, greater< short > >::iterator imd = md.begin();
  int n = 0;
  while( n < nAif && imd != md.end() )
    {
      Point4dl p = imd->second;
      ml.insert( pair< short, Point4dl >( p[3], p ) );

      ++imd;
      n++;
    }

  // Construct a set from the multimap of selected points
  list< Point4dl > sl;
  multimap< short, Point4dl >::iterator iml = ml.begin();
  while( iml != ml.end() )
    {
      sl.push_back( iml->second );
      ++iml;
    }

  return sl;
}
