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
#include <aims/mesh/curv.h>
#include <set>
#include <algorithm>
#include <float.h>
#define use_boost
#ifdef use_boost
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/operation_sparse.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#if BOOST_VERSION < 103300
typedef boost::numeric::ublas::sparse_matrix<float> boost_sparse_matrix;
#else
typedef boost::numeric::ublas::mapped_matrix<float> boost_sparse_matrix;
#endif
#endif

using namespace std;

//All the functions for 
//Curvature and Laplacian estimations

float fsign(float x) 
{
  if (x>0) return(1);
  else
    if (x<0) return(-1);
    else
      return(0);
}

 

// Cross product between two 3D points
Point3df cross(Point3df a, Point3df b)
{
  
  Point3df n;
  n[0] = a[1]*b[2] - a[2]*b[1];
  n[1] = a[2]*b[0] - a[0]*b[2];
  n[2] = a[0]*b[1] - a[1]*b[0];
  return (n);
}


TimeTexture<float> AimsMeshCurvature( const AimsSurface<3,Void> & mesh)
{
  TimeTexture<float>				tex;
  const vector<Point3df>			& vert = mesh.vertex(), & normal = mesh.normal() ;
  const vector< AimsVector<uint,3> >		& poly = mesh.polygon();
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
      bary[i] = a.norm() * fsign( a.dot(normal[i]) );
      bary2[i] = a.dot(normal[i] );
      tex[0].push_back( bary[i] ); //barycenter
      //tex[1].push_back( bary[i] );
      
    }

  return(tex);
}
 




Texture<float> AimsMeshBoixCurvature( const AimsSurface<3,Void> & mesh,
				      const vector<float> & ALPHA,
				      const vector<float> & BETA,
				      const vector<list <float> > & SURFACE)
{
  const vector<Point3df>			& vert = mesh.vertex() ;
  unsigned					i, n = vert.size();
  Texture<float>				tex(n);
  float                                         H,nume,deno,aire;
  list <float>::const_iterator              il,el;
 
  for (i=0; i<n; ++i)
    {
      nume = BETA[i];
      aire = 0;
      for (il = SURFACE[i].begin(), el = SURFACE[i].end(); il != el; ++il)
	aire += *il;

      deno = (aire * 0.5 + ALPHA[i] );
      H = nume / deno;
      tex.item(i) = H;
    }

  return(tex);
}
 

Texture<float> AimsMeshFiniteElementCurvature( const AimsSurface<3,Void> & mesh,
					       const vector< list<unsigned> > & neighbourso,
					       const vector< list<float> > & PHI,
					       const vector< list<float> > & THETA,
					       const vector< list<float> > & SURFACE,
					       const vector< list<float> > & DOT)
{
  const vector<Point3df>			& vert = mesh.vertex() ;
  unsigned					i, n = vert.size();
  Texture<float>				tex(n);
  list<unsigned>::const_iterator		ilist,elist;
  list<float>::const_iterator		        iphi,idot,itheta,isurf;
  unsigned					/*nb,*/s,p,d,t;	
  float 					K,surface;
  
  for (i=0; i<n; ++i)
    {
      s = SURFACE[i].size();
      p = PHI[i].size();
      t = THETA[i].size();
      d = DOT[i].size();
      //nb = neighbourso[i].size() ;
      if ( !( s == p && s == t && s == d && p==t && p==d && t==d ) )
	{
	  cout << "Problem with the mesh features..." << endl;
	  ASSERT(0);
	}
      K = 0;
      surface = 0;
      for ( elist =  neighbourso[i].end(),ilist = neighbourso[i].begin(),
	      iphi = PHI[i].begin(),idot = DOT[i].begin(),
	      isurf = SURFACE[i].begin(), itheta = THETA[i].begin();
	    ilist != elist;++ilist, ++iphi,++itheta,++idot,++isurf )
	{
	  K +=(*iphi + *itheta)*(*idot);
	  surface += *isurf;
	  //cout << *iphi << " " << *itheta << " " <<  *idot << " " << *isurf << endl;
	}
      
     if ( surface != 0 )
       K = K/(surface * 4);
     else
       {
	 cout << "Triangle with null surface\n";
	 K = 0;
       }
     tex.item(i) = K;
    }

  return(tex);
}





vector< list<float> > AimsMeshFiniteElementPhi( const AimsSurface<3,Void> & mesh,
						const vector< list<unsigned> > & neighbourso,
						const vector< list<float> > & surf)
{
  const vector<Point3df>			& vert = mesh.vertex() ;
  unsigned					i, n = vert.size();
  vector< list<float> >                         PHI(n);//id but triangle are put in order 
  list<unsigned>::iterator        		ilist,elist;
  list<unsigned>::const_iterator        	el;
  list<float>::const_iterator        	        efl,iflist;
  unsigned					v1, v2;
  float						phi,t;	
  vector< list<unsigned> >                     neigho = neighbourso;
  vector< list<float> >                        surfl = surf;
  
  for (i=0; i<n; ++i)
    {
      el = neighbourso[i].end();
      --el;
      neigho[i].push_front(*el);
      efl = surf[i].end();
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
	  PHI[i].push_back(phi);
	}
    }

  return(PHI);
}





