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


#ifndef AIMS_SIGNALFILTER_G3DSMOOTH_H
#define AIMS_SIGNALFILTER_G3DSMOOTH_H

#include <aims/utility/converter_volume.h>
#include <aims/signalfilter/glines.h>
#include <aims/signalfilter/gcolumns.h>
#include <aims/signalfilter/gslices.h>


template< class T >
class Gaussian3DSmoothing
{
public:

  Gaussian3DSmoothing( float sx=1.0f, float sy=1.0f, float sz=1.0f );
  virtual ~Gaussian3DSmoothing() { }

  AimsData< T >  doit( const AimsData<T>& );

private:

  float sigx;
  float sigy;
  float sigz;
};


template< class T > inline 
Gaussian3DSmoothing< T >::Gaussian3DSmoothing( float sx, float sy, float sz )
  : sigx( sx ), sigy( sy ), sigz( sz )
{
  ASSERT( sigx >= 0.1f && sigx <= 100.0f );
  ASSERT( sigy >= 0.1f && sigy <= 100.0f );
  ASSERT( sigz >= 0.1f && sigz <= 100.0f );
}


template< class T > inline
AimsData< T > Gaussian3DSmoothing< T >::doit( const AimsData< T >& data )
{
  float sx = sigx / data.sizeX();
  float sy = sigy / data.sizeY();
  float sz = sigz / data.sizeZ();

  carto::Converter< AimsData<T>, AimsData<float> > conv;
  AimsData< float >
    dataF( data.dimX(), data.dimY(), data.dimZ(), data.dimT() );
  dataF.setSizeX(data.sizeX());
  dataF.setSizeY(data.sizeY());
  dataF.setSizeZ(data.sizeZ());
  conv.convert( data, dataF );
  
  GaussianSlices gsli;
  gsli.doit( dataF, GCoef( sz ) );
  
  GaussianLines glin;
  glin.doit( dataF, GCoef( sx ) );
  
  GaussianColumns gcol;
  gcol.doit( dataF, GCoef( sy ) );

  carto::Converter< AimsData<float>, AimsData<T> > conv2;
  AimsData<T>	data3( data.dimX(), data.dimY(), data.dimZ(), data.dimT() );
  conv2.convert( dataF, data3 );

  return data3;
}

#endif
