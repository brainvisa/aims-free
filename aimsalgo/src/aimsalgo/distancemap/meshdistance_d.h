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

#ifndef AIMS_DISTANCEMAP_MESHDISTANCE_D_H
#define AIMS_DISTANCEMAP_MESHDISTANCE_D_H

#include <aims/distancemap/meshdistance.h>
#include <aims/distancemap/meshvoronoi.h>
#include <map>

template<class T>
Texture<float> aims::meshdistance::MeshDistance(
  const AimsSurface<3,Void> &  mesh, const Texture<T> & inittex,
  bool allowUnreached, float max_dist )
{
  Texture<float>			tex;
  const std::vector<Point3df>		& vert = mesh.vertex();
  const std::vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();

  ASSERT( inittex.nItem() == n );
  tex.reserve( n );

  // neighbours map

  std::map<unsigned, std::set<unsigned> >	neighbours;
  unsigned			v1, v2, v3;

  for( i=0; i<poly.size(); ++i )
  {
    v1 = poly[i][0];
    v2 = poly[i][1];
    v3 = poly[i][2];
    if( inittex.item(v1)!=MESHDISTANCE_FORBIDDEN
        && inittex.item(v2)!=MESHDISTANCE_FORBIDDEN )
    {
      neighbours[v1].insert( v2 );
      neighbours[v2].insert( v1 );
    }
    if( inittex.item(v1)!=MESHDISTANCE_FORBIDDEN
        && inittex.item(v3)!=MESHDISTANCE_FORBIDDEN )
    {
      neighbours[v1].insert( v3 );
      neighbours[v3].insert( v1 );
    }
    if( inittex.item(v2)!=MESHDISTANCE_FORBIDDEN
        && inittex.item(v3)!=MESHDISTANCE_FORBIDDEN )
    {
      neighbours[v2].insert( v3 );
      neighbours[v3].insert( v2 );
    }
  }

  // init texture

  for( i=0; i<n; ++i )
  {
    if( inittex.item(i) == 0 )
      tex.push_back( FLT_MAX );
    else if( inittex.item(i) == MESHDISTANCE_FORBIDDEN )
      tex.push_back( MESHDISTANCE_FORBIDDEN );
    else
      tex.push_back( 0 );
  }

  std::multimap<float,unsigned>	front1, front2;
  std::multimap<float,unsigned>	*cfront = &front1, *nfront = &front2, *tmpf;
  std::multimap<float,unsigned>::iterator	iv, fv;
  std::set<unsigned>				neigh;
  std::set<unsigned>::iterator		in, fn;
  float					d, d2, l, nd;
  Point3df				pos;

  //	init first front

  for( i=0; i<n; ++i )
    if( tex.item(i) == 0 )
      front1.insert( std::pair<float,unsigned>( 0, i ) );

  //	loop until current front is empty

  while( cfront->size() > 0  )
  {
    nfront->clear();
    neigh.clear();

    for( iv=cfront->begin(), fv=cfront->end(); iv!=fv; ++iv )
    {
      i = (*iv).second;
      d = (*iv).first;
      for( in=neighbours[i].begin(), fn=neighbours[i].end(); in!=fn; ++in )
      {
        d2 = tex.item( *in );
        pos = vert[i] - vert[*in];
        l = sqrt( pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2] );
        nd = d + l;
        if( d2 > nd && nd < max_dist )
        {
          tex.item( *in ) = nd;
          neigh.insert( *in );
        }
      }
    }

    for( in=neigh.begin(), fn=neigh.end(); in!=fn; ++in )
      nfront->insert( std::pair<float,unsigned>( tex.item( *in ), *in ) );

    tmpf = cfront;
    cfront = nfront;
    nfront = tmpf;
  }


  if( !allowUnreached )
    for( i=0; i<n; ++i )
      if( tex.item(i) == FLT_MAX )
        tex.item(i) = MESHDISTANCE_UNREACHED;

  return( tex );
}


namespace aims
{
  namespace meshdistance
  {

    template <typename T>
    class DistMapMatrixTraits
    {
    public:
      static void resize( T & mat, size_t size1, size_t size2 = 0 )
      {
      }
    };

    template <typename T>
    class DistMapMatrixTraits<std::vector<T> >
    {
    public:
      typedef T LineType;

      static void resize( std::vector<T> & mat, size_t size1,
                          size_t size2 = 0 )
      {
        mat.resize( size1 );
      }

