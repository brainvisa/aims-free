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


#ifndef AIMS_MESH_MESHINTERPOLER_D_H
#define AIMS_MESH_MESHINTERPOLER_D_H

#include <aims/mesh/meshinterpoler_p.h>

namespace aims
{

  template <typename T> TimeTexture<T> *
    MeshInterpoler::resampleTexture( const TimeTexture<T> & stex ) const
  {
    TimeTexture<T> *tex = new TimeTexture<T>;
    typename TimeTexture<T>::const_iterator it, et = stex.end();
    for( it=stex.begin(); it!=et; ++it )
      resampleTexture( it->second, (*tex)[ it->first ], it->first );

    return tex;
  }


  template <typename T> Texture<T> *
    MeshInterpoler::resampleTexture( const Texture<T> & source,
                                     Texture<T> & dest, int timestep ) const
  {
    // get corresponding timestep on source time mesh
    AimsSurfaceTriangle::const_iterator its, ets = d->source->end();
    its = d->source->upper_bound( timestep );
    --its;
    int stimestep = its->first;
    const AimsSurface<3,Void> & s1 = its->second;
    const std::vector< AimsVector<uint,3> > &poly1 = s1.polygon();

    const std::vector<float>      & coord1 = d->projCoord1[stimestep].data();
    const std::vector<float>      & coord2 = d->projCoord2[stimestep].data();
    const std::vector<float>      & coord3 = d->projCoord3[stimestep].data();
    const std::vector<unsigned>   & triCorresp
      = d->projTriangles[stimestep].data();
    unsigned                      i, n = triCorresp.size();

    std::vector<T> & tx = dest.data();
    tx.reserve( n );
    tx.insert( tx.end(), n, T() );

    for( i=0; i<n; ++i )
    {
      const AimsVector<uint,3> & tri = poly1[ triCorresp[i] ];
      const T & v1 = source[ tri[0] ];
      const T & v2 = source[ tri[1] ];
      const T & v3 = source[ tri[2] ];
      float a = coord1[i], b = coord2[i], c = coord3[i];
      tx[i] = v1 * a + v2 * b + v3 * c;
    }
  }


}

#endif

