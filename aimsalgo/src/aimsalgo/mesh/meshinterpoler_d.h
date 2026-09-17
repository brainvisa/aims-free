
#ifndef AIMS_MESH_MESHINTERPOLER_D_H
#define AIMS_MESH_MESHINTERPOLER_D_H

#include <aims/mesh/meshinterpoler_p.h>

namespace aims
{

  template <typename T> TimeTexture<T> *
    MeshInterpoler::resampleTexture( const TimeTexture<T> & stex,
                                     InterpolationType inttype ) const
  {
    TimeTexture<T> *tex = new TimeTexture<T>;
    typename TimeTexture<T>::const_iterator it, et = stex.end();
    for( it=stex.begin(); it!=et; ++it )
      resampleTexture( it->second, (*tex)[ it->first ], it->first, inttype );

    return tex;
  }


  template <typename T> void
    MeshInterpoler::resampleTexture( const Texture<T> & source,
                                     Texture<T> & dest, int timestep,
                                     InterpolationType inttype ) const
  {
    int stimestep;

    if( d->sourceVert )
    {
      stimestep = 0;
    }
    else
    {
      // get corresponding timestep on source time mesh
      AimsSurfaceTriangle::const_iterator its;
      its = d->source->upper_bound( timestep );
      --its;
      stimestep = its->first;
    }
    size_t n = d->projTriangles[stimestep].data().size();
    std::vector<T> & tx = dest.data();
    tx.clear();
    tx.reserve( n );
    tx.insert( tx.end(), n, T() );
    resampleTexture( &source.data()[0], &tx[0], timestep, inttype );
  }


  template <typename T> void
    MeshInterpoler::resampleTexture( const T *source, T *dest,
                                     int timestep,
                                     InterpolationType inttype ) const
  {
    int stimestep;
    const AimsVector<uint,3> *poly1;

    if( d->sourceVert )
    {
      stimestep = 0;
      poly1 = d->sourcePoly;
    }
    else
    {
      // get corresponding timestep on source time mesh
      AimsSurfaceTriangle::const_iterator its;
      its = d->source->upper_bound( timestep );
      --its;
      stimestep = its->first;
      const AimsSurface<3,Void> & s1 = its->second;
      poly1 = &s1.polygon()[0];
    }

    const std::vector<float>      & coord1 = d->projCoord1[stimestep].data();
    const std::vector<float>      & coord2 = d->projCoord2[stimestep].data();
    const std::vector<float>      & coord3 = d->projCoord3[stimestep].data();
    const std::vector<unsigned>   & triCorresp
      = d->projTriangles[stimestep].data();
    size_t                        i, n = triCorresp.size();

    switch( inttype )
    {
    case Linear:
      for( i=0; i<n; ++i )
      {
        const AimsVector<uint,3> & tri = poly1[ triCorresp[i] ];
        const T & v1 = source[ tri[0] ];
        const T & v2 = source[ tri[1] ];
        const T & v3 = source[ tri[2] ];
        float a = coord1[i], b = coord2[i], c = coord3[i];
        dest[i] = v1 * a + v2 * b + v3 * c;
      }
      break;
    case NearestNeighbour:
      for( i=0; i<n; ++i )
      {
        const AimsVector<uint,3> & tri = poly1[ triCorresp[i] ];
        const T & v1 = source[ tri[0] ];
        const T & v2 = source[ tri[1] ];
        const T & v3 = source[ tri[2] ];
        float a = fabs( coord1[i] ), b = fabs( coord2[i] ),
          c = fabs( coord3[i] );
        if( a >= b && a >= c )
          dest[i] = v1;
        else if( b >= a && b >= c )
          dest[i] = v2;
        else
          dest[i] = v3;
      }
      break;
    default:
      break;
    }
  }


}

#endif

