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


GeometricProperties::GeometricProperties(const AimsSurfaceTriangle & mesh) : _mesh(mesh)
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

  const vector<Point3df>			& vert = _mesh.vertex(), & normal = _mesh.normal() ;
  const vector< AimsVector<uint,3> >		& poly = _mesh.polygon();
  unsigned					i,n = vert.size();
  list<unsigned>::iterator			ilist,elist,jlist;

  _neighbourso.insert(_neighbourso.end(),n,list<unsigned>());

  _triangleNeighbourso = doTriangleNeighbor();

  for ( i=0; i<n; ++i)
  {
    set<unsigned> setinter;
    insert_iterator<set<unsigned> > ii( setinter,setinter.begin() );
    elist=_triangleNeighbourso[i].end();
    --elist;
    ilist=_triangleNeighbourso[i].begin();  // assumes --end() is valid...
    set<unsigned> tn1,tn2;
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
    _neighbourso[i].push_back(*tn1.begin() );
    _neighbourso[i].push_back(*setinter.begin());
    _neighbourso[i].push_back(*tn2.begin() );

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
      _neighbourso[i].push_back(*tn2.begin() );
    }
  }

  for (i=0; i<n;++i)
    if( _neighbourso[i].size() >= 2
        && *_neighbourso[i].begin() == *_neighbourso[i].rbegin() )
      _neighbourso[i].pop_front();//remove the repeated first element

  for (i=0; i<n;++i)
    {
      //cout << i << " " << neighbourso[i].size() << endl;
      Point3df nor = normal[i],a,b;
      //elist=neighbourso[i].end();
      ilist=_neighbourso[i].begin();
      a = vert[*ilist] - vert[i];
      jlist = ilist;
      ++jlist;
      b = vert[*jlist] - vert[*ilist];
      if ( cross(a,b).dot(nor) < 0 )//clockwise ordering
	_neighbourso[i].reverse();
    }
  
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
  set<unsigned>::iterator 	                inei,enei;

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
  set<unsigned>	        nodes;
  set<unsigned>		tmpNodes;
  bool			stopLoop=false;
  for ( i=0; i<n; ++i)
  {
    // cout << i << " -> " ;
    tmpNodes = trineigh[i];
    inei = tmpNodes.begin();
    if( inei != tmpNodes.end() )
    {
      _triangleNeighbourso[i].push_back( *inei );
      tmpNodes.erase( inei );
    }

    while ( !tmpNodes.empty() )
    {
      enei = tmpNodes.end();
      stopLoop = false;
      for ( inei=tmpNodes.begin(); inei!=enei && !stopLoop; ++inei )
      {
        nodes.clear();
        j = _triangleNeighbourso[i].back();
        nodes.insert(poly[*inei][0]);
        nodes.insert(poly[*inei][1]);
        nodes.insert(poly[*inei][2]);
        nodes.insert(poly[j][0]);
        nodes.insert(poly[j][1]);
        nodes.insert(poly[j][2]);
        if (nodes.size() == 4) // 2 adjacent triangles ?
        {
          _triangleNeighbourso[i].push_back(*inei);
          tmpNodes.erase( inei );
          stopLoop = true;
          //cout << *inei << " ";
        }
      }
    }
    // cout << endl;
  }
  return(_triangleNeighbourso);
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


// ---


VertexRemover::VertexRemover( AimsSurfaceTriangle & mesh,
                              rc_ptr<GeometricProperties> geom )
  : _mesh( mesh ), _geom( geom )
{
  if( geom.isNull() )
  {
    _geom.reset( new GeometricProperties( mesh ) );
    _geom->doNeighbor();
  }
}


// #if 0
namespace
{

  //---------------------------------------------------------------------------

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

}

