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


#include <cstdlib>
#include <aims/math/math_g.h>
#include <aims/mesh/geometric.h>
#include <aims/utility/cyclic_iterator.h>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <float.h>

using namespace std;
using namespace carto;
using namespace aims;
using namespace aims::meshgraph;


GeometricProperties::GeometricProperties( const AimsSurfaceTriangle & mesh)
  : _mesh( mesh ), _pmesh( 0 )
{
}


GeometricProperties::GeometricProperties( rc_ptr<AimsSurfaceTriangle> mesh )
  : _mesh( *mesh ), _pmesh( mesh )
{
}


GeometricProperties::~GeometricProperties()
{
}

void GeometricProperties::doPhi()
{
  
  if ( ! _phi.empty() )
    return;

  const vector<Point3df>			& vert = _mesh.vertex() ;
  unsigned					i, n = vert.size();
  list<unsigned>::iterator        		ilist,elist;
  list<unsigned>::const_iterator        	el;
  list<float>::const_iterator        	        efl,iflist;
  unsigned					v1, v2;
  float						phi,t;

  doNeighbor();
  vector< list<unsigned> >                     neigho = _neighbourso;
  
  doSurface();
  vector< list<float> >                        surfl = _surface ;

  _phi.insert(_phi.end(),n,list<float>());
  
  for (i=0; i<n; ++i)
    {
      el = _neighbourso[i].end();
      --el;
      neigho[i].push_front(*el);
      efl = _surface[i].end();
      --efl;
      surfl[i].push_front(*efl);
    }
  
  
  for (i=0; i<n; ++i)
    {
      phi = 0;
      ASSERT(neigho[i].size() == surfl[i].size() );
      iflist = surfl[i].begin();
      elist = neigho[i].end();
      --elist;
      ilist = neigho[i].begin();
      while ( ilist != elist ) 
	{
	  v1 = *ilist;
	  ++ilist;
	  v2 = *ilist;
	  t = *iflist;
	  ++iflist;
	  if (fabs(t) != 0 )
	    phi =   ( ( vert[v1] - vert[i] ).dot( vert[v1] - vert[v2]  ) )/(2 * t);
	  else
	    {
	      cout << "Triangle with a  null surface \n"; 
	      phi = 0;
	    }
	  _phi[i].push_back(phi);
	}
    }
  
}

void GeometricProperties::doTheta()
{
  if ( ! _theta.empty() )
    return;

  const vector<Point3df>			& vert = _mesh.vertex() ;
  unsigned					i, n = vert.size();
  list<float>::const_iterator		        il;
  list<unsigned>::iterator       		ilist,elist;
  unsigned					v1, v2;
  float						theta,t;	


  doNeighbor();
  vector< list<unsigned> >                     neigho = _neighbourso;
  
  doSurface();
  vector< list<float> >                        surfl = _surface ;

  _theta.insert(_theta.end(),n,list<float>());

  for (i=0; i<n; ++i)//the first element is the same as the last one (circular)
  {
    neigho[i].push_back(*_neighbourso[i].begin());
    surfl[i].push_back(*_surface[i].begin());
  }
  
  for (i=0; i<n; ++i)
  {
    theta = 0;
    il = surfl[i].begin();
    elist =  neigho[i].end();
    --elist;
    ilist = neigho[i].begin();
    ASSERT(neigho[i].size() == surfl[i].size() );
    while ( ilist != elist )
    {
      v1 = *ilist;
      ++ilist;
      v2 = *ilist;
      t = *il;
      ++il;
      if (fabs(t)  != 0)
        theta = ( ( vert[v2] - vert[i] ).dot( vert[v2] - vert[v1]  ) )/(2 * t);
      else
      {
        cout << "Triangle with null surface \n";
        theta = 0;
      }
      _theta[i].push_back(theta);
    }
  }
}

void GeometricProperties::doAlpha()
{
  if ( ! _alpha.empty() )
    return;
  
  const vector<Point3df>			& vert = _mesh.vertex() ;
  unsigned					i, n = vert.size();
  list<unsigned>::iterator       		ilist,elist;
  unsigned					v1, v2;
  float						a,t;	
  Point3df                                      nj1,nj2;

  doNeighbor();
  vector< list<unsigned> >                     neigho = _neighbourso;
  
  doSurface();
  vector< list<float> >                        surfl = _surface ;

  _alpha.insert(_alpha.end(),n,0);


  for (i=0; i<n; ++i)
    neigho[i].push_front(*neigho[i].rbegin());//the first element is the last one (circular)
  
  for (i=0; i<n; ++i)
    {
      a = 0;
      elist =  neigho[i].end();
      --elist;
      ilist = neigho[i].begin();
      while ( ilist != elist ) 
	{
	  v1 = *ilist;
	  ++ilist;
	  v2 = *ilist;
	  nj1 = (vert[i] - vert[v2]);
	  nj2 = (vert[i] - vert[v1]);
	  t = 0.5 * fabs(cross(nj1,nj2).norm());
	  if ( t != 0)
	    a +=(vert[v1] - vert[v2]).norm2() * nj1.dot(nj2)/(2*t);
	}
      _alpha[i] = -a/8;
    }
}

void GeometricProperties::doSimpleAlpha()
{
  if ( ! _simplealpha.empty() )
    return;
  
  const vector<Point3df>			& vert = _mesh.vertex() ;
  unsigned					i, n = vert.size();
  list<unsigned>::iterator       		ilist,elist;
  unsigned					v1, v2;
  float						a,co;	
  Point3df                                      nj1,nj2;
    
  doNeighbor();
  vector< list<unsigned> >                     neigho = _neighbourso;
  
  doSurface();
  vector< list<float> >                        surfl = _surface ;

  _simplealpha.insert(_simplealpha.end(),n,0);


  for (i=0; i<n; ++i)
    neigho[i].push_front(*neigho[i].rbegin());//the first element is the last one (circular)
  
  for (i=0; i<n; ++i)
    {
      a = 0;
      elist =  neigho[i].end();
      --elist;
      ilist = neigho[i].begin();
      while ( ilist != elist ) 
	{
	  v1 = *ilist;
	  ++ilist;
	  v2 = *ilist;
	  nj1 = (vert[i] - vert[v2]);
	  nj2 = (vert[i] - vert[v1]);
      co=nj1.dot(nj2) / (nj1.norm()*nj2.norm());
      a +=float(acos(double(co)));
	}
      _simplealpha[i] = a;
    }
}

void GeometricProperties::doBeta()
{

  if ( ! _beta.empty() )
    return;

  const vector<Point3df>			& vert = _mesh.vertex(), &normal = _mesh.normal() ;
  unsigned					i, n = vert.size();
  list<unsigned>::iterator       		ilist,elist;
  list<unsigned>::const_iterator       		il,el;
  unsigned					v1, v2, v3;
  float						b,nj1n,nj2n;	
  Point3df                                      nj1,nj2;

  doNeighbor();
  vector< list<unsigned> >                     neigho = _neighbourso;
  
  doSurface();
  vector< list<float> >                        surfl = _surface ;

  _beta.insert(_beta.end(),n,0);


  for (i=0; i<n; ++i)
    {
      il = _neighbourso[i].begin();
      el = _neighbourso[i].end();
      --el;
      neigho[i].push_front(*el);
      neigho[i].push_back(*il);
    }

  for (i=0; i<n; ++i)
    {
      b = 0;
      elist =  neigho[i].end();
      --elist;
      --elist;
      ilist = neigho[i].begin();
      while ( ilist != elist ) 
	{
	  v1 = *ilist;
	  ++ilist;
	  v2 = *ilist;
	  ++ilist;
	  v3 = *ilist;
	  --ilist;
	  nj1 = (normal[i] + normal[v1] + normal[v2]);
	  nj2 = (normal[i] + normal[v3] + normal[v2]);
	  nj1n = nj1.norm();
	  nj2n = nj2.norm();
	  if (nj1n  != 0 &&  nj2n != 0)
	    {
	      nj2 /= nj2n;
	      nj1 /= nj1n;
	      if (nj1.dot(nj2) != 1)
		b += (vert[v2] - vert[i]).norm() * acos( (double)nj1.dot(nj2) );
	      

	    }
	  else
	    cout << "Triangle of size null\n";
	    
	}
      _beta[i] = 0.25 * b;
    }

}

