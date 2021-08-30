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
