
#ifndef AIMS_SIGNALFILTER_G3DSMOOTH_H
#define AIMS_SIGNALFILTER_G3DSMOOTH_H

#include <aims/utility/converter_volume.h>
#include <aims/signalfilter/glines.h>
#include <aims/signalfilter/gcolumns.h>
#include <aims/signalfilter/gslices.h>


/** 3D Deriche's recursive gaussian smoothing filter
*/
template< class T >
class Gaussian3DSmoothing
{
public:

  Gaussian3DSmoothing( float sx=1.0f, float sy=1.0f, float sz=1.0f );
  virtual ~Gaussian3DSmoothing() { }

  carto::VolumeRef< T >  doit( const carto::rc_ptr<carto::Volume<T> >& );

private:

  float sigx;
  float sigy;
  float sigz;
};


template< class T > inline 
Gaussian3DSmoothing< T >::Gaussian3DSmoothing( float sx, float sy, float sz )
  : sigx( sx ), sigy( sy ), sigz( sz )
{
//   ASSERT( sigx >= 0.1f && sigx <= 100.0f );
//   ASSERT( sigy >= 0.1f && sigy <= 100.0f );
//   ASSERT( sigz >= 0.1f && sigz <= 100.0f );
}


template< class T > inline
carto::VolumeRef< T > Gaussian3DSmoothing< T >::doit(
  const carto::rc_ptr<carto::Volume< T > >& data )
{
  std::vector<float> vs = data->getVoxelSize();
  float sx = sigx / vs[0];
  float sy = sigy / vs[1];
  float sz = sigz / vs[2];

  carto::Converter< carto::VolumeRef<T>, carto::VolumeRef<float> > conv;
  carto::VolumeRef< float > dataF( data->getSize() );
  dataF.setVoxelSize(data->getVoxelSize());
  conv.convert( data, dataF );
  
  GaussianSlices gsli;
  gsli.doit( dataF, GCoef( sz ) );
  
  GaussianLines glin;
  glin.doit( dataF, GCoef( sx ) );
  
  GaussianColumns gcol;
  gcol.doit( dataF, GCoef( sy ) );

  carto::Converter< carto::VolumeRef<float>, carto::VolumeRef<T> > conv2;
  carto::VolumeRef<T>	data3( data->getSize() );
  conv2.convert( dataF, data3 );

  return data3;
}

#endif