vector<float> AimsMeshFiniteElementBeta( const AimsSurface<3,Void> & mesh,
					 const vector< list<unsigned> > & neighbourso)
{
  const vector<Point3df>			& vert = mesh.vertex(), &normal = mesh.normal() ;
  unsigned					i, n = vert.size();
  vector<float>                                 BETA(n); 
  list<unsigned>::iterator       		ilist,elist;
  list<unsigned>::const_iterator       		il,el;
  unsigned					v1, v2, v3;
  float						b,nj1n,nj2n;	
  Point3df                                      nj1,nj2;
  vector< list<unsigned> >                      neigho = neighbourso;

  for (i=0; i<n; ++i)
    {
      il = neighbourso[i].begin();
      el = neighbourso[i].end();
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
      BETA[i] = 0.25 * b;
    }

  return(BETA);
}





vector<float> AimsMeshFiniteElementAlpha( const AimsSurface<3,Void> & mesh,
					  const vector< list<unsigned> > & neighbourso)
{
  const vector<Point3df>			& vert = mesh.vertex() ;
  unsigned					i, n = vert.size();
  vector<float>                                 ALPHA(n); 
  list<unsigned>::iterator       		ilist,elist;
  unsigned					v1, v2;
  float						a,t;	
  Point3df                                      nj1,nj2;
  vector< list<unsigned> >                      neigho = neighbourso;

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
       ALPHA[i] = -a/8;
    }

  return(ALPHA);
}






vector< list<float> > AimsMeshFiniteElementDot( const AimsSurface<3,Void> & mesh,
						const vector< list<unsigned> > & neighbourso)
{
 
  const vector<Point3df>			& vert = mesh.vertex(), & normal = mesh.normal() ;
  unsigned					i, n = vert.size();
  vector< list<float> >                         DOT(n);
  list<unsigned>::const_iterator		ilist,elist;

  for (i=0; i<n; ++i)
    for (ilist = neighbourso[i].begin(),elist =  neighbourso[i].end(); ilist != elist ; ++ilist) 
      DOT[i].push_back(normal[i].dot( - vert[*ilist] + vert[i]) );
  //DOT[i].push_back(normal[i].dot( vert[*ilist] - vert[i]) ); //BUG in CHUNG paper ?! YES : SEE ref. Desbrun99

  return(DOT);
}






vector< list<float> > AimsMeshFiniteElementTheta( const AimsSurface<3,Void> & mesh,
						  const vector< list<unsigned> > & neighbourso,
						  const vector< list<float> > & surf)
{
  const vector<Point3df>			& vert = mesh.vertex() ;
  unsigned					i, n = vert.size();
  vector< list<float> >                         THETA(n);
  list<float>::const_iterator		        il;
  list<unsigned>::iterator       		ilist,elist;
  unsigned					v1, v2;
  float						theta,t;	
  vector< list<unsigned> >                      neigho = neighbourso;
  vector< list<float> >                         surfl=surf;

  for (i=0; i<n; ++i)//the first element is the same as the last one (circular)
    {
      neigho[i].push_back(*neighbourso[i].begin());
      surfl[i].push_back(*surf[i].begin());
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
	  THETA[i].push_back(theta);
	}
    }

  return(THETA);
}


//ie the sum of all the neighbouring triangle surface
vector< list<float> > AimsMeshFiniteElementSurface( const AimsSurface<3,Void> & mesh,
						    const vector< list<unsigned> > & neighbourso)
{
  const vector<Point3df>			& vert = mesh.vertex() ;
  unsigned					i, n = vert.size();
  vector< list<float> >                         SURFACE(n);
  list<unsigned>::iterator			ilist,elist;
  unsigned					v1, v2;
  float						t;	
  vector< list<unsigned> >                      neigho = neighbourso;

  for (i=0; i<n; ++i)
    neigho[i].push_back(*neighbourso[i].begin());//the first element is the last one (circular)

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
	  SURFACE[i].push_back(t);
	}
    }


  return(SURFACE);
}


