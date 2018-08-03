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

#include <aims/mesh/cutmesh_d.h>

using namespace aims;
using namespace carto;
using namespace std;


CutMesh::CutMesh( const AimsSurfaceTriangle & insurf, const Point4df & plane )
  : _plane( plane )
{
  _insurf.push_back( &insurf );
}


CutMesh::CutMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
                  const Point4df & plane )
  : _insurf( insurf ), _plane( plane )
{
}


CutMesh::~CutMesh()
{
}


void CutMesh::cut( bool buildborderline, bool meshplane, bool /* checkplane */ )
{
  _cut.clear();
  _borderline.reset( new AimsTimeSurface<2, Void> );
  _planemesh.reset( new AimsSurfaceTriangle );
  initializeOutputTextures();


  // ------------

  float p0, p1, p2, wt[3];

  vector<const AimsSurfaceTriangle *>::const_iterator
    iisv, eisv = _insurf.end();
  unsigned  count = 0;
  bool  first = true;

  for( iisv=_insurf.begin(); iisv!=eisv; ++iisv, ++count )
  {
    rc_ptr<AimsTimeSurface<2, Void> >  bline;
    AimsTimeSurface<2, Void>           *pbline;
    AimsSurfaceTriangle                *pcmesh;
    if( _cut.size() <= count )
      _cut.push_back( rc_ptr<AimsSurfaceTriangle>( new AimsSurfaceTriangle ) );
    else if( !_cut[count] )
      _cut[count].reset( new AimsSurfaceTriangle );
    AimsSurfaceTriangle & cut = *_cut[count];
    if( first )
    {
      pbline = _borderline.get();
      pcmesh = &cut;
    }
    else
    {
      bline.reset( new AimsTimeSurface<2, Void> );
      //cmesh.reset( new AimsSurfaceTriangle );
      pbline = bline.get();
      //pcmesh = cmesh.get();
      pcmesh = &cut;
    }
    vector<Point3df>                   & bvert = pbline->vertex();
    vector<AimsVector<uint, 2> >       & bpoly = pbline->polygon();
    vector<Point3df>                   & overt = pcmesh->vertex();
    vector<Point3df>                   & onorm = pcmesh->normal();
    vector<AimsVector<uint, 3> >       & opoly = pcmesh->polygon();
    const vector<Point3df>             & ivert = (*iisv)->vertex();
    const vector<Point3df>             & inorm = (*iisv)->normal();
    const vector<AimsVector<uint, 3> > & ipoly = (*iisv)->polygon();
    vector<int>                        itoo( ivert.size() );
    uint                               i, n = ipoly.size(), nv = ivert.size();
    uint                               nvo = 0, thrownpol = 0, cutpol = 0;
    uint                               pos[3] = { 0, 0, 0 }, npos;
    set<pair<uint, uint> >             polyline;
    map<pair<uint, uint>, uint >       cutseg;
    map<uint, uint>                    cutvert;
    pair<uint, uint>                   seg;
    map<pair<uint, uint>, uint >::iterator iseg, eseg = cutseg.end();

    fill_n( itoo.begin(), nv, -1 );

    for( i=0; i<n; ++i )
    {
      const AimsVector<uint, 3>	& pol = ipoly[i];
      const Point3df			& pv0 = ivert[ pol[0] ];
      const Point3df			& pv1 = ivert[ pol[1] ];
      const Point3df			& pv2 = ivert[ pol[2] ];

      /* compare to plane (could be an external function to allow several
        planes/intersections) */
      p0 = _plane[0] * pv0[0] + _plane[1] * pv0[1] + _plane[2] * pv0[2]
        + _plane[3];
      p1 = _plane[0] * pv1[0] + _plane[1] * pv1[1] + _plane[2] * pv1[2]
        + _plane[3];
      p2 = _plane[0] * pv2[0] + _plane[1] * pv2[1] + _plane[2] * pv2[2]
        + _plane[3];
      if( p0 >= 0 && p1 >= 0 && p2 >= 0 )	// good side: keep
      {
        int	& uv0 = itoo[ pol[0] ];
        if( uv0 < 0 )
        {
          overt.push_back( pv0 );
          onorm.push_back( inorm[ pol[0] ] );
          addTexturePoint( count, pol[0] );
          uv0 = nvo;
          ++nvo;
        }
        int	& uv1 = itoo[ pol[1] ];
        if( uv1 < 0 )
        {
          overt.push_back( pv1 );
          onorm.push_back( inorm[ pol[1] ] );
          addTexturePoint( count, pol[1] );
          uv1 = nvo;
          ++nvo;
        }
        int	& uv2 = itoo[ pol[2] ];
        if( uv2 < 0 )
        {
          overt.push_back( pv2 );
          onorm.push_back( inorm[ pol[2] ] );
          addTexturePoint( count, pol[2] );
          uv2 = nvo;
          ++nvo;
        }
        opoly.push_back( AimsVector<uint, 3>( (uint) uv0, (uint) uv1,
                                              (uint) uv2 ) );
      }
      else if( p0 <= 0 && p1 <= 0 && p2 <= 0 )	// bad side: throw away
      {
        // nothing to do
        ++thrownpol;
      }
      else	// cut polygon
      {
        ++cutpol;
        // sort points by side
        npos = 0;
        wt[0] = p0;
        wt[1] = p1;
        wt[2] = p2;
        if( p0 >= 0 )
        {
          ++npos;
          pos[0] = 0;
        }
        else
          pos[2] = 0;
        if( p1 >= 0 )
        {
          if( npos == 0 )
            pos[0] = 1;
          else
            pos[1] = 1;
          ++npos;
        }
        else if( npos == 0 )
          pos[1] = 1;
        else
          pos[2] = 1;
        if( npos == 0 )
        {
          pos[0] = 2;
          ++npos;
        }
        else if( npos == 2 )
          pos[2] = 2;
        else if( p2 >= 0 )
        {
          ++npos;
          pos[1] = 2;
        }
        else
          pos[1] = 2;
        // cut poly
        if( npos == 1 )	// 1 poly
        {
          uint pol0 = pol[ pos[0] ];
          int	& uv0 = itoo[ pol0 ];
          if( uv0 < 0 )
          {
            uv0 = (int) nvo;
            overt.push_back( ivert[ pol0 ] );
            onorm.push_back( inorm[ pol0 ] );
            addTexturePoint( count, pol0 );
            ++nvo;
          }

          p0 = wt[pos[0]];
          p1 = -wt[pos[2]];
          p2 = -wt[pos[1]];

          uint	uv1, uv2;
          if( pol[pos[0]] < pol[pos[2]] )
          {
            seg.first = pol[pos[0]];
            seg.second = pol[pos[2]];
          }
          else
          {
            seg.first = pol[pos[2]];
            seg.second = pol[pos[0]];
          }
          iseg = cutseg.find( seg );
          if( iseg == eseg )
          {
            uv1 = cutseg[ seg ] = nvo;
            overt.push_back( ( ivert[ pol[pos[0]] ] * p1
                              + ivert[ pol[pos[2]] ] * p0 )
                            / (p0 + p1) );
            onorm.push_back( ( inorm[ pol[pos[0]] ] * p1
                              + inorm[ pol[pos[2]] ] * p0 )
                            / (p0 + p1) );
            addTextureInterpolPoint( count, pol[pos[0]], p1, pol[pos[2]], p0 );
            ++nvo;
          }
          else
            uv1 = iseg->second;

          if( pol[pos[0]] < pol[pos[1]] )
          {
            seg.first = pol[pos[0]];
            seg.second = pol[pos[1]];
          }
          else
          {
            seg.first = pol[pos[1]];
            seg.second = pol[pos[0]];
          }
          iseg = cutseg.find( seg );
          if( iseg == eseg )
          {
            uv2 = cutseg[ seg ] = nvo;
            overt.push_back( ( ivert[ pol[pos[0]] ] * p2
                              + ivert[ pol[pos[1]] ] * p0 )
                            / (p0 + p2) );
            onorm.push_back( ( inorm[ pol[pos[0]] ] * p2
                              + inorm[ pol[pos[1]] ] * p0 )
                            / (p0 + p2) );
            addTextureInterpolPoint( count, pol[pos[0]], p2, pol[pos[1]], p0 );
            uv2 = nvo;
            ++nvo;
          }
          else
            uv2 = iseg->second;

          // check poly orientation
          if( ( pos[0] == 0 && pos[2] == 1 )
              || ( pos[0] == 1 && pos[2] == 2 )
              || ( pos[0] == 2 && pos[2] == 0 ) )
            opoly.push_back( AimsVector<uint, 3>( (uint) uv0, uv1,
                                                  uv2 ) );
          else
            opoly.push_back( AimsVector<uint, 3>( (uint) uv0, uv2,
                                                  uv1 ) );
          polyline.insert( pair<uint, uint>( uv1, uv2 ) );
        }
        else			// 2 polys
        {
          int	& uv0 = itoo[ pol[ pos[0] ] ];
          if( uv0 < 0 )
          {
            uint pol0 = pol[ pos[0] ];
            uv0 = (int) nvo;
            overt.push_back( ivert[ pol0 ] );
            onorm.push_back( inorm[ pol0 ] );
            addTexturePoint( count, pol0 );
            ++nvo;
          }
          int	& uv1 = itoo[ pol[ pos[1] ] ];
          if( uv1 < 0 )
          {
            uint pol0 = pol[ pos[1] ];
            uv1 = (int) nvo;
            overt.push_back( ivert[ pol0 ] );
            onorm.push_back( inorm[ pol0 ] );
            addTexturePoint( count, pol0 );
            ++nvo;
          }

          p0 = wt[pos[0]];
          p1 = wt[pos[1]];
          p2 = -wt[pos[2]];

          uint	uv2, uv3;
          if( pol[pos[0]] < pol[pos[2]] )
          {
            seg.first = pol[pos[0]];
            seg.second = pol[pos[2]];
          }
          else
          {
            seg.first = pol[pos[2]];
            seg.second = pol[pos[0]];
          }
          iseg = cutseg.find( seg );
          if( iseg == eseg )
          {
            uv2 = cutseg[ seg ] = nvo;
            overt.push_back( ( ivert[ pol[pos[0]] ] * p2
                              + ivert[ pol[pos[2]] ] * p0 )
                            / (p0 + p2) );
            onorm.push_back( ( inorm[ pol[pos[0]] ] * p2
                              + inorm[ pol[pos[2]] ] * p0 )
                            / (p0 + p2) );
            addTextureInterpolPoint( count, pol[pos[0]], p2, pol[pos[2]], p0 );
            ++nvo;
          }
          else
            uv2 = iseg->second;

          if( pol[pos[1]] < pol[pos[2]] )
          {
            seg.first = pol[pos[1]];
            seg.second = pol[pos[2]];
          }
          else
          {
            seg.first = pol[pos[2]];
            seg.second = pol[pos[1]];
          }
          iseg = cutseg.find( seg );
          if( iseg == eseg )
          {
            uv3 = cutseg[ seg ] = nvo;
            overt.push_back( ( ivert[ pol[pos[1]] ] * p2
                              + ivert[ pol[pos[2]] ] * p1 )
                            / (p1 + p2) );
            onorm.push_back( ( inorm[ pol[pos[1]] ] * p2
                              + inorm[ pol[pos[2]] ] * p1 )
                            / (p1 + p2) );
            addTextureInterpolPoint( count, pol[pos[1]], p2, pol[pos[2]], p1 );
            ++nvo;
          }
          else
            uv3 = iseg->second;

          // check poly orientation
          if( ( pos[0] == 0 && pos[1] == 1 )
              || ( pos[0] == 1 && pos[1] == 2 )
              || ( pos[0] == 2 && pos[1] == 0 ) )
          {
            opoly.push_back( AimsVector<uint, 3>( (uint) uv0,
                                                  (uint) uv1, uv2 ) );
            opoly.push_back( AimsVector<uint, 3>( uv2, (uint) uv1,
                                                  uv3 ) );
          }
          else
          {
            opoly.push_back( AimsVector<uint, 3>( (uint) uv0,
                                                  uv2, (uint) uv1 ) );
            opoly.push_back( AimsVector<uint, 3>( uv2, uv3,
                                                  (uint) uv1 ) );
          }
          polyline.insert( pair<uint, uint>( uv2, uv3 ) );
        }
      }
    }

    /* cout << "cutMesh : input vertices : " << ivert.size() << endl;
    cout << "          output         : " << overt.size() << endl;
    cout << "          input polygons : " << ipoly.size() << endl;
    cout << "          output         : " << opoly.size() << endl;
    cout << "thrown away polygons     : " << thrownpol << endl;
    cout << "cut         polygons     : " << cutpol << endl; */

    if( buildborderline )
    {
      // polygon mesh
      map<uint, uint>::iterator		ip, ep = cutvert.end();
      set<pair<uint, uint> >::iterator	ipp, epp = polyline.end();
      uint				nvb = bvert.size(), bv0, bv1;

      bpoly.reserve( bpoly.size() + polyline.size() );
      bvert.reserve( bvert.size() + polyline.size() * 2 );

      for( ipp=polyline.begin(); ipp!=epp; ++ipp )
      {
        ip = cutvert.find( ipp->first );
        if( ip == ep )
        {
          cutvert[ ipp->first ] = nvb;
          bvert.push_back( overt[ ipp->first ] );
          bv0 = nvb++;
        }
        else
          bv0 = ip->second;
        ip = cutvert.find( ipp->second );
        if( ip == ep )
        {
          cutvert[ ipp->second ] = nvb;
          bvert.push_back( overt[ ipp->second ] );
          bv1 = nvb++;
        }
        else
          bv1 = ip->second;
        bpoly.push_back( AimsVector<uint, 2>( bv0, bv1 ) );
      }

      if( first )
        first = false;
      else
      {
        SurfaceManip::meshMerge( *_borderline, *pbline );
      }
    }
    else
      first = false;
  }

  // mesh interior of polyline into _planemesh
  if( meshplane )
    SurfaceManip::meshPlanarPolygon( _plane, *_borderline, *_planemesh ); //,
                                     // checkplane );
}