      static void storeLine( std::vector<T> & mat, size_t line,
                             const std::vector<float> & tex )
      {
        LineType & map = mat[line];
        size_t i, n = tex.size();

        for( i=0; i<n; ++i )
          DistMapMatrixTraits<LineType>::storeElement( map, i, tex[i] );
      }

    };

    template <typename T, typename U>
    class DistMapMatrixTraits<std::map<T, U> >
    {
    public:
      static void resize( T & mat, size_t size1, size_t size2 = 0 )
      {
      }

      static void storeElement( std::map<T, U> & map, T i, float value )
      {
        if( value != FLT_MAX )
          map[i] = U( value );
      }
    };

  }
}


template <typename T, typename MapType=std::vector< std::map<size_t, float> > >
void
aims::meshdistance::pairwiseDistanceMaps( const AimsSurface<3,Void> & mesh,
                                          MapType & distmaps,
                                          const Texture<T> & inittex,
                                          float max_dist )
{
  const std::vector<Point3df>		& vert = mesh.vertex();
  const std::vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();
  std::vector<float>	tex, ctex( n );

  bool has_init = ( inittex.size() != 0 );
  ASSERT( !has_init || inittex.nItem() == n );
  DistMapMatrixTraits<MapType>::resize( distmaps, n );

  // neighbours map

  std::map<unsigned, std::set<unsigned> >	neighbours;
  unsigned			v1, v2, v3;

  for( i=0; i<poly.size(); ++i )
  {
    v1 = poly[i][0];
    v2 = poly[i][1];
    v3 = poly[i][2];
    if( !has_init || ( inittex[v1] != MESHDISTANCE_FORBIDDEN
                       && inittex[v2] != MESHDISTANCE_FORBIDDEN ) )
    {
      neighbours[v1].insert( v2 );
      neighbours[v2].insert( v1 );
    }
    if( !has_init || ( inittex.item(v1) != MESHDISTANCE_FORBIDDEN
                       && inittex.item(v3) != MESHDISTANCE_FORBIDDEN ) )
    {
      neighbours[v1].insert( v3 );
      neighbours[v3].insert( v1 );
    }
    if( !has_init || ( inittex.item(v2) != MESHDISTANCE_FORBIDDEN
                       && inittex.item(v3)!=MESHDISTANCE_FORBIDDEN ) )
    {
      neighbours[v2].insert( v3 );
      neighbours[v3].insert( v2 );
    }
  }

  // init texture

  for( i=0; i<n; ++i )
  {
    if( !has_init || inittex.item(i) != MESHDISTANCE_FORBIDDEN )
      ctex[i] = FLT_MAX;
    else
      ctex[i] = MESHDISTANCE_FORBIDDEN;
  }

  size_t start;  // start point for each map
  for( start=0; start<n; ++start )
  {
    if( has_init && inittex[start] == MESHDISTANCE_FORBIDDEN )
      continue;  // skip forbidden points

    // re-init new map
    tex = ctex;
    tex[start] = 0; // with one starting point

    std::multimap<float,unsigned>	front1, front2;
    std::multimap<float,unsigned>	*cfront = &front1, *nfront = &front2, *tmpf;
    std::multimap<float,unsigned>::iterator	iv, fv;
    std::set<unsigned>				neigh;
    std::set<unsigned>::iterator		in, fn;
    float					d, d2, l, nd;
    Point3df				pos;

    //	init first front

    front1.insert( std::pair<float,unsigned>( 0., start ) );

    //	loop until current front is empty

    while( cfront->size() > 0  )
    {
      nfront->clear();
      neigh.clear();

      for( iv=cfront->begin(), fv=cfront->end(); iv!=fv; ++iv )
      {
        i = (*iv).second;
        d = (*iv).first;
        for( in=neighbours[i].begin(), fn=neighbours[i].end(); in!=fn; ++in )
        {
          d2 = tex[ *in ];
          pos = vert[i] - vert[*in];
          l = sqrt( pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2] );
          nd = d + l;
          if( d2 > nd && nd < max_dist )
          {
            tex[ *in ] = nd;
            neigh.insert( *in );
          }
        }
      }

      for( in=neigh.begin(), fn=neigh.end(); in!=fn; ++in )
        nfront->insert( std::pair<float,unsigned>( tex[ *in ], *in ) );

      tmpf = cfront;
      cfront = nfront;
      nfront = tmpf;
    }

    // fill in the output sparse map
    DistMapMatrixTraits<MapType>::storeLine( distmaps, start, tex );

  }
}


#endif

