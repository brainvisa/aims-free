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

#include <aims/mesh/meshinterpoler_d.h>
#ifdef CARTO_USE_KDTREE
#include <kdtree++/kdtree.hpp>
#else
#warning libkdtree not used, mesh interpolation will be SLOW !
#endif

using namespace aims;
using namespace std;


MeshInterpoler::Private::Private()
  : source( 0 ), dest( 0 ), sourceVert( 0 ), /*sourceNormal( 0 ),*/
  sourcePoly( 0 ), /*nSourceVertex( 0 ),*/ nSourcePolygon( 0 ),
  destVertex( 0 ), /*destNormal( 0 ),*/ destPolygon( 0 ), /*nDestVertex( 0 ),*/
  nDestPolygon( 0 )
{
}


MeshInterpoler::MeshInterpoler( const AimsSurfaceTriangle & source, const
  AimsSurfaceTriangle & dest )
  : d( new Private )
{
  d->source = &source;
  d->dest = &dest;
}


MeshInterpoler::MeshInterpoler( const Point3df* sourceVert,
                                const AimsVector<uint,3>* sourcePoly,
                                uint nSourcePolygon,
                                const Point3df* destVertex,
                                const AimsVector<uint,3>* destPolygon,
                                uint nDestVertex,
                                uint nDestPolygon )
  : d( new Private )
{
  d->sourceVert = sourceVert;
  d->sourcePoly = sourcePoly;
  d->nSourcePolygon = nSourcePolygon;
  d->destVertex = destVertex;
  d->destPolygon = destPolygon;
  d->nDestVertex = nDestVertex;
  d->nDestPolygon = nDestPolygon;
}


MeshInterpoler::~MeshInterpoler()
{
  delete d;
}


const TimeTexture<uint> & MeshInterpoler::projectedTriangles() const
{
  return d->projTriangles;
}


const TimeTexture<float> & MeshInterpoler::projectedTriCoord1() const
{
  return d->projCoord1;
}


const TimeTexture<float> & MeshInterpoler::projectedTriCoord2() const
{
  return d->projCoord2;
}


const TimeTexture<float> & MeshInterpoler::projectedTriCoord3() const
{
  return d->projCoord3;
}


void MeshInterpoler::project()
{
  cout << "project...\n";
  findNeighbours();
  cout << "findNeighbours done.\n";
  processCoordinates();
  cout << "processCoordinates done.\n";
}


#ifdef CARTO_USE_KDTREE
namespace
{

  struct Bracket_accessor_PointIndex
  {
    typedef float result_type;
    typedef pair<uint, Point3df> _Val;

    result_type
    operator()(_Val const& V, size_t const N) const
    {
      return V.second[N];
    }
  };

}
#endif


void MeshInterpoler::findNeighbours()
{
  d->projTriangles.clear();
  if( d->sourceVert )
  {
    findNeighbours_timestep( d->sourceVert, d->sourcePoly, d->nSourcePolygon,
                             d->destVertex, d->nDestVertex, 0 );
    return;
  }

  AimsSurfaceTriangle::const_iterator its, itd, ets = d->source->end();
  for( its=d->source->begin(); its!=ets; ++its )
  {
    // get corresponding timestep on destination time mesh
    itd = d->dest->upper_bound( its->first );
    --itd;
    int timestep = its->first;
    const AimsSurface<3,Void> & s1 = its->second;
    const AimsSurface<3,Void> & s2 = itd->second;

    const vector<Point3df> & vert1 = s1.vertex();
    const vector<Point3df> & vert2 = s2.vertex();
    const vector< AimsVector<uint,3> > &tri1 = s1.polygon();
    unsigned                      n2 = vert2.size();
    unsigned                      nt1 = tri1.size();
    findNeighbours_timestep( &vert1[0], &tri1[0], nt1, &vert2[0], n2,
      timestep );
  }
}