vector< list<unsigned> > AimsMeshOrderNode(const AimsSurface<3,Void> & mesh)
{
  const vector<Point3df>			& vert = mesh.vertex(), & normal = mesh.normal() ;
  const vector< AimsVector<uint,3> >		& poly = mesh.polygon();
   unsigned					i,n = vert.size();
   vector<list<unsigned> >                       neighbourso(n);//id but nodes are put in order
  vector< list<unsigned> >                      trineigho(n);//id but triangle are put in order
  list<unsigned>::iterator			ilist,elist,jlist;

  trineigho = AimsMeshOrderTriangle(mesh);

  for ( i=0; i<n; ++i)
    {
      set<unsigned> setinter;
      insert_iterator<set<unsigned> > ii( setinter,setinter.begin() );
      elist=trineigho[i].end();
      --elist;
      ilist= trineigho[i].begin();
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
      neighbourso[i].push_back(*tn1.begin() );
      neighbourso[i].push_back(*setinter.begin());
      neighbourso[i].push_back(*tn2.begin() );
      
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
	  neighbourso[i].push_back(*tn2.begin() );
	}
    }

  for (i=0; i<n;++i)
    neighbourso[i].pop_front();//remove the repeated first element

  for (i=0; i<n;++i)
    {
      //cout << i << " " << neighbourso[i].size() << endl;
      Point3df nor = normal[i],a,b;
      //elist=neighbourso[i].end();
      ilist=neighbourso[i].begin();
      a = vert[*ilist] - vert[i];
      jlist = ilist;
      ++jlist;
      b = vert[*jlist] - vert[*ilist];
      if ( cross(a,b).dot(nor) < 0 )//clockwise ordering
	neighbourso[i].reverse();
    }
  
 
  return(neighbourso);
}




vector< list<unsigned> > AimsMeshOrderTriangle(const AimsSurface<3,Void> & mesh)
{
  const vector<Point3df>			& vert = mesh.vertex() ;
  const vector< AimsVector<uint,3> >		& poly = mesh.polygon();
  unsigned					i,j,n = vert.size();
  unsigned					v1, v2, v3;
  map<unsigned, set<unsigned> >			neighbours;//give for each vertex a list of the  adjacent triangles 
  vector< set <unsigned> >                      trineigh(n); //give for each vertex a list of the  adjacent triangles 
  vector< list<unsigned> >                      trineigho(n);//id but triangle are put in order 
  set<unsigned>::iterator 	                inei,enei;

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
      trineigho[i].push_back( *inei );
      tmpNodes.erase( inei );
      while ( !tmpNodes.empty() )
	{
	  enei = tmpNodes.end();
	  stopLoop = false;
	  for ( inei=tmpNodes.begin(); inei!=enei && !stopLoop; ++inei )
	    {
	      nodes.clear();
	      j = trineigho[i].back();
	      nodes.insert(poly[*inei][0]);
	      nodes.insert(poly[*inei][1]);
	      nodes.insert(poly[*inei][2]);
	      nodes.insert(poly[j][0]);
	      nodes.insert(poly[j][1]);
	      nodes.insert(poly[j][2]);
	      if (nodes.size() == 4) // 2 adjacent triangles ?
		{
		  trineigho[i].push_back(*inei);
		  tmpNodes.erase( inei );
		  stopLoop = true;
		  //cout << *inei << " ";
		}
	    }
          if( !stopLoop )
          {
            cout << "warning, mesh is not closed.\n";
            tmpNodes.clear();
          }
	}
      // cout << endl;
    } 
  return(trineigho);
}






Texture<float> AimsMeshLaplacian( const Texture<float> &inittex, 
                                  const map<unsigned, set< pair<unsigned,float> > > &lapl)
{
  unsigned					n =inittex.nItem();
  Texture<float>				tex(n);
  AimsMeshLaplacian( inittex.data(), tex.data(), lapl );

  return(tex);
}


template <typename T>
void AimsMeshLaplacian( const vector<T> &inittex, vector<T> & outtex,
                        const map<unsigned, set< pair<unsigned,float> > > &lapl)
{
  unsigned neigh,node, n =inittex.size();
  map<unsigned, set< pair<unsigned,float> > >::const_iterator il,el;
  set< pair<unsigned,float> >::iterator      ip,ep;
  float                                        L,weight;
  ASSERT ( lapl.size() == n);
  // resize/clear output texture
  if( outtex.size() != n )
    outtex.resize( n );

  for (il = lapl.begin(), node=0, el = lapl.end(); il != el; ++il)
    {
      node = il->first;
      L = 0;

      //Weighted sum on the neighbour of the node
      for ( ip = (il->second).begin(), ep = (il->second).end(); ip != ep; ++ip    )
        {
          neigh = ip->first;
          weight = ip->second;
          L += weight * (- inittex[node] + inittex[neigh]);
        }
        
      outtex[node] = L;
    }
}


template void AimsMeshLaplacian( const vector<float> &inittex,
  vector<float> & outtex,
  const map<unsigned, set< pair<unsigned,float> > > &lapl);
