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


#include <aims/perfusion/perfMTT.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


MTTMap::MTTMap() : PerfusionMapBase( PerfusionMapBase::mtt )
{
  _deps.push_back( (int)PerfusionMapBase::cbv );
  _deps.push_back( (int)PerfusionMapBase::cbf );
}


void MTTMap::saveMap( const string& filename )
{
  string filebasename( FileUtil::basename( filename ) );
  string directoryname( FileUtil::dirname( filename ) );

  std::string mapFilename = directoryname + "/mtt_" + filebasename;

  doSave( mapFilename );
}


AimsData< float >& 
MTTMap::doit( AimsData< float >&, 
	      AimsData< float >&,
	      AimsData< float >&,
	      AimsData< float >&,
	      aims::BucketMap< Void >& mask, 
	      LMGamma< float >&,
	      std::map< Point3d, LMGamma< float >, bestPoint3d >&,
	      PerfusionParameters&,
	      AimsData< float > *cbv,
	      AimsData< float > *cbf )
{
  int dx = cbv->dimX();
  int dy = cbv->dimY();
  int dz = cbv->dimZ();
  float sx = cbv->sizeX();
  float sy = cbv->sizeY();
  float sz = cbv->sizeZ();

  float val;

  m_map = AimsData< float >( dx, dy, dz );
  m_map.setSizeXYZT( sx, sy, sz );

  BucketMap< Void >::Bucket::iterator it = mask[ 0 ].begin();

  while( it != mask[0].end() )
    {
      Point3d pt = it->first;

      // MTT Map : MTT = CBV / CBF in sec
      val = (*cbv)( pt ) / ( ((*cbf)( pt ) < 0.001f) ? 0.001f : (*cbf)( pt ) );
      m_map( pt ) = ( val > 20.0f ) ? 20.0f : val;

      ++it;
    }

  return m_map;
}
