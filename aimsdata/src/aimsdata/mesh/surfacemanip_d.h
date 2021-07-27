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

#ifndef AIMS_SURFACE_SURFACEOPERATION_D_H
#define AIMS_SURFACE_SURFACEOPERATION_D_H

#include <aims/mesh/surfacemanip.h>
#include <aims/mesh/texture.h>
#include <aims/resampling/motion.h>

namespace aims
{

  template<int D, class T> std::vector<std::set<uint> >
  SurfaceManip::surfaceNeighbours( const AimsSurface<D,T> & surf )
  {
    const std::vector< AimsVector<uint,D> >	& poly = surf.polygon();
    uint					n = poly.size();
    std::vector<std::set<uint> >		neigh( surf.vertex().size() );

    for ( uint i=0; i<n; ++i )
      for ( uint j=0; j<D; ++j )
        for ( uint k=0; k<D; ++k )
          if ( j != k )
            neigh[poly[i][j]].insert( poly[i][k] );

    return neigh;
  }

  template<int D, class T> std::vector<std::set<uint> >
  SurfaceManip::surfaceNeighbours( const AimsTimeSurface<D,T> & surf )
  {
    const std::vector< AimsVector<uint,D> >	& poly = surf.polygon();
    uint					n = poly.size();
    std::vector<std::set<uint> >		neigh( surf.vertex().size() );

    for ( uint i=0; i<n; ++i )
      for ( uint j=0; j<D; ++j )
        for ( uint k=0; k<D; ++k )
          if ( j != k )
            neigh[poly[i][j]].insert( poly[i][k] );

    return neigh;
  }

  template<int D, class T> std::vector<std::set<uint> > 
  SurfaceManip::surfaceNeighbours2ndOrder( const AimsTimeSurface<D,T> & surf )
  {
    const std::vector< AimsVector<uint,D> >	& poly = surf.polygon();
    uint					n = poly.size();
    std::vector<std::set<uint> >		neigh( surf.vertex().size() );
    std::vector<std::set<uint> >			neigh_final( surf.vertex().size() );

    for ( uint i=0; i<n; ++i )
      for ( uint j=0; j<D; ++j )
        for ( uint k=0; k<D; ++k )
          if ( j != k )
            neigh[poly[i][j]].insert( poly[i][k] );

    for ( uint i=0; i<neigh.size() ; ++i)
    {
        std::set<uint> voisin=neigh[i];
        std::set<uint> voisin_final=neigh[i];
        std::set<uint>::iterator voisin_pt=voisin.begin();
        for ( ; voisin_pt!=voisin.end(); ++voisin_pt)
        {
            uint j=*voisin_pt;
            std::set<uint> vois2=neigh[j];
            std::set<uint>::iterator v_pt=vois2.begin();
            for ( ; v_pt!=vois2.end(); ++v_pt)
                voisin_final.insert(*v_pt);
        }
        neigh_final[i]=voisin_final;
    }	

    return neigh_final;
  }


  template<int D, class T> std::vector<std::map<uint, float> >
  SurfaceManip::surfaceNeighbourDistance( const AimsTimeSurface<D,T> & surf )
  {
    const std::vector< AimsVector<uint,D> >	& poly = surf.polygon();
    uint					n = poly.size();
    const std::vector<Point3df>			& vert  = surf.vertex();
    std::vector<std::map<uint, float> > neighDist(surf.vertex().size());
    Point3df					diff;
    float					dist;

    for ( uint i=0; i<n; ++i )
      for ( uint j=0; j<D; ++j )
	for ( uint k=0; k<D; ++k )
	  if ( j != k )
	    {
	      diff=vert[poly[i][j]] - vert[poly[i][k]]; // COURANT - VOISIN
	      dist = diff.norm();	//DISTANCE ENTRE LES DEUX;
	      neighDist[poly[i][j]][poly[i][k]] = dist;
	    }

    return neighDist;
  }