template void AimsMeshLaplacian( const vector<double> &inittex,
  vector<double> & outtex,
  const map<unsigned, set< pair<unsigned,float> > > &lapl);









map<unsigned, set< pair<unsigned,float> > >  AimsMeshWeightFiniteElementLaplacian( const AimsSurface<3,Void> & mesh, 
                                      const float Wmax)

{
  unsigned					neigh,i, n =mesh.vertex().size();
  list<unsigned>::const_iterator		ilist,elist;
  list<float>::const_iterator		        iphi,itheta,isurf,esurf;
  float						weight,s,p,t,surface;	
  map<unsigned, set< pair<unsigned,float> > > lapl;
  map<unsigned, set< pair<unsigned,float> > >::iterator il,el;
  vector< list<unsigned> > neighbourso;
  vector< list<float> >  PHI;
  vector< list<float> > THETA;
  vector< list<float> >  SURFACE;
  set<float> values;

  cout << "Node ordering..." << flush;
  neighbourso = AimsMeshOrderNode(mesh);
  cout << "done \n";
  cout << "Extract mesh features..." <<flush;
  SURFACE =  AimsMeshFiniteElementSurface(mesh,neighbourso);
  PHI = AimsMeshFiniteElementPhi(mesh,neighbourso,SURFACE);
  THETA = AimsMeshFiniteElementTheta(mesh,neighbourso,SURFACE);
  cout << "done \n";

  for (i=0; i<n; ++i)
  {
    s = SURFACE[i].size()  ;
    p = PHI[i].size();
    t = THETA[i].size();
    if ( !( s == p && s == t && p==t ) )
    {
      cout << "Problem with the mesh features..." << endl;
      ASSERT(0);
    }
    elist =  neighbourso[i].end();
    ilist = neighbourso[i].begin();
    iphi = PHI[i].begin();
    isurf = SURFACE[i].begin();
    esurf = SURFACE[i].end();
    itheta = THETA[i].begin();
    surface = 0;
    while ( isurf != esurf )
    {
      surface += *isurf;
      ++isurf;
    }

    while ( ilist != elist )
    {
      neigh = *ilist;

      if (surface != 0 )
        weight = (*iphi + *itheta) / surface;
      else
      {
        cerr << "Triangle(s) with null surface on vertex " << i << endl;
        // ASSERT (0);
        weight = 0.;
      }

      values.insert(weight);
      lapl[i].insert( pair<unsigned,float>(neigh,weight) );
      ++ ilist;
      ++iphi;
      ++itheta;
    }
  }

  //Threshold definition
  set<float>::iterator iv;
  set<float>::reverse_iterator riv;
  unsigned   nbValues,inc = 0 ;
  set< pair<unsigned,float> >::iterator isp,esp;
  float Wm, WM;
  nbValues = (unsigned) rint ( (1 - Wmax) * values.size() );
  cout << "Thresholding  " << 100*(1 - Wmax) << "% of the Laplacian weights (" << nbValues << "/" <<  values.size() << ")" << endl;
  iv = values.begin();
  riv = values.rbegin();
  cout << "Weights:" << endl << "Before: min: " << *iv << " max: " << *riv << endl;
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
  cout << "After: min: " << Wm << " max: " << WM << endl;
  for (il = lapl.begin(), el = lapl.end(); il != el; ++il)
    for ( isp = (il->second).begin(), esp = (il->second).end(); isp != esp; ++isp)
    {
      neigh = isp->first;
      weight = isp->second;
      if (weight > WM)
      {
        il->second.erase(pair<unsigned,float>(neigh,weight) );
        il->second.insert(pair<unsigned,float>(neigh,WM) );
      }
      else
        if (weight < Wm)
        {
          il->second.erase(pair<unsigned,float>(neigh,weight) );
          il->second.insert(pair<unsigned,float>(neigh,Wm) );
        }
    }

  return(lapl);
}



float AimsMeshFiniteElementDt(  const Texture<float> &tex,
			       const Texture<float> &lapl,
			       float tmax)
{
  set<float>      s1,s2;
  float           maxv,minv,dt;
  unsigned        i,n=tex.nItem();

  for (i=0; i<n; ++i)
    s1.insert(tex.item(i));

  maxv = *s1.rbegin();
  minv = *s1.begin();

  for (i=0; i<n; ++i)
    if (lapl.item(i) != 0 && fabs(tex.item(i)) < tmax)
      {
      s2.insert( fabs((maxv - tex.item(i)) / lapl.item(i)));
      s2.insert( fabs((minv - tex.item(i)) / lapl.item(i)));
      }	  
  s2.erase(0);
  dt = *s2.begin();

  return(dt);
}







