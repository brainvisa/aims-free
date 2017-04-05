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

/*
 *  Surface class
 */
#include <cstdlib>
#include <aims/mesh/surfaceOperation_d.h>
#include <list>
#include <algorithm>
#include <math.h>


using namespace aims;
using namespace carto;
using namespace std;


template std::vector<std::set<uint> >
SurfaceManip::surfaceNeighbours( const AimsTimeSurface<2, Void> & surf );
template std::vector<std::set<uint> >
SurfaceManip::surfaceNeighbours( const AimsSurface<3, Void> & surf );
template std::vector<std::set<uint> >
SurfaceManip::surfaceNeighbours( const AimsSurfaceTriangle & surf );
template std::vector<std::set<uint> >
SurfaceManip::surfaceNeighbours( const AimsSurfaceFacet & surf );

template std::vector<std::set<uint> >
SurfaceManip::surfaceNeighbours2ndOrder( const AimsTimeSurface<2, Void> & surf );
template std::vector<std::set<uint> >
SurfaceManip::surfaceNeighbours2ndOrder( const AimsSurfaceTriangle & surf );
template std::vector<std::set<uint> >
SurfaceManip::surfaceNeighbours2ndOrder( const AimsSurfaceFacet & surf );

template std::vector<std::map<uint, float> >
SurfaceManip::surfaceNeighbourDistance( const AimsTimeSurface<2, Void> & surf );
template std::vector<std::map<uint, float> >
SurfaceManip::surfaceNeighbourDistance( const AimsSurfaceTriangle & surf );
template std::vector<std::map<uint, float> >
SurfaceManip::surfaceNeighbourDistance( const AimsSurfaceFacet & surf );

template void
SurfaceManip::invertSurfacePolygons( AimsTimeSurface<2, Void> & surface );
template void
SurfaceManip::invertSurfacePolygons( AimsSurfaceTriangle & surface );
template void
SurfaceManip::invertSurfacePolygons( AimsSurfaceFacet & surface );

template void SurfaceManip::sortPolygonsAlongDirection(
  AimsTimeSurface<2, Void> & mesh, int timestep, const Point3df & direction );
template void SurfaceManip::sortPolygonsAlongDirection(
  AimsTimeSurface<3, Void> & mesh, int timestep, const Point3df & direction );
template void SurfaceManip::sortPolygonsAlongDirection(
  AimsTimeSurface<4, Void> & mesh, int timestep, const Point3df & direction );


namespace
{
  //  2nd version of mesh cut (12/2006)

  enum _PointClass
  {
    _Neighbour,
    _NewBorder,
    _Bifurcation,
  };


  struct _Front;

  struct _Point
  {
    _Point();
    ~_Point();
    void addNeigh( _Point & p );
    _Point* firstTrigoNeighbour( _Point &, _Point & );
    bool inCone( _Point & );
    void swapNeighBours();
    void orderNeighbours();

    unsigned         node;
    Point2df         pos;
    _Point           *leftneigh;
    _Point           *rightneigh;
    _Point           *twin; // usused
    bool             done;
    bool             sunk;
    _Front           *front;
  };


  struct _PointClassification
  {
    _PointClass type;
    _Point  *neighbour;
  };


  struct _Front
  {
    // lower pt left
    _Point  *pleft;
    // higher pt left
    //_Point  *pleft2;
    // lower pt right
    _Point  *pright;
    // higher pt right
    //_Point  *pright2;
  };


  struct _PointSet
  {
    const Point3df & vertexPos( const _Point & ) const;
    void setVertexPos( const _Point &, const Point3df & pos );
    unsigned createVertex();

    list<_Point>   unsorted;
    multimap<float, _Point*> sorted;
    const vector<Point3df>  *vertex;
    vector<Point3df>  additional;
    set<_Point *>     donepoints;
    list<_Front>      fronts;
  };


  struct _VertexMap
  {
    _VertexMap( unsigned n ) : maxindex(0), vertmap( n, -1 ) {}
    unsigned vertex( unsigned node );

    int         maxindex;
    vector<int> vertmap;
  };

  void planarMeshTestJunction( _Point & p, _Point & p1,
                               AimsSurface<3, Void> & mesh, 
                               _PointSet & pts, _VertexMap & vmap );


  inline _Point::_Point()
  : node( 0 ), leftneigh( 0 ), rightneigh( 0 ), twin( 0 ), 
  done( false ), sunk( false ), front( 0 )
  {
  }

  inline _Point::~_Point()
  {
  }

  inline void _Point::addNeigh( _Point & p )
  {
    if( !leftneigh )
    {
      leftneigh = &p;
      return;
    }
    if( rightneigh )
    {
      cerr << "more than 2 connections on the same point.\n";
      throw runtime_error( "more than 2 connections on the same point." );
      // TODO duplicate point
    }
    if( firstTrigoNeighbour( p, *leftneigh ) == &p )
      rightneigh = &p;
    else
    {
      rightneigh = leftneigh;
      leftneigh = &p;
    }
  }

  _Point* _Point::firstTrigoNeighbour( _Point & p1, _Point & p2 )
  {
    Point2df  v1 = p1.pos - pos;
    Point2df  v2 = p2.pos - pos;
    if( v1[0] * v2[1] - v1[1] - v2[0] >= 0 )
      return &p1;
    return &p2;
  }

  bool _Point::inCone( _Point & p )
  {
    if( sunk )
      return false;
    Point2df  v1 = rightneigh->pos - pos;
    v1.normalize();
    Point2df  v2 = p.pos - pos;
    v2.normalize();
    float c = acos( v1.dot( v2 ) );
    Point2df  v3( -v1[1], v1[0] );
    if( v2.dot( v3 ) < 0 )
      c += M_PI;
    v2 = leftneigh->pos - pos;
    v2.normalize();
    float d = acos( v1.dot( v2 ) );
    if( v2.dot( v3 ) < 0 )
      d += M_PI;
    return c <= d;
  }

  inline void _Point::swapNeighBours()
  {
    _Point  *p0 = leftneigh;
    leftneigh = rightneigh;
    rightneigh = p0;
  }

  void _Point::orderNeighbours()
  {
    if( leftneigh->rightneigh != this )
      leftneigh->swapNeighBours();
    if( rightneigh->leftneigh != this )
      rightneigh->swapNeighBours();
  }


  inline const Point3df & _PointSet::vertexPos( const _Point & p ) const
  {
    if( p.node < vertex->size() )
      return (*vertex)[ p.node ];
    return additional[ p.node - vertex->size() ];
  }

  inline void _PointSet::setVertexPos( const _Point & p,
                                       const Point3df & pos )
  {
    if( p.node >= vertex->size() )
      additional[ p.node - vertex->size() ] = pos;
  }

  inline unsigned _PointSet::createVertex()
  {
    additional.push_back( Point3df() );
    return additional.size() + vertex->size() - 1;
  }


  inline unsigned _VertexMap::vertex( unsigned node )
  {
    if( node >= vertmap.size() )
      vertmap.push_back( -1 );
    int & x = vertmap[ node ];
    if( x < 0 )
      x = maxindex++;
    return (unsigned) x;
  }


  void planarMeshSortPoints( const AimsSurface<2, Void> & pmesh,
                             const Point4df & plane,
                             _PointSet & pts )
  {
    const vector<Point3df>                  & vert = pmesh.vertex();
    const vector<AimsVector<unsigned, 2> >  & poly = pmesh.polygon();
    vector<AimsVector<unsigned, 2> >::const_iterator  ip, ep = poly.end();
    Point2df                                pos;
    unsigned                                iv0, iv1;
    vector<bool>                            donep( vert.size(), false );
    Point3df                                n, u, v;

    // local coord system (u,v)
    n = Point3df( -plane[0], -plane[1], -plane[2] );
    n.normalize();
    if( n[0] != 0 || n[1] != 0 )
      u = Point3df( -n[1], n[0], 0 );
    else
      u = Point3df( 0, -n[2], n[1] );
    u.normalize();
    v = Point3df( n[1] * u[2] - n[2] * u[1], 
                  n[2] * u[0] - n[0] * u[2], 
                  n[0] * u[1] - n[1] * u[0] );
    v.normalize();

    // init unsorted list
    // pts.unsorted.reserve( unsigned( vert.size() * 1.5 ) );
    vector<_Point *>  unsorted;
    unsorted.reserve( unsigned( vert.size() ) );
    pts.unsorted.insert( pts.unsorted.end(), vert.size(), _Point() );
    pts.vertex = &vert;

    list<_Point>::iterator  iup, eup = pts.unsorted.end();
    for( iup=pts.unsorted.begin(); iup!=eup; ++iup )
      unsorted.push_back( &*iup );

    for( ip=poly.begin(); ip!=ep; ++ip )
    {
      iv0 = (*ip)[0];
      iv1 = (*ip)[1];
      _Point & p0 = *unsorted[ iv0 ];
      _Point & p1 = *unsorted[ iv1 ];
      p0.node = iv0;
      p1.node = iv1;
      p0.addNeigh( p1 );
      p1.addNeigh( p0 );
      if( !donep[ iv0 ] )
      {
        p0.pos = Point2df( u.dot( vert[ iv0 ] ), v.dot( vert[ iv0 ] ) );
        pts.sorted.insert( pair<float, _Point*>( p0.pos[1], &p0 ) );
        donep[ iv0 ] = true;
      }
      if( !donep[ iv1 ] )
      {
        p1.pos = Point2df( u.dot( vert[ iv1 ] ), v.dot( vert[ iv1 ] ) );
        pts.sorted.insert( pair<float, _Point*>( p1.pos[1], &p1 ) );
        donep[ iv1 ] = true;
      }
    }
    cout << "sortPoints done." << endl << flush;

    // check for interruptions / open polygons
    list<_Point>::iterator ilp, ilp2, elp = pts.unsorted.end();
    multimap<float, _Point*>::iterator  isp, esp = pts.sorted.end();
    bool found;
    _Point  *p2, *p3;
    const float margin = 1e-7;
    ilp=pts.unsorted.begin();
    while( ilp!=elp )
    {
      _Point & p = *ilp;
      if( !p.leftneigh || !p.rightneigh )
      {
        if( !p.leftneigh && !p.rightneigh )
        {
          // unconnected point: juste remove it
          isp = pts.sorted.find( p.pos[1] );
          for( ; isp->second != &p; ++isp ) {}
          pts.sorted.erase( isp );
          ilp2 = ilp;
          ++ilp;
          pts.unsorted.erase( ilp2 );
          continue;
        }

        cerr << "planarMeshSortPoints: Open polygon" << endl;
        // look for another point at the same position
        isp = pts.sorted.lower_bound( p.pos[1] - margin );
        found = false;
        if( isp != esp )
        {
          while( isp != esp && isp->first <= p.pos[1] + margin )
          {
            if( isp->second != &p
                && fabs( isp->second->pos[0] - p.pos[0] ) <= margin
                && ( ( isp->second->leftneigh && !isp->second->rightneigh )
                     || ( !isp->second->leftneigh && isp->second->rightneigh )
                   ) )
            {
              found = true;
              break;
            }
            ++isp;
          }
          if( found )
          {
            cout << ".. found nearby unconnected point: merging "
              << p.node << " and " << isp->second->node << endl;
            // merge the 2 points
            if( isp->second->leftneigh )
              p2 = isp->second->leftneigh;
            else
              p2 = isp->second->rightneigh;
            if( !p.leftneigh )
              p.leftneigh = p2;
            else
              p.rightneigh = p2;
            if( p2->leftneigh == isp->second )
              p2->leftneigh = &p;
            else
              p2->rightneigh = &p;
            for( ilp2 = ilp, ++ilp2; &*ilp2!=isp->second; ++ilp2 ) {}
            pts.unsorted.erase( ilp2 );
            pts.sorted.erase( isp );
            // cout << "merge done.\n";
          }
        }
        if( !found )
        {
          // cout << "looking for other end\n";
          set<_Point *> done1;
          // look for the other end of polyline
          if( p.leftneigh )
            p2 = p.leftneigh;
          else
            p2 = p.rightneigh;
          p3 = &p;
          done1.insert( &p );
          while( p2->leftneigh && p2->rightneigh )
          {
            // cout << p2->node << " " << flush;
            if( done1.find( p2 ) != done1.end() )
            {
              cerr << "BUG: loop in open polygon\n";
              throw runtime_error( "BUG in planarMeshSortPoints: loop in open "
                                   "polygon" );
            }
            done1.insert( p2 );
            if( p2->leftneigh == p3 )
            {
              p3 = p2;
              p2 = p2->rightneigh;
            }
            else
            {
              p3 = p2;
              p2 = p2->leftneigh;
            }
          }
          // connect other end: p2 to p
          cout << ".. closing open polygon" << endl;
          if( !p.rightneigh )
            p.rightneigh = p2;
          else
            p.leftneigh = p2;
          if( !p2->rightneigh )
            p2->rightneigh = &p;
          else
            p2->leftneigh = &p;
        }
      }
      ++ilp;
    }

    // #if 0
    // look for duplicate points
    _Point  *np, *oldp;
    set<_Point *> todel;
    set<_Point *>::iterator nodel = todel.end();

    for( ilp=pts.unsorted.begin(); ilp!=elp; ++ilp )
    {
      _Point & p = *ilp;
      if( todel.find( &p ) == nodel )
      {
        if( p.leftneigh->pos == p.pos )
        {
          oldp = p.leftneigh;
          np = oldp->leftneigh;
          if( np == &p )
            np = oldp->rightneigh;
          p.leftneigh = np;
          if( np->leftneigh == oldp )
            np->leftneigh = &p;
          else
            np->rightneigh = &p;
          todel.insert( oldp );
          cout << "remove duplicate point " << oldp->node << " " << oldp->pos
            << endl;
        }
        if( p.rightneigh->pos == p.pos )
        {
          oldp = p.rightneigh;
          np = oldp->leftneigh;
          if( np == &p )
            np = oldp->rightneigh;
          p.rightneigh = np;
          if( np->leftneigh == oldp )
            np->leftneigh = &p;
          else
            np->rightneigh = &p;
          todel.insert( oldp );
          cout << "remove duplicate point " << oldp->node << " " << oldp->pos
            << endl;
        }
      }
    }
    ilp = pts.unsorted.begin();
    while( ilp != elp )
    {
      if( todel.find( &*ilp ) != nodel )
      {
        ilp2 = ilp;
        ++ilp;
        isp = pts.sorted.find( ilp2->pos[1] );
        while( isp->second != &*ilp2 )
          ++isp;
        pts.sorted.erase( isp );
        pts.unsorted.erase( ilp2 );
      }
      else
        ++ilp;
    }
    cout << "duplicate points cleared" << endl;
    // #endif
  }