void GeometricProperties::doDot()
{
   if ( ! _dot.empty() )
    return;

  const vector<Point3df>			& vert = _mesh.vertex(), & normal = _mesh.normal() ;
  unsigned					i, n = vert.size();
  list<unsigned>::const_iterator		ilist,elist;
  
  doNeighbor();
  _dot.insert(_dot.end(),n,list<float>());

  
  for (i=0; i<n; ++i)
    for (ilist = _neighbourso[i].begin(),elist =  _neighbourso[i].end(); ilist != elist ; ++ilist) 
      _dot[i].push_back(normal[i].dot( - vert[*ilist] + vert[i]) );
  //DOT[i].push_back(normal[i].dot( vert[*ilist] - vert[i]) ); //BUG in CHUNG paper ?! YES : SEE ref. Desbrun99
}

void GeometricProperties::doSurface()
{
   if ( ! _surface.empty() )
    return;
  
  const vector<Point3df>			& vert = _mesh.vertex() ;
  unsigned					i, n = vert.size();
  list<unsigned>::iterator			ilist,elist;
  unsigned					v1, v2;
  float						t;	

  doNeighbor();
  _surface.insert(_surface.end(),n,list<float>());

  vector< list<unsigned> >                      neigho = _neighbourso;


  for (i=0; i<n; ++i)
    neigho[i].push_back(*_neighbourso[i].begin());//the first element is the last one (circular)

  for (i=0; i<n; ++i)
    {
      elist =  neigho[i].end();
      --elist;
      ilist = neigho[i].begin();
      while ( ilist != elist ) 
      {
        v1 = *ilist;
        ++ilist;
        v2 = *ilist;
        t = 0.5 * ( cross(vert[v2] - vert[i], vert[v1] - vert[i]).norm() );
        _surface[i].push_back(t);
      }
    }


}


void GeometricProperties::doNeighbor()
{
  if ( ! _neighbourso.empty() )
    return;

  unsigned i, n = _mesh.vertex().size();

  _neighbourso.insert( _neighbourso.end(), n, list<unsigned>() );

  _triangleNeighbourso = doTriangleNeighbor();

  for ( i=0; i<n; ++i)
    buildSortVerticesNeighborhood( i );
}


void GeometricProperties::buildSortVerticesNeighborhood( size_t i )
{
  set<unsigned> setinter;
  insert_iterator<set<unsigned> > ii( setinter, setinter.begin() );
  Neighborhood & tn = _triangleNeighbourso[i];
  Neighborhood & nbo = _neighbourso[i];
  const vector< AimsVector<uint,3> > & poly = _mesh.polygon();
  const vector<Point3df> & vert = _mesh.vertex(), & normal = _mesh.normal();
  list<unsigned>::iterator ilist, elist, jlist;

  nbo.clear();

  if( tn.size() < 2 )
  {
    if( tn.empty() )
      return;
    // 1 polygon
    const AimsVector<uint, 3> & p = poly[*tn.begin()];
    if( p[0] != i )
      nbo.push_back( p[0] );
    if( p[1] != i )
      nbo.push_back( p[1] );
    if( p[2] != i )
      nbo.push_back( p[2] );
  }
  else
  {
    elist = tn.end();
    --elist;  // assumes --end() is valid...
    ilist = tn.begin();
    set<unsigned> tn1, tn2;
    tn1.insert(poly[*ilist][0]);
    tn1.insert(poly[*ilist][1]);
    tn1.insert(poly[*ilist][2]);
    //cout << poly[*ilist][0] << " " << poly[*ilist][1] << " "<< poly[*ilist][2]<< endl;
    tn1.erase(i);
    ++ilist;
    tn2.insert(poly[*ilist][0]);
    tn2.insert(poly[*ilist][1]);
    tn2.insert(poly[*ilist][2]);
    //cout << poly[*ilist][0] << " " << poly[*ilist][1] << " "<< poly[*ilist][2]<< endl;
    tn2.erase(i);
    set_intersection( tn1.begin(),tn1.end(),tn2.begin(),tn2.end(),ii );
    tn1.erase(*setinter.begin());
    tn2.erase(*setinter.begin());
    //cout << "setinter size: " << setinter.size() << endl;
    nbo.push_back(*tn1.begin() );
    nbo.push_back(*setinter.begin());
    nbo.push_back(*tn2.begin() );

    while (ilist != elist)
    {
      //cout << "in loop" << endl;
      tn1.clear();
      tn2.clear();
      tn1.insert(poly[*ilist][0]);
      tn1.insert(poly[*ilist][1]);
      tn1.insert(poly[*ilist][2]);
      //cout << poly[*ilist][0] << " " << poly[*ilist][1] << " "<< poly[*ilist][2]<< endl;
      tn1.erase(i);
      ++ilist;
      tn2.insert(poly[*ilist][0]);
      tn2.insert(poly[*ilist][1]);
      tn2.insert(poly[*ilist][2]);
      //cout << poly[*ilist][0] << " " << poly[*ilist][1] << " "<< poly[*ilist][2]<< endl;
      tn2.erase(i);

      setinter.clear();
      insert_iterator<set<unsigned> > jj( setinter,setinter.begin() );
      set_intersection( tn1.begin(),tn1.end(),tn2.begin(),tn2.end(),jj );
      //cout << "setinter size: " << setinter.size() << endl;
      tn2.erase(*setinter.begin());
      nbo.push_back(*tn2.begin() );
    }

    if( nbo.size() >= 2 && *nbo.begin() == *nbo.rbegin() )
      nbo.pop_front();//remove the repeated first element
  }

  Point3df nor( 0, 0, 1 ), a, b;
  if( normal.size() > i )
    nor = normal[i];
  ilist = nbo.begin();
  a = vert[*ilist] - vert[i];
  jlist = ilist;
  ++jlist;
  b = vert[*jlist] - vert[*ilist];
  if ( cross(a,b).dot(nor) < 0 ) //clockwise ordering
    nbo.reverse();
}


GeometricProperties::NeighborList GeometricProperties::doTriangleNeighbor()
{

  if ( ! _triangleNeighbourso.empty() )
    return (_triangleNeighbourso);

  const vector<Point3df>			& vert = _mesh.vertex() ;
  const vector< AimsVector<uint,3> >		& poly = _mesh.polygon();
  unsigned					i,j,n = vert.size();
  unsigned					v1, v2, v3;
  map<unsigned, set<unsigned> >			neighbours;//give for each vertex a list of the  adjacent triangles 
  vector< set <unsigned> >                      trineigh(n); //give for each vertex a list of the  adjacent triangles 
  set<unsigned>::iterator 	                inei, enei;

  _triangleNeighbourso.insert(_triangleNeighbourso.end(),n,list<unsigned>());

  //  neighbours map

  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
     
      neighbours[v1].insert( v2 );
      neighbours[v2].insert( v1 );
      neighbours[v1].insert( v3 );
      neighbours[v3].insert( v1 );
      neighbours[v2].insert( v3 );
      neighbours[v3].insert( v2 );
      
      trineigh[v1].insert( i );
      trineigh[v2].insert( i );
      trineigh[v3].insert( i );
    }

  //order neighbour nodes 
  // same as sortPolygons() but avoids copies of list into set for each
  // (since we already have them in trineigh)
  set<unsigned> nodes;
  set<unsigned> tmpNodes;
  bool stopLoop=false;

  for ( i=0; i<n; ++i)
  {
    // cout << i << " -> " ;
    tmpNodes = trineigh[i];
    Neighborhood & npoly = _triangleNeighbourso[i];
    inei = tmpNodes.begin();
    if( inei != tmpNodes.end() )
    {
      npoly.push_back( *inei );
      tmpNodes.erase( inei );
    }

    while ( !tmpNodes.empty() )
    {
      enei = tmpNodes.end();
      stopLoop = false;
      for ( inei=tmpNodes.begin(); inei!=enei && !stopLoop; ++inei )
      {
        nodes.clear();
        j = npoly.back();
        nodes.insert(poly[*inei][0]);
        nodes.insert(poly[*inei][1]);
        nodes.insert(poly[*inei][2]);
        nodes.insert(poly[j][0]);
        nodes.insert(poly[j][1]);
        nodes.insert(poly[j][2]);
        if (nodes.size() == 4) // 2 adjacent triangles ?
        {
          npoly.push_back(*inei);
          tmpNodes.erase( inei );
          stopLoop = true;
          //cout << *inei << " ";
        }
        else if( npoly.size() > 1 )
        {
          // not connected to the end of the chain: try the beginning
          nodes.clear();
          j = npoly.front();
          nodes.insert(poly[*inei][0]);
          nodes.insert(poly[*inei][1]);
          nodes.insert(poly[*inei][2]);
          nodes.insert(poly[j][0]);
          nodes.insert(poly[j][1]);
          nodes.insert(poly[j][2]);
          if( nodes.size() == 4 ) // 2 adjacent triangles ?
          {
            // insert at beginning
            npoly.insert( npoly.begin(), *inei );
            tmpNodes.erase( inei );
            stopLoop = true;
            // cout << "insert " << *inei << " ";
          }
        }
      }
    }
    // cout << endl;
  }
  return(_triangleNeighbourso);
}