//Regularization of the  curvature map
//Threshold outlier curvature values 
//From mean and std deviation
//The value of the thresholded node
//is then the average of its neighbourhood
Texture<float> AimsRegularizeTexture( const Texture<float> &tex,
				      const AimsSurface<3,Void> & mesh,
				      float alpha) 
{

  const vector<Point3df>		& vert = mesh.vertex();
  const vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();
  map<unsigned, set<unsigned> >		neighbours;
  set<unsigned>::iterator		il,el;
  unsigned				v1, v2, v3;
  Texture<float>			outTex;

  ASSERT( tex.nItem() == n );
  
  //Neighbouring 
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
  
  
  float max=-FLT_MAX, min=FLT_MAX,mean = 0, std = 0, Hmax, nval;

  for (i = 0; i < tex.nItem(); ++i)
    {
      if (tex.item(i)>max)
	max = tex.item(i);
      if (tex.item(i)<min)
	min = tex.item(i);
      outTex.push_back( tex.item(i) );
    }

  for (i = 0; i < tex.nItem(); ++i)
    mean += tex.item(i);
  mean = mean/tex.nItem();
  
  for (i = 0; i < tex.nItem(); ++i)
    std += (tex.item(i) - mean) * (tex.item(i) - mean) ;
  std = sqrt(std / tex.nItem() );
  
  Hmax = mean +  alpha * std;
  
  cout << "min: " << min << " max: " << max 
       << " mean:" << mean << " std: " << std << " Hmax:" << Hmax << endl;
  
  unsigned inc1 = 0, inc2 = 0; 
  Point3df pos;
  float    d;

  for (i = 0; i < tex.nItem(); ++i)
    {
      if ( (tex.item(i) > Hmax) || (tex.item(i)< -Hmax) )
	{
	  ++inc1;
	  nval = 0;
	  mean = 0;
	  for ( il=neighbours[i].begin(), el=neighbours[i].end(); il!=el; ++il )
	    if( (tex.item(*il) < Hmax) && (tex.item(*il) > -Hmax) )
	      {
		pos = vert[i] - vert[*il];
		d = sqrt(pos[0]*pos[0] + pos[2]*pos[2] + pos[1]*pos[1]) ;
		mean += tex.item(*il) / d  ;
		nval += 1/d ;
	      }
	  if (nval != 0) 
	  outTex.item(i) = mean / nval; 
	  else
	    {
	      ++inc2;
	      outTex.item(i) = fsign(tex.item(i)) * Hmax;
	    }
	}
    }    
  cout << "Nb of regularized points: "<< inc1 << endl; 
  cout << "Nb of non regularized points: " << inc2 <<  endl;
  
  return(outTex);


}

Texture<float> AimsRegularizeTexture(const Texture<float> & tex,
				     float ratio )
{

  Texture<float>                otex = tex;
  set<float>                    values; 
  unsigned 			 i, n = tex.nItem();
  set<float>::iterator iv;
  set<float>::reverse_iterator riv;
  unsigned   nbValues,inc = 0 ;
  float Wm, WM;

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
      otex.item(i) = WM;
    else
      if (tex.item(i) < Wm)
	otex.item(i) = Wm;

  return(otex);
}


namespace aims
{

  void makeLaplacianMatrix( const LaplacianWeights & weights,
                            LaplacianWeights & lmat, float dt )
  {
    /* make weights matrix A = I + dt * W
       W diagonal being substracted the sum of weights of its line.
       so that applying the laplacian smoothing is a matricial operation:
       X(t+1) = A.X(t)
       and X(t) = W^t.X(0)
    */
    cout << "makeLaplacianMatrix...\n";
    float w = 0;
    unsigned i, j;
    LaplacianWeights::const_iterator iw, ew = weights.end();

    for( iw=weights.begin(); iw!=ew; ++iw )
    {
      set<pair<unsigned int, float> >::const_iterator
        ij, ej = iw->second.end();
      w = 0;
      i = iw->first;
      set<pair<unsigned int, float> > & ow = lmat[ i ];

      for( ij=iw->second.begin(); ij!=ej; ++ij )
      {
        j = ij->first;
        if( i == j )
        {
          cout << "problem: ii found: " << i << endl;
        }
        else
        {
          w += ij->second;
          ow.insert( make_pair( j, ij->second * dt ) );
        }
      }
      ow.insert( make_pair( i, 1. - dt * w ) );
    }
    cout << "makeLaplacianMatrix done.\n";
  }


