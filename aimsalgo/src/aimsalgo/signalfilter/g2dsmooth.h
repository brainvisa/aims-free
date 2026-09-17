
#ifndef AIMS_SIGNALFILTER_G2DSMOOTH_H
#define AIMS_SIGNALFILTER_G2DSMOOTH_H

#include <aims/utility/converter_volume.h>
#include <aims/signalfilter/glines.h>
#include <aims/signalfilter/gcolumns.h>


template< class T >
class Gaussian2DSmoothing
{
public:

  Gaussian2DSmoothing( float sx=1.0f, float sy=1.0f );
  virtual ~Gaussian2DSmoothing() { }

  carto::VolumeRef< T >  doit( const carto::rc_ptr<carto::Volume<T> > & );

private:

  float sigx;
  float sigy;
};


template< class T > inline 
Gaussian2DSmoothing< T >::Gaussian2DSmoothing( float sx, float sy )
  : sigx( sx ), sigy( sy )
{
  ASSERT( sigx >= 0.1f && sigx <= 100.0f );
  ASSERT( sigy >= 0.1f && sigy <= 100.0f );
}


template< class T > inline
carto::VolumeRef< T > Gaussian2DSmoothing< T >::doit(
  const carto::rc_ptr<carto::Volume< T > > & data )
{
  float sx = sigx / data->getVoxelSize()[0];
  float sy = sigy / data->getVoxelSize()[0];

  carto::Converter< carto::VolumeRef<T>, carto::VolumeRef<float> >
    conv;
  carto::VolumeRef< float > dataF( data->getSize() );
  dataF.setVoxelSize(data->getVoxelSize());
  conv.convert( data, dataF );

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