void GeometricProperties::sortPolygons( Neighborhood & npoly )
{
  //order neighbour nodes
  set<unsigned>  nodes;
  set<unsigned>  tmpNodes( npoly.begin(), npoly.end() );
  bool           stopLoop = false;
  const vector< AimsVector<uint,3> > & poly = _mesh.polygon();
  set<unsigned>::iterator inei, enei;
  unsigned j, n;

  npoly.clear();

  inei = tmpNodes.begin();
  if( inei != tmpNodes.end() )
  {
    npoly.push_back( *inei );
    tmpNodes.erase( inei );
  }

  while( !tmpNodes.empty() )
  {
    enei = tmpNodes.end();
    stopLoop = false;
    n = tmpNodes.size();
    for( inei=tmpNodes.begin(); inei!=enei && !stopLoop; ++inei )
    {
      nodes.clear();
      j = npoly.back();
      nodes.insert(poly[*inei][0]);
      nodes.insert(poly[*inei][1]);
      nodes.insert(poly[*inei][2]);
      nodes.insert(poly[j][0]);
      nodes.insert(poly[j][1]);
      nodes.insert(poly[j][2]);
      if( nodes.size() == 4 ) // 2 adjacent triangles ?
      {
        npoly.push_back( *inei );
        tmpNodes.erase( inei );
        stopLoop = true;
      }
      else if( npoly.size() > 1 )
      {
        // not connected to the end of the chain: try the beginning
        nodes.clear();
        j = npoly.front();
        nodes.insert(poly[*inei][0]);
        nodes.insert(poly[*inei][1]);
        nodes.insert(poly[*inei][2]);
        nodes.insert(poly[j][0]);
        nodes.insert(poly[j][1]);
        nodes.insert(poly[j][2]);
        if( nodes.size() == 4 ) // 2 adjacent triangles ?
        {
          // insert at beginning
          npoly.insert( npoly.begin(), *inei );
          tmpNodes.erase( inei );
          stopLoop = true;
        }
      }
    }
    if( tmpNodes.size() == n )
    {
      cerr << "could not order polygons:\n";
      for( inei=tmpNodes.begin(); inei!=enei; ++inei )
        cout << *inei << ": " << poly[*inei] << endl;
      cout << "already sorted:\n";
      Neighborhood::iterator in, en = npoly.end();
      for( in=npoly.begin(); in!=en; ++in )
        cout << *in << ": " << poly[*in] << endl;
      break; // abort
    }
  }
}


void GeometricProperties::doGraph()
{
  if( _graphvertices.empty() && _graphfaces.empty() )
    list2graph_mesh_conversion( _mesh.vertex(), _mesh.polygon(),
                                _graphvertices, _graphfaces );
}


void GeometricProperties::graphToMesh()
{
  Graph2ListMeshConvertor<AimsSurfaceTriangle> g2l( _pmesh );
  g2l( _graphvertices, _graphfaces );
  if( !_pmesh )
  {
    _pmesh = g2l.mesh();
  }
  if( _pmesh->header().hasProperty( "vertex_number" ) )
    _pmesh->header().removeProperty( "vertex_number" );
  if( _pmesh->header().hasProperty( "polygon_number" ) )
    _pmesh->header().removeProperty( "polygon_number" );
}


const GeometricProperties::WeightNeighborList & GeometricProperties::getPhi() const
{
  return(_phi);
}

const GeometricProperties::WeightNeighborList & GeometricProperties::getTheta() const
{
  return(_theta);
}

const GeometricProperties::WeightNeighborList & GeometricProperties::getDot() const
{
  return(_dot);
}

const GeometricProperties::WeightNeighborList & GeometricProperties::getSurface() const
{
  return(_surface);
}

const AimsSurfaceTriangle & GeometricProperties::getMesh() const
{
  return(_mesh);
}


rc_ptr<AimsSurfaceTriangle> GeometricProperties::getRcMesh()
{
  return _pmesh;
}


const GeometricProperties::WeightList & GeometricProperties::getAlpha() const
{
  return(_alpha);
}

const GeometricProperties::WeightList & GeometricProperties::getSimpleAlpha() const 
{
  return(_simplealpha);
}

const GeometricProperties::WeightList & GeometricProperties::getBeta() const 
{
  return(_beta);
}

const GeometricProperties::NeighborList & GeometricProperties::getNeighbor() const
{
  return(_neighbourso);
}

GeometricProperties::NeighborList & GeometricProperties::getNeighbor()
{
  return(_neighbourso);
}

const GeometricProperties::NeighborList &
  GeometricProperties::getTriangleNeighbor() const
{
  return(_triangleNeighbourso);
}

GeometricProperties::NeighborList & GeometricProperties::getTriangleNeighbor()
{
  return(_triangleNeighbourso);
}

Curvature::Curvature(const AimsSurfaceTriangle & mesh) : GeometricProperties(mesh)
{
}

Curvature::Curvature( rc_ptr<AimsSurfaceTriangle> mesh ) : GeometricProperties(mesh)
{
}

Curvature::~Curvature()
{
}

void Curvature::regularize(Texture<float> & tex, float ratio)
{
  set<float>                    values; 
  unsigned 			 i, n = tex.nItem();
  set<float>::iterator iv;
  set<float>::reverse_iterator riv;
  unsigned   nbValues,inc = 0 ;
  float Wm, WM;

   
  cout << "Texture regularization : " << endl << ratio << " % of points are conserved \n";

  for ( i=0; i<n; ++i )
    values.insert(tex.item(i));
  
  ASSERT(ratio <= 1 && ratio >= 0);
  nbValues = (unsigned) rint ( (1 - ratio) * values.size() );
  cout <<  "Nb of rejected values: " << nbValues << "/" <<  values.size() <<  endl;
  iv = values.begin();
  riv = values.rbegin();
  cout << "Values before: min: " << *iv << " max: " << *riv << endl; 
  while ( inc < nbValues)
    {
      ++inc;
      ++iv;
    }
  Wm = *iv;
  inc = 0;
  while ( inc < nbValues)
    {
      ++inc;
      ++riv;
    }
  WM = *riv;
  cout << "Values after: min: " << Wm << " max: " << WM << endl; 
  
  for ( i=0; i<n; ++i )
    if (tex.item(i) > WM)
      tex.item(i) = WM;
    else
      if (tex.item(i) < Wm)
	tex.item(i) = Wm;
}

void Curvature::getTextureProperties(const Texture<float> & tex)
{
  set<float> val;
  float m=0,sigma=0;
  for (unsigned i=0; i< tex.nItem(); ++i)
    {
      m+=tex.item(i);
      val.insert(tex.item(i));
    }
  m /= tex.nItem();
  for (unsigned i=0; i<tex.nItem(); ++i)
    sigma += (tex.item(i) - m)*(tex.item(i) - m);
  sigma = sqrt(sigma);
  sigma /= tex.nItem();
  
  
  cout << "min: " << *val.begin() << ", max: " << *val.rbegin()
       << endl << "mean: " << m << " sigma: " << sigma << endl; 

}


FiniteElementCurvature::FiniteElementCurvature(const AimsSurfaceTriangle & mesh) : Curvature(mesh) //recursif montant
{
}


FiniteElementCurvature::FiniteElementCurvature(
  rc_ptr<AimsSurfaceTriangle> mesh ) : Curvature(mesh)
{
}


FiniteElementCurvature::~FiniteElementCurvature()
{
}

Texture<float> FiniteElementCurvature::doIt()
{
  

  const vector<Point3df>			& vert = getMesh().vertex() ;
  unsigned					i, n = vert.size();
  Texture<float>				tex(n);
  list<unsigned>::const_iterator		ilist,elist;
  list<float>::const_iterator		        iphi,idot,itheta,isurf;
  unsigned					/*nb,*/s,p,d,t;	
  float 					K,surf;
 
  doPhi();
  doTheta();
  doDot();
  doSurface();
  
  
  const NeighborList  & neighbourso  = getNeighbor();
  const WeightNeighborList  & phi = getPhi();
  const WeightNeighborList & theta = getTheta();
  const WeightNeighborList		& dot = getDot();
  const WeightNeighborList		& surface = getSurface() ;   

  for (i=0; i<n; ++i)
  {
    s = surface[i].size();
    p = phi[i].size();
    t = theta[i].size();
    d = dot[i].size();
    //nb =neighbourso[i].size() ;
    if ( !( s == p && s == t && s == d && p==t && p==d && t==d ) )
    {
      cout << "Problem with the mesh features..." << endl;
      ASSERT(0);
    }
    K = 0;
    surf = 0;
    for ( elist =  neighbourso[i].end(),ilist = neighbourso[i].begin(),
          iphi = phi[i].begin(),idot = dot[i].begin(),
          isurf = surface[i].begin(), itheta = theta[i].begin();
          ilist != elist;++ilist, ++iphi,++itheta,++idot,++isurf )
    {
      K +=(*iphi + *itheta)*(*idot);
      surf += *isurf;
      //cout << *iphi << " " << *itheta << " " <<  *idot << " " << *isurf << endl;
    }

    if ( surf != 0 )
      K = K/(surf * 4);
    else
    {
      cout << "Triangle with null surface\n";
      K = 0;
    }
    tex.item(i) = K;
  }

  return(tex);
}