void CutMesh::cutBorder( int timestep )
{
  // TODO: currently limited to the 1st mesh

  AimsSurfaceTriangle::const_iterator imesh = _insurf[0]->find( timestep );
  if( imesh == _insurf[0]->end() )
    return;

  const vector<Point3df> & ivert = imesh->second.vertex();
  const vector<AimsVector<uint, 3> > & ipoly = imesh->second.polygon();
  _borderline.reset( new AimsTimeSurface<2, Void> );
  vector<Point3df> & overt = _borderline->vertex();
  vector<AimsVector<uint, 2> > & opoly = _borderline->polygon();
  vector<AimsVector<uint, 3> >::const_iterator ip, ep = ipoly.end();
  float p0, p1, p2, p01, p02, scal, x;
  Point3df norm = Point3df( _plane[0], _plane[1], _plane[2] );
  Point3df op[2];
  int nop;
  size_t novert = 0;

  for( ip=ipoly.begin(); ip!=ep; ++ip )
  {
    const AimsVector<uint, 3> & pol = *ip;
    const Point3df & pv0 = ivert[ pol[0] ];
    const Point3df & pv1 = ivert[ pol[1] ];
    const Point3df & pv2 = ivert[ pol[2] ];

    /* compare to plane */
    p0 = _plane[0] * pv0[0] + _plane[1] * pv0[1] + _plane[2] * pv0[2]
      + _plane[3];
    p1 = _plane[0] * pv1[0] + _plane[1] * pv1[1] + _plane[2] * pv1[2]
      + _plane[3];
    p2 = _plane[0] * pv2[0] + _plane[1] * pv2[1] + _plane[2] * pv2[2]
      + _plane[3];

    p01 = p0 * p1;
    p02 = p0 * p2;
    nop = 0; // 0 intersection points found

    if( p01 <= 0 || p02 <= 0 ) // polygon intersects plane
    {
      if( p01 <= 0 ) // pv0 - pv1 cuts the plane
      {
        Point3df pv01 = pv1 - pv0;
        scal = pv01.dot( norm );
        if( scal == 0 ) // pv0 and p1 are on the plane
        {
          // both point are found
          op[0] = pv0;
          op[1] = pv1;
          nop = 2;
        }
        else
        {
          x = - ( _plane[3] + pv0.dot( norm ) ) / scal; // position of intersection point
          op[nop] = pv0 + pv01 * x;
          ++nop;
        }
      }

      if( nop < 2 && p02 <= 0 ) // pv0 - pv2 cuts the plane
      {
        Point3df pv02 = pv2 - pv0;
        scal = pv02.dot( norm );
        if( scal == 0 ) // pv0 and p2 are on the plane
        {
          // both point are found
          op[0] = pv0;
          op[1] = pv2;
          nop = 2;
        }
        else
        {
          x = - ( _plane[3] + pv0.dot( norm ) ) / scal; // position of intersection point
          op[nop] = pv0 + pv02 * x;
          ++nop;
        }
      }

      if( nop < 2 ) // pv1 - pv2 cuts the plane
      {
        Point3df pv12 = pv2 - pv1;
        scal = pv12.dot( norm );
        if( scal == 0 ) // pv1 and p2 are on the plane
        {
          // both point are found
          op[0] = pv1;
          op[1] = pv2;
          nop = 2;
        }
        else
        {
          x = - ( _plane[3] + pv1.dot( norm ) ) / scal; // position of intersection point
          op[nop] = pv1 + pv12 * x;
          ++nop;
        }
      }

      if( op[0] != op[1] )
      {
        // append 2 vertices and a polygon
        // note: the final polygon is completely topologically disconnected
        // (no shared vertex)
        overt.push_back( op[0] );
        overt.push_back( op[1] );
        opoly.push_back( AimsVector<uint, 2>( novert, novert+1 ) );
        novert += 2;
      }

    }

  }
}


namespace aims
{

  template class CutTexturedMesh<float>;
  template class CutTexturedMesh<int16_t>;
  template class CutTexturedMesh<int32_t>;
  template class CutTexturedMesh<uint16_t>;
  template class CutTexturedMesh<uint32_t>;
  template class CutTexturedMesh<Point2df>;

}

