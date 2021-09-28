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
#include <aims/transformation/transformation_chain.h>
#include <aims/utility/progress.h>
#include <stdexcept>


namespace { // anonymous namespace for file-local symbols

template <class T>
carto::const_ref<T> external_ref(const T& object)
{
  return carto::const_ref<T>(&object, true);
}

} // anonymous namespace


namespace aims
{

template <typename T>
Resampler<T>::Resampler()
  : _ref( 0 ), _defval( T() ), _verbose_stream( &std::cout )
{
}


template <typename T>
void Resampler<T>::
resample_inv_to_vox( const carto::Volume< T >& inVolume,
                     const soma::Transformation3d& inverse_transform_to_vox,
                     const T& outBackground,
                     carto::Volume< T > & outVolume,
                     bool verbose ) const
{

  Point3df outResolution;
  std::vector<float> vs = outVolume.getVoxelSize();
  outResolution[0] = vs[0];
  outResolution[1] = vs[1];
  outResolution[2] = vs[2];

  std::vector<int> sz = outVolume.getSize();
  int outSizeX = sz[0];
  int outSizeY = sz[1];
  int outSizeZ = sz[2];
  int outSizeT = sz[3];
  if( outSizeT > inVolume.getSizeT() )
    outSizeT = inVolume.getSizeT();

  T* o;

  std::ostream *stream = _verbose_stream;
  if( !stream )
    stream = &std::cout;
  aims::Progression progress(0, static_cast<size_t>(outSizeX)
                             * outSizeY * outSizeZ * outSizeT, 0, 100, "%", 3,
                             *stream );
  int x, y, z, t;
  for ( t = 0; t < outSizeT; t++ )
    {
      updateParameters( inVolume, t, verbose );

      for ( z = 0; z < outSizeZ; z++ )
        {

          for ( y = 0; y < outSizeY; y++ )
            {
              o = &outVolume.at( 0, y, z, t );

              for ( x = 0; x < outSizeX; x++ )
                {
                  const Point3df outLoc ( x * outResolution[0],
                                          y * outResolution[1],
                                          z * outResolution[2] );

                  doResample( inVolume, inverse_transform_to_vox, outBackground,
                              outLoc, *o, t );
                  ++ o;
                  ++progress;
                }

              if(verbose || _verbose_stream) {
                progress.print();
              }
            }
        }
    }
}


template <typename T>
void Resampler<T>::
resample_inv( const carto::Volume< T >& input_data,
              const soma::Transformation3d& inverse_transform_to_mm,
              const T& background,
              carto::Volume< T > & output_volume,
              bool verbose ) const
{

  Point3df in_voxel_size( input_data.getVoxelSize() );

  aims::AffineTransformation3d mm_to_voxel_transform;
  mm_to_voxel_transform.scale( Point3df( 1, 1, 1 ), in_voxel_size );

  aims::TransformationChain3d transform_chain;
  transform_chain.push_back(external_ref(inverse_transform_to_mm));
  transform_chain.push_back(external_ref(mm_to_voxel_transform));

  resample_inv_to_vox(input_data, transform_chain, background,
                      output_volume, verbose);
}

template <typename T>
void Resampler<T>::
resample_inv( const carto::Volume< T >& input_data,
              const soma::Transformation3d& inverse_transform_to_mm,
              const T& background,
              const Point3df& output_location,
              T &output_value,
              int timestep ) const
{

  Point3df in_voxel_size( input_data.getVoxelSize() );

  aims::AffineTransformation3d mm_to_voxel_transform;
  mm_to_voxel_transform.scale( Point3df( 1, 1, 1 ), in_voxel_size );

  aims::TransformationChain3d transform_chain;
  transform_chain.push_back(external_ref(inverse_transform_to_mm));
  transform_chain.push_back(external_ref(mm_to_voxel_transform));

  resample_inv_to_vox(input_data, transform_chain, background,
                      output_location, output_value, timestep);
}


template < typename T >
void
Resampler< T >::resample( const carto::Volume< T >& inVolume,
                          const aims::AffineTransformation3d& transform3d,
                          const T& outBackground,
                          carto::Volume< T > & outVolume,
                          bool verbose ) const
{

  Point3df inResolution( inVolume.getVoxelSize() );

  aims::AffineTransformation3d normTransform3d;

  normTransform3d.scale( inResolution, Point3df( 1, 1, 1 ) );
  normTransform3d = transform3d * normTransform3d;
  normTransform3d = normTransform3d.inverse();

  resample_inv_to_vox(inVolume, normTransform3d, outBackground, outVolume,
                      verbose);

}


template < typename T >
void
Resampler< T >::resample( const carto::Volume< T >& inVolume,
                          const aims::AffineTransformation3d& transform3d,
                          const T& outBackground,
                          const Point3df& outLocation,
                          T& outValue, int t ) const
{

  Point3df inResolution( inVolume.getVoxelSize() );

  aims::AffineTransformation3d normTransform3d;

  normTransform3d.scale( inResolution, Point3df( 1, 1, 1 ) );
  normTransform3d = transform3d * normTransform3d;
  normTransform3d = normTransform3d.inverse();

  updateParameters( inVolume, t, carto::verbose );

  doResample( inVolume, normTransform3d, outBackground, outLocation,
              outValue, t );

}


template <typename T>
void Resampler<T>::doit( const aims::AffineTransformation3d& motion,
                         carto::Volume<T> & thing ) const
{
  if( _ref.isNull() )
    throw std::runtime_error( "Resampler used without a ref volume to resample"
    );
  resample( *_ref, motion, _defval, thing, carto::verbose );
}


template <typename T>
carto::VolumeRef<T> Resampler<T>::doit(
  const aims::AffineTransformation3d& motion,
  int dimX, int dimY, int dimZ,
  const Point3df& resolution ) const
{
  if( _ref.isNull() )
    throw std::runtime_error( "Resampler used without a ref volume to resample"
    );
  carto::VolumeRef<T>	thing( dimX, dimY, dimZ, _ref->getSizeT() );
  std::vector<float> vs( 4, 1. );
  vs[0] = resolution[0];
  vs[1] = resolution[1];
  vs[2] = resolution[2];
  vs[3] = _ref->getVoxelSize()[3];
  thing->header().setProperty( "voxel_size", vs );

  resample( *_ref, motion, _defval, *thing, carto::verbose );
  thing->copyHeaderFrom( _ref->header() );
  thing->header().setProperty( "voxel_size", vs );
  if( !motion.isIdentity() )
  {
    carto::PropertySet & ph = thing->header();
    try
    {
      // remove any referential ID since we are no longer in the same ref
      ph.removeProperty( "referential" );
    }
    catch( ... )
    {
    }
    try
    {
      carto::Object trs = ph.getProperty( "transformations" );
      carto::Object tit = trs->objectIterator();
      aims::AffineTransformation3d motioninv = motion.inverse();
      std::vector<std::vector<float> > trout;
      trout.reserve( trs->size() );
      for( ; tit->isValid(); tit->next() )
      {
        aims::AffineTransformation3d m( tit->currentValue() );
        m *= motioninv;
        trout.push_back( m.toVector() );
      }
      ph.setProperty( "transformations", trout );
    }
    catch( ... )
    {
      // setup a new transformations list
      std::vector<std::vector<float> > trout;
      std::vector<std::string> refsout;
      const carto::PropertySet & iph = _ref->header();
      try
      {
        carto::Object iref = iph.getProperty( "referential" );
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
      ph.setProperty( "transformations", trout );
      ph.setProperty( "referentials", refsout );
    }
  }
  return thing;
}


template <typename T>
void Resampler<T>::setRef(const carto::rc_ptr<carto::Volume<T> >& ref)
{
  _ref = ref;
}

} // namespace aims

#endif