  int planarMeshDirectTriangle( const _Point & p1, const _Point & p2,
                                const _Point & p3 )
  {
    // > 0 if (p1, p2, p3) is a direct triangle
    Point2df  v( p3.pos - p2.pos ), w( p1.pos - p2.pos );
    float x = v[0] * w[1] - v[1] * w[0];
    if( x >= 0 )
    {
      if( x < 1e-10 )
        return 0;
      else
        return 1;
    }
    return -1; // x >= 0 ? 1 : -1;
  }


  void planarMeshInsertTriangle( _Point & p, _Point & p1, _Point & p2, 
                                 AimsSurface<3, Void> & mesh,
                                 _VertexMap & vertmap, const _PointSet & pts )
  {
    vector<Point3df>                  & vert = mesh.vertex();
    vector<AimsVector<unsigned, 3> >  & poly = mesh.polygon();
    unsigned i = vertmap.vertex( p.node ), j, k;
    if( vert.size() <= i )
      vert.push_back( pts.vertexPos( p ) );
    j = vertmap.vertex( p1.node );
    if( vert.size() <= j )
      vert.push_back( pts.vertexPos( p1 ) );
    k = vertmap.vertex( p2.node );
    if( vert.size() <= k )
      vert.push_back( pts.vertexPos( p2 ) );
    // check polygon ordering
    // z component of vector product
    Point2df  v( p1.pos - p.pos ), w( p2.pos - p.pos );
    if( v[0] * w[1] - v[1] * w[0] >= 0 )
    {
      // cout << "...... add tri: " << i << ", " << j << ", " << k << endl;
      poly.push_back( AimsVector<unsigned, 3>( i, j, k ) );
    }
    else
    {
      // cout << "...... add tri: " << i << ", " << k << ", " << j << endl;
      poly.push_back( AimsVector<unsigned, 3>( i, k, j ) );
    }
  }


  bool planarMeshTestIntersections( _Point & /* p */, _Point & /* p2 */ )
  {
    return false;
  }


  _Point* planarMeshInsertPoint( _Point & p1, _Point & p2, _PointSet & pts )
  {
    /* cout << ".... planarMeshInsertPoint " << p1.node << " - " << p2.node
      << endl; */
    pts.unsorted.push_back( _Point() );
    _Point & p0 = pts.unsorted.back();
    if( &p1 == p2.leftneigh )
    {
      p0.leftneigh = &p1;
      p0.rightneigh = &p2;
      p1.rightneigh = &p0;
      p2.leftneigh = &p0;
    }
    else
    {
      p0.leftneigh = &p2;
      p0.rightneigh = &p1;
      p2.rightneigh = &p0;
      p1.leftneigh = &p0;
    }
    p0.front = p1.front;
    return &p0;
  }


  _Point* planarMeshInsertPoint( _Point & p1, _Point & p2, float y,
                                 _PointSet & pts )
  {
    /*cout << ".... planarMeshInsertPoint " << p1.node << " - " << p2.node
      << " at " << y << endl;*/
    /* cout << "..... insertPoint: neigh of " << p1.node << ": "
      << p1.leftneigh->node
      << ", " << p1.rightneigh->node << endl;
    cout << "....... neigh of " << p2.node << ": " << p2.leftneigh->node
      << ", " << p2.rightneigh->node << endl; */
    _Point & p0 = *planarMeshInsertPoint( p1, p2, pts );
    p0.node = pts.createVertex();
    /*cout << "...... planarMeshInsertPoint(y): new node " << p0.node << endl;
    cout << "....... neigh of " << p0.node << ": " << p0.leftneigh->node
      << ", " << p0.rightneigh->node << endl;
    cout << "....... neigh of " << p1.node << ": " << p1.leftneigh->node
      << ", " << p1.rightneigh->node << endl;
    cout << "....... neigh of " << p2.node << ": " << p2.leftneigh->node
      << ", " << p2.rightneigh->node << endl; */
    float z = 0;
    if( p1.pos[1] == p2.pos[1] )
      z = 0.5;
    else
      z = ( y - p1.pos[1] ) / ( p2.pos[1] - p1.pos[1] );
    if( z < 0 || z > 1 )
    {
      // cout << endl;
      cerr << "planarMeshInsertPoint z=" << z << endl;
      throw runtime_error( "planarMeshInsertPoint: point not in segment" );
    }
    pts.setVertexPos( p0, pts.vertexPos( p2 ) * z
                      + pts.vertexPos( p1 ) * ( 1. - z ) );

    p0.pos = p2.pos * z + p1.pos * ( 1. - z );
    return &p0;
  }


  void planarMeshMergeFronts( _Point & p1, _Point & p2 )
  {
    if( p1.front == p2.front )
      return;
    list<_Point *> todo;
    todo.push_back( &p1 );
    _Front  *oft = p1.front, *nft = p2.front;
    _Point  *p;
    while( !todo.empty() )
    {
      p = todo.front();
      todo.pop_front();
      p->front = nft;
      if( p->leftneigh )
      {
        if( p->leftneigh->front == oft )
          todo.push_back( p->leftneigh );
        else if( nft && p->leftneigh->front != nft )
          nft->pleft = p;
      }
      if( p->rightneigh )
      {
        if( p->rightneigh->front == oft )
          todo.push_back( p->rightneigh );
        else if( nft && p->rightneigh->front != nft )
          nft->pright = p;
      }
    }
  }


  void planarMeshRaiseBorderPoint( _Point & oldp, _Point & newp,
                                   AimsSurface<3, Void> & mesh,
                                   _PointSet & pts, _VertexMap & vertmap )
  {
    /* cout << ".... planarMeshRaiseBorderPoint " << oldp.node << " "
      << newp.node << " "; */
    oldp.orderNeighbours();
    _Point  *p1 = oldp.rightneigh;
    _Point  *p2 = oldp.leftneigh;
    if( p1 == &newp )
    {
      // cout << p2->node << endl;

      if( newp.leftneigh == p2 || newp.rightneigh == p2 )
      {
        // last triangle in a border front
        planarMeshInsertTriangle( newp, *p2, oldp, mesh, vertmap, pts );
        oldp.sunk = true;
        pts.donepoints.erase( &oldp );
        // cout << "end of border front\n";
        newp.done = true;
        newp.front = oldp.front;
        oldp.front = 0;
        if( newp.front )
        {
          if( newp.front->pleft == &oldp )
            newp.front->pleft = &newp;
          else
            newp.front->pright = &newp;
        }
        pts.donepoints.insert( &newp );
        //planarMeshMergeFronts( newp, *p2 );
        return;
      }

      if( &oldp == &newp || &oldp == p2 || &newp == p2 )
      {
        cerr << "BUG in planarMeshRaiseBorderPoint: identical points\n";
        throw runtime_error( "BUG in planarMeshRaiseBorderPoint: identical "
                             "points" );
      }

      // check if p2 is above newp
      if( !p2->done && p2->pos[1] > newp.pos[1] )
      {
        // then insert a new point: we cannot create a triangle
        // higher than newp
        p2 = planarMeshInsertPoint( oldp, *p2, newp.pos[1], pts );
        p2->done = true;
        pts.donepoints.insert( p2 );
      }
      int ta = planarMeshDirectTriangle( newp, *p2, oldp );
      if( ta >= 0 )
      {
        if( ta > 0 )
          planarMeshInsertTriangle( newp, *p2, oldp, mesh, vertmap, pts );
        if( p2->leftneigh == &oldp )
          p2->leftneigh = &newp;
        else
          p2->rightneigh = &newp;
        if( newp.rightneigh == &oldp )
          newp.rightneigh = p2;
        else
          newp.leftneigh = p2;
        oldp.sunk = true;
        newp.front = oldp.front;
        oldp.front = 0;
        if( newp.front )
        {
          if( newp.front->pleft == &oldp )
            newp.front->pleft = &newp;
          else
            newp.front->pright = &newp;
        }
        pts.donepoints.erase( &oldp );
        bool d = newp.done;
        newp.done = true;
        //planarMeshMergeFronts( newp, *p2 );
        planarMeshTestJunction( *p2, newp, mesh, pts, vertmap );
        newp.done = d;
      }
      // else cout << "...... " << oldp.node << ": inflexion\n";
    }
    else if( p2 == &newp )
    {
      // cout << p1->node << endl;

      if( newp.leftneigh == p1 || newp.rightneigh == p1 )
      {
        // last triangle in a connected component
        planarMeshInsertTriangle( newp, *p1, oldp, mesh, vertmap, pts );
        oldp.sunk = true;
        pts.donepoints.erase( &oldp );
        // cout << "end of border front\n";
        newp.done = true;
        newp.front = oldp.front;
        oldp.front = 0;
        if( newp.front )
        {
          if( newp.front->pleft == &oldp )
            newp.front->pleft = &newp;
          else
            newp.front->pright = &newp;
        }
        pts.donepoints.insert( &newp );
        //planarMeshMergeFronts( newp, *p1 );
        return;
      }

      if( &oldp == &newp || &oldp == p1 || &newp == p1 )
      {
        cerr << "BUG in planarMeshRaiseBorderPoint: identical points\n";
        throw runtime_error( "BUG in planarMeshRaiseBorderPoint: identical "
                             "points" );
      }

      // check if p1 is above newp
      if( !p1->done && p1->pos[1] > newp.pos[1] )
      {
        // then insert a new point: we cannot create a triangle
        // higher than newp
        p1 = planarMeshInsertPoint( oldp, *p1, newp.pos[1], pts );
        p1->done = true;
        pts.donepoints.insert( p1 );
      }
      int ta = planarMeshDirectTriangle( newp, oldp, *p1 );
      if( ta >= 0 )
      {
        if( ta > 0 )
          planarMeshInsertTriangle( newp, *p1, oldp, mesh, vertmap, pts );
        if( p1->leftneigh == &oldp )
          p1->leftneigh = &newp;
        else
          p1->rightneigh = &newp;
        if( newp.rightneigh == &oldp )
          newp.rightneigh = p1;
        else
          newp.leftneigh = p1;
        oldp.sunk = true;
        newp.front = oldp.front;
        oldp.front = 0;
        if( newp.front )
        {
          if( newp.front->pleft == &oldp )
            newp.front->pleft = &newp;
          else
            newp.front->pright = &newp;
        }
        pts.donepoints.erase( &oldp );
        bool d = newp.done;
        newp.done = true;
        //planarMeshMergeFronts( newp, *p2 );
        planarMeshTestJunction( *p1, newp, mesh, pts, vertmap );
        newp.done = d;
      }
      // else cout << "...... " << oldp.node << ": inflexion\n";
    }
    else
    {
      // cout << endl << flush;
      cerr << "  planarMeshRaiseBorderPoint on unconnected point\n";
      cerr << "oldp: " << oldp.node << ", left: " << oldp.leftneigh->node
        << ", right: " << oldp.rightneigh->node << endl;
      cerr << "newp: " << newp.node << ", left: " << newp.leftneigh->node
        << ", right: " << newp.rightneigh->node << endl;
      throw
        runtime_error( "planarMeshRaiseBorderPoint on unconnected point" );
    }
  }