  void makeLaplacianMatrix( const LaplacianWeights & weights,
                            boost_sparse_matrix & lmat,
                            float dt )
  {
    /* make weights matrix A = I + dt * W
       W diagonal being substracted the sum of weights of its line.
       so that applying the laplacian smoothing is a matricial operation:
       X(t+1) = A.X(t)
       and X(t) = W^t.X(0)
    */
    cout << "makeLaplacianMatrix... " << weights.size() << endl;
    float w = 0;
    unsigned i, j;
    lmat.resize( weights.size(), weights.size() );
    LaplacianWeights::const_iterator iw, ew = weights.end();

    for( iw=weights.begin(); iw!=ew; ++iw )
    {
      set<pair<unsigned int, float> >::const_iterator
        ij, ej = iw->second.end();
      w = 0;
      i = iw->first;

      for( ij=iw->second.begin(); ij!=ej; ++ij )
      {
        j = ij->first;
        if( i == j )
        {
          cout << "problem: ii found: " << i << endl;
        }
        else
        {
          w += ij->second;
          lmat( i, j ) = ij->second * dt;
        }
      }
      lmat( i, i ) = 1. - dt * w;
    }
    cout << "makeLaplacianMatrix done: " << lmat.size1() << " x " << lmat.size2() << endl;
  }


  void laplacianMatrixThreshold( LaplacianWeights & lmat, float threshold )
  {
    /* remove weights under threshold (in absolute value) on non-diagonal
       coefs, and re-norm diagonal to ensure conservation of the total weight
    */
    cout << "laplacianMatrixThreshold...\n";
    float w = 0, t;
    unsigned i, j;
    LaplacianWeights::iterator iw, ew = lmat.end();

    for( iw=lmat.begin(); iw!=ew; ++iw )
    {
      set<pair<unsigned int, float> > & line = iw->second;
      set<pair<unsigned int, float> >::const_iterator
        ij = line.begin(), ej = line.end(), ti, ii = ej;
      w = 0;
      i = iw->first;
      while( ij!=ej )
      {
        j = ij->first;
        if( i == j )
        {
          ii = ij;
          ++ij;
        }
        else
        {
          t = ij->second;
          if( fabs( t ) <= threshold )
          {
            ti = ij;
            ++ij;
            // remove matrix entry here
            line.erase( ti );
          }
          else
          {
            w += t;
            ++ij;
          }
        }
      }
      // diagonal coef: 1 - sum( line weights )
      if( ii != ej )
        line.erase( ii );
      line.insert( make_pair( i, 1. - w ) );
    }
    cout << "laplacianMatrixThreshold done.\n";
  }


  void laplacianMatrixThreshold( boost_sparse_matrix & lmat, float threshold )
  {
    /* remove weights under threshold (in absolute value) on non-diagonal
       coefs, and re-norm diagonal to ensure conservation of the total weight
    */
    cout << "laplacianMatrixThreshold...\n";
    float w = 0, t;
    unsigned i, j;
    boost_sparse_matrix::iterator1 iw, ew = lmat.end1();
    boost_sparse_matrix::iterator2 jw, ejw;

    for( iw=lmat.begin1(); iw!=ew; ++iw )
    {
      boost_sparse_matrix::iterator2
        ij = iw.begin(), ej = iw.end();
      w = 0;
      i = iw.index1();
      while( ij!=ej )
      {
        j = ij.index2();
        if( i == j )
          ++ij;
        else
        {
          t = *ij;
          if( fabs( t ) <= threshold )
          {
            ++ij;
            // remove matrix entry here
#if BOOST_VERSION < 103300
            lmat.erase( i, j );
#else
            lmat.erase_element( i, j );
#endif
          }
          else
          {
            w += t;
            ++ij;
          }
        }
      }
      // diagonal coef: 1 - sum( line weights )
      lmat( i, i ) =1. - w;
    }
    cout << "laplacianMatrixThreshold done.\n";
  }