  template<int D, class T> 
  void SurfaceManip::invertSurfacePolygons( AimsTimeSurface<D,T> & surface )
  {
    typename AimsTimeSurface<D,T>::iterator		is, fs=surface.end();
    typename std::vector< AimsVector<uint,D> >::iterator	ip, fp;
    uint					n = D/2;
    for ( is=surface.begin(); is!=fs; ++is )
    {
      AimsSurface<D, T>		& surf = is->second;
      std::vector< AimsVector<uint,D> >	& poly = surf.polygon();

      for ( ip=poly.begin(), fp=poly.end(); ip!=fp; ++ip )
      {
        AimsVector<uint,D>	& p = *ip;
        for ( uint i=0; i<n; ++i )
        {
          uint vert = p[i];
          p[i] = p[D-i-1];
          p[D-i-1] = vert;
        }
      }
    }
  }


  template<int D, class T>
  void SurfaceManip::meshMerge( AimsTimeSurface<D,T> & dst,
                                const AimsTimeSurface<D,T> & add )
  {
    typename AimsTimeSurface<D, T>::const_iterator is, es = add.end();
    for( is=add.begin(); is!=es; ++is )
    {
      int t = is->first;
      const AimsSurface<D, T> & imesh = is->second;
      AimsSurface<D, T> & omesh = dst[t];
      std::vector<Point3df> & vert = omesh.vertex();
      std::vector<Point3df> & norm = omesh.normal();
      std::vector<AimsVector<uint,D> > & poly = omesh.polygon();
      const std::vector<Point3df>  & vert2 = imesh.vertex();
      const std::vector<Point3df>  & norm2 = imesh.normal();
      const std::vector<AimsVector<uint,D> >  & poly2 = imesh.polygon();
      unsigned i, j, n = vert.size(), m = vert2.size(), p = poly2.size();

      // copy vertices & normals
      for ( i=0; i<m; ++i )
        vert.push_back( vert2[i] );
      for ( i=0, m=norm2.size(); i<m; ++i )
        norm.push_back( norm2[i] );

      // translate & copy polygons
      for ( i=0; i<p; ++i )
      {
        AimsVector<uint,D>  pol;
        for ( j=0; j<D; ++j )
          pol[j] = poly2[i][j] + n;
        poly.push_back( pol );
      }
      if( t == 0 )
        dst.header().setProperty( "vertex_number", int( n + m ) );
    }
  }

  template<int D, class T>
  void SurfaceManip::meshMerge( AimsTimeSurface<D,T> & dst, 
				const std::list<AimsTimeSurface<D,T> > & src )
  {
    dst.erase();
    typename std::list<AimsTimeSurface<D,T> >::const_iterator
      im, em = src.end();
    for ( im=src.begin(); im!=em; ++im )
      meshMerge( dst, *im );
  }


  template<int D, class T>
  void SurfaceManip::meshTransform( AimsTimeSurface<D,T> & mesh, 
                                    const Motion & motion )
  {
    typename AimsTimeSurface<D,T>::iterator	itime, tend = mesh.end();
    float					norm;

    for( itime=mesh.begin(); itime!=tend; ++itime )
      {
        typedef std::vector< Point3df > Points;
        Points& vertex = itime->second.vertex();
        Points& normal = itime->second.normal();
        unsigned nnorm = normal.size();
        for( Points::size_type k = 0; k < vertex.size(); ++k )
          {
            vertex[ k ] = motion.transform( vertex[ k ] );
            /* Denis: applying this transformation to normals is WRONG if 
               there is any scale factor ! */
            if( k < nnorm )
            {
              normal[ k ] = motion.transform( normal[ k ] ) 
                - motion.translation();
              // normalize normals
              norm = normal[k][0] * normal[k][0] + normal[k][1] * normal[k][1] 
                + normal[k][2] * normal[k][2];
              norm = 1. / sqrt( norm );
              normal[k][0] *= norm;
              normal[k][1] *= norm;
              normal[k][2] *= norm;
            }
          }
      }
    if( !motion.isDirect() )
      invertSurfacePolygons( mesh );
  }