  _Point* planarMeshTestBifurcation( _Point & p, _PointSet & pts )
  {
    // cout << ".. planarMeshTestBifurcation " << flush;
    float x, x1;
    _Point  *p0, *p1, *p2, *p3, *i;
    bool  loop;
    set<_Point *> donep;
    set<_Point *>::iterator  ip, e = pts.donepoints.end(), ed = donep.end();

    for( ip=pts.donepoints.begin(); ip!=e; ++ip )
      if( !(*ip)->sunk && donep.find( *ip ) == ed && (*ip)->inCone( p ) )
      {
        i = *ip;
        // cout << "exam candidate " << i->node << ". " << flush;
        // candidate
        if( !i->leftneigh->done && !i->rightneigh->done )
        {
          // cout << "2 walls\n";
          return i; // no question here
        }
        // find left end in not done list
        p1 = i->leftneigh;
        p2 = &*i;  // nearest point to p (bifurcation target candidate)
        p0 = p2; // loop test
        p3 = p0;  // beginning of last segment
        loop = false;
        x = fabs( p.pos[0] - p2->pos[0] );
        // cout << "left search. x:" << x << " " << flush;
        while( p1->done )
        {
          donep.insert( p1 );
          // cout << "p1:" << p1->node << ". " << flush;
          x1 = fabs( p.pos[0] - p1->pos[0] );
          // cout << "x1:" << x1 << " ";
          if( x1 < x )
          {
            x = x1;
            p2 = p1;  // nearest point on x axis
            // cout << "nearest: " << p2->node << ". ";
          }
          if( p1->leftneigh != p3 )
          {
            p3 = p1;
            p1 = p1->leftneigh;
          }
          else
          {
            p3 = p1;
            p1 = p1->rightneigh;
          }
          if( p1 == p0 )
          {
            loop = true;
            // cout << "planarMeshTestBifurcation: left search loops\n";
            break;
          }
        }
        if( loop ) // loop all done: other connected comp.
          continue;
        // p must be right from this segment
        Point2df  v( p1->pos - p3->pos ), w( p.pos - p3->pos );
        if( w[0] * v[1] - w[1] * v[0] < 0 )
          // if( ( p.pos - p3->pos ).dot( Point2df( -v[1], v[0] ) ) > 0 )
        {
          /*cout << "left end not found: " << p1->node << " out, p3:"
            << p3->node << ").\n"; */
          continue;
        }
        // find right end in not done list
        p1 = i->rightneigh;
        // p2 = &*i;
        p0 = &*i;
        p3 = p0;
        // cout << "right search " << flush;
        while( p1->done )
        {
          donep.insert( p1 );
          x1 = fabs( p.pos[0] - p1->pos[0] );
          if( x1 < x )
          {
            x = x1;
            p2 = p1;  // nearest point on x axis
            // cout << "nearest: " << p2->node << ". ";
          }
          if( p1->rightneigh != p3 )
          {
            p3 = p1;
            p1 = p1->rightneigh;
          }
          else
          {
            p3 = p1;
            p1 = p1->leftneigh;
          }
        }
        // p must be lef from this segment
        v = Point2df( p1->pos - p3->pos );
        if( ( p.pos - p3->pos ).dot( Point2df( -v[1], v[0] ) ) < 0 )
        {
          // cout << "right end not found.\n";
          continue;
        }
        // cout << "found: " << p2->pos << endl;
        return p2;
      }
    // cout << "no possible bifurcation found\n";
    return 0;
  }


  void planarMeshTestJunction( _Point & p, _Point & p1,
                               AimsSurface<3, Void> & mesh, 
                               _PointSet & pts, _VertexMap & vmap )
  {
    if( !p1.done )
      return;
    _Point  *p2 = p.leftneigh;
    if( p2 == &p1 )
      p2 = p.rightneigh;
    if( !p2->done )
      return;
    /* if( p2->pos[1] > p1.pos[1] )
    { cout << "(no junction)\n";
      //throw runtime_error( "should never happen" );
      return;
    } */
    if( planarMeshDirectTriangle( *p.leftneigh, p, *p.rightneigh ) >= 0 )
    {
      planarMeshRaiseBorderPoint( p, p1, mesh, pts, vmap );
    }
  }


  void
  planarMeshInsertNeighbour( _Point & p, _Point & oldp,
                             AimsSurface<3, Void> & mesh, 
                             _PointSet & pts, _VertexMap & vmap )
  {
    if( !oldp.done || p.done || oldp.sunk )
      return;
    _Point  *p2 = oldp.leftneigh;
    if( p2 == &p )
      p2 = oldp.rightneigh;
    if( p2->done && !p2->sunk )
      planarMeshRaiseBorderPoint( oldp, p, mesh, pts, vmap );
  }


  void planarMeshNewBorder( _Point & p, _PointSet & pts )
  {
    p.done = true;
    Point2df  v( p.rightneigh->pos - p.pos ), w( p.leftneigh->pos - p.pos );
    if( v[0] * w[1] - v[1] * w[0] < 0 )
      p.swapNeighBours();
    p.orderNeighbours();
    _Front  front;
    front.pleft = &p;
    //front.pleft2 = 0;
    front.pright = &p;
    //front.pright2 = 0;
    pts.fronts.push_back( front );
    p.front = &pts.fronts.back();
  }


  void planarMeshBifurcation( _Point & p, _Point & p0,
                              AimsSurface<3, Void> & mesh,
                              _PointSet & pts,
                              _VertexMap & vmap )
  {
    /* cout << ".. planarMeshBifurcation " << p0.node << " -> " << p.node
      << endl; */
    Point2df  v( p.rightneigh->pos - p.pos ), w( p.leftneigh->pos - p.pos );
    if( v[0] * w[1] - v[1] * w[0] > 0 )
      p.swapNeighBours();
    // duplicate points p and p0 because they now belong to 2 borders
    pts.unsorted.push_back( _Point() );
    _Point  & p2 = pts.unsorted.back();
    p2.leftneigh = p.leftneigh;
    p2.node = p.node;
    p2.pos = p.pos;
    p2.twin = &p;
    if( p2.leftneigh->rightneigh == &p )
      p2.leftneigh->rightneigh = &p2;
    else
      p2.leftneigh->leftneigh = &p2;
    p.leftneigh = &p0;
    p.twin = &p2;

    pts.unsorted.push_back( _Point() );
    _Point  & p3 = pts.unsorted.back();
    p3.leftneigh = &p2;
    p3.rightneigh = p0.rightneigh;
    p3.node = p0.node;
    p3.pos = p0.pos;
    p3.twin = &p0;
    p3.done = true;
    pts.donepoints.insert( &p3 );
    if( p3.rightneigh->leftneigh == &p0 )
      p3.rightneigh->leftneigh = &p3;
    else
      p3.rightneigh->rightneigh = &p3;
    p0.rightneigh = &p;
    p0.twin = &p3;

    p2.rightneigh = &p3;

    if( p0.front )
    {
      _Front & front1 = *p0.front;
      front1.pright = &p;
      pts.fronts.push_back( _Front() );
      _Front &front2 = pts.fronts.back();
      front2.pleft = &p2;
      front2.pright = &p3;
    }

    /* cout << "p0: " << &p0 << ", p3: " << &p3 << endl;
    cout << ".... insert bifurcation 1\n"; */
    planarMeshRaiseBorderPoint( p0, p, mesh, pts, vmap );
    p.done = true;
    pts.donepoints.insert( &p );
    if( !p0.sunk )
      cerr << "warning in planarMeshBifurcation: p0 not sunk\n";
    // cout << ".... insert bifurcation 2\n";
    planarMeshRaiseBorderPoint( p3, p2, mesh, pts, vmap );
    p2.done = true;
    pts.donepoints.insert( &p2 );
    if( !p3.sunk )
      cerr << "warning in planarMeshBifurcation: p3 not sunk\n";
    // cout << ".... bifurcation done.\n";
  }


  void planarMeshNeighbour( _Point & p, _PointClassification & c,
                            _PointSet & pts, AimsSurface<3, Void> & mesh,
                            _VertexMap & vmap )
  {
    _Point  *p2 = p.leftneigh;
    if( p2 == c.neighbour )
    {
      p2 = p.rightneigh;
      if( c.neighbour->rightneigh != &p )
        p.swapNeighBours();
    }
    else if( c.neighbour->leftneigh != &p )
      p.swapNeighBours();

    planarMeshTestIntersections( p, *c.neighbour );

    /* cout << ".. proces neighbour: " << p.node << ", neigh: "
      << p.leftneigh->node << ", " << p.rightneigh->node << endl; */
    planarMeshInsertNeighbour( p, *c.neighbour, mesh, pts, vmap );
    planarMeshInsertNeighbour( p, *p2, mesh, pts, vmap );
    // check/merge junction
    planarMeshTestJunction( p, *p.leftneigh, mesh, pts, vmap );
    p.done = true;
    pts.donepoints.insert( &p );
  }


  _PointClassification planarMeshClassifyPoint( _Point & p, _PointSet & pts )
  {
    _PointClassification  pc;
    pc.neighbour = 0;
    // cout << "planarMeshClassifyPoint " << p.node << " / " << p.pos << " ";
    if( p.rightneigh && p.rightneigh->done )
    {
      // linked to one or more borders
      // cout << "neighbour(1)\n";
      pc.type = _Neighbour;
      pc.neighbour = p.rightneigh;
      return pc;
    }
    else if( p.leftneigh && p.leftneigh->done )
    {
      // cout << "neighbour(2)\n";
      pc.type = _Neighbour;
      pc.neighbour = p.leftneigh;
      return pc;
    }
    else
    {
      // not directly linked to a border
      _Point  *pb = planarMeshTestBifurcation( p, pts );
      if( pb )
      {
        // cout << "bifurcation\n";
        pc.type = _Bifurcation;
        pc.neighbour = pb;
        return pc;
      }
      // cout << "new border\n";
      pc.type = _NewBorder;
      return pc;
    }
  }


  void planarMeshProcessPoint( _Point & p, _PointSet & pts,
                               AimsSurface<3, Void> & mesh, _VertexMap & vmap )
  {
    _PointClassification c = planarMeshClassifyPoint( p, pts );
    switch( c.type )
    {
    case _Neighbour:
      planarMeshNeighbour( p, c, pts, mesh, vmap );
      break;
    case _Bifurcation:
      planarMeshBifurcation( p, *c.neighbour, mesh, pts, vmap );
      break;
    case _NewBorder:
      planarMeshNewBorder( p, pts );
      pts.donepoints.insert( &p );
      break;
    default:
      break;
    }
  }


  void meshPlane2( const Point4df & plane,
                   const AimsTimeSurface<2, Void> & border,
                   AimsSurfaceTriangle & cmesh, bool = false )
  {
    _PointSet pts;
    planarMeshSortPoints( border.begin()->second, plane, pts );

    AimsSurface<3, Void>  & outmesh = cmesh[0];
    outmesh.vertex().reserve( border.begin()->second.vertex().size() );
    _VertexMap
      vmap( unsigned( border.begin()->second.vertex().size() * 1.5 ) );
    multimap<float, _Point*>::iterator  ip, ep = pts.sorted.end();

    unsigned cnt = 0;
    try
    {
      for( ip=pts.sorted.begin(); ip!=ep /*&& cnt < 10*/; ++ip, ++cnt )
        planarMeshProcessPoint( *ip->second, pts, outmesh, vmap );
    }
    catch( ... )
    {
      unsigned n = outmesh.vertex().size();
      cout << n << " vertices / " << outmesh.polygon().size()
        << " polygons in planar mesh\n";
      vector<Point3df>  & norm = outmesh.normal();
      if( norm.size() < n )
      {
        norm.reserve( n );
        Point3df  nx( -plane[0], -plane[1], -plane[2] );
        nx.normalize();
        norm.insert( norm.end(), n - norm.size(), nx );
      }
      throw;
    }
    cout << cnt << " points processed\n";

    unsigned n = outmesh.vertex().size();
    cout << n << " vertices / " << outmesh.polygon().size()
      << " polygons in planar mesh\n";
    vector<Point3df>  & norm = outmesh.normal();
    if( norm.size() < n )
    {
      norm.reserve( n );
      Point3df  nx( -plane[0], -plane[1], -plane[2] );
      nx.normalize();
      norm.insert( norm.end(), n - norm.size(), nx );
    }
  }

}


namespace
{
  struct ProjPoint
  {
    ProjPoint() : vertex( -1 ), x( 0 ), y( 0 ) {}
    ProjPoint( int v, const Point3df & p, float xp, float yp ) 
      : vertex( v ), pos( p ), x( xp ), y( yp ) {}
    bool operator < ( const ProjPoint & p2 ) const
    {
      return ( y < p2.y ) || ( ( y == p2.y ) && x < p2.x );
    }

