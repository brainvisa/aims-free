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


#include <aims/perfusion/perfDelay.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


void DelayMap::saveMap( const string& filename )
{
  string filebasename( FileUtil::basename( filename ) );
  string directoryname( FileUtil::dirname( filename ) );

  std::string mapFilename = directoryname + "/delay_" + filebasename;

  doSave( mapFilename );
}


AimsData< float >& 
DelayMap::doit( AimsData< float >&, 
		AimsData< float >& d,
		AimsData< float >&,
		AimsData< float >&,
		aims::BucketMap< Void >& mask, 
		LMGamma< float >& aifParam,
		std::map< Point3d, LMGamma< float >, bestPoint3d >& mFit,
		PerfusionParameters&,
		AimsData< float > *,
		AimsData< float > * )
{
  int dx = d.dimX();
  int dy = d.dimY();
  int dz = d.dimZ();
  float sx = d.sizeX();
  float sy = d.sizeY();
  float sz = d.sizeZ();

  float theDelay;

  // AIF take-off time
  float t0Aif = aifParam.param()[ 1 ];

  m_map = AimsData< float >( dx, dy, dz );
  m_map.setSizeXYZT( sx, sy, sz );

  BucketMap< Void >::Bucket::iterator it = mask[ 0 ].begin();

  if ( mFit.size() > 1 )
    {
      while( it != mask[0].end() )
	{
	  Point3d pt = it->first;

	  // Map of take-off delay; delay = t0sig - t0Aif in s
	  theDelay = mFit[ pt ].param()[ 1 ] - t0Aif;
	  m_map( pt ) = ( theDelay < 0.0f ) ? 0.0f : theDelay;

	  ++it;
	}
    }

  return m_map;
}