BoixCurvature::BoixCurvature(const AimsSurfaceTriangle & mesh) : Curvature(mesh) //recursif montant
{
}


BoixCurvature::BoixCurvature( rc_ptr<AimsSurfaceTriangle> mesh )
  : Curvature(mesh)
{
}


BoixCurvature::~BoixCurvature()
{
}


Texture<float> BoixCurvature::doIt()
{
  const vector<Point3df>			& vert = getMesh().vertex() ;
  unsigned					i, n = vert.size();
  Texture<float>				tex(n);
  float                                         H,nume,deno,aire;
  list <float>::const_iterator              il,el;

  doAlpha();
  doBeta();
  doSurface();

  const WeightNeighborList		& surface = getSurface() ; 
  const WeightList & alpha = getAlpha()  ;
  const WeightList & beta = getBeta()  ;
 
  for (i=0; i<n; ++i)
    {
      nume = beta[i];
      aire = 0;
      for (il = surface[i].begin(), el = surface[i].end(); il != el; ++il)
	aire += *il;

      deno = (aire * 0.5 + alpha[i] );
      H = nume / deno;
      tex.item(i) = H;
    }

  return(tex);
}


BarycenterCurvature::BarycenterCurvature(const AimsSurfaceTriangle & mesh) : Curvature(mesh) //recursif montant
{
}


BarycenterCurvature::BarycenterCurvature( rc_ptr<AimsSurfaceTriangle> mesh )
  : Curvature(mesh)
{
}


BarycenterCurvature::~BarycenterCurvature()
{
}

Texture<float> BarycenterCurvature::doIt()
{
  Texture<float>				tex;
  const vector<Point3df>			& vert = getMesh().vertex(), & normal = getMesh().normal() ;
  const vector< AimsVector<uint,3> >		& poly = getMesh().polygon();
  unsigned					i, n = vert.size();
  vector<float>					bary(n), bary2(n);
  map<unsigned, set<unsigned> >			neighbours;
  set<unsigned>::iterator 	                inei,enei;
  unsigned					v1, v2, v3;
  Point3df					a;
  
  //  neighbours map

  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
     
      neighbours[v1].insert( v2 );
      neighbours[v2].insert( v1 );
      neighbours[v1].insert( v3 );
      neighbours[v3].insert( v1 );
      neighbours[v2].insert( v3 );
      neighbours[v3].insert( v2 );
      
    }


  //Barycenter and sign 
  for (i=0; i<n; ++i)
    {
      a = Point3df(0,0,0) ;
      bary[i] = 0 ; 
      for (inei = neighbours[i].begin(), enei = neighbours[i].end(); inei != enei; ++inei)
	a += vert[*inei];
      a /= neighbours[i].size(); //barycenter
      a = vert[i] - a;
      bary[i] = a.norm() * sign( a.dot(normal[i]) );
      bary2[i] = a.dot(normal[i] );
      tex.push_back( bary[i] ); //barycenter
      
    }

  return(tex);

}


BoixGaussianCurvature::BoixGaussianCurvature(const AimsSurfaceTriangle & mesh) : Curvature(mesh) //recursif montant
{
}


BoixGaussianCurvature::BoixGaussianCurvature(
  rc_ptr<AimsSurfaceTriangle> mesh ) : Curvature(mesh)
{
}


BoixGaussianCurvature::~BoixGaussianCurvature()
{
}

Texture<float> BoixGaussianCurvature::doIt()
{
  const vector<Point3df>			& vert = getMesh().vertex() ;
  unsigned					i, n = vert.size();
  Texture<float>				tex(n);
  float                                         H,nume,deno,aire;
  list<float>::const_iterator              il,el;

  doAlpha();
  doSimpleAlpha();
  doSurface();

  const WeightNeighborList		& surface = getSurface() ; 
  const WeightList & alpha = getAlpha()  ;
  const WeightList & simplealpha = getSimpleAlpha()  ;
 
  for (i=0; i<n; ++i)
    {
      nume = 2*M_PI - simplealpha[i];
      aire = 0;
      for (il = surface[i].begin(), el = surface[i].end(); il != el; ++il)
	aire += *il;

      deno = (aire * 0.5 + alpha[i] );
      H = nume / deno;
      tex.item(i) = H;
    }

  return(tex);
}


// ---

GaussianCurvature::GaussianCurvature( const AimsSurfaceTriangle & mesh )
  : Curvature( mesh )
{
}


GaussianCurvature::GaussianCurvature( rc_ptr<AimsSurfaceTriangle> mesh )
  : Curvature( mesh )
{
}


GaussianCurvature::~GaussianCurvature()
{
}


Texture<float> GaussianCurvature::doIt()
{
  const vector<Point3df> & vert = getMesh().vertex() ;
  unsigned i, n = vert.size();
  Texture<float> tex(n);
  float meanCurvature, orientedMeanCurvature, orientedGaussianCurvature,
    voronoiArea;
  pair<float, float> principalCurvatures;
  Point3df normal;

  doNeighbor();

  for( i=0; i<n; ++i )
  {
    localProcess( i, tex[i], meanCurvature, principalCurvatures,
                  orientedMeanCurvature, orientedGaussianCurvature, normal,
                  voronoiArea );
  }

  return tex;
}


void GaussianCurvature::localProcess(
  size_t i_vert, float & gaussianCurvature,
  float & meanCurvature,
  std::pair<float, float> & principalCurvatures,
  float & orientedMeanCurvature,
  float & orientedGaussianCurvature,
  Point3df & normal, float & voronoiArea )
{

  const Neighborhood & nh = getNeighbor()[i_vert];
  // Curvature cannot be computed if vertex has not at least three neighbors
  assert( nh.size() >= 2 );
  const vector<Point3df> & vert = getMesh().vertex() ;
  const Point3df & p = vert[i_vert];
  normal = Point3df( 0., 0., 0. );

  // initializations
  voronoiArea = 0.;
  // mean curvature vector
  Point4df m;
  // Gaussian curvature
  gaussianCurvature = 0;

  Point3df e1, e2, e3;
  // Loop through all neighboring triangles
  Neighborhood::const_iterator iNh1 = nh.begin();
  const_cyclic_iterator<Neighborhood> iNh2 (nh, nh.begin()); ++iNh2;

  for (; iNh1 != nh.end(); ++iNh1, ++iNh2)
  {
    // compute the edges, their squared length and their length
    e1 = p - vert[*iNh1];
    float n21 = e1.norm2();
    float n1 = std::sqrt(n21);

    e2 = p - vert[*iNh2];
    float n22 = e2.norm2();
    float n2 = std::sqrt(n22);

    e3 = vert[*iNh2] - vert[*iNh1];
    float n23 = e3.norm2();
    //float n3 = std::sqrt(n23);
    //e3 *= 1/norm<float>(e3);

    float d1 = e1.dot(e3);
    float d2 = e2.dot(e3);

    float biarea = std::sqrt(n21*n23 - d1*d1);

    // Subtracting the angle between the two side edges to the gaussian curvature
    gaussianCurvature -= std::acos( max(float(-1),
                                    min(float(1), e1.dot(e2) / (n1*n2))));

    //cot1 =  d1 / norm<float>(e1 - d1 * e3);
    //cot2 = -d2 / norm<float>(e2 - d2 * e3);

    //cot1 =  d1 / dist<float>(e1, d1 * e3);
    //cot2 = -d2 / dist<float>(e2, d2 * e3);

    // NB: aren't the denominator all proportional to area? In that case
    // needed only once?
    /*
    float cot1 = d1 / biarea;
    float cot2 = -d2 / biarea;
    m += cot1 * e2 + cot2 * e1;
    */
    normal += (d1 * e2 - d2 * e1) * (float(1)/biarea);

    if( e1.dot(e2) <= 0 )
    {
      //n3 = dist<float>(m_vertices[*iNh1], m_vertices[*iNh2]);
      //area += heronsFormula(n1, n2, n3) / 2;
      voronoiArea += biarea / 4;
    }
    else
    {
      if (d1 <= 0 || d2 >= 0)
      {
        //n3 = dist<float>(m_vertices[*iNh1], m_vertices[*iNh2]);
        //area += heronsFormula(n1, n2, n3) / 4;
        voronoiArea += biarea / 8;
      }
      else
      {
        voronoiArea += 1.0/8.0 * (n22 * d1 -  n21 * d2 ) / biarea;
      }
    }
  }

  // finishing computation of gaussian curvature
  gaussianCurvature += 2*M_PI;
  gaussianCurvature /= voronoiArea;
  // finishing computation of mean curvature
  // NB: [Meyer et al.] the mean curvature is half the norm of K, hence the factor 4.
  meanCurvature = normal.norm() / (4 * voronoiArea);
  // computes the principal curvatures from the unsigned mean and Gaussian curvatures
  float delta = std::sqrt(max(float(0),
                              square(meanCurvature) - gaussianCurvature));
  principalCurvatures.first = meanCurvature + delta;
  principalCurvatures.second = meanCurvature - delta;

  // takes into account the sign
  if( normal.dot( cross(e1, e2) ) < 0)
  {
    orientedMeanCurvature =  -meanCurvature;
    orientedGaussianCurvature = -gaussianCurvature;
    principalCurvatures.first = -principalCurvatures.first;
    principalCurvatures.second = -principalCurvatures.second;
    normal *= -1 / normal.norm();
  }
  else
  {
    orientedMeanCurvature =  meanCurvature;
    orientedGaussianCurvature = gaussianCurvature;
    normal *= 1 / normal.norm();
  }

}