    int		vertex;
    Point3df	pos;
    float	x;
    float	y;
  };

  class DirectionalSort
  {
  public:
    bool operator () ( const ProjPoint *x, const ProjPoint *y )
    {
      return (*x) < (*y);
    }
  };

  typedef map<ProjPoint *, vector<uint>, DirectionalSort>	PointMap;

  struct FrenchFrie
  {
    FrenchFrie() 
      : bleft( 0 ), bright( 0 ), tleft( 0 ), tright( 0 ), 
	fleft( 0 ), fright( 0 ) {}

    ProjPoint	*bleft;
    ProjPoint	*bright;
    ProjPoint	*tleft;
    ProjPoint	*tright;
    ProjPoint	*fleft;
    ProjPoint	*fright;
  };

  class itcompare
  {
  public:
    bool operator () ( const list<FrenchFrie>::iterator & x, 
		       const list<FrenchFrie>::iterator & y )
    {
      return &*x < &*y;
    }
  };

  bool checkLine( AimsTimeSurface<2, Void> & lmesh )
  {
    vector<Point3df>			& vert = lmesh.vertex();
    //vector<Point3df>			& norm = lmesh.normal();
    vector<AimsVector<uint, 2> >	& poly = lmesh.polygon();
    unsigned				i, j, n = poly.size();
    uint				p1, p2, p3, p4, ci;
    float				t1, t2, w;
    Point3df				u, v, wp, z1, z2, cp;
    bool				result = true;
    set<uint>				polytodel;
    map<uint,uint>			verttodel;
    map<uint,uint>::iterator		iv2, ev2 = verttodel.end();

    // check each pair of segments for crossings
    for( i=0; i<n; ++i )
      for( j=i+1; j<n; ++j )
	{
	  p1 = poly[i][0];
	  p2 = poly[i][1];
	  p3 = poly[j][0];
	  p4 = poly[j][1];
	  Point3df	& vp1 = vert[p1];
	  Point3df	& vp2 = vert[p2];
	  Point3df	& vp3 = vert[p3];
	  Point3df	& vp4 = vert[p4];
	  if( p3 == p4 )
	    {
	      cerr << "malformed curve: segment with start=end\n";
	      polytodel.insert( j );
	      result = false;
	    }
	  else if( vp3 == vp4 )
	    {
	      cerr << "malformed curve: segment with start pos=end pos\n";
	      polytodel.insert( j );
	      if( verttodel.find( p3 ) == ev2 )
		verttodel[ p4 ] = p3;
	      result = false;
	    }
	  else if( p1 != p3 && p1 != p4 && p2 != p3 && p2 != p4 )
	    {
	      if( vp1 == vp3 )
		{
		  if( verttodel.find( p1 ) == ev2 )
		    verttodel[ p3 ] = p1;
		  result = false;
		}
	      else if( vp1 == vp4 )
		{
		  if( verttodel.find( p1 ) == ev2 )
		    verttodel[ p4 ] = p1;
		  result = false;
		}
	      else if( vp2 == vp3 )
		{
		  if( verttodel.find( p2 ) == ev2 )
		    verttodel[ p3 ] = p2;
		  result = false;
		}
	      else if( vp2 == vp4 )
		{
		  if( verttodel.find( p2 ) == ev2 )
		    verttodel[ p4 ] = p2;
		  result = false;
		}
	      else
		{
		  u = vp2 - vp1;
		  v = vp4 - vp3;
		  // z1 = v^(u^v): orthogonal to v in plane (u,v)
		  wp = Point3df( u[1] * v[2] - u[2]* v[1], 
				 u[2] * v[0] - u[0]* v[2], 
				 u[0] * v[1] - u[1]* v[0] );
		  z1 = Point3df( v[1] * wp[2] - v[2]* wp[1], 
				 v[2] * wp[0] - v[0]* wp[2], 
				 v[0] * wp[1] - v[1]* wp[0] );
		  w = u.dot( z1 );
		  if( w != 0 )
		    {
		      t1 = z1.dot( vp3 - vp1 ) / w;
		      z2 = Point3df( u[1] * wp[2] - u[2]* wp[1], 
				     u[2] * wp[0] - u[0]* wp[2], 
				     u[0] * wp[1] - u[1]* wp[0] );
		      t2 = z2.dot( vp1 - vp3 ) / v.dot( z2 );
		      if( t1 > 0 && t1 < 1 && t2 > 0 && t2 < 1 )
			{
			  result = false;
			  // add crossing point as vertex and 2 new segments
			  cp = vp1 + u * t1;
			  cerr << "crossing segments " << i << " " << j 
			       << ": " << p1 << vp1 << p2 << vp2 << p3 << vp3 
			       << p4 << vp4 << cp << " " << t1 
			       << " " << t2 << " " << vp3 + v * t2 << endl;
			  ci = vert.size();
			  vert.push_back( cp );
			  vert.push_back( cp );
			  poly[i][1] = ci;
			  poly[j][1] = ci+1;
			  poly.push_back( AimsVector<uint, 2>( ci, p2 ) );
			  poly.push_back( AimsVector<uint, 2>( ci+1, p4 ) );
			}
		    }
		}
	    }
	}

    set<uint>::reverse_iterator	ip, ep = polytodel.rend();
    for( ip=polytodel.rbegin(); ip!=ep; ++ip )
      poly.erase( poly.begin() + *ip );
    map<uint,uint>::reverse_iterator	iv, ev = verttodel.rend();
    for( iv=verttodel.rbegin(); iv!=ev; ++iv )
      {
	vert.erase( vert.begin() + iv->first );
	//norm.erase( norm.begin() + iv->first );
      }

    int	k;
    for( k=poly.size()-1; k>=0; --k )
      {
	iv2 = verttodel.find( poly[k][0] );
	if( iv2!=ev2 )
	  poly[k][0] = iv2->second;
	iv2 = verttodel.find( poly[k][1] );
	if( iv2!=ev2 )
	  poly[k][1] = iv2->second;
      }
    /*
    cout << "removed " << polytodel.size() << " segments and " 
	 << verttodel.size() << " vertices\n";
    */

    return result;
  }