  template <int D, typename T>
  AimsTimeSurface<D,T> *
      SurfaceManip::meshExtract( const AimsTimeSurface<D,T> & mesh,
                                 const TimeTexture<int16_t> & tex,
                                 int16_t value,
                                 std::vector<size_t> ** overtIndex )
  {
    AimsTimeSurface<D,T>  *omesh = new AimsTimeSurface<D,T>;
    typename AimsTimeSurface<D,T>::const_iterator  imt, jmt, emt = mesh.end();
    typename TimeTexture<int16_t>::const_iterator  itt, jtt, ett = tex.end();
    if( overtIndex )
      * overtIndex = new std::vector<size_t>;
    jmt = mesh.begin();
    imt = jmt;
    jtt = tex.begin();
    itt = jtt;
    int ti = 0;
    while( jmt != emt || jtt != ett )
    {
      if( jmt != emt )
        imt = jmt;
      if( jtt != ett )
        itt = jtt;
      AimsSurface<D,T>  & mh = (*omesh)[ ti ];
      std::vector<Point3df>  & overt = mh.vertex();
      std::vector<Point3df>  & onorm = mh.normal();
      std::vector<AimsVector<uint, D> > & opoly = mh.polygon();
      const std::vector<Point3df>  & vert = imt->second.vertex();
      const std::vector<Point3df>  & norm = imt->second.normal();
      const std::vector<AimsVector<uint, D> > & poly = imt->second.polygon();
      typename std::vector<AimsVector<uint, D> >::const_iterator
          im, em = poly.end();
      int i = 0, k;
      const Texture<int16_t>  & tx = itt->second;
      std::map<uint, uint> vmap;
      typename std::map<uint, uint>::iterator iv, ev = vmap.end();
      unsigned nt;
      bool  absmaj, hasdist;
      int16_t y, mval = 0;
      Point3df  g;
      float dist, dist2;
      for( im=poly.begin(); im != em; ++im )
      {
        std::map<int16_t, unsigned> vals;
        nt = 0;
        absmaj = false;
        g = Point3df( 0, 0, 0 );
        // let's have a vote
        for( k=0; k<D; ++k )
        {
          g += vert[ (*im)[k] ];
          y = tx[ (*im)[k] ];
          unsigned  & x = vals[y];
          ++x;
          if( x > nt )
          {
            nt = x;
            mval = y;
            absmaj = true;
          }
          else if( x == nt )
            absmaj = false;
        }
        if( vals[ value ] == nt )
        {
          // 'value' is amongst the most taken value
          if( !absmaj )
          {
            // take nearest vertex from barycenter with value in majority
            g /= D;
            dist = 0;
            hasdist = false;
            for( k=0; k<D; ++k )
            {
              y = tx[ (*im)[k] ];
              if( vals[ y ] == nt )
              {
                dist2 = ( vert[ (*im)[k] ] - g ).norm2();
                if( !hasdist || dist2 < dist )
                {
                  dist = dist2;
                  mval = y;
                }
              }
            }
            if( mval == value )
              absmaj = true;
          }
          // else absmaj == true: value has absolute majority
        }
        else
          // value is in minority
          absmaj = false;
        if( absmaj )
        { // copy polygon
          AimsVector<uint, D> p;
          for( k=0; k<D; ++k )
          {
            iv = vmap.find( (*im)[k] );
            if( iv == ev )
            {
              vmap[ (*im)[k] ] = i;
              p[k] = i;
              overt.push_back( vert[ (*im)[k] ] );
              onorm.push_back( norm[ (*im)[k] ] );
              if( overtIndex )
                (** overtIndex).push_back((*im)[k]);
              ++i;
            }
            else
              p[k] = iv->second;
          }
          opoly.push_back( p );
        }
      }
      ++ti;
      if( jmt != emt )
        ++jmt;
      if( jtt != ett )
        ++jtt;
    }
    return omesh;
  }