void GaussianCurvature::localProcess(
  const VertexPointer & i_vert, float & gaussianCurvature,
  float & meanCurvature,
  std::pair<float, float> & principalCurvatures,
  float & orientedMeanCurvature,
  float & orientedGaussianCurvature,
  Point3df & normal, float & voronoiArea )
{

  // we need the graph structures for the mesh.
  doGraph();

  const MeshGraphVertex::VertexIndexCollection & nh = i_vert->neighbors();
  // Curvature cannot be computed if vertex has not at least three neighbors
  assert( nh.size() >= 2 );
  const Point3df & p = i_vert->pos();
  normal = Point3df( 0., 0., 0. );

  // initializations
  voronoiArea = 0.;
  // mean curvature vector
  Point4df m;
  // Gaussian curvature
  gaussianCurvature = 0;

  Point3df e1, e2, e3;
  // Loop through all neighboring triangles
  MeshGraphVertex::VertexIndexCollection::const_iterator iNh1 = nh.begin();
  const_cyclic_iterator<MeshGraphVertex::VertexIndexCollection>
    iNh2 (nh, nh.begin()); ++iNh2;

  for (; iNh1 != nh.end(); ++iNh1, ++iNh2)
  {
    // compute the edges, their squared length and their length
    e1 = p - (*iNh1)->pos();
    float n21 = e1.norm2();
    float n1 = std::sqrt(n21);

    e2 = p - (*iNh2)->pos();
    float n22 = e2.norm2();
    float n2 = std::sqrt(n22);

    e3 = (*iNh2)->pos() - (*iNh1)->pos();
    float n23 = e3.norm2();
    //float n3 = std::sqrt(n23);
    //e3 *= 1/norm<float>(e3);

    float d1 = e1.dot(e3);
    float d2 = e2.dot(e3);

    float biarea = std::sqrt(n21*n23 - d1*d1);

    // Subtracting the angle between the two side edges to the gaussian curvature
    gaussianCurvature -= std::acos( max(float(-1),
                                    min(float(1), e1.dot(e2) / (n1*n2))));

    //cot1 =  d1 / norm<float>(e1 - d1 * e3);
    //cot2 = -d2 / norm<float>(e2 - d2 * e3);

    //cot1 =  d1 / dist<float>(e1, d1 * e3);
    //cot2 = -d2 / dist<float>(e2, d2 * e3);

    // NB: aren't the denominator all proportional to area? In that case
    // needed only once?
    /*
    float cot1 = d1 / biarea;
    float cot2 = -d2 / biarea;
    m += cot1 * e2 + cot2 * e1;
    */
    normal += (d1 * e2 - d2 * e1) * (float(1)/biarea);

    if( e1.dot(e2) <= 0 )
    {
      //n3 = dist<float>(m_vertices[*iNh1], m_vertices[*iNh2]);
      //area += heronsFormula(n1, n2, n3) / 2;
      voronoiArea += biarea / 4;
    }
    else
    {
      if (d1 <= 0 || d2 >= 0)
      {
        //n3 = dist<float>(m_vertices[*iNh1], m_vertices[*iNh2]);
        //area += heronsFormula(n1, n2, n3) / 4;
        voronoiArea += biarea / 8;
      }
      else
      {
        voronoiArea += 1.0/8.0 * (n22 * d1 -  n21 * d2 ) / biarea;
      }
    }
  }

  // finishing computation of gaussian curvature
  gaussianCurvature += 2*M_PI;
  gaussianCurvature /= voronoiArea;
  // finishing computation of mean curvature
  // NB: [Meyer et al.] the mean curvature is half the norm of K, hence the factor 4.
  meanCurvature = normal.norm() / (4 * voronoiArea);
  // computes the principal curvatures from the unsigned mean and Gaussian curvatures
  float delta = std::sqrt(max(float(0),
                              square(meanCurvature) - gaussianCurvature));
  principalCurvatures.first = meanCurvature + delta;
  principalCurvatures.second = meanCurvature - delta;

  // takes into account the sign
  if( normal.dot( cross(e1, e2) ) < 0)
  {
    orientedMeanCurvature =  -meanCurvature;
    orientedGaussianCurvature = -gaussianCurvature;
    principalCurvatures.first = -principalCurvatures.first;
    principalCurvatures.second = -principalCurvatures.second;
    normal *= -1 / normal.norm();
  }
  else
  {
    orientedMeanCurvature =  meanCurvature;
    orientedGaussianCurvature = gaussianCurvature;
    normal *= 1 / normal.norm();
  }

}


// ---


VertexRemover::VertexRemover( rc_ptr<GeometricProperties> geom )
  : _geom( geom )
{
  _geom->doGraph();
}


VertexRemover::VertexRemover( rc_ptr<AimsSurfaceTriangle> mesh )
  : _geom( new GeometricProperties( mesh ) )
{
  _geom->doGraph();
}


namespace
{

  //NB: SIMPLE because edges are encoded in nodes. This is more efficient, on the other hand less general, since
  // edges cannot have properties of their own (e.g. such as a cost) as there have no label to name them.
  template < typename T >
  struct SimpleOrientedGraphNode
  {
    explicit SimpleOrientedGraphNode(const T & v) : value(v) {}
    std::list<typename std::list<SimpleOrientedGraphNode<T> >::iterator> from;
    std::list<typename std::list<SimpleOrientedGraphNode<T> >::iterator> to;
    T value;
  };


  /// operator< on the address in memory of the pointed object
  template < typename TIterator >
  struct Lesser_PointeeAddress
  {
    bool operator()(TIterator i, TIterator j) const
    { return &*i < &*j; }
  };


  template < typename T >
  struct ValueFinder
  {
    ValueFinder(T value) : m_value(value) {}
    bool operator()(const SimpleOrientedGraphNode<T> & i) { return i.value == m_value; }
    T m_value;
  };

  template < typename T >
  ValueFinder<T> valueFinder(T value) { return ValueFinder<T>(value); }


  template < typename T >
  struct GroComp
  {
    //typedef std::list<MeshVertexNode><uint> ::iterator T;

    GroComp(T a, T b, T c)
      : m_a(a)
      , m_b(b)
      , m_c(c)
    {}

    template < typename TFace >
    bool operator()(const TFace & f)
    {
      return
        f->face == AimsVector<T, 3>(m_a, m_b, m_c) ||
        f->face == AimsVector<T, 3>(m_c, m_a, m_b) ||
        f->face == AimsVector<T, 3>(m_b, m_c, m_a) ||
        f->face == AimsVector<T, 3>(m_b, m_a, m_c) ||
        f->face == AimsVector<T, 3>(m_c, m_b, m_a) ||
        f->face == AimsVector<T, 3>(m_a, m_c, m_b)
        ;
    }

    T m_a;
    T m_b;
    T m_c;
  };


