/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#include <aims/perfusion/perfBBB.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


void BBBMap::saveMap( const string& filename )
{
  string filebasename( FileUtil::basename( filename ) );
  string directoryname( FileUtil::dirname( filename ) );

  std::string mapFilename = directoryname + "/bbb_" + filebasename;

  doSave( mapFilename );
}


AimsData< float >&
BBBMap::doit( AimsData< float >& q, 
	      AimsData< float >&,
	      AimsData< float >&,
	      AimsData< float >&,
	      aims::BucketMap< Void >& mask,
	      LMGamma< float >&,
	      std::map< Point3d, LMGamma< float >, bestPoint3d >&,
	      PerfusionParameters& pp,
	      AimsData< float > *,
	      AimsData< float > * )
{
  int t, dt = q.dimT();
  int dx = q.dimX();
  int dy = q.dimY();
  int dz = q.dimZ();
  float sx = q.sizeX();
  float sy = q.sizeY();
  float sz = q.sizeZ();
  float val, yy0, yy1, k2, delta;

  int skip = pp.skip();
  float te = pp.te() / 1000.0f;

  getDR2AndIDR2( q, mask, skip, te );

  m_map = AimsData< float >( dx, dy, dz );
  m_map.setSizeXYZT( sx, sy, sz );

  BucketMap< Void >::Bucket::iterator it = mask[ 0 ].begin();

  delta = DR2_2 * IDR2_2 - DR2_IDR2 * DR2_IDR2;

  if ( fabs( delta ) > 1.0e-8 )
    {
      // Solve using Cramer's rule
      while( it != mask[0].end() )
	{
	  Point3d pt = it->first;

	  yy0 = yy1 = 0.0f;

	  for ( t=skip; t<dt; t++ )
	    {
	      val = q( pt[ 0 ], pt[ 1 ], pt[ 2 ], t );
	      yy0 += DR2( t ) * val;
	      yy1 += IDR2( t ) * val;
	    }

	  //k1 = ( yy0 * IDR2_2 - yy1 * DR2_IDR2 ) / delta ;
	  k2 = ( yy0 * DR2_IDR2 - yy1 * DR2_2 ) / delta;
          m_map( pt ) = ( k2 < 0.0f ) ? 0.0f : k2;

	  ++it;
	}
    }

  return m_map;
}


void BBBMap::getDR2AndIDR2( AimsData< float >& q, BucketMap< Void >& mask,
			    int skip, float te )
{
  int t, dt = q.dimT();
  float n, val, st = q.sizeT();

  DR2 = AimsData< float >( dt );
  IDR2 = AimsData< float >( dt );

  BucketMap< Void >::Bucket::iterator it;

  for ( t=skip; t<dt; t++ )
    {
      it = mask[ 0 ].begin();
      n = 0.0f;

      while( it != mask[ 0 ].end() )
	{
	  Point3d pt = it->first;
	  val = q( pt[ 0 ], pt[ 1 ], pt[ 2 ], t );
	  if ( val > -0.02f / te )
	    {
	      DR2( t ) += val;
	      n++;
	    }
	  ++it;
	}

      DR2( t ) /= n;
    }

  IDR2( skip ) = DR2( skip ) * st;

  for ( t=skip+1; t<dt; t++ )
    IDR2( t ) = IDR2( t - 1 ) + DR2( t ) * st;

  DR2_2 = IDR2_2 = DR2_IDR2 = 0.0f;

  for ( t=skip; t<dt; t++ )
    {
      DR2_2 += DR2( t ) * DR2( t );
      IDR2_2 += IDR2( t ) * IDR2( t );
      DR2_IDR2 += DR2( t ) * IDR2( t );
    }
}