  template <int D, typename T>
  TimeTexture<float>*
    SurfaceManip::meshDensity( const AimsTimeSurface<D,T> & mesh,
                               bool asDistance )
  {
    TimeTexture<float> *tex = new TimeTexture<float>;
    typename AimsTimeSurface<D,T>::const_iterator is, es = mesh.end();
    uint i;

    for( is=mesh.begin(); is!=es; ++is )
    {
      const AimsSurface<D,T>                 & surf = is->second;
      const std::vector<Point3df>            & vert = surf.vertex();
      const std::vector<AimsVector<uint,D> > & poly = surf.polygon();

      // create corresponding timestep texture
      std::vector<float> & tx = (*tex)[ is->first ].data();
      std::vector<unsigned> counts;
      tx.reserve( vert.size() );
      tx.insert( tx.end(), vert.size(), 0. );
      counts.reserve( vert.size() );
      counts.insert( counts.end(), vert.size(), 0 );

      // record edges sizes on all polygons
      typename std::vector<AimsVector<uint,D> >::const_iterator
        ip, ep = poly.end(), jp;
      float n;

      for( ip=poly.begin(); ip!=ep; ++ip )
      {
        for( i=0; i<D; ++i )
        {
          // edge distance, accounted on both vertices of the edge
          uint vi = (*ip)[i];
          uint vj = (*ip)[ (i+1) % D ];
          n = ( vert[ vi ] - vert[ vj ] ).norm();
          tx[ vi ] += n;
          tx[ vj ] += n;
          // count
          ++counts[ vi ];
          ++counts[ vj ];
        }
      }

      // now average distances, and invert them to get a density
      std::vector<float>::iterator          it, et = tx.end();
      std::vector<unsigned>::const_iterator itc;
      if( asDistance )
        for( it=tx.begin(), itc=counts.begin(); it!=et; ++it, ++itc )
          if( *itc != 0. )
            *it /= float( *itc );
          else
            *it = 0.; // to avoid NaN
      else
        for( it=tx.begin(), itc=counts.begin(); it!=et; ++it, ++itc )
          *it = float( *itc ) / *it;
    }

    return tex;
  }


  template <int D, typename T>
  TimeTexture<float>*
    SurfaceManip::meshEdgeLengthRatioTexture(
        const AimsTimeSurface<D,T> & nummesh,
        const AimsTimeSurface<D,T> & denommesh )
  {
    TimeTexture<float> *tex = new TimeTexture<float>;
    typename AimsTimeSurface<D,T>::const_iterator is, es = nummesh.end(),
      ids, eds = denommesh.end();
    uint i;

    for( is=nummesh.begin(), ids=denommesh.begin(); is!=es && ids!=eds;
        ++is, ++ids )
    {
      const AimsSurface<D,T>                 & surf = is->second;
      const std::vector<Point3df>            & vert = surf.vertex();
      const std::vector<AimsVector<uint,D> > & poly = surf.polygon();
      const std::vector<Point3df>            & dvert = ids->second.vertex();

      // create corresponding timestep texture
      std::vector<float> & tx = (*tex)[ is->first ].data();
      std::vector<unsigned> counts;
      tx.reserve( vert.size() );
      tx.insert( tx.end(), vert.size(), 0. );
      counts.reserve( vert.size() );
      counts.insert( counts.end(), vert.size(), 0 );

      // record edges sizes on all polygons
      typename std::vector<AimsVector<uint,D> >::const_iterator
        ip, ep = poly.end(), jp;
      float n, dn;

      for( ip=poly.begin(); ip!=ep; ++ip )
      {
        for( i=0; i<D; ++i )
        {
          // edge distance, accounted on both vertices of the edge
          uint vi = (*ip)[i];
          uint vj = (*ip)[ (i+1) % D ];
          n = ( vert[ vi ] - vert[ vj ] ).norm();
          dn = ( dvert[ vi ] - dvert[ vj ] ).norm();
          if( dn == 0. )
            n = 1.; // what to do else ? (leave inf ?)
          else
            n /= dn;
          tx[ vi ] = std::max( tx[ vi ], n );
          tx[ vj ] = std::max( tx[ vj ], n );
          // count
          ++counts[ vi ];
          ++counts[ vj ];
        }
      }

      /*
      // now average distances
      std::vector<float>::iterator          it, et = tx.end();
      std::vector<unsigned>::const_iterator itc;
      for( it=tx.begin(), itc=counts.begin(); it!=et; ++it, ++itc )
        if( *itc != 0. )
          *it /= float( *itc );
        else
          *it = 0.; // to avoid NaN
          */
    }

    return tex;
  }