  void meshPlane( const Point4df & plane, 
		  AimsTimeSurface<2, Void> & border, 
		  AimsSurfaceTriangle & cmesh, bool checkplane )
  {
    if( checkplane )
      checkLine( border );

    // FrenchFries algorithm, on idea of Yann

    // LOTS of gcc-2.96 compiler bugs in this, I have to decompose...
    PointMap				sorted;
    PointMap::const_iterator		ip, ep = sorted.end();
    const vector<Point3df>		& bvert = border.vertex();
    vector<Point3df>			& cvert = cmesh.vertex();
    vector<Point3df>			& cnorm = cmesh.normal();
    const vector<AimsVector<uint, 2> >	& bpoly = border.polygon();
    vector<AimsVector<uint, 3> >	& cpoly = cmesh.polygon();
    Point3df				n, u, v;

    // local coord system
    n = Point3df( -plane[0], -plane[1], -plane[2] );
    n.normalize();
    if( n[0] != 0 || n[1] != 0 )
      u = Point3df( -n[1], n[0], 0 );
    else
      u = Point3df( 0, -n[2], n[1] );
    u.normalize();
    v = Point3df( n[1] * u[2] - n[2] * u[1], 
		  n[2] * u[0] - n[0] * u[2], 
		  n[0] * u[1] - n[1] * u[0] );
    v.normalize();

    // copy vertices
    int	i, nv = bvert.size();
    cvert = bvert;

    // build sorted list of neighbourhood
    vector<AimsVector<uint, 2> >::const_iterator	ibp, ebp = bpoly.end();
    vector<ProjPoint>					pts( bvert.size() );
    uint						p1, p2;

    for( ibp=bpoly.begin(); ibp!=ebp; ++ibp )
      {
	p1 = (*ibp)[0];
	p2 = (*ibp)[1];
	//cout << "poly: " << p1 << "-" << p2 << " " << flush ;
	if( pts[ p1 ].vertex < 0 )
	  {
	    pts[p1] = ProjPoint( p1, bvert[p1], bvert[p1].dot( u ), 
				 bvert[p1].dot( v ) );
	    //cout << "." << flush;
	  }
	if( pts[ p2 ].vertex < 0 )
	  {
	    pts[p2] = ProjPoint( p2, bvert[p2], bvert[p2].dot( u ), 
				 bvert[p2].dot( v ) );
	    //cout << "." << flush;
	  }
	sorted[ &pts[p1] ].push_back( p2 );
	sorted[ &pts[p2] ].push_back( p1 );
      }

    // build fries tree
    set<uint>				done;
    set<uint>::iterator		notdone = done.end();
    list<FrenchFrie>			fries;
    list<FrenchFrie>::iterator		current, old, fend = fries.end();
    uint				p, tp1, tp2;
    bool				p1done, p2done, triang, flat;
    float				x, y, x1, x2, wt1, wt2, sinus, len;
    list<ProjPoint>			added;
    ProjPoint				*right;
    map<float, uint>			sneigh;
    map<float, uint>::iterator		icur, nend = sneigh.end();
    int					ncount;
    bool	bug = false;

    for( ip=sorted.begin(); ip!=ep && !bug; ++ip )
      {
	p = ip->first->vertex;
	done.insert( p );
	y=x = ip->first->x;
	y = ip->first->y;
	old = fend;

	if( ip->second.size() >= 2 )
	  {
	    p1 = ip->second[0];
	    p2 = ip->second[1];
	    //if( ip->second.size() > 2 )
	    //  cout << "num of neighbours: " << ip->second.size() << endl;
	  }
	else
	  {
	    cerr << "WARNING: open curve, meshing it will fail\n";
	    continue;	// must be handled before
	  }

	// scan lines at y in all fries
	/*cout << "scanning " << fries.size() << " fries for node " << p 
	     << " " << x << " " << y << endl;
	cout << "p1: " << p1 << " " << pts[p1].x << " " << pts[p1].y 
	     << ", p2: " << p2 << " " << pts[p2].x << " " << pts[p2].y 
	     << endl;*/
	for( current=fries.begin(); current!=fend; ++current )
	  {
	    /*cout << "frie: " << current->bleft->vertex << " " 
	      << current->tleft->vertex << " ";
	    if( current->bright )
	      cout <<  current->bright->vertex << " ";
	    else
	      cout << "* ";
	    cout <<  current->tright->vertex << " " 
		 << current->fleft->vertex << " "
		 << current->fright->vertex << endl;*/
	    flat = false;
	    if( current->tleft->y == current->bleft->y )
	      {
		flat = true;
		cout << "x1 flat ";
		/* vector<uint>::const_iterator	en = ip->second.end();
		if( y > current->tleft->y )
		  {
		    cout << "y>ty " << y << " " << current->tleft->y;
		    for( vector<uint>::const_iterator i=ip->second.begin(); 
			 i!=en; ++i )
		      cout << *i << " ";
		    cout << endl;
		  }

		if( find( ip->second.begin(), en, 
			  (uint) current->tleft->vertex ) != en 
		    || find( ip->second.begin(), en, 
			     (uint) current->bleft->vertex ) != en 
		    || find( ip->second.begin(), en, 
			     (uint) current->fleft->vertex ) != en )
			     cout << "linked "; */
		  /*x1 = x;
		  else*/
		x1 = current->tleft->x;
		wt1 = 1;
		/*if( x1 > x )
		  cout << "! ";*/
	      }
	    else
	      {
		wt1 = ( y - current->bleft->y ) 
		  / ( current->tleft->y - current->bleft->y );
		x1 = current->tleft->x * wt1 + current->bleft->x * (1-wt1);
	      }
	    //cout << "scan p:" << p << " x: " << x << " " << x1 << endl;
	    if( x1 <= x )
	      {
		triang = false;
		if( current->bright )
		  right = current->bright;
		else
		  {
		    right = current->bleft;
		    triang = true;
		  }
		if( current->tright->y == right->y )
		  {
		    cout << "x2 flat ";
		    /*vector<uint>::const_iterator	en = ip->second.end();
		      for( vector<uint>::const_iterator i=ip->second.begin(); 
			 i!=en; ++i )
		      cout << *i << " ";
		      cout << endl;*/

		    /*if( find( ip->second.begin(), en, 
			      (uint) current->tright->vertex ) != en 
			|| find( ip->second.begin(), en, 
				 (uint) right->vertex ) != en 
			|| find( ip->second.begin(), en, 
				 (uint) current->fright->vertex ) != en )
		      x2 = x;
		      else*/
		    x2 = current->tright->x;
		    wt2 = 1;
		    flat = true;
		  }
		else
		  {
		    flat = false;
		    wt2 = ( y - right->y ) / ( current->tright->y - right->y );
		    x2 = current->tright->x * wt2 + right->x * (1-wt2);
		  }
		//cout << "x2: " << x2 << endl;
		if( x2 >= x )
		  {
		    // draw/cut frie
		    if( flat) cout << "cut ";
		    tp1 = current->bleft->vertex;
		    tp2 = right->vertex;
		    if( wt1 == 1 )
		      current->bleft = current->tleft;
		    else
		      {
			cvert.push_back( current->tleft->pos * wt1 
					 + current->bleft->pos * (1-wt1) );
			added.push_back
			  ( ProjPoint( cvert.size()-1, cvert[cvert.size()-1], 
				       x1, y ) );
			done.insert( cvert.size()-1 );
			current->bleft = &added.back();
		      }
		    if( wt2 == 1 )
		      current->bright = current->tright;
		    else
		      {
			cvert.push_back( current->tright->pos * wt2 
					 + right->pos * (1-wt2) );
			added.push_back
			  ( ProjPoint( cvert.size()-1, cvert[cvert.size()-1], 
				       x2, y ) );
			done.insert( cvert.size()-1 );
			current->bright = &added.back();
		      }
		    if( !flat )
		      {
			if( !triang )
			  cpoly.push_back( AimsVector<uint,3>
					   ( tp1, tp2, 
					     current->bright->vertex ) );
			cpoly.push_back( AimsVector<uint,3>
					 ( tp1, current->bright->vertex, 
					   current->bleft->vertex ) );
			//cout << "poly\n";
		      }
		    if( old == fend )
		      old = current;
		    else
		      break;
		  }
	      }
	  }

	// sort neighbours by angle
	ncount = ip->second.size();
	sneigh.clear();

	for( i=0; i<ncount; ++i )
	  {
	    p1 = ip->second[i];
	    x1 = pts[p1].x - pts[p].x;
	    x2 = pts[p1].y - pts[p].y;
	    len = 1. / sqrt( x1*x1 + x2*x2 );
	    sinus = ( pts[p1].y - pts[p].y ) * len;
#if defined(__GNUC__) && defined (__sun__)
	    float f = ::acos( ( pts[p1].x - pts[p].x ) * len );
#else
	    float f = acos( ( pts[p1].x - pts[p].x ) * len );
#endif
	    if ( sinus >= 0 )
	      sneigh[ f ] = p1;
	    else
	      sneigh[ -f ] = p1;
	  }

	icur = sneigh.begin();
	while ( icur!=nend )
	  {
	    p1 = icur->second;
	    ++icur;
	    if ( icur == nend )
	      {
		cerr << "meshPlane BUG: odd number of neighbours\n";
		bug = true;
		continue;
	      }
	    p2 = icur->second;
	    ++icur;
	    if ( ncount > 2 )
	      cout << "multiple passes\n";

	    p1done = ( done.find( p1 ) != notdone );
	    p2done = ( done.find( p2 ) != notdone );
	    if ( p1done && p2done )	// end of branch point
	      {
		if ( current == fend )	// real end of branch
		  {
		    //cout << "end branch\n";
		    fries.erase( old );
		  }
		else			// merge
		  {
		    //cout << "merge branches\n";
		    if( old->bleft->x >= current->bright->x )
		      { // WARNING: test insuffisant
			current->bright = old->bright;
			current->tright = old->tright;
			current->fright = old->fright;
		      }
		    else
		      {
			current->bleft = old->bleft;
			current->tleft = old->tleft;
			current->fleft = old->fleft;
		      }
		    fries.erase( old );
		  }
	      }
	    else if( !p1done && !p2done )	// new branch point
	      {
		if( old == fend )
		  {
		    current = fries.insert( fend, FrenchFrie() );
		    /*cout << "new branch " << p << " " << p1 << " " << p2 
		      << " " << &*current << endl;*/
		    current->bleft = &pts[ p ];
		    /*cout << pts[p].x << " " << pts[p1].x << " " << pts[p2].x 
		      << endl;
		      cout << pts[p].y << " " << pts[p1].y << " " << pts[p2].y 
		      << endl;*/
		    sinus = ( pts[p1].y - pts[p].y ) * 
		      ( pts[p2].x - pts[p].x )
		      - ( pts[p1].x - pts[p].x ) * ( pts[p2].y - pts[p].y );
		    //cout << "sinus: " << sinus << endl;
		    if( sinus > 0 
			|| (sinus == 0 && ( pts[p1].x < pts[p2].x ) ) )
		      {
			//cout << "sinus >= 0\n";
			current->tleft = &pts[p1];
			current->tright = &pts[p2];
		      }
		    else
		      {
			current->tleft = &pts[p2];
			current->tright = &pts[p1];
		      }
		    current->fleft = current->bleft;
		    current->fright = current->tright;
		  }
		else		// bifurcation
		  {
		    current = fries.insert( fend, FrenchFrie() );
		    /*cout << "bifurcation " << &pts[ p1 ] << " " 
		      << &pts[ p2 ] << " " << &*current << endl;*/
		    current->bleft = &pts[ p ];
		    current->fleft = current->bleft;
		    float	sinus = ( pts[p1].y - pts[p].y ) * 
		      ( pts[p2].x - pts[p].x )
		      - ( pts[p1].x - pts[p].x ) * ( pts[p2].y - pts[p].y );
		    if( sinus > 0 
			|| (sinus == 0 && ( pts[p1].x < pts[p2].x ) ) )
		      {
			current->tleft = &pts[ p2 ];
			current->tright = old->tright;
			old->tright = &pts[ p1 ];
		      }
		    else
		      {
			current->tleft = &pts[ p1 ];
			current->tright = old->tright;
			old->tright = &pts[ p2 ];
		      }
		    current->bright = old->bright;
		    current->fright = old->fright;
		    old->bright = current->bleft;
		    old->fright = current->bleft;
		    //cout << old->tleft << endl;
		  }
	      }
	    else		// regular border
	      {
		/*cout << "regular " << p << " " << &*old << " " << old->tleft 
		  << endl;*/
		if( old == fend )
		  {
		    cerr << "***BUG*** old=end\n" << flush;
		    bug = true;
		    break;
		  }
		else if( p1done )
		  {
		    /*cout << "p1done " << old->tleft->vertex 
		      << endl;*/
		    if( old->tleft->vertex == (int) p )
		      old->tleft = &pts[p2];
		    else
		      old->tright = &pts[p2];
		  }
		else
		  {
		    /*cout << "p2done " << old->tleft->vertex 
		      << endl;*/
		    if( old->tleft->vertex == (int) p )
		      old->tleft = &pts[p1];
		    else
		      old->tright = &pts[p1];
		  }
	      }
	    if( ncount > 2 )
	      cout << "pass end\n";
	  }
      }

    // copy normals
    cnorm.clear();
    nv = cvert.size();
    cnorm.reserve( nv );
    for( i=0; i<nv; ++i )
      cnorm.push_back( n );

    /*
    cout << "plane mesh vertices : " << cmesh.vertex().size() << endl;
    cout << "           polygons : " << cpoly.size() << endl;
    */
  }

}


void SurfaceManip::cutMesh( const AimsSurfaceTriangle & insurf, 
			    const Point4df & plane, AimsSurfaceTriangle & cut, 
                            AimsTimeSurface<2, Void> & borderline )
{
  vector<const AimsSurfaceTriangle *> insurfl(1);
  insurfl[0] = &insurf;
  vector<rc_ptr<AimsSurfaceTriangle> > outc(1);
  outc[0].reset( &cut );
  cutMesh( insurfl, plane, outc, borderline );
  outc[0].release();
}


void SurfaceManip::cutMesh( const vector<const AimsSurfaceTriangle *> & insurf,
                            const Point4df & plane,
                            vector<rc_ptr<AimsSurfaceTriangle> > & cut,
                            AimsTimeSurface<2, Void> & borderline )
{
  AimsSurfaceTriangle   pmesh;
  cutMesh( insurf, plane, cut, pmesh, borderline, false, false );
}


void SurfaceManip::cutMesh( const AimsSurfaceTriangle & insurf, 
			    const Point4df & plane, AimsSurfaceTriangle & cut, 
			    AimsSurfaceTriangle & pmesh, 
                            bool checkplane )
{
  vector<const AimsSurfaceTriangle *> insurfl(1);
  insurfl[0] = &insurf;
  vector<rc_ptr<AimsSurfaceTriangle> > outc(1);
  outc[0].reset( &cut );
  cutMesh( insurfl, plane, outc, pmesh, checkplane );
  outc[0].release();
}


void SurfaceManip::cutMesh( const vector<const AimsSurfaceTriangle *> & insurf,
                            const Point4df & plane,
                            vector<rc_ptr<AimsSurfaceTriangle> > & cut,
                            AimsSurfaceTriangle & pmesh, 
                            bool checkplane )
{
  AimsTimeSurface<2, Void>      borderline;
  cutMesh( insurf, plane, cut, pmesh, borderline, true, checkplane );
}


void SurfaceManip::cutMesh( const AimsSurfaceTriangle & insurf, 
			    const Point4df & plane, AimsSurfaceTriangle & cut, 
			    AimsSurfaceTriangle & pmesh, 
                            AimsTimeSurface<2, Void> & borderline, 
                            bool checkplane )
{
  vector<const AimsSurfaceTriangle *> insurfl(1);
  insurfl[0] = &insurf;
  vector<rc_ptr<AimsSurfaceTriangle> > outc(1);
  outc[0].reset( &cut );
  cutMesh( insurfl, plane, outc, pmesh, borderline, checkplane );
  outc[0].release();
}


void SurfaceManip::cutMesh( const vector<const AimsSurfaceTriangle *> & insurf,
                            const Point4df & plane,
                            vector<rc_ptr<AimsSurfaceTriangle> > & cut, 
                            AimsSurfaceTriangle & pmesh, 
                            AimsTimeSurface<2, Void> & borderline, 
                            bool checkplane )
{
  cutMesh( insurf, plane, cut, pmesh, borderline, true, checkplane );
}


void SurfaceManip::cutMesh( const AimsSurfaceTriangle & insurf, 
			    const Point4df & plane, AimsSurfaceTriangle & cut, 
			    AimsSurfaceTriangle & pmesh, 
                            AimsTimeSurface<2, Void> & borderline, 
                            bool meshplane, bool checkplane )
{
  vector<const AimsSurfaceTriangle *> insurfl(1);
  vector<rc_ptr<AimsSurfaceTriangle> > outc(1);
  insurfl[0] = &insurf;
  outc[0].reset( &cut );
  cutMesh( insurfl, plane, outc, pmesh, borderline, meshplane, checkplane );
  outc[0].release();
}


