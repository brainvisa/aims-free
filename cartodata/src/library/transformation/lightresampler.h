#ifndef CARTODATA_TRANSFORMATION_LIGHTRESAMPLER_H
#define CARTODATA_TRANSFORMATION_LIGHTRESAMPLER_H

#include <cartodata/volume/volume.h>
#include <soma-io/transformation/affinetransformation3d_base.h>
#include <cartobase/containers/nditerator.h>


namespace carto
{

  /** Light, simple resampler used for flipping operations.

      Contrarily to Resampler classes in aimsalgo, no interpolation is done,
      neither in space nor in values. We take the nearest voxel, report its
      value in the output. This also ensures the conservation of values in
      flipping operations.
  */
  template <typename T>
  class LightResampler
  {
  public:
      static VolumeRef<T> allocateResampledVolume(
        const Volume<T> & vol, const AffineTransformationBase & tr );
      static void resampleVolume(
        const Volume<T> & input, Volume<T> & output,
        const AffineTransformationBase & tr, const T & background = 0 );
      /// adapt info in header (referential, transformations)
      static Object transformHeader( const Object & header,
                                     const AffineTransformationBase & tr );
      static std::vector<int> getTransformedDims(
        const std::vector<int> & dims, const AffineTransformationBase & tr );
      static std::vector<float> getTransformedVoxelSize(
        const std::vector<float> & vs, const AffineTransformationBase & tr );
  };


  template <typename T>
  std::vector<int> LightResampler<T>::getTransformedDims(
    const std::vector<int> & dims, const AffineTransformationBase & tr )
  {
    int d, n = dims.size(), i;
    std::vector<int> out_dims( n, 0 );
    for( d=0; d<n; ++d )
    {
      std::vector<int> p( n, 0 );
      p[d] = dims[d];
      p = tr.transformVector( p );
      for( i=0; i<n; ++i )
        out_dims[i] += p[i];
    }
    for( d=0; d<n; ++d )
      out_dims[d] = fabs( out_dims[d] );

    return out_dims;
  }


  template <typename T>
  std::vector<float> LightResampler<T>::getTransformedVoxelSize(
    const std::vector<float> & vs, const AffineTransformationBase & tr )
  {
    unsigned d, n = tr.order(), nv = vs.size(), i;
    n = std::max( n, nv );
    std::vector<float> out_vs( n, 0.f );
    for( d=0; d<n; ++d )
    {
      std::vector<float> p( n, 0.f );
      if( d < nv )
        p[d] = vs[d];
      else
        p[d] = 1.f;
      p = tr.transformVector( p );
      for( i=0; i<n; ++i )
        out_vs[i] += p[i];
    }
    for( d=0; d<n; ++d )
      out_vs[d] = fabs( out_vs[d] );

    return out_vs;
  }


  template <typename T>
  VolumeRef<T> LightResampler<T>::allocateResampledVolume(
    const Volume<T> & vol, const AffineTransformationBase & tr )
  {
    std::vector<int> odim = getTransformedDims( vol.getSize(), tr );
    VolumeRef<T> res( odim );
    Object reshdr = transformHeader( Object::reference( vol.header() ), tr );
    res.copyHeaderFrom( reshdr );
    res.setVoxelSize( getTransformedVoxelSize( vol.getVoxelSize(), tr ) );

    return res;
  }


  template <typename T>
  Object LightResampler<T>::transformHeader(
    const Object & header,  const AffineTransformationBase & tr )
  {
    Object out_hdr;
    return out_hdr;
  }


  template <typename T>
  void LightResampler<T>::resampleVolume(
    const Volume<T> & input, Volume<T> & output,
    const AffineTransformationBase & tr, const T & background )
  {
    std::vector<float> ovs = output.getVoxelSize();
    while( ovs.size() < 3 )
      ovs.push_back( 1. );
    std::vector<float> vs = input.getVoxelSize();
    while( vs.size() < 3 )
      vs.push_back( 1. );

    std::unique_ptr<AffineTransformationBase> trinv = tr.inverse();
    // embed voxel size in trinv
    AffineTransformationBase ivs( tr.order() );
    unsigned i, n = vs.size();
    for( i=0; i<n; ++i )
      ivs.matrix()( i, i ) = 1.f / vs[i];
    *trinv = ivs * *trinv;

    line_NDIterator<T> oit( &output.at( 0 ), output.getSize(),
                            output.getStrides(), true );

    n = output.getSize().size();
    std::vector<float> ipos( n, 0.f );
    ipos[oit.line_direction()] = ovs[oit.line_direction()];
    std::vector<float> xoff = trinv->transformVector( ipos );
    std::vector<int> vipos;
    std::vector<int> idims = input.getSize();
    int dx = idims[0], dy = idims[1], dz = idims[2];
    unsigned m = idims.size();

    T *p, *pp;

    for( ; !oit.ended(); ++oit )
    {
      p = &*oit;
      vipos = oit.position();
      ipos.clear();
      ipos.insert( ipos.end(), vipos.begin(), vipos.end() );
      for( i=0; i<n; ++i )
        ipos[i] *= ovs[i];

      ipos = trinv->transform( ipos );
      for( pp=p + oit.line_length(); p!=pp; oit.inc_line_ptr( p ) )
      {
        bool doit = true;
        for( i=0; i<m; ++i )
        {
          vipos[i] = int(rint(ipos[i]));
          if( vipos[i] < 0 || vipos[i] >= idims[i] )
          {
            *p = background;
            doit = false;
            break;
          }
        }
        if( doit )
          *p = input.at( vipos );
        for( i=0; i<m; ++i )
          ipos[i] += xoff[i];
      }
    }
  }

}


#endif

