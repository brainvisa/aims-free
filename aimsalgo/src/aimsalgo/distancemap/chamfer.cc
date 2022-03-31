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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/distancemap/chamfer.h>
#include <cartodata/volume/volume.h>
#include <cartobase/containers/nditerator.h>
#include <iostream>

using namespace carto;
using namespace std;

static void AimsVolumeChamferPreparation(rc_ptr<Volume<int16_t> > &vol);


static void AimsVolumeChamferPreparation(rc_ptr<Volume<int16_t> > &vol)
{
  line_NDIterator<int16_t> it( &vol->at( 0 ), vol->getSize(),
                               vol->getStrides(), true );
  int16_t *p, *pp;
  for( ; !it.ended(); ++it )
  {
    p = &*it;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
    {
      if (*p)
        *p = 0;
      else
        *p = AIMS_CHAMFER_DOMAIN;
    }
  }
}


VolumeRef<int16_t>
AimsConnectivityChamferDistanceMap( const rc_ptr<Volume<int16_t> > &vol,
                                    Connectivity::Type connectivity)
{
  ASSERT( vol->getSizeT() == 1 );

  VolumeRef<int16_t> volbin = new Volume<int16_t>( *vol );
  AimsDistmapMask f,b;


  ASSERT( volbin->getBorders()[0] > 0 && volbin->getBorders()[1] > 0
          && volbin->getBorders()[2] > 0 );
  volbin->fillBorder( AIMS_CHAMFER_OUTSIDE_DOMAIN );

  AimsVolumeChamferPreparation(volbin);

  AimsCreateAndSplitConnectivityMask(volbin, connectivity, f, b);
				         
  //cout << "Forward" << endl;
  AimsForwardSweepingWithBorder (volbin,f,AIMS_CHAMFER_OUTSIDE_DOMAIN);
  //cout << "Backward" << endl;
  AimsBackwardSweepingWithBorder(volbin,b,AIMS_CHAMFER_OUTSIDE_DOMAIN);

  return volbin;
}


VolumeRef<int16_t>
AimsChamferDistanceMap( const rc_ptr<Volume<int16_t> > &vol,
                        int xmask,int ymask,int zmask,
                        float mult_factor)
{
  ASSERT( vol->getSizeT() == 1 );

  VolumeRef<int16_t> volbin = new Volume<int16_t>( *vol );
  AimsDistmapMask f,b;

  ASSERT( mult_factor > 0 );

  ASSERT( volbin->getBorders()[0] > 0 && volbin->getBorders()[1] > 0
          && volbin->getBorders()[2] > 0 );
  volbin->fillBorder( AIMS_CHAMFER_OUTSIDE_DOMAIN );

  AimsVolumeChamferPreparation(volbin);

  AimsCreateAndSplitCubicMask(volbin,xmask,ymask,zmask,f,b,mult_factor);
     
  //cout << "Forward" << endl;
  AimsForwardSweepingWithBorder (volbin,f,AIMS_CHAMFER_OUTSIDE_DOMAIN);
  //cout << "Backward" << endl;
  AimsBackwardSweepingWithBorder(volbin,b,AIMS_CHAMFER_OUTSIDE_DOMAIN);

  return volbin;
}


VolumeRef<float>
AimsFloatChamferDistanceMap( const rc_ptr<Volume<int16_t> > &vol, int side,
                             int xsize, int ysize, int zsize,
                             float mult_factor)
{
  ASSERT( vol->getSizeT() == 1 );

  VolumeRef<int16_t> distmap = new Volume<int16_t>( *vol );

  ASSERT( vol->getBorders()[0] > 0 && vol->getBorders()[1] > 0
          && vol->getBorders()[2] > 0 );

  int dx = distmap->getSizeX(), dy = distmap->getSizeY(),
    dz = distmap->getSizeZ();

  if( side==AIMS_CHAMFER_INSIDE )
    for( int z=0; z<dz; z++ )
      for( int y=0; y<dy; y++ )
        for( int x=0; x<dx; x++ )
          distmap(x,y,z) = (distmap(x,y,z)==0 ? 32767 : 0);  

  distmap = AimsChamferDistanceMap(distmap,
                                   xsize,ysize,zsize,mult_factor);

  VolumeRef<float> distfloat( distmap->getSize(), distmap->getBorders() );
  distfloat->copyHeaderFrom( vol->header() );

  distfloat->setVoxelSize( vol->getVoxelSize() );

  for( int z=0; z<dz; z++ )
    for( int y=0; y<dy; y++ )
      for( int x=0; x<dx; x++ )
        distfloat(x,y,z) = (float)distmap(x,y,z) / mult_factor;  
 
  return distfloat;
}


VolumeRef<float>
AimsFloatSignedChamferDistanceMap( const rc_ptr<Volume<int16_t> > &vol,
                                   int xsize,int ysize,int zsize,
                                   float mult_factor)
{
  ASSERT( vol->getSizeT() == 1 );
  ASSERT( vol->getBorders()[0] > 0 && vol->getBorders()[1] > 0
          && vol->getBorders()[2] > 0 );
  VolumeRef<float>
  inside = AimsFloatChamferDistanceMap(vol,AIMS_CHAMFER_INSIDE ,
                                       xsize,ysize,zsize,mult_factor);
  VolumeRef<float>
  outside = AimsFloatChamferDistanceMap(vol,AIMS_CHAMFER_OUTSIDE,
                                        xsize,ysize,zsize,mult_factor);

  vector<int> sz = vol->getSize();
  VolumeRef<float> res( sz, vol->getBorders() );
  res.setVoxelSize( vol->getVoxelSize() );
  res->copyHeaderFrom( vol->header() );

  for( int z=0; z<sz[2]; z++ )
    for( int y=0; y<sz[1]; y++ )
      for( int x=0; x<sz[0]; x++ )
        res(x,y,z) = inside(x,y,z) - outside(x,y,z);  

  return res;
}