void SurfaceManip::cutMesh( const vector<const AimsSurfaceTriangle *> & insurf,
                            const Point4df & plane,
                            vector<rc_ptr<AimsSurfaceTriangle> > & outcut, 
                            AimsSurfaceTriangle & pmesh, 
                            AimsTimeSurface<2, Void> & borderline, 
                            bool meshplane, bool checkplane )
{
  float					p0, p1, p2, wt[3];
  borderline.clear();

  vector<const AimsSurfaceTriangle *>::const_iterator
    iisv, eisv = insurf.end();
  unsigned  count = 0;
  bool  first = true;

  for( iisv=insurf.begin(); iisv!=eisv; ++iisv, ++count )
  {
    rc_ptr<AimsTimeSurface<2, Void> >  bline;
    AimsTimeSurface<2, Void>           *pbline;
    // rc_ptr<AimsSurfaceTriangle>        cmesh;
    AimsSurfaceTriangle                *pcmesh;
    if( outcut.size() < count )
    {
      outcut.push_back( rc_ptr<AimsSurfaceTriangle>
                        ( new AimsSurfaceTriangle ) );
    }
    else if( !outcut[count] )
      outcut[count].reset( new AimsSurfaceTriangle );
    AimsSurfaceTriangle & cut = *outcut[count];
    if( first )
    {
      pbline = &borderline;
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
        p0 = plane[0] * pv0[0] + plane[1] * pv0[1] + plane[2] * pv0[2] 
          + plane[3];
        p1 = plane[0] * pv1[0] + plane[1] * pv1[1] + plane[2] * pv1[2] 
          + plane[3];
        p2 = plane[0] * pv2[0] + plane[1] * pv2[1] + plane[2] * pv2[2] 
          + plane[3];
        if( p0 >= 0 && p1 >= 0 && p2 >= 0 )	// good side: keep
          {
            int	& uv0 = itoo[ pol[0] ];
            if( uv0 < 0 )
              {
                overt.push_back( pv0 );
                onorm.push_back( inorm[ pol[0] ] );
                uv0 = nvo;
                ++nvo;
              }
            int	& uv1 = itoo[ pol[1] ];
            if( uv1 < 0 )
              {
                overt.push_back( pv1 );
                onorm.push_back( inorm[ pol[1] ] );
                uv1 = nvo;
                ++nvo;
              }
            int	& uv2 = itoo[ pol[2] ];
            if( uv2 < 0 )
              {
                overt.push_back( pv2 );
                onorm.push_back( inorm[ pol[2] ] );
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
                int	& uv0 = itoo[ pol[ pos[0] ] ];
                if( uv0 < 0 )
                  {
                    uv0 = (int) nvo;
                    overt.push_back( ivert[ pol[ pos[0] ] ] );
                    onorm.push_back( inorm[ pol[ pos[0] ] ] );
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
                    uv0 = (int) nvo;
                    overt.push_back( ivert[ pol[ pos[0] ] ] );
                    onorm.push_back( inorm[ pol[ pos[0] ] ] );
                    ++nvo;
                  }
                int	& uv1 = itoo[ pol[ pos[1] ] ];
                if( uv1 < 0 )
                  {
                    uv1 = (int) nvo;
                    overt.push_back( ivert[ pol[ pos[1] ] ] );
                    onorm.push_back( inorm[ pol[ pos[1] ] ] );
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
      meshMerge( borderline, *pbline );
    }
  }

  // mesh interior of polyline into pmesh
  if( meshplane )
    meshPlane2( plane, borderline, pmesh, checkplane );
}


void SurfaceManip::cutMesh( const AimsSurfaceTriangle & insurf,
                            const Point4df & plane,
                            AimsTimeSurface<2, Void> & borderline,
                            int timestep )
{
  AimsSurfaceTriangle::const_iterator i = insurf.find( timestep );
  if( i != insurf.end() )
    cutMesh( i->second, plane, borderline );
}


void SurfaceManip::cutMesh( const AimsSurface<3, Void> & insurf,
                            const Point4df & plane,
                            AimsTimeSurface<2, Void> & borderline )
{
  const vector<Point3df> & ivert = insurf.vertex();
  const vector<AimsVector<uint, 3> > & ipoly = insurf.polygon();
  vector<Point3df> & overt = borderline.vertex();
  vector<AimsVector<uint, 2> > & opoly = borderline.polygon();
  vector<AimsVector<uint, 3> >::const_iterator ip, ep = ipoly.end();
  float p0, p1, p2, p01, p02, scal, x;
  Point3df norm = Point3df( plane[0], plane[1], plane[2] );
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
    p0 = plane[0] * pv0[0] + plane[1] * pv0[1] + plane[2] * pv0[2] + plane[3];
    p1 = plane[0] * pv1[0] + plane[1] * pv1[1] + plane[2] * pv1[2] + plane[3];
    p2 = plane[0] * pv2[0] + plane[1] * pv2[1] + plane[2] * pv2[2] + plane[3];

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
          x = - ( plane[3] + pv0.dot( norm ) ) / scal; // position of intersection point
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
          x = - ( plane[3] + pv0.dot( norm ) ) / scal; // position of intersection point
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
          x = - ( plane[3] + pv1.dot( norm ) ) / scal; // position of intersection point
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


bool SurfaceManip::checkMesh( const AimsSurfaceTriangle & insurf, 
			      AimsSurfaceTriangle *outsurf )
{
  const vector<Point3df>		& ivert = insurf.vertex();
  const vector<Point3df>		& inorm = insurf.normal();
  const vector<AimsVector<uint, 3> >	& ipoly = insurf.polygon();
  unsigned				i, j, np = ipoly.size();
  Point3df				na, nb, nl, ua1, ua2, ua3;
  Point3df				ub1, ub2, ub3, pca[3], pcb[3];
  Point3df				nca[3], ncb[3];
  set<unsigned>				ptodel;
  float					x, ta[3], tb[3], y1, y2, y3;
  bool					cuta[3], cutb[3], weaka[3], weakb[3];
  int					ia1, ia2, ib1, ib2;
  float					amin, amax, bmin, bmax;
  bool 					clean = true, copied = false, done;
  const float				eps = 1e-6, oneeps = 1-1e-6, 
    eps2 = -1e-6, oneeps2 = 1+1e-6;

  for( i=0; i<np; ++i )
    {
      cout << i << endl;
      const AimsVector<uint, 3>	& pola = ipoly[i];
      const Point3df			& sa1 = ivert[ pola[0] ];
      const Point3df			& sa2 = ivert[ pola[1] ];
      const Point3df			& sa3 = ivert[ pola[2] ];
      ua1 = sa2 - sa1;
      ua2 = sa3 - sa2;
      ua3 = sa1 - sa3;
      na = Point3df( ua1[1] * ua2[2] - ua1[2] * ua2[1], 
		     ua1[2] * ua2[0] - ua1[0] * ua2[2], 
		     ua1[0] * ua2[1] - ua1[1] * ua2[0] ); // normal
      x = na.dot( sa1 );
      if( na == Point3df( 0, 0, 0 ) )
	{
	  //cout << "flat 1: " << ua1 << " " << ua2 << "\n";
	  ptodel.insert( i );	// flat triangle
	  clean = false;
	  if( !outsurf )
	    return false;
	}
      else
	for( j=i+1; j<np; ++j )
	  {
	    const AimsVector<uint, 3>	& polb = ipoly[j];
	    const Point3df		& sb1 = ivert[ polb[0] ];
	    const Point3df		& sb2 = ivert[ polb[1] ];
	    const Point3df		& sb3 = ivert[ polb[2] ];

	    /* make a fast check of "non-intersection" before doing 
	       complex things: if all vertices of a triangle are on the 
	       same side of the plane of the other triangle, there is 
	       no intersection */
	    y1 = na.dot( sb1 ) - x;
	    y2 = na.dot( sb2 ) - x;
	    y3 = na.dot( sb3 ) - x;
	    done = false;
	    if( !( y1 >= 0 && y2 >= 0 && y3 >= 0 ) 
		&& !( y1 <= 0 && y2 <= 0 && y3 <= 0 ) )
	      {
		ub1 = sb2 - sb1;
		ub2 = sb3 - sb2;
		ub3 = sb1 - sb3;
		nb = Point3df( ub1[1] * ub2[2] - ub1[2] * ub2[1], 
			       ub1[2] * ub2[0] - ub1[0] * ub2[2], 
			       ub1[0] * ub2[1] - ub1[1] * ub2[0] ); // normal

		if( nb == Point3df( 0, 0, 0 ) )
		  {
		    //cout << "flat 2\n";
		    ptodel.insert( j );	// flat triangle
		  }
		else
		  {
		    y3 = nb.dot( sb1 );
		    y1 = nb.dot( sa1 ) - y3;
		    y2 = nb.dot( sa2 ) - y3;
		    y3 = nb.dot( sa3 ) - y3;
		    if( !( y1 >= 0 && y2 >= 0 && y3 >= 0 ) 
			&& !( y1 <= 0 && y2 <= 0 && y3 <= 0 ) )
		      {
			// direction of intersection
			nl = Point3df( na[1] * nb[2] - na[2] * nb[1], 
				       na[2] * nb[0] - na[0] * nb[2], 
				       na[0] * nb[1] - na[1] * nb[0] );
			cuta[0] = false;
			cuta[1] = false;
			cuta[2] = false;
			cutb[0] = false;
			cutb[1] = false;
			cutb[2] = false;
			ia1 = ia2 = ib1 = ib2 = -1;

			// intersection with pola
			x = nb.dot( ua1 );
			if( x != 0 )
			  {
			    x = nb.dot( sb1 - sa1 ) / x;
			    if( x > eps2 && x < oneeps2 )
			      {	// plane of polb intersects [sa1 sa2]
				cuta[0] = true;
				pca[0] = sa1 * (1-x) + sa2 * x;
				nca[0] = inorm[pola[0]] * (1-x) 
				  + inorm[pola[1]] * x;
				// coord on intersect line
				ta[0] = nl.dot( pca[0] );
				ia1 = 0;
				if( x > eps && x < oneeps )
				  weaka[0] = false;
				else
				  weaka[0] = true;
				//cout << "cutA 1: x=" << x << "\n";
			      }
			  }
			x = nb.dot( ua2 );
			if( x != 0 )
			  {
			    x = nb.dot( sb1 - sa2 ) / x;
			    if( x > eps2 && x < oneeps2 )
			      {	// plane of polb intersects [sa2 sa3]
				cuta[1] = true;
				pca[1] = sa2 * (1-x) + sa3 * x;
				nca[1] = inorm[pola[1]] * (1-x) 
				  + inorm[pola[2]] * x;
				ta[1] = nl.dot( pca[1] );
				if( ia1 >= 0 )
				  ia2 = 1;
				else
				  ia1 = 1;
				if( x > eps && x < oneeps )
				  weaka[1] = false;
				else
				  {
				    weaka[1] = true;
				    if( ia2 >= 0 && weaka[ia1] )
				      ia2 = -1;
				  }
				/*cout << "cutA 2 " << ia1 << " " << ia2 
				  << ": x=" << x << "\n";*/
			      }
			  }
			if( ia2 < 0 && ia1 >= 0 )
			  {
			    x = nb.dot( ua3 );
			    if( x != 0 )
			      {
				x = nb.dot( sb1 - sa3 ) / x;
				if( x > eps2 && x < oneeps2 )
				  {	// plane of polb intersects [sa3 sa1]
				    ta[2] = x;
				    cuta[2] = true;
				    pca[2] = sa3 * (1-x) + sa1 * x;
				    nca[2] = inorm[pola[2]] * (1-x) 
				      + inorm[pola[0]] * x;
				    ta[2] = nl.dot( pca[2] );
				    ia2 = 2;
				    if( x > eps && x < oneeps )
				      weaka[2] = false;
				    else
				      weaka[2] = true;
				    //cout << "cutA 3: x=" << x << "\n";
				  }
			      }
			  }

			if( ia1 >= 0 && ( !weaka[ia1] || !weaka[ia2] ) )
			  {
			    // intersection with triangle polb
			    x = nb.dot( ub1 );
			    if( x != 0 )
			      {
				x = nb.dot( sa1 - sb1 ) / x;
				if( x > eps2 && x < oneeps2 )
				  {	// plane of pola intersects [sb1 sb2]
				    cutb[0] = true;
				    pcb[0] = sb1 * (1-x) + sb2 * x;
				    ncb[0] = inorm[polb[0]] * (1-x) 
				      + inorm[polb[1]] * x;
				    tb[0] = nl.dot( pcb[0] );
				    ib1 = 0;
				    if( x > eps && x < oneeps )
				      weakb[0] = false;
				    else
				      weakb[0] = true;
				  }
			      }
			    x = nb.dot( ub2 );
			    if( x != 0 )
			      {
				x = nb.dot( sa1 - sb2 ) / x;
				if( x > eps2 && x < oneeps2 )
				  {	// plane of pola intersects [sb2 sb3]
				    cutb[1] = true;
				    pcb[1] = sb2 * (1-x) + sb3 * x;
				    ncb[1] = inorm[polb[1]] * (1-x) 
				      + inorm[polb[2]] * x;
				    tb[1] = nl.dot( pcb[1] );
				    if( ib1 >= 0 )
				      ib2 = 1;
				    else
				      ib1 = 1;
				    if( x > eps && x < oneeps )
				      weakb[1] = false;
				    else
				      {
					weakb[1] = true;
					if( ib2 >= 0 && weakb[ib1] )
					  ib2 = -1;
				      }
				  }
			      }
			    if( ib2 < 0 )
			      {
				x = nb.dot( ub3 );
				if( x != 0 )
				  {
				    x = nb.dot( sa1 - sb3 ) / x;
				    if( x > eps2 && x < oneeps2 )
				      {	// plane of pola intersects [sb3 sb1]
					cutb[2] = true;
					pcb[2] = sb3 * (1-x) + sb1 * x;
					ncb[2] = inorm[polb[2]] * (1-x) 
					  + inorm[polb[0]] * x;
					tb[2] = nl.dot( pcb[2] );
					ib2 = 2;
					if( x > eps && x < oneeps )
					  weakb[2] = false;
					else
					  weakb[2] = true;
				      }
				  }
			      }

			    // check intersection between triangles
			    // (compare coords on n line axis)
			    if( ib1 >= 0 && ( !weakb[ib1] || !weakb[ib2] ) )
			      {
				if( ta[ia1] < ta[ia2] )
				  {
				    amin = ta[ia1];
				    amax = ta[ia2];
				  }
				else
				  {
				    amin = ta[ia2];
				    amax = ta[ia1];
				  }
				if( tb[ib1] < tb[ib2] )
				  {
				    bmin = tb[ib1];
				    bmax = tb[ib2];
				  }
				else
				  {
				    bmin = tb[ib2];
				    bmax = tb[ib1];
				  }
				if( amin < bmax && amax > bmin )
				  {
				    // INTERSECT
				    cout << "intersection\n";
				    clean = false;
				    if( !outsurf )
				      return false;
				    if( !copied )
				      {
					*outsurf = insurf;
					copied = true;
				      }

				    vector<Point3df>	& overt 
				      = outsurf->vertex();
				    vector<Point3df>	& onorm 
				      = outsurf->normal();
				    vector<AimsVector<uint, 3> > 
				      & opoly = outsurf->polygon();
				    unsigned	vn = overt.size();

				    // insert vertices & normals
				    // (we should check for duplicate 
				    // positions)
				    cout << ia1 << " " << ia2 << " " << ib1 
					 << " " << ib2 << endl;
				    if( ia1 < 0 || ia2 < 0 || ib1 < 0 
					|| ib2 < 0 )
				      cerr << "BUG\n";
				    else
				      {
					cout << pca[ia1] << endl;
					cout << pca[ia2] << endl;
					cout << pcb[ib1] << endl;
					cout << pcb[ib2] << endl;
				    overt.push_back( pca[ia1] );
				    overt.push_back( pca[ia2] );
				    overt.push_back( pcb[ib1] );
				    overt.push_back( pcb[ib2] );
				    nca[ia1].normalize();
				    nca[ia2].normalize();
				    ncb[ib1].normalize();
				    ncb[ib2].normalize();
				    onorm.push_back( nca[ia1] );
				    onorm.push_back( nca[ia2] );
				    onorm.push_back( ncb[ib1] );
				    onorm.push_back( ncb[ib2] );
				    // modify/insert triangles
				    if( !cuta[0] )
				      {
					opoly.push_back( AimsVector<uint,3>
							 ( pola[0], pola[1], 
							   vn ) );
					opoly.push_back( AimsVector<uint,3>
							 ( pola[1], vn+1, 
							   vn ) );
					opoly[i] = AimsVector<uint,3>
					  ( vn, vn+1, pola[2] );
				      }
				    else if( !cuta[1] )
				      {
					opoly.push_back( AimsVector<uint,3>
							 ( pola[1], pola[2], 
							   vn ) );
					opoly.push_back( AimsVector<uint,3>
							 ( pola[2], vn+1, 
							   vn ) );
					opoly[i] = AimsVector<uint,3>
					  ( pola[0], vn, vn+1 );
				      }
				    else
				      {
					opoly.push_back( AimsVector<uint,3>
							 ( pola[2], pola[0], 
							   vn ) );
					opoly.push_back( AimsVector<uint,3>
							 ( pola[0], vn+1, 
							   vn ) );
					opoly[i] = AimsVector<uint,3>
					  ( vn, pola[1], vn+1 );
				      }
				    if( !cutb[0] )
				      {
					opoly.push_back( AimsVector<uint,3>
							 ( polb[0], polb[1], 
							   vn+2 ) );
					opoly.push_back( AimsVector<uint,3>
							 ( polb[1], vn+3, 
							   vn+2 ) );
					opoly[j] = AimsVector<uint,3>
					  ( vn+2, vn+3, polb[2] );
				      }
				    else if( !cutb[1] )
				      {
					opoly.push_back( AimsVector<uint,3>
							 ( polb[1], polb[2], 
							   vn+2 ) );
					opoly.push_back( AimsVector<uint,3>
							 ( polb[2], vn+3, 
							   vn+2 ) );
					opoly[j] = AimsVector<uint,3>
					  ( polb[0], vn+2, vn+3 );
				      }
				    else
				      {
					opoly.push_back( AimsVector<uint,3>
							 ( polb[2], polb[0], 
							   vn+2 ) );
					opoly.push_back( AimsVector<uint,3>
							 ( polb[0], vn+3, 
							   vn+2 ) );
					opoly[j] = AimsVector<uint,3>
					  ( vn+2, polb[1], vn+3 );
				      }
				  }
				  }
			      }
			  }
		      }
		  }
	      }
	  }
    }

  cout << "flat polygons: " << ptodel.size() << endl;
  if( outsurf && !ptodel.empty() )
    {
      if( !copied )
	{
	  *outsurf = insurf;
	  copied = true;
	}

      set<unsigned>::reverse_iterator	ip, ep = ptodel.rend();
      vector<AimsVector<uint, 3> >	& opoly = outsurf->polygon();

      for( ip=ptodel.rbegin(); ip!=ep; ++ip )
	opoly.erase( opoly.begin() + *ip );
    }

  return clean;
}


void SurfaceManip::meshPlanarPolygon( const Point4df & plane,
                                      const AimsTimeSurface<2, Void> & border,
                                      AimsSurfaceTriangle & outmesh )
{
  meshPlane2( plane, border, outmesh );
}


float SurfaceManip::meshArea( const AimsSurfaceTriangle & surf )
{
  return meshArea( surf.begin()->second );
}


float SurfaceManip::meshArea( const AimsSurface<3, Void> & surf )
{
  const vector<Point3df>		& vert = surf.vertex();
  const vector<AimsVector<uint, 3> >	& poly = surf.polygon();
  unsigned				ip, np = poly.size();
  float					area = 0;
  Point3df				v, n, ca, cb;

  for( ip=0; ip<np; ++ip )
    {
      const Point3df  & p1 = vert[ poly[ip][0] ];
      const Point3df  & p2 = vert[ poly[ip][1] ];
      const Point3df  & p3 = vert[ poly[ip][2] ];
      ca = p1 - p3;
      cb = p2 - p3;
      // normal to plane
      n = Point3df( ca[1] * cb[2] - ca[2] * cb[1], 
		    ca[2] * cb[0] - ca[0] * cb[2], 
		    ca[0] * cb[1] - ca[1] * cb[0] );
      // orthogonal to ca in plane
      v = Point3df( ca[1] * n[2] - ca[2] * n[1], 
		    ca[2] * n[0] - ca[0] * n[2], 
		    ca[0] * n[1] - ca[1] * n[0] );
      if( !v.isNull() )
	{
	  v /= v.norm();
          area += ca.norm() * fabs( cb.dot( v ) ) * 0.5;
        }
    }
  return area;
}


float SurfaceManip::meshVolume( const AimsSurfaceTriangle & surf )
{
  return meshVolume( surf.begin()->second );
}


float SurfaceManip::meshVolume( const AimsSurface<3, Void> & surf )
{
  const vector<Point3df>                & vert = surf.vertex();
  const vector<AimsVector<uint, 3> >    & poly = surf.polygon();
  unsigned                              ip, np = poly.size();
  float                                 volume = 0;
  Point3df                              ca, cb;
  Point3df                              x( 0, 0, 1 ); // any direction is OK
  Point3df                              q1, q2, q3;
  float                                 h, A, B, C, BC, D;

  for( ip=0; ip<np; ++ip )
  {
    const Point3df  & p1 = vert[ poly[ip][0] ];
    const Point3df  & p2 = vert[ poly[ip][1] ];
    const Point3df  & p3 = vert[ poly[ip][2] ];
    // project triangle to plane normal to x
    A = x.dot( p1 );
    B = x.dot( p2 );
    C = x.dot( p3 );

    // A,B,C can be either positive or negative and can lead to
    // "negative" volumes according to the orientation and origin
    q1 = p1 - x * A;
    q2 = p2 - x * B;
    q3 = p3 - x * C;

    // (q1, q2, q3) is the projected triangle
    ca = q3 - q1;
    cb = q3 - q2;
    BC = cb.norm();

    if ( BC != 0 )
    {
      cb.normalize();
      // projected triangle height
      h = ( ca - cb * cb.dot( ca ) ).norm();
  
      D = ( B + C ) / 2;
      // volume between original triangle and its projection
      // the sign compares the normal to the triangle and x
      volume += ( crossed( ca, cb ).dot( x ) > 0 ? 1 : -1 )
          * BC * h * ( ( D - A ) / 3 + A/2 );
      // (obtained by the integral along h of the trapezium orthogonal to h)
    }
  }

  return volume;
}


Point3df SurfaceManip::nearestPointToMesh( const Point3df & target, 
                                 const AimsSurfaceTriangle & mesh, 
                                 unsigned nPt, unsigned* index )
{
  const vector<Point3df>        & vert = mesh.vertex();
  unsigned			n = vert.size();
  unsigned			i,v = 0;
  float				d,dmin = numeric_limits<float>::max(); 
  Point3df			v1, barycenter;
  map<float,unsigned> 		distance;
  map<float,unsigned>::iterator 	id, ed;
 
  for( i=0; i<n; ++i )
  {
    v1 = vert[i] - target;
    d = sqrt( v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2] );
    distance[d] = i;
  }

  /* cout << "Closest point to the target: " << (distance.begin()) -> second 
     << " at " << (distance.begin()) -> first << " mm\n";
  */

  i = 0;
  v1 = Point3df(0,0,0);
  for( id = distance.begin(), ed = distance.end(); id != ed && i < nPt; 
       ++id, ++i )
    barycenter += vert[id->second];

  barycenter[0] /= nPt;
  barycenter[1] /= nPt;
  barycenter[2] /= nPt;

  for( i=0; i<n; ++i )
  {
    v1 = vert[i] - target;
    d = sqrt( v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2] );
    distance[d] = i;

    if( d < dmin )
    {
      dmin = d;
      v = i;
    }
  }

  if( index )
    *index = v;

  //cout << "Closest point to the barycenter: " << v << " at  "
  //     << dmin << " mm\n";

  return vert[v];
}


namespace
{

  inline
  void split_tri_1( vector<AimsVector<uint, 3> > &opoly,
                    AimsVector<uint, 3> & op,
                    const AimsVector<uint, 3> & p,
                    uint a, uint b, uint c, bool hasa, bool hasb, bool hasc )
  {
    // triangle cut on 1 edge
    /*       p[0]
              /\
              / |\
            /  | \
            /   |  \
          /___ |___\
      p[1]     b   p[2]
    */
    const uint *p0 , *p1, *p2, *p3;
    if( hasa )
    {
      p0 = &p[2];
      p1 = &p[0];
      p2 = &a;
      p3 = &p[1];
    }
    else if( hasb )
    {
      p0 = &p[0];
      p1 = &p[1];
      p2 = &b;
      p3 = &p[2];
    }
    else // hasc
    {
      p0 = &p[1];
      p1 = &p[2];
      p2 = &c;
      p3 = &p[0];
    }
    {
      op[0] = *p0;
      op[1] = *p1;
      op[2] = *p2;
      opoly.push_back( op );
    }
    {
      op[0] = *p0;
      op[1] = *p2;
      op[2] = *p3;
      opoly.push_back( op );
    }
  }


  inline
  void split_tri_2( vector<AimsVector<uint, 3> > &opoly,
                    AimsVector<uint, 3> & op,
                    const AimsVector<uint, 3> & p,
                    uint a, uint b, uint c, bool hasa, bool hasb, bool hasc )
  {
    // triangle cut on 2 edges
    /*       p[0]
              /\
              /  \
          a  ------  c
            /\____ \
          /______\_\
      p[1]         p[2]
    */
    const uint *p0 , *p1, *p2, *p3, *p4;
    if( !hasa )
    {
      p0 = &p[2];
      p1 = &c;
      p2 = &p[0];
      p3 = &p[1];
      p4 = &b;
    }
    else if( !hasb )
    {
      p0 = &p[0];
      p1 = &a;
      p2 = &p[1];
      p3 = &p[2];
      p4 = &c;
    }
    else // !hasc
    {
      p0 = &p[1];
      p1 = &b;
      p2 = &p[2];
      p3 = &p[0];
      p4 = &a;
    }
    {
      op[0] = *p0;
      op[1] = *p1;
      op[2] = *p4;
      opoly.push_back( op );
    }
    {
      op[0] = *p1;
      op[1] = *p2;
      op[2] = *p3;
      opoly.push_back( op );
    }
    {
      op[0] = *p3;
      op[1] = *p4;
      op[2] = *p1;
      opoly.push_back( op );
    }
  }


  inline
  void split_tri_3( vector<AimsVector<uint, 3> > &opoly,
                    AimsVector<uint, 3> & op,
                    const AimsVector<uint, 3> & p,
                    uint a, uint b, uint c, bool hasa, bool hasb, bool hasc )
  {
    // triangle split "normally" with 3 edges cut
    /*       p[0]
              /\
              /  \
          a  ------  c
            / \  / \
          /___\/___\
      p[1]    b    p[2]
    */
    {
      op[0] = p[0];
      op[1] = a;
      op[2] = c;
      opoly.push_back( op );
    }
    {
      op[0] = a;
      op[1] = p[1];
      op[2] = b;
      opoly.push_back( op );
    }
    {
      op[0] = b;
      op[1] = c;
      op[2] = a;
      opoly.push_back( op );
    }
    {
      op[0] = b;
      op[1] = p[2];
      op[2] = c;
      opoly.push_back( op );
    }
  }

}


AimsSurfaceTriangle* SurfaceManip::refineMeshTri4(
  const AimsSurfaceTriangle & mesh, const std::vector<uint> & selectedPolygons )
{
  AimsSurfaceTriangle *nmesh = new AimsSurfaceTriangle;

  AimsSurfaceTriangle::const_iterator imesh, em = mesh.end();
  for( imesh=mesh.begin(); imesh!=em; ++imesh )
  {
    const AimsSurface<3,Void> & surf = imesh->second;
    AimsSurface<3,Void> & nsurf = (*nmesh)[imesh->first];
    const vector<Point3df> & ivert = surf.vertex();
    const vector<Point3df> & inorm = surf.normal();
    const vector<AimsVector<uint, 3> > & ipoly = surf.polygon();
    vector<Point3df> & overt = nsurf.vertex();
    vector<Point3df> & onorm = nsurf.normal();
    vector<AimsVector<uint, 3> > & opoly = nsurf.polygon();
    bool hasnorm = !inorm.empty();

    /* at this moment we do not know the exact final number of polygons:
       max is ipoly.size() * 4 (if every poly is split in 4)
       but some may be partially split.
    */
    // reserve 4 times the number of triangles
    opoly.reserve( ipoly.size() * 4 );
    // build the list of edges and new vertices indices
    map<pair<uint, uint>, uint > edges;
    map<pair<uint, uint>, uint >::iterator ie, ee = edges.end();
    vector<AimsVector<uint, 3> >::const_iterator ip, ep = ipoly.end();
    uint vid = ivert.size(); // index of current new vertex
    vector<uint>::const_iterator isel = selectedPolygons.begin(),
      esel = selectedPolygons.end();
    bool do_select = !selectedPolygons.empty();
    size_t i = 0;
    for( ip=ipoly.begin(); ip!=ep; ++ip, ++i )
    {
      if( do_select && *isel != i )
        continue; // skip this unselected polygon
      const AimsVector<uint, 3> & p = *ip;
      // identify edge with sorted vert indices
      pair<uint, uint> e( min( p[0], p[1] ), max( p[0], p[1] ) );
      if( edges.find( e ) == ee )
        edges[ e ] = vid++;
      e = make_pair( min( p[1], p[2] ), max( p[1], p[2] ) );
      if( edges.find( e ) == ee )
        edges[ e ] = vid++;
      e = make_pair( min( p[2], p[0] ), max( p[2], p[0] ) );
      if( edges.find( e ) == ee )
        edges[ e ] = vid++;
      if( do_select )
      {
        ++isel;
        if( isel == esel )
          break;
      }
    }
    // reserve final vertices/normals space
    overt.reserve( vid );
    overt.insert( overt.begin(), ivert.begin(), ivert.end() );
    overt.insert( overt.end(), vid - ivert.size(), Point3df() );
    if( hasnorm )
    {
      onorm.reserve( vid );
      onorm.insert( onorm.begin(), inorm.begin(), inorm.end() );
      onorm.insert( onorm.end(), vid - inorm.size(), Point3df() );
    }
    // insert new vertices
    for( ie=edges.begin(); ie!=ee; ++ie )
    {
      // at the middle of existing edges
      overt[ ie->second ] = ( ivert[ ie->first.first ]
        + ivert[ ie->first.second ] ) * 0.5;
      if( hasnorm )
      {
        onorm[ ie->second ] = ( inorm[ ie->first.first ]
          + inorm[ ie->first.second ] ) * 0.5;
        onorm[ ie->second ].normalize();
      }
    }
    // remake triangles
    uint a, b, c;
    bool hasa, hasb, hasc;
    int nedges;
    isel = selectedPolygons.begin();
    AimsVector<uint, 3> op;
    for( ip=ipoly.begin(), i=0; ip!=ep; ++ip, ++i )
    {
      const AimsVector<uint, 3> & p = *ip;
      // get triangle edges and mid-vertices
      nedges = 0;
      hasa = false;
      hasb = false;
      hasc = false;
      pair<uint, uint> e( min( p[0], p[1] ), max( p[0], p[1] ) );
      ie = edges.find( e );
      if( ie != ee )
      {
        ++nedges;
        a = ie->second;
        hasa = true;
      }
      e = make_pair( min( p[1], p[2] ), max( p[1], p[2] ) );
      ie = edges.find( e );
      if( ie != ee )
      {
        b = ie->second;
        ++nedges;
        hasb = true;
      }
      e = make_pair( min( p[2], p[0] ), max( p[2], p[0] ) );
      ie = edges.find( e );
      if( ie != ee )
      {
        c = ie->second;
        ++nedges;
        hasc = true;
      }

      switch( nedges )
      {
      case 3:
        // triangle split "normally" with 3 edges cut
        /*       p[0]
                  /\
                  /  \
              a  ------  c
                / \  / \
              /___\/___\
          p[1]    b    p[2]
        */
        split_tri_3( opoly, op, p, a, b, c, hasa, hasb, hasc );
        break;

      case 2:
        // triangle cut on 2 edges
        /*       p[0]
                  /\
                 /  \
             a  ------  c
               /\____ \
              /______\_\
          p[1]         p[2]
        */
        split_tri_2( opoly, op, p, a, b, c, hasa, hasb, hasc );
        break;

      case 1:
        // triangle cut on 1 edge
        /*       p[0]
                  /\
                 / |\
                /  | \
               /   |  \
              /___ |___\
          p[1]     b   p[2]
        */
        split_tri_1( opoly, op, p, a, b, c, hasa, hasb, hasc );
        break;

      default: // nedges == 0
        // keep this unselected polygon as is
        opoly.push_back( *ip );
        break;
      }

    }
  }

  return nmesh;
}

//--------------------------------------------------------------
bool SurfaceManip::checkMeshIntersect( const AimsSurfaceTriangle & surf1,
                                       const AimsSurfaceTriangle & surf2 )
{
	const float eps = 1e-6;

	// Get vertices, polygons and normals from both surfaces
    const vector<Point3df> & vert1 = surf1.vertex();
    const vector<AimsVector<uint, 3> > & poly1 = surf1.polygon();
    const vector<Point3df> & vert2 = surf2.vertex();
    const vector<Point3df> & norm2 = surf2.normal();
    const vector<AimsVector<uint, 3> > & poly2 = surf2.polygon();

    vector<AimsVector<uint, 3> >::const_iterator it1;
    vector<AimsVector<uint, 3> >::const_iterator it1End = poly1.end();
    vector<AimsVector<uint, 3> >::const_iterator it2;
    vector<AimsVector<uint, 3> >::const_iterator it2End = poly2.end();
    for ( it1 = poly1.begin(); it1 != it1End; ++it1 )
    {
    	// Get the first two points of the current triangle of surf1
        const Point3df & p11 = vert1[ (*it1)[0] ];
        const Point3df & p12 = vert1[ (*it1)[1] ];
        Point3df p11p12 = p12 - p11;

        for ( it2 = poly2.begin(); it2 != it2End; ++it2 )
        {
        	// Get the points of the current triangle of surf2
			const Point3df & p21 = vert2[ (*it2)[0] ];
			const Point3df & p22 = vert2[ (*it2)[1] ];
			const Point3df & p23 = vert2[ (*it2)[2] ];

			// Get the p21 normal
			Point3df p21Norm = norm2[ (*it2)[0] ];
			if ( p21Norm == Point3df( 0., 0., 0. ) )
			{
				// The triangle is a segment or a point...
				continue;
			}

			Point3df p21p11 = p11 - p21;
			float a = -1 * p21Norm.dot( p21p11 );
			float b = p21Norm.dot( p11p12 );
			if ( fabs( b ) < eps )
			{
				// The p11p12 ray is parallel to the triangle plane
				if ( a == 0. )
				{
					// The ray is on the triangle plane => intersection
					return true;
				}
				else
				{
					// The ray is not on the triangle plane
					continue;
				}
			}

			float r = a/b;
			if ( r < 0. || r > 1. )
			{
				// r < 0 => The ray direction is not the good one...
				// r > 1 => The segment does not intersect
				continue;
			}

			// Get the intersection (ray X triangle plane) point
			Point3df interPt = p11 + r * p11p12;

			// Check if the intersection point is inside the triangle
			Point3df u = p22 - p21;
			Point3df v = p23 - p21;
			Point3df w = interPt - p21;
			float uu, uv, vv, wu, wv, D;
			uu = u.dot( u );
			uv = u.dot( v );
			vv = v.dot( v );
			wu = w.dot( u );
			wv = w.dot( v );
			D = uv * uv - uu * vv;

			float s = ( uv * wv - vv * wu ) / D;
			if ( s < 0. || s > 1. )
			{
				// The intersection point is outside the triangle
				continue;
			}
			float t = ( uv * wu - uu *wv ) / D;
			if ( t < 0. || ( s + t ) > 1. )
			{
				// The intersection point is outside the triangle
				continue;
			}

			// Intersection !
			return true;
        }
    }

    // No intersection has been found
    return false;
}


vector<Point3df>* SurfaceManip::lineDirections(
  const AimsTimeSurface<2, Void> & mesh )
{
  vector<int> counts( mesh.vertex().size(), 0 );
  vector<Point3df> *dirs = new vector<Point3df>( mesh.vertex().size(),
                                                 Point3df( 0, 0, 0 ) );
  vector<AimsVector<uint, 2> >::const_iterator ip, ep = mesh.polygon().end();
  const vector<Point3df> & vert = mesh.vertex();
  Point3df dir;

  for( ip=mesh.polygon().begin(); ip!=ep; ++ip )
  {
    dir = vert[ (*ip)[1] ] - vert[ (*ip)[0] ];
    dir.normalize();
    dir /= 2;
    (*dirs)[ (*ip)[0] ] += dir;
    (*dirs)[ (*ip)[1] ] += dir;
    ++counts[ (*ip)[0] ];
    ++counts[ (*ip)[1] ];
  }

  long i, n = vert.size();
  for( i=0; i<n; ++i )
    if( counts[i] < 2 )
      (*dirs)[i] *= 2;

  return dirs;
}


// ---

template void SurfaceManip::meshMerge( AimsTimeSurface<2, Void> &,
				       const AimsTimeSurface<2, Void> & );
template void 
SurfaceManip::meshMerge( AimsTimeSurface<2, Void> &, 
                         const list<AimsTimeSurface<2, Void> > & );
template void SurfaceManip::meshMerge( AimsSurfaceTriangle &, 
				       const AimsSurfaceTriangle & );
template void SurfaceManip::meshMerge( AimsSurfaceTriangle &, 
				       const list<AimsSurfaceTriangle> & );
template void SurfaceManip::meshMerge( AimsSurfaceFacet &, 
				       const AimsSurfaceFacet & );
template void SurfaceManip::meshMerge( AimsSurfaceFacet &, 
				       const list<AimsSurfaceFacet> & );

template void SurfaceManip::meshTransform( AimsTimeSurface<2,Void> &, 
                                           const Motion & );
template void SurfaceManip::meshTransform( AimsSurfaceTriangle &, 
                                           const Motion & );
template void SurfaceManip::meshTransform( AimsSurfaceFacet &, 
                                           const Motion & );

template AimsTimeSurface<3,Void> *
  SurfaceManip::meshExtract( const AimsTimeSurface<3,Void> &,
                             const TimeTexture<int16_t> &, int16_t,
                             vector<size_t> ** overtIndex);

template TimeTexture<float>*
  SurfaceManip::meshDensity( const AimsTimeSurface<3,Void> &, bool );
template TimeTexture<float>*
  SurfaceManip::meshEdgeLengthRatioTexture( const AimsTimeSurface<3,Void> &,
    const AimsTimeSurface<3,Void> & );

template AimsSurface<2,Void> *
  SurfaceManip::meshTextureBoundary( const AimsSurface<3,Void> & mesh,
      const Texture<int16_t> & tex, int16_t region );
template AimsTimeSurface<2,Void> *
  SurfaceManip::meshTextureBoundary( const AimsSurfaceTriangle & mesh,
    const TimeTexture<int16_t> & tex, int16_t region );
template AimsSurface<2,Void> *
  SurfaceManip::meshTextureBoundary( const AimsSurface<3,Void> & mesh,
      const Texture<int32_t> & tex, int32_t region );
template AimsTimeSurface<2,Void> *
  SurfaceManip::meshTextureBoundary( const AimsSurfaceTriangle & mesh,
    const TimeTexture<int32_t> & tex, int32_t region );
template AimsSurface<2,Void> *
  SurfaceManip::meshTextureBoundary( const AimsSurface<3,Void> & mesh,
      const Texture<float> & tex, float region );
template AimsTimeSurface<2,Void> *
  SurfaceManip::meshTextureBoundary( const AimsSurfaceTriangle & mesh,
    const TimeTexture<float> & tex, float region );

