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


#ifndef AIMS_RESAMPLING_RESAMPLER_D_H
#define AIMS_RESAMPLING_RESAMPLER_D_H

#include <aims/resampling/resampler.h>
#include <cartobase/config/verbose.h>
#include <iomanip>
#include <stdexcept>

template < typename T >
void 
Resampler< T >::resample( const AimsData< T >& inVolume,
                          const Motion& transform3d,
                          const T& outBackground,
                          AimsData< T >& outVolume,
                          bool verbose )
{

  Point3df inResolution;
  inResolution[0] = inVolume.sizeX();
  inResolution[1] = inVolume.sizeY();
  inResolution[2] = inVolume.sizeZ();

  Point3df outResolution;
  outResolution[0] = outVolume.sizeX();
  outResolution[1] = outVolume.sizeY();
  outResolution[2] = outVolume.sizeZ();

  Motion normTransform3d;

  normTransform3d.scale( inResolution, Point3df( 1, 1, 1 ) );
  normTransform3d = transform3d * normTransform3d;
  normTransform3d = normTransform3d.inverse();

  int outSizeX = outVolume.dimX();
  int outSizeY = outVolume.dimY();
  int outSizeZ = outVolume.dimZ();
  int outSizeT = outVolume.dimT();
  if( outSizeT > inVolume.dimT() )
    outSizeT = inVolume.dimT();

  typename AimsData< T >::iterator o;

  if ( verbose )
    {

      std::cout << std::setw( 4 ) << 0;
      if( outSizeT > 1 )
        std::cout << ", t: " << std::setw( 4 ) << 0;
      std::cout << std::flush;

    }
  Point3df outLoc;
  int x, y, z, t;
  for ( t = 0; t < outSizeT; t++ )
    {
      updateParameters( inVolume, t, verbose );
      outLoc = Point3df( 0.0, 0.0, 0.0 );

      for ( z = 0; z < outSizeZ; z++ )
        {

          for ( y = 0; y < outSizeY; y++ )
            {
              o = &outVolume( 0, y, z, t );

              for ( x = 0; x < outSizeX; x++ )
                {

                  doResample( inVolume, normTransform3d, outBackground, 
                              outLoc, *o, t );
                  ++ o;
                  outLoc[0] += outResolution[0];

                }
              outLoc[1] += outResolution[1];
              outLoc[0] = 0.0;

            }
          outLoc[2] += outResolution[2];
          outLoc[1] = 0.0;

          if ( verbose )
            {

              if( outSizeT > 1 )
                std::cout << "\b\b\b\b\b\b\b\b\b";
              std::cout << "\b\b\b\b" << std::setw( 4 ) << z + 1;
              if( outSizeT > 1 )
                std::cout << ", t: " << std::setw( 4 ) << t;
              std::cout << std::flush;

            }

        } 

    }

}


template < typename T >
void
Resampler< T >::resample( const AimsData< T >& inVolume,
                          const Motion& transform3d,
                          const T& outBackground,
                          const Point3df& outLocation,
                          T& outValue, int t )
{

  Point3df inResolution;
  inResolution[0] = inVolume.sizeX();
  inResolution[1] = inVolume.sizeY();
  inResolution[2] = inVolume.sizeZ();

  Motion normTransform3d;

  normTransform3d.scale( inResolution, Point3df( 1, 1, 1 ) );
  normTransform3d = transform3d * normTransform3d;
  normTransform3d = normTransform3d.inverse();

  doResample( inVolume, normTransform3d, outBackground, outLocation,
              outValue, t );

}


template <typename T>
void Resampler<T>::updateParameters( const AimsData< T > &, int, 
                                     bool )
{
}


template <typename T>
void Resampler<T>::doit( const Motion& motion, AimsData<T>& thing )
{
  if( !_ref )
    throw std::runtime_error( "Resampler used without a ref volme to resample"
    );
  resample( *_ref, motion, _defval, thing, carto::verbose );
}


template <typename T>
AimsData<T> Resampler<T>::doit( const Motion& motion, int dimX, int dimY, 
                                int dimZ, const Point3df& resolution )
{
  if( !_ref )
    throw std::runtime_error( "Resampler used without a ref volme to resample"
    );
  AimsData<T>	thing( dimX, dimY, dimZ, _ref->dimT() );
  thing.setSizeXYZT( resolution[0], resolution[1], resolution[2], 
                     _ref->sizeT() );

  resample( *_ref, motion, _defval, thing, carto::verbose );
  thing.setHeader( _ref->header()->cloneHeader() );
  thing.setSizeXYZT( resolution[0], resolution[1], resolution[2],
                     _ref->sizeT() );
  if( !motion.isIdentity() )
  {
    aims::PythonHeader
        *ph = dynamic_cast<aims::PythonHeader *>( thing.header() );
    try
    {
      // remove any referential ID since we are no longer in the same ref
      ph->removeProperty( "referential" );
    }
    catch( ... )
    {
    }
    try
    {
      if( ph )
      {
        carto::Object trs = ph->getProperty( "transformations" );
        carto::Object tit = trs->objectIterator();
        Motion motioninv = motion.inverse();
        std::vector<std::vector<float> > trout;
        trout.reserve( trs->size() );
        for( ; tit->isValid(); tit->next() )
        {
          Motion m( tit->currentValue() );
          m *= motioninv;
          trout.push_back( m.toVector() );
        }
        ph->setProperty( "transformations", trout );
      }
    }
    catch( ... )
    {
      // setup a new transformations list
      std::vector<std::vector<float> > trout;
      std::vector<std::string> refsout;
      const aims::PythonHeader
          *iph = dynamic_cast<const aims::PythonHeader *>( _ref->header() );
      if( iph )
        try
        {
          carto::Object iref = iph->getProperty( "referential" );
          std::string refid = iref->getString();
          refsout.push_back( refid );
        }
        catch( ... )
        {
        }
      if( refsout.empty() )
        refsout.push_back( "Coordinates aligned to another file or to "
            "anatomical truth" );

      trout.push_back( motion.inverse().toVector() );
      ph->setProperty( "transformations", trout );
      ph->setProperty( "referentials", refsout );
    }
  }
  return thing;
}

#endif