  namespace internal
  {
    inline uint take_mid_point_and_insert(
      std::map<std::pair<uint,uint>, uint> & segments,
      uint i, uint j,
      const std::vector<Point3df> & vert,
      std::vector<Point3df> & overt )
    {
      std::pair<uint,uint> seg = std::make_pair( i, j );
      std::map<std::pair<uint,uint>, uint>::const_iterator
        iseg = segments.find( seg );
      uint iout;
      if( iseg == segments.end() )
      {
        Point3df p = ( vert[i] + vert[j] ) / 2;
        iout = overt.size();
        overt.push_back( p );
        segments[ seg ] = iout;
      }
      else
        iout = iseg->second;
      return iout;
    }

  }


  template <typename T>
  AimsSurface<2,Void> *
  SurfaceManip::meshTextureBoundary( const AimsSurface<3,Void> & mesh,
    const Texture<T> & tex, T region )
  {
    const std::vector<Point3df> & vert = mesh.vertex();
    const std::vector<AimsVector<uint,3> > & poly = mesh.polygon();
    AimsSurface<2,Void> *outmesh = new AimsSurface<2,Void>;
    std::map<std::pair<uint,uint>, uint> segments;
    std::vector<AimsVector<uint,3> >::const_iterator ip, ep = poly.end();
    uint i, j, k;
    bool ii, jj, kk;
    std::vector<Point3df> & overt = outmesh->vertex();
    std::vector<AimsVector<uint,2> > & opoly = outmesh->polygon();

    for( ip=poly.begin(); ip!=ep; ++ip )
    {
      bool done = false;
      i = (*ip)[0];
      j = (*ip)[1];
      k = (*ip)[2];
      // order indices
      if( i > j )
      {
        i = j;
        j = (*ip)[0];
      }
      if( i > k )
      {
        k = i;
        i = (*ip)[2];
      }
      if( j > k )
      {
        uint w = k;
        k = j;
        j = w;
      }
      // find boundaries in triangle
      if( region >= 0 )
      {
        if( tex[i] == region )
          ii = true;
        else
          ii = false;
        if( tex[j] == region )
          jj = true;
        else
          jj = false;
        if( tex[k] == region )
          kk = true;
        else
          kk = false;
      }
      else
      {
        if( tex[i] != tex[j] && tex[i] != tex[k] && tex[j] != tex[k] )
        {
          // all different
          Point3df center = ( vert[i] + vert[j] + vert[k] ) / 3.;
          uint iop = overt.size();
          overt.push_back( center );
          uint iout = internal::take_mid_point_and_insert( segments, i, j, vert,
                                                           overt );
          opoly.push_back( AimsVector<uint,2>( iout, iop ) );
          iout = internal::take_mid_point_and_insert( segments, i, k, vert,
                                                           overt );
          opoly.push_back( AimsVector<uint,2>( iout, iop ) );
          iout = internal::take_mid_point_and_insert( segments, j, k, vert,
                                                           overt );
          opoly.push_back( AimsVector<uint,2>( iout, iop ) );
          done = true;
        }
        else
        {
          ii = false;
          if( tex[i] == tex[j] )
            jj = false;
          else
            jj = true;
          if( tex[i] == tex[k] )
            kk = false;
          else
            kk = true;
        }
      }

      if( !done && ( ii != jj || ii != kk ) )
      {
        std::vector<uint> ipts;
        if( ii != jj )
          ipts.push_back( internal::take_mid_point_and_insert( segments, i, j,
                                                               vert, overt ) );
        if( ii != kk )
          ipts.push_back( internal::take_mid_point_and_insert( segments, i, k,
                                                               vert, overt ) );
        if( jj != kk )
          ipts.push_back( internal::take_mid_point_and_insert( segments, j, k,
                                                               vert, overt ) );
        opoly.push_back( AimsVector<uint,2>( ipts[0], ipts[1] ) );
      }
    }
    return outmesh;
  }