  LaplacianWeights* sparseMult( const LaplacianWeights & in1,
                                const LaplacianWeights & in2,
                                float sparseThresh )
  {
#ifdef use_boost
    cout << "sparseMult...\n";
    LaplacianWeights *out = new LaplacianWeights;
    unsigned ncoef = 0; // debug

    LaplacianWeights::const_iterator iin1, ein1 = in1.end(),
      iin2, ein2 = in2.end();
    set<pair<unsigned int, float> >::const_iterator ik, ek, ik2, ek2;
    cout << "convert to boost sparse matrix...\n";
    boost_sparse_matrix
      mat1( in1.size(), in1.size() ), mat2( in2.size(), in2.size() );
    unsigned i;
    for( iin1=in1.begin(); iin1!=ein1; ++iin1 )
    {
      i = iin1->first;
      for( ik=iin1->second.begin(), ek=iin1->second.end(); ik!=ek; ++ik )
        mat1( i, ik->first ) = ik->second;
    }
    for( iin2=in2.begin(); iin2!=ein2; ++iin2 )
    {
      i = iin2->first;
      for( ik=iin2->second.begin(), ek=iin2->second.end(); ik!=ek; ++ik )
        mat2( i, ik->first ) = ik->second;
    }
    cout << "convert done. Sizes: " << mat1.size1() << " x " << mat1.size2()
      << ", " << mat2.size1() << " x " << mat1.size2() << endl;
    boost_sparse_matrix mat3( mat1.size1(), mat2.size2() );
    boost::numeric::ublas::sparse_prod( mat1, mat2, mat3 );
    cout << "mult done. converting back...\n";
    boost_sparse_matrix::iterator1 il, el = mat3.end1();
    boost_sparse_matrix::iterator2 ic, ec;
    for( il=mat3.begin1(); il!=el; ++il )
    {
      set<pair<unsigned int, float> > & line = (*out)[ il.index1() ];
      for( ic=il.begin(), ec=il.end(); ic!=ec; ++ic )
      {
        line.insert( make_pair( ic.index2(), *ic ) );
        ++ncoef;
      }
    }

    sparseThresh = sparseThresh; // compilation warning...
    cout << "out size: " << mat3.size1() << " x " << mat3.size2() << " / " << out->size() << endl;
    cout << "\nsparseMult done, weights num: " << ncoef << endl;
    return out;

#else

    cout << "sparseMult...\n";
    LaplacianWeights *out = new LaplacianWeights;
    unsigned ncoef = 0; // debug

    vector<map<unsigned, float> > lin2( in2.size() );
    LaplacianWeights::const_iterator iin1, ein1 = in1.end(),
      iin2, ein2 = in2.end();
    set<pair<unsigned int, float> >::const_iterator ik, ek, ik2, ek2;
    unsigned i, j, k, n = in1.size();
    map<unsigned, float>::iterator ic, ec;

    // transpose in2 for faster access
    for( iin2=in2.begin(); iin2!=ein2; ++iin2 )
    {
      i = iin2->first;
      for( ik2=iin2->second.begin(), ek2=iin2->second.end(); ik2!=ek2;
          ++ik2 )
        lin2[ ik2->first ][ i ] = ik2->second;
    }
    cout << "transposition done.\n";

    // out(i,j) = S_k( in1(i,k) * in2(k,j) )
    // with in2(k,j) = lin2(j,k)

    for( iin1=in1.begin(); iin1!=ein1; ++iin1 )
    {
      i = iin1->first;
      if( i % 1000 == 0 )
        cout << "\rline: " << i << flush;
      set<pair<unsigned int, float> > & oline = (*out)[i];
      const set<pair<unsigned int, float> > & iline = iin1->second;
      ek = iline.end();

      for( j=0; j<n; ++j )
      {
        map<unsigned, float> & col2 = lin2[j];
        ec = col2.end();
        float oij = 0.F;
        if( col2.size() <= iline.size() )
        {
          // iterate on in2 cols
          for( ic=col2.begin(), ec=col2.end(); ic!=ec; ++ic )
          {
            k = ic->first;
            ik = iline.lower_bound( make_pair( k, -FLT_MAX ) );
            if( ik != ek && ik->first == k )
              oij += ik->second * ic->second;
          }
        }
        else
        {
          // iterate on in1 lines
          ic = col2.begin();
          for( ik=iline.begin(); ik!=ek; ++ik )
          {
            k = ik->first;
            while( ic != ec && ic->first < k )
              ++ic;
            if( ic != ec && ic->first == k )
            {
              oij += ik->second * ic->second;
            }
          }
        }
//         for( ic=col2.begin(), ec=col2.end(); ic!=ec; ++ic )
//         {
//           iin2 = in2.find( ik->first );
//           if( iin2 != ein2 )
//           {
//             ik2 = iin2->second.lower_bound( make_pair( j, -FLT_MAX ) );
//             if( ik2 != iin2->second.end() && ik2->first == j )
//               oij += ik->second * ik2->second;
//           }
//         }
        if( fabs( oij ) > sparseThresh )
        {
          oline.insert( make_pair( j, oij ) );
          ++ncoef;
        }
      }
    }

    cout << "\nsparseMult done, weights num: " << ncoef << endl;
    return out;
#endif
  }


  LaplacianWeights*
    makeLaplacianSmoothingCoefficients( const LaplacianWeights & weights,
                                        unsigned niter, float dt,
                                        float sparseThresh )

