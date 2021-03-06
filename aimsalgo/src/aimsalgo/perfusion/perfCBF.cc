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


#include <aims/perfusion/perfCBF.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


CBFMap::CBFMap() : PerfusionMapBase( PerfusionMapBase::cbf )
{
  _deps.push_back( (int)PerfusionMapBase::cbv );

  _backdeps.push_back( (int)PerfusionMapBase::mtt );
}


void CBFMap::saveMap( const string& filename )
{
  string filebasename( FileUtil::basename( filename ) );
  string directoryname( FileUtil::dirname( filename ) );

  std::string mapFilename = directoryname + "/cbf_" + filebasename;

  doSave( mapFilename );
}


AimsData< float >& 
CBFMap::doit( AimsData< float >&, 
	      AimsData< float >&,
	      AimsData< float >&,
	      AimsData< float >& r,
	      aims::BucketMap< Void >& mask, 
	      LMGamma< float >&,
	      std::map< Point3d, LMGamma< float >, bestPoint3d >&,
	      PerfusionParameters& pp,
	      AimsData< float > *cbv,
	      AimsData< float > * )
{
  int t, dt = r.dimT();
  int dx = r.dimX();
  int dy = r.dimY();
  int dz = r.dimZ();
  float sx = r.sizeX();
  float sy = r.sizeY();
  float sz = r.sizeZ();
  float st = r.sizeT();

  int skip = pp.skip();
  float val, rMax, rInt;

  m_map = AimsData< float >( dx, dy, dz );
  m_map.setSizeXYZT( sx, sy, sz );

  BucketMap< Void >::Bucket::iterator it = mask[ 0 ].begin();

  while( it != mask[0].end() )
    {
      Point3d pt = it->first;

      // CBF.R(t) integration
      rMax = rInt = 0.0f;
      for ( t=skip; t<dt; t++ )
	{
	  val = r( pt[0], pt[1], pt[2], t );
	  if ( val > rMax )  rMax = val;
	  rInt += val * st;
	}

      // CBF Map : CBF = CBV.max(C(t))/Int(C(t)) in ml/min/100g
      m_map( pt ) = (*cbv)( pt ) * rMax / ( (rInt < 0.001f) ? 0.001f : rInt );

      ++it;
    }

  return m_map;
}
