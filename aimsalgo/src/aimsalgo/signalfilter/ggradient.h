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



#ifndef AIMS_SIGNALFILTER_GGRADIENT_H
#define AIMS_SIGNALFILTER_GGRADIENT_H

#include <aims/utility/converter_volume.h>
#include <aims/signalfilter/gslices.h>
#include <aims/signalfilter/glines.h>
#include <aims/signalfilter/gcolumns.h>


template< class T >
class GaussianGradient
{
public:

  GaussianGradient( float sx=1.0f, float sy=1.0f, float sz=1.0f );
  virtual ~GaussianGradient() { }

  carto::VolumeRef< float >  doit( const carto::rc_ptr<carto::Volume<T> >& );
  AimsVector< carto::VolumeRef< float >, 3 > doitGradientVector(
    const carto::rc_ptr<carto::Volume< T > >& data ) ;
private:
  
  float sigx;
  float sigy;
  float sigz;
};


template< class T > inline
GaussianGradient< T >::GaussianGradient( float sx, float sy, float sz )
  : sigx( sx ), sigy( sy ), sigz( sz )
{
  ASSERT( sigx >= 0.1f && sigx <= 100.0f );
  ASSERT( sigy >= 0.1f && sigy <= 100.0f );
  ASSERT( sigz >= 0.1f && sigz <= 100.0f );
}


template< class T > inline carto::VolumeRef< float >
GaussianGradient< T >::doit( const carto::rc_ptr<carto::Volume< T > >& data )
{
  int x,y,z;
  std::vector<float> vs = data->getVoxelSize();
  float sx = sigx / vs[0];
  float sy = sigy / vs[1];
  float sz = sigz / vs[2];

  carto::Converter< carto::VolumeRef<T>, carto::VolumeRef<float> > conv;
  AimsVector< carto::VolumeRef< float >, 3 > res;

  carto::VolumeRef< float> imaF;
  imaF=carto::VolumeRef<float>( data->getSize() );
  conv.convert( data, imaF );

  for ( int i=0; i<3; i++ )
	  res[i]=imaF.copy();



  carto::VolumeRef<float> grad;
  grad=carto::VolumeRef<float>( data->getSize() );

  GaussianSlices gsli;
  GaussianLines glin;
  GaussianColumns gcol;

  // d / dx
  glin.doit( res[ 0 ], GCoef( sx, GCoef::gradient ) );
  gcol.doit( res[ 0 ], GCoef( sy ) );  // because default is smoothing
  gsli.doit( res[ 0 ], GCoef( sz ) );

  // d / dy
  glin.doit( res[ 1 ], GCoef( sx ) );
  gcol.doit( res[ 1 ], GCoef( sy, GCoef::gradient ) );
  gsli.doit( res[ 1 ], GCoef( sz ) );

  // d / dz
  glin.doit( res[ 2 ], GCoef( sx ) );
  gcol.doit( res[ 2 ], GCoef( sy ) );
  gsli.doit( res[ 2 ], GCoef( sz, GCoef::gradient ) );

  for (z=0; z< data->getSizeZ(); z++)
  	for (y=0; y< data->getSizeY(); y++)
		for(x=0; x< data->getSizeX(); x++)
		{
			grad(x,y,z)=sqrt ( (res[0](x,y,z)*res[0](x,y,z))
									+	(res[1](x,y,z)*res[1](x,y,z))
									+	(res[2](x,y,z)*res[2](x,y,z)) );
		}

  return grad;
}

template< class T > inline AimsVector< carto::VolumeRef< float >, 3 >
GaussianGradient< T >::doitGradientVector(
  const carto::rc_ptr<carto::Volume< T > >& data )
{
  std::vector<float> vs = data->getVoxelSize();
  float sx = sigx / vs[0];
  float sy = sigy / vs[1];
  float sz = sigz / vs[2];

  carto::Converter< carto::VolumeRef<T>, carto::VolumeRef<float> > conv;
  AimsVector< carto::VolumeRef< float >, 3 > res;

  carto::VolumeRef< float> imaF;
  imaF=carto::VolumeRef<float>( data->getSize() );
  conv.convert( data, imaF );

  for ( int i=0; i<3; i++ )
	  res[i]=imaF.copy();



  carto::VolumeRef<float> grad;
  grad=carto::VolumeRef<float>( data->getSize() );

  GaussianSlices gsli;
  GaussianLines glin;
  GaussianColumns gcol;

  // d / dx
  glin.doit( res[ 0 ], GCoef( sx, GCoef::gradient ) );
  gcol.doit( res[ 0 ], GCoef( sy ) );  // because default is smoothing
  gsli.doit( res[ 0 ], GCoef( sz ) );

  // d / dy
  glin.doit( res[ 1 ], GCoef( sx ) );
  gcol.doit( res[ 1 ], GCoef( sy, GCoef::gradient ) );
  gsli.doit( res[ 1 ], GCoef( sz ) );

  // d / dz
  glin.doit( res[ 2 ], GCoef( sx ) );
  gcol.doit( res[ 2 ], GCoef( sy ) );
  gsli.doit( res[ 2 ], GCoef( sz, GCoef::gradient ) );

  return res ;
}
#endif
