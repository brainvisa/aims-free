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
        const Volume<T> & vol, const AffineTransformation3dBase & tr );
      static void resampleVolume(
        const Volume<T> & input, Volume<T> & output,
        const AffineTransformation3dBase & tr, const T & background = 0 );
      /// adapt info in header (referential, transformations)
      static Object transformHeader( const Object & header,
                                     const AffineTransformation3dBase & tr );
      static std::vector<int> getTransformedDims(
        const std::vector<int> & dims, const AffineTransformation3dBase & tr );
      static std::vector<float> getTransformedVoxelSize(
        const std::vector<float> & vs, const AffineTransformation3dBase & tr );
  };


  template <typename T>
  std::vector<int> LightResampler<T>::getTransformedDims(
    const std::vector<int> & dims, const AffineTransformation3dBase & tr )
  {
    int d, n = dims.size();
    std::vector<int> out_dims( n, 1 );
    if( n > 3 )
      n = 3;
    Point3df zero( 0, 0, 0 ), td( 0, 0, 0 );
    for( d=0; d<n; ++d )
    {
      Point3df p( 0, 0, 0 );
      p[d] = dims[d];
      td += tr.transformVector( p ) - tr.transformVector( zero );
    }
    for( d=0; d<n; ++d )
      out_dims[d] = fabs( td[d] );
    for( n=dims.size(); d<d; ++d )
      out_dims[d] = dims[d];

    return out_dims;
  }


  template <typename T>
  std::vector<float> LightResampler<T>::getTransformedVoxelSize(
    const std::vector<float> & vs, const AffineTransformation3dBase & tr )
  {
    int d, n = vs.size();
    std::vector<float> out_vs( n, 1. );
    if( n > 3 )
      n = 3;
    Point3df zero( 0, 0, 0 ), td( 0, 0, 0 );
    for( d=0; d<n; ++d )
    {
      Point3df p( 0, 0, 0 );
      p[d] = vs[d];
      td += tr.transformVector( p ) - tr.transformVector( zero );
    }
    for( d=0; d<n; ++d )
      out_vs[d] = fabs( td[d] );
    for( n=vs.size(); d<d; ++d )
      out_vs[d] = vs[d];

    return out_vs;
  }


  template <typename T>
  VolumeRef<T> LightResampler<T>::allocateResampledVolume(
    const Volume<T> & vol, const AffineTransformation3dBase & tr )
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
    const Object & header,  const AffineTransformation3dBase & tr )
  {
    Object out_hdr;
    return out_hdr;
  }


  template <typename T>
  void LightResampler<T>::resampleVolume(
    const Volume<T> & input, Volume<T> & output,
    const AffineTransformation3dBase & tr, const T & background )
  {
    std::vector<float> ovs = output.getVoxelSize();
    while( ovs.size() < 3 )
      ovs.push_back( 1. );
    std::vector<float> vs = input.getVoxelSize();
    while( vs.size() < 3 )
      vs.push_back( 1. );

    std::unique_ptr<AffineTransformation3dBase> trinv = tr.inverse();
    // embed voxel size in trinv
    AffineTransformation3dBase ivs;
    ivs.matrix()( 0, 0 ) = 1.f / vs[0];
    ivs.matrix()( 1, 1 ) = 1.f / vs[1];
    ivs.matrix()( 2, 2 ) = 1.f / vs[2];
    *trinv = ivs * *trinv;

    line_NDIterator<T> oit( &output.at( 0 ), output.getSize(),
                            output.getStrides(), true );

    Point3df ipos( 0, 0, 0 );
    ipos[oit.line_direction()] = ovs[oit.line_direction()];
    Point3df xoff = trinv->transformVector( ipos );
    std::vector<int> vipos;
    std::vector<int> idims = input.getSize();
    int dx = idims[0], dy = idims[1], dz = idims[2];

    T *p, *pp;

    for( ; !oit.ended(); ++oit )
    {
      p = &*oit;
      vipos = oit.position();
      ipos = trinv->transform( vipos[0] * ovs[0], vipos[1] * ovs[1],
                              vipos[2] * ovs[2] );
      for( pp=p + oit.line_length(); p!=pp; oit.inc_line_ptr( p ) )
      {
        vipos[0] = int(rint(ipos[0]));
        vipos[1] = int(rint(ipos[1]));
        vipos[2] = int(rint(ipos[2]));
        if( vipos[0] < 0 || vipos[1] < 0 || vipos[2] < 0
            || vipos[0] >= dx || vipos[1] >=dy || vipos[2] >= dz )
          *p = background;
        else
          *p = input.at( vipos );
        ipos += xoff;
      }
    }
  }

}


#endif