  template <typename T>
  AimsTimeSurface<2,Void> *
  SurfaceManip::meshTextureBoundary( const AimsSurfaceTriangle & mesh,
    const TimeTexture<T> & tex, T region )
  {
    AimsSurfaceTriangle::const_iterator imesh, jmesh, emesh = mesh.end();
    imesh = mesh.begin();
    jmesh = imesh;
    if( jmesh != emesh )
      ++jmesh;
    typename TimeTexture<T>::const_iterator
      itex = tex.begin(), jtex=itex, etex = tex.end();
    if( jtex != etex )
      ++jtex;
    AimsTimeSurface<2,Void> *outmesh = new AimsTimeSurface<2,Void>;
    while( imesh != emesh && itex != etex )
    {
      unsigned mtimestep = imesh->first;
      unsigned ttimestep = itex->first;
      AimsSurface<2, Void> *tmesh = meshTextureBoundary( imesh->second,
                                                         itex->second,
                                                         region );
      (*outmesh)[ std::max( mtimestep, ttimestep ) ] = *tmesh;
      delete tmesh;
      if( jmesh == emesh && jtex == etex )
        break;
      if( jmesh != emesh && ( jtex == etex || jmesh->first < jtex->first ) )
      {
        imesh = jmesh;
        ++jmesh;
      }
      else if( jtex != etex
               && ( jmesh == emesh || jmesh->first > jtex->first ) )
      {
        itex = jtex;
        ++jtex;
      }
      else
      {
        if( jmesh != emesh )
        {
          imesh = jmesh;
          ++jmesh;
        }
        if( jtex != etex )
        {
          itex = jtex;
          ++jtex;
        }
      }
    }
    return outmesh;
  }


  template <int D, typename T>
  void SurfaceManip::sortPolygonsAlongDirection( AimsTimeSurface<D, T> & mesh,
                                                 int timestep,
                                                 const Point3df & direction )
  {
    const std::vector<Point3df> & vert = mesh[timestep].vertex();
    std::vector<AimsVector<uint, D> > & poly = mesh[timestep].polygon();
    std::multimap<float, AimsVector<uint, D> > sorted;
    typename std::vector<AimsVector<uint, D> >::iterator
      ip, ep = poly.end();
    typename std::multimap<float, AimsVector<uint, D> >::iterator
      im, em = sorted.end();
    int d;
    Point3df center;

    // sort using a map
    for( ip=poly.begin(); ip!=ep; ++ip )
    {
      center = vert[(*ip)[0]];
      for( d=1; d<D; ++d )
        center += vert[(*ip)[d]];
      sorted.insert( std::make_pair( center.dot( direction ), *ip ) );
    }

    // rewrite polygons
    for( im=sorted.begin(), ip=poly.begin(); im!=em; ++im, ++ip )
      *ip = im->second;
  }


}

#endif