void MeshInterpoler::findNeighbours_timestep( const Point3df *vert1,
  const AimsVector<uint,3>* tri1, uint nt1,
  const Point3df* vert2, uint n2, int timestep )
{
  unsigned                      i, t;
  float                         dd, dmin;
  unsigned                      trimin;
  Point3df                      bary;

  Texture<unsigned> & triCorresp = d->projTriangles[ timestep ];
  triCorresp.reserve( n2 );
  triCorresp.data().insert( triCorresp.data().end(), n2, 0 );

#ifdef CARTO_USE_KDTREE
  // make a list of source triangles centers
  vector<pair<uint, Point3df> > centers( nt1 );
  for( t=0; t<nt1; ++t )
  {
    const AimsVector<uint, 3>     & tri = tri1[t];
    const Point3df                & p1 = vert1[ tri[0] ];
    const Point3df                & p2 = vert1[ tri[1] ];
    const Point3df                & p3 = vert1[ tri[2] ];

    // center of the triangle
    bary = ( p1 + p2 + p3 ) * 0.33333;

    centers[t].first = t;
    centers[t].second = bary;
  }

  // make a KDtree with polygons centers
  KDTree::KDTree<3, pair<uint, Point3df>, Bracket_accessor_PointIndex >
    kdtree( centers.begin(), centers.end() );

  // find nearest polygon center for each vertex of dest mesh
  for( i=0; i<n2; ++i )
  {
    const Point3df    & pt = vert2[i];
    triCorresp[i] = kdtree.find_nearest( make_pair( 0U, pt ) ).first->first;
  }

#else

  for( i=0; i<n2; ++i )
  {
    const Point3df    & pt = vert2[i];

    dmin = 1e38;
    trimin = 0;

    for( t=0; t<nt1; ++t )
    {
      const AimsVector<uint, 3>     & tri = tri1[t];
      const Point3df                & p1 = vert1[ tri[0] ];
      const Point3df                & p2 = vert1[ tri[1] ];
      const Point3df                & p3 = vert1[ tri[2] ];

      // distance to the center of the triangle
      // (we assume there is no need to project onto it)
      bary = ( p1 + p2 + p3 ) * 0.33333 - pt;

      dd = bary.norm2();
      if( dd < dmin )
      {
        trimin = t;
        dmin = dd;
      }
    }
    triCorresp[i] = trimin;
  }

#endif
}


void MeshInterpoler::processCoordinates()
{
  if( d->sourceVert )
  {
    processCoordinates_timestep( d->sourceVert, d->sourcePoly, d->destVertex,
      d->nDestVertex, 0 );
    return;
  }

  AimsSurfaceTriangle::const_iterator its, itd, ets = d->source->end();
  for( its=d->source->begin(); its!=ets; ++its )
  {
    // get corresponding timestep on destination time mesh
    itd = d->dest->upper_bound( its->first );
    --itd;
    int timestep = its->first;
    const AimsSurface<3,Void> & s1 = its->second;
    const AimsSurface<3,Void> & s2 = itd->second;

    const vector<Point3df> & vert1 = s1.vertex();
    const vector<Point3df> & vert2 = s2.vertex();
    const vector< AimsVector<uint,3> > &polyo = s1.polygon();

    unsigned                      n = vert2.size();

    processCoordinates_timestep( &vert1[0], &polyo[0], &vert2[0], n,
      timestep );
  }
}


void MeshInterpoler::processCoordinates_timestep( const Point3df *vert1,
  const AimsVector<uint,3> *polyo, const Point3df *vert2, uint n,
  int timestep )
{
  vector<float>                 & coord1 = d->projCoord1[timestep].data();
  vector<float>                 & coord2 = d->projCoord2[timestep].data();
  vector<float>                 & coord3 = d->projCoord3[timestep].data();
  const vector<unsigned> & triCorresp = d->projTriangles[timestep].data();

  coord1.clear();
  coord1.reserve(n);
  coord2.clear();
  coord2.reserve(n);
  coord3.clear();
  coord3.reserve(n);

  unsigned                      i, t;
  unsigned                      tri, j1, j2, j3;
  float                         d1, d2, d3, x1, x2, a, b, l1, l2;
  Point3df                      vec1, vec2, vec3, vec4;
  bool                          xxfar;

  for( i=0; i<n; ++i )
  {
    const Point3df            & pt = vert2[i];

    tri = triCorresp[i];
    const AimsVector<uint,3>  & triang = polyo[tri];
    j1 = triang[0];
    j2 = triang[1];
    j3 = triang[2];
    const Point3df            & p1 = vert1[j1];
    const Point3df            & p2 = vert1[j2];
    const Point3df            & p3 = vert1[j3];

    // project point on triangle

    vec1 = pt - p1;

    vec2 = p2 - p1;
    l1 = 1. / vec2.norm();
    vec2 *= l1;

    vec3 = p3 - p1;
    l2 = 1. / vec3.norm();
    vec3 *= l2;

    // make an orthogonal referential

    a = - vec2.dot( vec3 );
    b = 1. / sqrt( 1. - a * a );
    a *= b;
    vec4 = a * vec2 + b * vec3;

    // project

    x1 = vec1.dot( vec2 );
    x2 = vec1.dot( vec4 );

    // return to triangle coordinates

    d2 = ( x1 + a * x2 ) * l1;
    d3 = ( b * x2 ) * l2;
    xxfar = false;

    if( d2 < 0 )
    {
      if( d2 < -1 )
        xxfar = true;
      d2 = 0;
    }
    else if( d2 > 1 )
    {
      if( d2 > 2 )
        xxfar = true;
      d2 = 1;
    }
    if( d3 < 0 )
    {
      if( d3 < -1 )
        xxfar = true;
      d3 = 0;
    }
    else if( d3 > 1. - d2 )
    {
      if( d3 > 2 - d2 )
        xxfar = true;
      d3 = 1. - d2;
    }

    d1 = 1. - d2 - d3;
    coord1.push_back( d1 );
    coord2.push_back( d2 );
    coord3.push_back( d3 );
  }
}