bool VertexRemover::operator()( size_t i )
{
  //std::cout << "." << std::flush;

  // list of neighbors
  GeometricProperties::NeighborList & all_neigh
    = geometricProperties().getNeighbor();
  GeometricProperties::Neighborhood neighbors = all_neigh[i]; // COPY it.
  Point3df i_pos = _mesh.vertex()[i];

  // Add new faces
  list<AimsVector<uint, 3> > tri;
  // get delaunay faces for more than 4 neighbors
  if( neighbors.size() >= 4 )
  {
    tri = simple_delaunay_triangulation( simple_neighborhood_flattening(
      i_pos, neighbors ) );
  }
  // For three neighbors, simply add the only triangle possible
  else if( neighbors.size() == 3 )
  {
    AimsVector<uint, 3> tmp( 0, 1, 2 );
    tri.push_back(tmp);
  }
  // For two points or less, do not add any face. Note that if this happens, it means that the mesh
  // is in really bad shape -- actually the following might crash.
  else
  {
    std::cout << "w2!";
    return false;
  }

  // Check consistency between number of points and number of faces
  if( tri.size() != neighbors.size() - 2 )
  {
    std::cout << "wK!" << tri.size() << "-" << neighbors.size() - 2;
  }

  // remove triangles involving i

  vector<size_t> poly;
  size_t count = 0, p_index = 0, nn = neighbors.size() - 1;
  poly.reserve( nn );
  vector<AimsVector<uint, 3> >::iterator ip, jp, ep = _mesh.polygon().end();

  for( ip=_mesh.polygon().begin(); ip!=ep && count<nn; ++ip, ++p_index )
  {
    if( (*ip)[0] == i || (*ip)[1] == i || (*ip)[2] == i )
    {
      ++count;
      poly.push_back( p_index ); // record index of polygon
    }
  }

  // rewrite (partially) poly list with removed elements, and shift numbers,
  // once
  vector<size_t>::const_iterator ipv = poly.begin(), epv = poly.end();
  size_t pp = 0;
  ip = _mesh.polygon().begin(); // write iterator
  jp = ip; // read iter
  while( jp != ep ) // until all have been read
  {
    while( ipv != epv && pp == *ipv )
    {
      // skip this
      ++jp;
      ++ipv;
      ++pp;
    }
    if( jp == ep )
      break;

    if( ip != jp )
      *ip = *jp; // copy polygon
    if( (*ip)[0] >= i ) // shift vertices numbers
      --(*ip)[0];
    if( (*ip)[1] >= i )
      --(*ip)[1];
    if( (*ip)[2] >= i )
      --(*ip)[2];

    ++ip;  // increment both pointers
    ++jp;
    ++pp;
  }
  // erase the last ones
  _mesh.polygon().resize( _mesh.polygon().size() - poly.size() );

  // remove i from neighbors
  GeometricProperties::Neighborhood::iterator in, en = neighbors.end();
  for( in=neighbors.begin(); in!=en; ++in )
  {
    GeometricProperties::Neighborhood & nbr = all_neigh[*in];
    nbr.erase( std::find( nbr.begin(), nbr.end(), i ) );
  }
  // and remove the neigborhood of i
  all_neigh.erase( all_neigh.begin() + i );
  // remove vertex i in mesh
  _mesh.vertex().erase( _mesh.vertex().begin() + i );
  if( _mesh.normal().size() > i )
    _mesh.normal().erase( _mesh.normal().begin() + i );

  // shift vertices nums in neighbors (we still need them)
  nn = neighbors.size();
  for( in=neighbors.begin(), en=neighbors.end(); in!=en; ++in )
    if( *in >= i )
      --*in;
  // shift vertices nums in all_neigh
  GeometricProperties::NeighborList::iterator ian, ean = all_neigh.end();
  for( ian=all_neigh.begin(); ian!=ean; ++ian )
  {
    for( in=ian->begin(), en=ian->end(); in!=en; ++in )
      if( *in >= i )
        --*in;
  }

  // add new delaunay polygons
  // (we need an indexed access in neighbors, soon...)
  vector<unsigned> vneighbors( neighbors.begin(), neighbors.end() );
  vector<AimsVector<uint, 3> > & mpoly = _mesh.polygon();
  list<AimsVector<uint, 3> >::const_iterator it, et = tri.end();

  for( it=tri.begin(); it!=et; ++it )
  {
    const AimsVector<uint, 3> & tpoly = *it;
    // use real vertices indices, from delaunay sub-mesh
    AimsVector<uint, 3> npoly( vneighbors[tpoly[0]],
                               vneighbors[tpoly[1]],
                               vneighbors[tpoly[2]] );
    mpoly.push_back( npoly );
    // update neigbors of joined vertices
    // FIXME TODO we mess up the neighbors ordering by appending at the end
    // vertices should be reordered (how?) in each neighborhood
    {
      GeometricProperties::Neighborhood & nbr = all_neigh[npoly[0]];
      if( std::find( nbr.begin(), nbr.end(), npoly[1] ) == nbr.end() )
        nbr.push_back( npoly[1] );
      if( std::find( nbr.begin(), nbr.end(), npoly[2] ) == nbr.end() )
        nbr.push_back( npoly[2] );
    }
    {
      GeometricProperties::Neighborhood & nbr = all_neigh[npoly[1]];
      if( std::find( nbr.begin(), nbr.end(), npoly[2] ) == nbr.end() )
        nbr.push_back( npoly[2] );
      if( std::find( nbr.begin(), nbr.end(), npoly[0] ) == nbr.end() )
        nbr.push_back( npoly[0] );
    }
    {
      GeometricProperties::Neighborhood & nbr = all_neigh[npoly[2]];
      if( std::find( nbr.begin(), nbr.end(), npoly[0] ) == nbr.end() )
        nbr.push_back( npoly[0] );
      if( std::find( nbr.begin(), nbr.end(), npoly[1] ) == nbr.end() )
        nbr.push_back( npoly[1] );
    }
  }

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
  const vector<Point3df> & vert = _mesh.vertex();

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
      / double( (p2 - point).norm() ) * double( (p - point).norm() ) ) );
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
// #endif


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
                       const AimsVector<size_t, 2> & p2 )
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

Curvature * CurvatureFactory::createCurvature(const AimsSurfaceTriangle & mesh, const string & method)
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
