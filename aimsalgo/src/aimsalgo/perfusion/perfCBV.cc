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


#include <aims/perfusion/perfCBV.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


CBVMap::CBVMap() : PerfusionMapBase( PerfusionMapBase::cbv )
{
  _backdeps.push_back( (int)PerfusionMapBase::cbf );
  _backdeps.push_back( (int)PerfusionMapBase::mtt );
}


void CBVMap::saveMap( const string& filename )
{
  string filebasename( FileUtil::basename( filename ) );
  string directoryname( FileUtil::dirname( filename ) );

  std::string mapFilename = directoryname + "/cbv_" + filebasename;

  doSave( mapFilename );
}


AimsData< float >& 
CBVMap::doit( AimsData< float >&, 
	      AimsData< float >& d,
	      AimsData< float >& aif,
	      AimsData< float >&,
	      aims::BucketMap< Void >& mask, 
	      LMGamma< float >&,
	      std::map< Point3d, LMGamma< float >, bestPoint3d >&,
	      PerfusionParameters& pp,
	      AimsData< float > *,
	      AimsData< float > * ) 
{
  // kh = (1-Hart)/(1-Hcap), with Hart=0.45 and Hcap=0.25
  // rho = 1.04 g/mm3
  float kr = ( 1.0f - 0.45f) / ( ( 1.0f - 0.25f ) * 1.04f );

  int skip = pp.skip();
  float dose = pp.dose();
  float phiGd = pp.phiGd();

  int t, dt = d.dimT();
  int dx = d.dimX();
  int dy = d.dimY();
  int dz = d.dimZ();
  float sx = d.sizeX();
  float sy = d.sizeY();
  float sz = d.sizeZ();
  float st = d.sizeT();

  float yInt;

  m_map = AimsData< float >( dx, dy, dz );
  m_map.setSizeXYZT( sx, sy, sz );

  // AIF(t) integration
  //float limSup = (float)dt * st;
  //TrapezeIntegrator ti( 20 );
  //PerfusionIntegrable paif( &aif );
  //float aifInt = ti.eval( paif, 0.0f, limSup );
  float aifInt = 0.0f;
  for ( t=0; t<dt; t++ )  aifInt += aif( t ) * st;

  BucketMap< Void >::Bucket::iterator it = mask[ 0 ].begin();

  while( it != mask[0].end() )
    {
      Point3d pt = it->first;

      // Cm(t) integration
      yInt = 0.0f;
      for ( t=skip; t<dt; t++ )  yInt += d (  pt[0], pt[1], pt[2], t ) * st;

      // CBV Map : CBV = kh.Int(Cm(t))/(rho.Int(Ca(t)))*phiGd/dose
      m_map( pt ) = phiGd * kr * yInt / aifInt / dose;

      ++it;
    }

  return m_map;
}