  /// Remove a vertex from a mesh.
  /// Does all the bookkeeping crap: removing the vertex means removing associated faces, and warning neghbors
  /// about the missing of their fellow.
  /// The function does *not* remesh the surface, i.e. if nothing else is done, the surface will present a whole
  /// where the vertex were lying.
  template < typename TVertexNode, typename TFaceNode >
  typename std::list<TVertexNode>::iterator
  remove_vertex
  (
   typename std::list<TVertexNode>::iterator    i,
   std::list<TVertexNode> &                     graph_vertices,
   std::list<TFaceNode> &                       graph_faces
  )
  {
    // remove faces point belongs to
    for (typename TVertexNode::FaceIndexCollection::iterator j = i->faces().begin(); j != i->faces().end(); ++j)
    {
      // Remove index to this face
      for (std::size_t k = 0; k < 3; ++k)
      {
        // don't remove face from facelist of point i itself -- because j is iterating on this very list,
        // plus, it will be deleted with i anyway.
        if ((*j)->face[k] == i) continue;
#ifndef NDEBUG
        std::size_t tmp = (*j)->face[k]->faces().size();
#endif
        (*j)->face[k]->faces().remove(*j);
        assert((*j)->face[k]->faces().size() == tmp-1);
      }
      // remove face itself
#ifndef NDEBUG
      std::size_t tmp = graph_faces.size();
#endif
      graph_faces.erase(*j);
      assert(graph_faces.size() == tmp-1);
    }

    // remove point in neighbor's list
    for (typename TVertexNode::VertexIndexCollection::iterator j = i->neighbors().begin(); j != i->neighbors().end(); ++j)
    {
      (*j)->neighbors().remove(i);
    }
    // remove point
    return graph_vertices.erase(i);
  }


}

bool VertexRemover::operator()( VertexPointer & i )
{
  //std::cout << "." << std::flush;
  typedef SimpleOrientedGraphNode<typename std::list<MeshVertexNode<uint> >::iterator>
    NeighborGraphNode;
  typedef std::list<NeighborGraphNode> NeighborGraph;

  GeometricProperties & gp = geometricProperties();

  // save list of neighbors
  std::vector<VertexPointer> m_neighbors( i->neighbors().begin(),
                                          i->neighbors().end() );

  // Add new faces

  list<AimsVector<uint ,3> > m_tri;
  // get delaunay faces for more than 4 neighbors
  if (m_neighbors.size() >= 4)
  {
    m_tri = simple_delaunay_triangulation(simple_neighborhood_flattening(
      i->pos(), m_neighbors));
  }
  // For three neighbors, simply add the only triangle possible
  else if (m_neighbors.size() == 3)
  {
    m_tri.push_back( AimsVector<uint, 3>( 0, 1, 2 ) );
  }
  // For two points or less, do not add any face. Note that if this happens, it means that the mesh
  // is in really bad shape -- actually the following might crash.
  else std::cout << "w2!";

  // Check consistency between number of points and number of faces
  if (m_tri.size() != m_neighbors.size() - 2)
  {
    std::cout << "wK!" << m_tri.size() << "-" << m_neighbors.size() - 2;
  }

  //initialize oriented edges
  std::vector<NeighborGraph> m_neighborGraph;

  m_neighborGraph.resize(i->neighbors().size());

  vector<map<VertexPointer, typename list<NeighborGraphNode>::iterator,
    Lesser_PointeeAddress<VertexPointer> > > m_convmap;

  m_convmap.resize(i->neighbors().size());

  for (std::size_t j = 0; j < i->neighbors().size(); ++j)
  {
    // first pass: vertices
    {
      for (typename MeshVertexNode<uint>::VertexIndexCollection::const_iterator k = m_neighbors[j]->neighbors().begin(); k != m_neighbors[j]->neighbors().end(); ++k)
      {
        if (*k == i) continue;
        NeighborGraphNode node(*k);
        m_convmap[j][*k] = m_neighborGraph[j].insert(m_neighborGraph[j].end(), node);
      }
    }
    // second pass: edges
    {
      typename MeshVertexNode<uint>::VertexIndexCollection::const_iterator k = m_neighbors[j]->neighbors().begin();
      const_cyclic_iterator<typename MeshVertexNode<uint>::VertexIndexCollection> k2(m_neighbors[j]->neighbors(), m_neighbors[j]->neighbors().begin());
      ++k2;
      const_cyclic_iterator<typename MeshVertexNode<uint>::VertexIndexCollection> k3(m_neighbors[j]->neighbors(), m_neighbors[j]->neighbors().begin());
      ++k3;
      ++k3;
      for (; k != m_neighbors[j]->neighbors().end(); ++k, ++k2, ++k3)
      {
        if (*k2 == i) continue;
        if (*k != i)  m_convmap[j][*k2]->from.push_back(m_convmap[j][*k]);
        if (*k3 != i) m_convmap[j][*k2]->to.push_back(m_convmap[j][*k3]);
      }
    }
  }

  // Updating neighbor graphs
  {
    for (std::list<AimsVector<uint, 3> >::iterator newf = m_tri.begin();
         newf != m_tri.end(); ++newf)
    {
      for (std::size_t n = 0; n < 3; ++n)
      {
        typename NeighborGraph::iterator p2, p3;

        p2 = std::find_if(m_neighborGraph[(*newf)[n]].begin(), m_neighborGraph[(*newf)[n]].end(), valueFinder(m_neighbors[(*newf)[(n+1)%3]]));
        p3 = std::find_if(m_neighborGraph[(*newf)[n]].begin(), m_neighborGraph[(*newf)[n]].end(), valueFinder(m_neighbors[(*newf)[(n+2)%3]]));
        if (p2 == m_neighborGraph[(*newf)[n]].end()) p2 = m_neighborGraph[(*newf)[n]].insert(p2, NeighborGraphNode(m_neighbors[(*newf)[(n+1)%3]]));
        if (p3 == m_neighborGraph[(*newf)[n]].end()) p3 = m_neighborGraph[(*newf)[n]].insert(p3, NeighborGraphNode(m_neighbors[(*newf)[(n+2)%3]]));

        p2->to.push_back(p3);
        p3->from.push_back(p2);
      }
    }
  }

  // Checking that neighbors have at least 3 neighbors
  for (std::size_t k = 0; k < m_neighborGraph.size(); ++k)
  {
    if (m_neighborGraph[k].size() < 3)
    {
      //std::cout << "WREJECT!(small-neighborhood)";
      return false;
    }
  }

  // Checking that neighbors have correct neighborhood
  // Incorrect neighborhood may appear when a neighbor was already sharing an edge with another neighbor of
  // i that is not on its side, i.e that is not n-1 or n+1.
  for (std::size_t k = 0; k < m_neighborGraph.size(); ++k)
  {
    typename NeighborGraph::iterator p = m_neighborGraph[k].begin();
    typename NeighborGraph::iterator p0 = p;
    bool flag = false;
    for (;;)
    {
      if (flag) { if (p == p0) break; }
      else flag = true;
      if (p->to.size() != 1 || p->from.size() != 1)
      {
        //std::cout << "WREJECT!(incorrect-neighborhood)";
        return false;
      }
      p = p->to.front();
    }
  }


  // adding faces
  {
    for( std::list<AimsVector<uint, 3> >::iterator newf = m_tri.begin();
         newf != m_tri.end(); ++newf )
    {
      // convert into graph faces
      MeshGraphFace f;
      for (std::size_t n = 0; n < 3; ++n)
      {
        f.face[n] = m_neighbors[(*newf)[n]];
      }
      // add face to graph
      MeshGraphFaces::iterator
        gf = gp.getFaces().insert( gp.getFaces().end(), f );
      // add face index to face points
      for (std::size_t n = 0; n < 3; ++n)
      {
        GroComp<VertexPointer> grocomp(f.face[0], f.face[1], f.face[2]);
        typename MeshGraphVertex::FaceIndexCollection::iterator p = find_if(f.face[n]->faces().begin(), f.face[n]->faces().end(), grocomp);
        if (p != f.face[n]->faces().end())
        {
          std::cout << "FATAL: face already there: " << std::endl;
          std::cout << &*f.face[0] << " " << &*f.face[1] << " " << &*f.face[2] << std::endl;
          std::cout << &*(*p)->face[0] << " " << &*(*p)->face[1] << " " << &*(*p)->face[2] << std::endl;
          std::cout << "I'll continue as if nothing happened, but this is a bug and your algorithm is likely to go bananas :)" << std::endl;
        }
        f.face[n]->faces().push_back(gf);
      }
    }
  }

  // Adding neighbors
  for (std::size_t j = 0; j < i->neighbors().size(); ++j)
  {
    typename MeshVertexNode<uint>::VertexIndexCollection ntmp = m_neighbors[j]->neighbors();

    // remove all neighbors
    m_neighbors[j]->neighbors().clear();

    typename NeighborGraph::iterator p = m_neighborGraph[j].begin();
    typename NeighborGraph::iterator p0 = p;
    bool flag = false;
    for (;;)
    {
      if (flag) { if (p == p0) break; }
      else flag = true;
      m_neighbors[j]->neighbors().push_back(p->value);
      if (p->to.size() != 1)
      {
        std::cout << "wW!";
      }
      if (p->to.size() == 0) break;
      p = p->to.front();
    }
  }

  // remove point
  i = remove_vertex( i, gp.getVertices(), gp.getFaces() );
  return true;
}


