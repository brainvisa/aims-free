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



#include <aims/perfusion/perfMask.h>
#include <aims/perfusion/perfSplitVol.h>

#include <aims/utility/utility_g.h>
#include <aims/bucket/bucket.h>
#include <aims/connectivity/connectivity_g.h>
#include <aims/signalfilter/vfilter.h>

#include <aims/morphology/morphology_g.h>

using namespace aims;
using namespace std;


BucketMap< Void > PerfusionMask::doit( AimsData< short >& d,
				       PerfusionParameters& pp )
{
  PerfusionSplitVolume< short > vol;
  AimsData< short > tmpvol = vol.volume( d, pp.skip() );
  AimsData< short > lastvol;

  if ( pp.hasVFilter() )
    {
      AimsVFilter< short > vf;
      lastvol = vf.doit( tmpvol );
    }
  else lastvol = tmpvol;

  float maxv = (float)lastvol.maximum();

  AimsThreshold< short, short > tB( AIMS_GREATER_THAN, 
				    (short)( pp.bThres() * maxv ) );
  AimsData< short > mask = tB.bin( lastvol );

  AimsBucket< Void > LVcompo;
  int i, n, max = 0, lindex = 0;

  if ( pp.lvThres() < 1.0f )
    {
      AimsThreshold< short, short > tLV( AIMS_GREATER_THAN, 
					 (short)( pp.lvThres() * maxv ) );
      AimsData< short > maskLV = tLV.bin( lastvol );

      AimsConnectedComponent( LVcompo, maskLV, 
			      Connectivity::CONNECTIVITY_26_XYZ );

      //Look for the bigest one
      n = LVcompo.size();
      for ( i=0; i<n; i++ )
      	if ( (int)LVcompo[ i + 1 ].size() > max ) 
      	  {
	    max = LVcompo[ i + 1 ].size();
	    lindex = i + 1;
	  }
    }

  list<AimsBucketItem<Void> >::iterator itb = LVcompo[ lindex ].begin();
  list<AimsBucketItem<Void> >::iterator ite = LVcompo[ lindex ].end();
  while( itb != ite )
    {
      mask( itb->location() ) = 0;
      ++itb;
    }

  AimsBucket< Void > Bcompo;
  AimsConnectedComponent( Bcompo, mask, Connectivity::CONNECTIVITY_26_XYZ );

  //Look for the bigest one
  int index = 0;
  max = 0;
  n = Bcompo.size();
  for ( i=0; i<n; i++ )
    if ( (int)Bcompo[ i + 1 ].size() > max ) 
      {
  	max = Bcompo[ i + 1 ].size();
  	index = i + 1;
      }

  BucketMap< Void >	bck;
  AimsBucket< Void >	tmpb;
  tmpb[ 0 ] = Bcompo[ index ];	// ugly trick to copy (slow)
  bck = tmpb;
  bck.setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ(), 1.0f );

  return bck;
}
