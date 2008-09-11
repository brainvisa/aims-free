/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#include <aims/math/math_g.h>
#include <aims/mesh/geometric.h>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <float.h>

using namespace std;
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
  float						T,phi,t;

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
      T = 0;
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
  float						T,theta,t;	


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
      T = 0;
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
      ilist=_triangleNeighbourso[i].begin();
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
      _triangleNeighbourso[i].push_back( *inei );
      tmpNodes.erase( inei );
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
  unsigned					nb,s,p,d,t;	
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
      nb =neighbourso[i].size() ;
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
  cout << "Method must be either fem, boix, barycenter, or boixgaussian \n";
  throw invalid_argument(string("Unknown Curvature method ") + method);
}