std::vector<Point2df>
VertexRemover::simple_neighborhood_flattening(
  const Point3df & point,
  const GeometricProperties::Neighborhood & neighbors )
{

  std::vector<double> angles;
  std::vector<double> norms;
  angles.reserve(neighbors.size());
  norms.reserve(neighbors.size());
  const vector<Point3df> & vert = geometricProperties().getRcMesh()->vertex();

  typename GeometricProperties::Neighborhood::const_iterator n
    = neighbors.begin();
  const_cyclic_iterator<GeometricProperties::Neighborhood>
    n2(neighbors, ++neighbors.begin());
  //std::cout << "point: " << point << std::endl;
  for (; n != neighbors.end(); ++n, ++n2)
  {
    const Point3df & p = vert[*n];
    const Point3df & p2 = vert[*n2];
    norms.push_back( double( (p - point).norm() ) );
    angles.push_back(std::acos( (p2 - point).dot(p - point)
      / ( double( (p2 - point).norm() ) * double( (p - point).norm() ) ) ) );
  }

  std::partial_sum(angles.begin(), angles.end(), angles.begin());

  std::transform(angles.begin(), angles.end(), angles.begin(),
                 std::bind2nd(std::multiplies<double>(),
                              2*M_PI/angles.back()));

  std::vector<Point2df> res(neighbors.size());
  for (std::size_t i = 0; i < res.size(); ++i)
  {
    res[i][0] = std::cos(angles[i]) * norms[i];
    res[i][1] = std::sin(angles[i]) * norms[i];
  }

  return res;
}


std::vector<Point2df>
VertexRemover::simple_neighborhood_flattening(
  const Point3df & point,
  const std::vector<VertexPointer> & neighbors )
{

  std::vector<double> angles;
  std::vector<double> norms;
  angles.reserve(neighbors.size());
  norms.reserve(neighbors.size());

  typename vector<VertexPointer>::const_iterator n
    = neighbors.begin();
  const_cyclic_iterator<vector<VertexPointer>>
    n2(neighbors, ++neighbors.begin());
  //std::cout << "point: " << point << std::endl;
  for (; n != neighbors.end(); ++n, ++n2)
  {
    const Point3df & p = (*n)->pos();
    const Point3df & p2 = (*n2)->pos();
    norms.push_back( double( (p - point).norm() ) );
    angles.push_back(std::acos( (p2 - point).dot(p - point)
      / ( double( (p2 - point).norm() ) * double( (p - point).norm() ) ) ) );
  }

  std::partial_sum(angles.begin(), angles.end(), angles.begin());

  std::transform(angles.begin(), angles.end(), angles.begin(),
                 std::bind2nd(std::multiplies<double>(),
                              2*M_PI/angles.back()));

  std::vector<Point2df> res(neighbors.size());
  for (std::size_t i = 0; i < res.size(); ++i)
  {
    res[i][0] = std::cos(angles[i]) * norms[i];
    res[i][1] = std::sin(angles[i]) * norms[i];
  }

  return res;
}


// ---

namespace
{

  struct Temp
  {
    double angle;
    std::size_t index;
    friend bool operator<(const Temp & x, const Temp & y) { return x.angle < y.angle; }
  };
  struct TempComp
  {
    bool operator()(const Temp & x, const Temp & y) { return x.angle < y.angle; }
  };


  template < typename T >
  T det( const AimsVector<AimsVector<T, 3>, 3> & m )
  {
    return m[0][0] * m[1][1] * m[2][2]
      + m[1][0] * m[2][1] * m[0][2]
      + m[2][0] * m[0][1] * m[1][2]
      - m[0][2] * m[1][1] * m[2][0]
      - m[1][2] * m[2][1] * m[0][0]
      - m[2][2] * m[0][1] * m[1][0];
  }


  /// Checks if a 2D point lies within the circumcircle of a triangle, whose vertices are given
  /// in a counterclockwise fashion.
  struct PointInCircumcircle2D_counterclockwise
  {
    bool                ///< True iff point lies within circumcircle.
    operator()
    (
      const Point2df & p,  ///< a point
      const Point2df & a,  ///< first triangle vertex
      const Point2df & b,  ///< second triangle vertex
      const Point2df & c   ///< third triangle vertex.
    )
    {
      float d = 0.0;
      AimsVector<AimsVector<float, 3>, 3> m;

      m[0][0] = a[0];
      m[1][0] = a[1];
      m[2][0] = a.norm2();
      m[0][1] = b[0];
      m[1][1] = b[1];
      m[2][1] = b.norm2();
      m[0][2] = c[0];
      m[1][2] = c[1];
      m[2][2] = c.norm2();
      d += det(m);

      m[0][2] = p[0];
      m[1][2] = p[1];
      m[2][2] = p.norm2();
      d -= det(m);

      m[0][1] = c[0];
      m[1][1] = c[1];
      m[2][1] = c.norm2();
      d += det(m);

      m[0][0] = b[0];
      m[1][0] = b[1];
      m[2][0] = b.norm2();
      d -= det(m);

      return d > 0;
    }
  };


  /// Checks if a 2D point lies within the circumcircle of a 2D triangle.
  struct PointInCircumcircle2D
  {
    bool                ///< True iff point lies within circumcircle
    operator()
    (
      const Point2df & p,  ///< a point
      const Point2df & a,  ///< first triangle vertex
      const Point2df & b,  ///< second triangle vertex
      const Point2df & c   ///< third triangle vertex
    )
    {
      // ensure a, b, c lie counter-clockwise
      if ((b[0]-a[0])*(c[1]-a[1]) - (b[1]-a[1])*(c[0]-a[0]) >= 0)
      {
        return PointInCircumcircle2D_counterclockwise()(p,a,b,c);
      }
      else
      {
        return PointInCircumcircle2D_counterclockwise()(p,a,c,b);
      }
    }
  };


  inline bool is_in_circumcircle
  (
    const Point2df & p,  ///< a point
    const Point2df & a,  ///< first triangle vertex
    const Point2df & b,  ///< second triangle vertex
    const Point2df & c   ///< third triangle vertex
  )
  {
    return PointInCircumcircle2D()(p,a,b,c);
  }

  // ---
  template < typename T >
  inline T angle(T x, T y, T norm)
  {
    if (x < 0)
    {
      if (y < 0)  return -M_PI - 2.0 * std::atan( y / (norm - x) );
      else        return  M_PI - 2.0 * std::atan( y / (norm - x) );
    }
    else          return 2.0 * std::atan( y / (norm + x) );
  }

  //--------------------------------------------------------------------------

  template < typename T >
  inline T angle(T x, T y)
  {
    T norm = x*x + y*y;
    if (norm < 128*std::numeric_limits<T>::epsilon())
    {
      return 0;
    }
    norm = std::sqrt(norm);
    return angle(x,y,norm);
  }

  // for set<Point2dul>
  class less_pt
  {
  public:
    inline
    bool operator () ( const AimsVector<size_t, 2> & p1,
                       const AimsVector<size_t, 2> & p2 ) const
    {
      return p1[1] < p2[1] || ( p1[1] == p2[1] && p1[0] < p2[0] );
    }
  };

}