  {
#ifdef use_boost
    boost_sparse_matrix weightLaplMat;
    makeLaplacianMatrix( weights, weightLaplMat, dt ); // matricial representation
    laplacianMatrixThreshold( weightLaplMat, sparseThresh );
    boost_sparse_matrix
      *weightLaplPow = &weightLaplMat,
      *weightLapl2, *weightLapl3 = 0;

    unsigned t, iterbin = niter;
    cout << "size: " << weightLaplMat.size1() << " x " << weightLaplMat.size2() << ", niter: " << niter << endl;

    if( iterbin & 1 ) // small bit is set: keep weightLaplMat
    {
      weightLapl3 = &weightLaplMat;
    }
    iterbin = iterbin >> 1;

    for( t=0; iterbin!=0; ++t, iterbin=iterbin>>1 )
    {
      //if (t%10 == 0)
      {
        cout << "                ";
        cout << "\r" << rint(100.*t/log(float(niter))*log(2.)) << "%" << flush;
      }
      weightLapl2 = new boost_sparse_matrix(
        weightLaplMat.size1(), weightLaplMat.size2() );
      // multiply the weights matrix to power niter
      sparse_prod( *weightLaplPow, *weightLaplPow, *weightLapl2 );
      // renormalize diagonal
      laplacianMatrixThreshold( *weightLapl2, sparseThresh );
      if( weightLaplPow != & weightLaplMat && weightLaplPow != weightLapl3 )
        delete weightLaplPow;
      weightLaplPow = weightLapl2;
      // weightLapl2 is weightLaplMat ^ (2^t)
      if( iterbin & 1 )
      {
        // keep weightLaplPow * weightLapl3 in weightLapl3
        if( weightLapl3 )
        {
          weightLapl2 = weightLapl3;
          weightLapl3 = new boost_sparse_matrix(
          weightLaplMat.size1(), weightLaplMat.size2() );
          sparse_prod( *weightLaplPow, *weightLapl2, *weightLapl3 );
          if( weightLapl2 != & weightLaplMat )
            delete weightLapl2;
          laplacianMatrixThreshold( *weightLapl3, sparseThresh );
        }
        else
          weightLapl3 = weightLaplPow;
      }
    }
    weightLaplPow = weightLapl3; // result
    if( !weightLaplPow ) // zero iterations
      weightLaplPow = &weightLaplMat; // take initial matrix

    // convert to LaplacianWeights type
    LaplacianWeights *weightLaplPowL = new LaplacianWeights;
    boost_sparse_matrix::iterator1 il, el = weightLaplPow->end1();
    boost_sparse_matrix::iterator2 ic, ec;
    for( il=weightLaplPow->begin1(); il!=el; ++il )
    {
      set<pair<unsigned int, float> >
        & line = (*weightLaplPowL)[ il.index1() ];
      for( ic=il.begin(), ec=il.end(); ic!=ec; ++ic )
        line.insert( make_pair( ic.index2(), *ic ) );
    }
    cout << "makeLaplacianSmoothingCoefficients done, size: " << weightLaplPow->size1() << " x " << weightLaplPow->size2() << endl;
    if( weightLaplPow != &weightLaplMat )
      delete weightLaplPow;
    return weightLaplPowL;

#else

    LaplacianWeights weightLaplMat;
    makeLaplacianMatrix( weights, weightLaplMat, dt ); // matricial representation
    laplacianMatrixThreshold( weightLaplMat, sparseThresh );
    LaplacianWeights *weightLaplPow = &weightLaplMat, *weightLapl2;

    unsigned t;

    for( t=0; t<niter; ++t )
    {
      //if (t%10 == 0)
      {
        cout << "                ";
        cout << "\r" << rint(100*t/niter) << "%" << flush;
      }
      // multiply the weights matrix to power niter
      weightLapl2 = sparseMult( *weightLaplPow, weightLaplMat,
                                sparseThresh );
      // renormalize diagonal
      laplacianMatrixThreshold( *weightLapl2, sparseThresh );
      if( t != 0 )
        delete weightLaplPow;
      weightLaplPow = weightLapl2;
    }
    return weightLaplPow;
#endif
  }


  template <typename T>
  void applyLaplacianMatrix( const vector<T> &inittex, vector<T> & outtex,
                             const LaplacianWeights &lapl )
  {
    unsigned neigh,node, n =inittex.size();
    map<unsigned, set< pair<unsigned,float> > >::const_iterator il,el;
    set< pair<unsigned,float> >::iterator      ip,ep;
    float                                      L,weight;
    ASSERT ( lapl.size() == n);
    // resize/clear output texture
    if( outtex.size() != n )
      outtex.resize( n );

    for( il = lapl.begin(), el = lapl.end(); il != el; ++il )
    {
      node = il->first;
      L = 0;

      //Weighted sum on the neighbour of the node
      for( ip = (il->second).begin(), ep = (il->second).end(); ip != ep;
           ++ip )
      {
        neigh = ip->first;
        weight = ip->second;
        L += weight * inittex[neigh];
      }

      outtex[node] = L;
    }
  }


  template void applyLaplacianMatrix( const vector<float> &inittex,
    vector<float> & outtex, const LaplacianWeights &lapl);
  template void applyLaplacianMatrix( const vector<double> &inittex,
    vector<double> & outtex, const LaplacianWeights &lapl);

}