AimsSurfaceTriangle *MeshInterpoler::resampleMesh(
  const AimsSurfaceTriangle & sourceshape ) const
{
  AimsSurfaceTriangle *mesh = new AimsSurfaceTriangle;
  AimsSurfaceTriangle::const_iterator itss, etss = sourceshape.end(),
    its, itd;
  for( itss=sourceshape.begin(); itss!=etss; ++itss )
  {
    int timestep;
    const AimsVector<uint,3> *poly2;
    unsigned long np2;

    if( d->sourceVert )
    {
      timestep = 0;
      poly2 = d->destPolygon;
      np2 = d->nDestPolygon;
    }
    else
    {
      // get corresponding timestep on source time mesh
      its = d->source->upper_bound( itss->first );
      --its;
      // get corresponding timestep on destination time mesh
      itd = d->dest->upper_bound( its->first );
      --itd;
      timestep = its->first;

      const AimsSurface<3,Void> & s2 = itd->second;
      poly2 = &s2.polygon()[0];
      np2 = s2.polygon().size();
    }

    const AimsSurface<3,Void> & rs1 = itss->second;
    const vector<Point3df> & vert1 = rs1.vertex();
    const vector< AimsVector<uint,3> > &poly1 = rs1.polygon();
    const vector<Point3df> & norm1 = rs1.normal();

    const vector<float>           & coord1 = d->projCoord1[timestep].data();
    const vector<float>           & coord2 = d->projCoord2[timestep].data();
    const vector<float>           & coord3 = d->projCoord3[timestep].data();
    const vector<unsigned> & triCorresp = d->projTriangles[timestep].data();
    unsigned                      i, n = triCorresp.size();
    bool hasnormals = !norm1.empty();

    AimsSurface<3,Void> & surf = (*mesh)[timestep];
    vector<Point3df> & vert = surf.vertex();
    vector<Point3df> & norm = surf.normal();
    vector<AimsVector<uint,3> > & poly = surf.polygon();
    vert.reserve( n );
    vert.insert( vert.end(), n, Point3df() );
    norm.reserve( n );
    norm.insert( norm.end(), n, Point3df() );
    // just copy polygons from dest mesh
    poly.reserve( np2 );
    poly.insert( poly.end(), poly2, poly2 + np2 );

    for( i=0; i<n; ++i )
    {
      const AimsVector<uint,3> & tri = poly1[ triCorresp[i] ];
      const Point3df & v1 = vert1[ tri[0] ];
      const Point3df & v2 = vert1[ tri[1] ];
      const Point3df & v3 = vert1[ tri[2] ];
      float a = coord1[i], b = coord2[i], c = coord3[i];
      vert[i] = v1 * a + v2 * b + v3 * c;
      if( hasnormals )
      {
        norm[i] = norm1[ tri[0] ] * a + norm1[ tri[1] ] * b
          + norm1[ tri[2] ] * c;
        norm[i].normalize();
      }
    }
  }

  return mesh;
}


void MeshInterpoler::reloadProjectionParams(
  const TimeTexture<uint> & projTriangles,
  const TimeTexture<float> & projTriCoord1,
  const TimeTexture<float> & projTriCoord2,
  const TimeTexture<float> & projTriCoord3 )
{
  d->projTriangles = projTriangles;
  d->projCoord1 = projTriCoord1;
  d->projCoord2 = projTriCoord2;
  d->projCoord3 = projTriCoord3;
}


// ---

template TimeTexture<float> *
  MeshInterpoler::resampleTexture( const TimeTexture<float> & ) const;
template void MeshInterpoler::resampleTexture(
  const Texture<float> &, Texture<float> &, int ) const;
template void
  MeshInterpoler::resampleTexture( const float *, float *, int ) const;

template TimeTexture<Point2df> *
  MeshInterpoler::resampleTexture( const TimeTexture<Point2df> & ) const;
template void MeshInterpoler::resampleTexture(
  const Texture<Point2df> &, Texture<Point2df> &, int ) const;
template void
  MeshInterpoler::resampleTexture( const Point2df *, Point2df *, int ) const;