std::list< AimsVector<uint, 3> >
aims::simple_delaunay_triangulation( const std::vector<Point2df> & in_points )
{
  std::vector<Point2df> points = in_points; // we will modify points
  typedef AimsVector<uint, 3> Face;
  typedef std::list<Face> FaceList;

  // Get size now before points are added
  std::size_t n = points.size();

  // Don't waste time calling this for less than 4 points.
  assert(points.size() >= 4);

  FaceList faces;

  // compute bounding triangle
  //bounding_triangle(points.begin(), points.end(), std::back_inserter(points));

  {
    float miny, k1, k2;
    k1 = k2 = -std::numeric_limits<float>::max();
    miny = std::numeric_limits<float>::max();
    for (std::size_t i = 0; i < points.size(); ++i)
    {
      miny = min(miny, points[i][1]);
      k1 = max(k1, points[i][1] - std::sqrt(3.0f)*points[i][0]);
      k2 = max(k2, points[i][1] + std::sqrt(3.0f)*points[i][0]);
    }

    Point2df M;
    M[0] = (k2 - k1) / (2*std::sqrt(3.0f));
    M[1] = 2*miny/3 + (k1+k2)/6;

    Point2df A;
    A[0] = (miny - k1) * (1/std::sqrt(3.0f));
    A[1] = miny;
    Point2df B;
    B[0] = (k2 - miny) * (1/std::sqrt(3.0f));
    B[1] = miny;
    Point2df C;
    C[0] = (k2 - k1) / (2*std::sqrt(3.0f));
    C[1] = (k1+k2)/2;

    A += 2.0f*(A-M);
    B += 2.0f*(B-M);
    C += 2.0f*(C-M);

    points.push_back(A);
    points.push_back(B);
    points.push_back(C);

    Face face( points.size()-1, points.size()-2, points.size()-3 );
    faces.push_back(face);
  }


  for (std::size_t i = 0; i < n; ++i)
  {
    //std::cout << "another point :" << points[i] << std::endl;

    // Remove triangles if we lie in their circumcircle, and collect their vertex.
    std::set<std::size_t> neighbors;
    //e.insert(0);
    //e.insert(i-1);
    {
      FaceList::iterator f = faces.begin();
      while (f != faces.end())
      {
        //std::cout << "face " << points[(*f)[0]] << " " <<  points[(*f)[1]] << " " <<  points[(*f)[2]] << ":";
        if( is_in_circumcircle( points[i], points[(*f)[0]], points[(*f)[1]],
                                points[(*f)[2]] ) )
        {
          //std::cout << "inside" << std::endl;
          neighbors.insert((*f)[0]);
          neighbors.insert((*f)[1]);
          neighbors.insert((*f)[2]);
          f = faces.erase(f);
        }
        else
        {
          //std::cout << "outside circle" << std::endl;
          ++f;
        }
      }
    }

    assert(neighbors.size() >= 3);

    // order neighbors according to the angle they make with the current point

    std::vector<Temp> tmp;
    tmp.reserve(neighbors.size());
    for (std::set<std::size_t>::iterator iNeighbor = neighbors.begin(); iNeighbor != neighbors.end(); ++iNeighbor)
    {
      Temp t;
      t.index = *iNeighbor;
      Point2df v = points[*iNeighbor]-points[i];
      t.angle = angle(v[0], v[1]);
      tmp.push_back(t);
    }
    //std::sort(tmp.begin(), tmp.end(), TempComp());
    std::sort(tmp.begin(), tmp.end());

    // Form new faces
    for (std::size_t it = 0; it < tmp.size(); ++it)
    {
      //std::cout << "Adding face " << i << " " << tmp[it].index << " " << tmp[ (it+1) % size(tmp) ].index << std::endl;
      Face face( i, tmp[it].index, tmp[ (it+1) % tmp.size() ].index );
      faces.push_back(face);
    }
  }

  /*
    {
      std::cout << "faces" << std::endl;
      for (FaceList::iterator f = faces.begin(); f != faces.end(); ++f)
      {
        std::cout << (*f)[0] << " " << (*f)[1] << " " << (*f)[2] << std::endl;
      }
      std::cout << "endfaces" << std::endl;
    }
*/

  // Remove extra faces
  {
    FaceList::iterator f = faces.begin();
    while (f != faces.end())
    {
      // Remove faces with supertriangle vertex
      if ((*f)[0] >= n ||
          (*f)[1] >= n ||
          (*f)[2] >= n)
      {
        f = faces.erase(f);
      }
      else
      {
        ++f;
      }
    }
  }
/*
    {
      FaceList::iterator f = faces.begin();
      std::cout << "cross2D: " << cross(points[(*f)[1]] - points[(*f)[0]], points[(*f)[2]] - points[(*f)[0]]) << std::endl;
    }
*/

  // Check that normals are ok
  {
    for (FaceList::iterator f = faces.begin(); f != faces.end(); ++f)
    {
      if( cross( points[(*f)[1]] - points[(*f)[0]],
                 points[(*f)[2]] - points[(*f)[0]] ) < 0 )
      {
        std::cout << "wS!";
      }
    }
  }


  // Check if we have to resort to normal testing to remove faces
  if (faces.size() != n-2)
  {
    /*
    std::cout << "NI!" << faces.size() << "-" << n << std::endl;
    {
      std::cout << "faces2" << std::endl;
      for (FaceList::iterator f = faces.begin(); f != faces.end(); ++f)
      {
        std::cout << (*f)[0] << " " << (*f)[1] << " " << (*f)[2] << std::endl;
      }
      std::cout << "endfaces" << std::endl;
    }
    */

    // Remove faces with bad normal
    /*
    {
      FaceList::iterator f = faces.begin();
      while (f != faces.end())
      {
        if (cross(points[(*f)[1]] - points[(*f)[0]], points[(*f)[2]] - points[(*f)[0]]) < 0)
        {
          f = faces.erase(f);
        }
        else
        {
          ++f;
        }
      }
    }
    */

    {
      FaceList::iterator f = faces.begin();
      while (f != faces.end())
      {
        bool flagdel = false;
        int count = 0;
        for (int i = 0; i < 3; ++i)
        {
          if ((*f)[i] > (*f)[(i+1)%3])
          {
            if (++count > 1)
            {
              flagdel = true;
              //f = faces.erase(f);
              break;
            }
          }
        }
        if (flagdel) f = faces.erase(f);
        else ++f;
      }
    }

    // Check again that we have the expected number of faces
    if (faces.size() != n-2)
    {
      std::cout << "NI2!" << faces.size() << "-" << n << std::endl;
      {
        std::cout << "faces2" << std::endl;
        for (FaceList::iterator f = faces.begin(); f != faces.end(); ++f)
        {
          std::cout << (*f)[0] << " " << (*f)[1] << " " << (*f)[2] << std::endl;
        }
        std::cout << "endfaces" << std::endl;
        std::cout << "points" << std::endl;
        for (std::size_t i = 0; i < n; ++i) std::cout << points[i] << std::endl;
        std::cout << "end points" << std::endl;
      }
    }
  }

  // check that all outer edges is missing
  {
    std::set<AimsVector<size_t, 2>, less_pt> outer_edges;
    for (FaceList::iterator f = faces.begin(); f != faces.end(); ++f)
    {
      for (std::size_t i = 0; i < 3; ++i)
      {
        std::size_t delta = std::abs(int((*f)[i]) - int((*f)[(i+1)%3]));
        if (delta == 1 || delta == (n-1))
        {
          AimsVector<size_t, 2> tmp( min((*f)[i], (*f)[(i+1)%3]),
                                     max((*f)[i], (*f)[(i+1)%3]) );
          outer_edges.insert(tmp);
        }
      }
    }
    if (outer_edges.size() != n)
    {
      std::cout << "wU!";
    }
  }

  /*
  std::cout << "nfaces: " << faces.size() << std::endl;
  for (FaceList::const_iterator i = faces.begin(); i != faces.end(); ++i)
  {
    std::cout << (*i)[0] << " " << (*i)[1] << " " << (*i)[2] << std::endl;
  }
  */
  return faces;
}


// ---

Curvature * CurvatureFactory::createCurvature(
  const AimsSurfaceTriangle & mesh, const string & method )
{
  if ( method == "fem")
    return(new FiniteElementCurvature(mesh) );
  if ( method == "boix")
    return(new BoixCurvature(mesh) );
  if ( method == "barycenter")
    return(new BarycenterCurvature(mesh) );
  if ( method =="boixgaussian")
    return(new BoixGaussianCurvature(mesh) );
  if ( method =="gaussian")
    return(new GaussianCurvature(mesh) );
  cout << "Method must be either fem, boix, barycenter, boixgaussian, "
       << "or gaussian\n";
  throw invalid_argument(string("Unknown Curvature method ") + method);
}


Curvature * CurvatureFactory::createCurvature(
  rc_ptr<AimsSurfaceTriangle> mesh, const string & method )
{
  if ( method == "fem")
    return(new FiniteElementCurvature(mesh) );
  if ( method == "boix")
    return(new BoixCurvature(mesh) );
  if ( method == "barycenter")
    return(new BarycenterCurvature(mesh) );
  if ( method =="boixgaussian")
    return(new BoixGaussianCurvature(mesh) );
  if ( method =="gaussian")
    return(new GaussianCurvature(mesh) );
  cout << "Method must be either fem, boix, barycenter, boixgaussian, "
       << "or gaussian\n";
  throw invalid_argument(string("Unknown Curvature method ") + method);
}

