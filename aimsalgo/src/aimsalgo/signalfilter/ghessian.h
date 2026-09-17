
#ifndef AIMS_SIGNALFILTER_GHESSIAN_H
#define AIMS_SIGNALFILTER_GHESSIAN_H

#include <aims/utility/converter_volume.h>
#include <aims/signalfilter/gslices.h>
#include <aims/signalfilter/glines.h>
#include <aims/signalfilter/gcolumns.h>


template< class T >
class GaussianHessian
{
public:

  GaussianHessian( float sx=1.0f, float sy=1.0f, float sz=1.0f );
  virtual ~GaussianHessian() { }

  AimsVector< carto::VolumeRef< float >, 6 > doit(
    const carto::rc_ptr<carto::Volume<T> >& );

private:

  float sigx;
  float sigy;
  float sigz;
};


template< class T > inline 
GaussianHessian< T >::GaussianHessian( float sx, float sy, float sz )
  : sigx( sx ), sigy( sy ), sigz( sz )
{
  ASSERT( sigx >= 0.1f && sigx <= 100.0f );
  ASSERT( sigy >= 0.1f && sigy <= 100.0f );
  ASSERT( sigz >= 0.1f && sigz <= 100.0f );
}


template< class T > inline AimsVector< carto::VolumeRef< float >, 6 >
GaussianHessian< T >::doit( const carto::rc_ptr<carto::Volume< T > >& data )
{
  std::vector<float> vs = data->getVoxelSize();
  float sx = sigx / vs[0];
  float sy = sigy / vs[1];
  float sz = sigz / vs[2];

  carto::Converter< carto::VolumeRef<T>, carto::VolumeRef<float> > conv;
  AimsVector< carto::VolumeRef< float >, 6 > res;

 carto::VolumeRef< float> imaF;
  imaF=carto::VolumeRef<float>( data->getSize() );
  conv.convert( data, imaF );

  for ( int i=0; i<6; i++ )
    res[i]=imaF.copy();

  GaussianSlices gsli;
  GaussianLines glin;
  GaussianColumns gcol;

  // d2 / dx2
  glin.doit( res[ 0 ], GCoef( sx, GCoef::laplacian ) );
  gcol.doit( res[ 0 ], GCoef( sy ) );  // because default is smoothing
  gsli.doit( res[ 0 ], GCoef( sz ) );

  // d2 / dxdy
  glin.doit( res[ 1 ], GCoef( sx, GCoef::gradient ) );
  gcol.doit( res[ 1 ], GCoef( sy, GCoef::gradient ) );
  gsli.doit( res[ 1 ], GCoef( sz ) );

  // d2 / dxdz
  glin.doit( res[ 2 ], GCoef( sx, GCoef::gradient ) );
  gcol.doit( res[ 2 ], GCoef( sy ) );
  gsli.doit( res[ 2 ], GCoef( sz, GCoef::gradient ) );

  // d2 / dy2
  glin.doit( res[ 3 ], GCoef( sx ) );
  gcol.doit( res[ 3 ], GCoef( sy, GCoef::laplacian ) );
  gsli.doit( res[ 3 ], GCoef( sz ) );

  // d2 / dydz
  glin.doit( res[ 4 ], GCoef( sx ) );
  gcol.doit( res[ 4 ], GCoef( sy, GCoef::gradient ) );
  gsli.doit( res[ 4 ], GCoef( sz, GCoef::gradient ) );

  // d2 / dz2
  glin.doit( res[ 5 ], GCoef( sx ) );
  gcol.doit( res[ 5 ], GCoef( sy ) );
  gsli.doit( res[ 5 ], GCoef( sz, GCoef::laplacian ) );

  return res;
}

#endif
