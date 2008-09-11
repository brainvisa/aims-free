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


#include <aims/distancemap/meshparcellation_d.h>
#include <aims/distancemap/distancemap_g.h>
#include <aims/morphology/morphology_g.h>
#include <graph/graph/graph.h>
#include <fstream>

 
using namespace aims::meshdistance;
using namespace aims;
using namespace std;
using namespace carto;


AimsSurfaceTriangle aims::meshdistance::MakePlaneMesh(unsigned n, Point3df Xi, Point3df Xf)
{
  
  //vector<Point3df>		vert(n*n),normal(n*n) ;
  //vector< AimsVector<uint,3> >	poly(2*n*n -4*n +2); 
  AimsSurfaceTriangle			mesh;
  float					z,dx,dy,x0,y0;
  unsigned				i,j,k=0;



  cout << " Xi: " << Xi[0] << " " << Xi[1] << " " << Xi[2] << endl;
  cout << " Xf: " << Xf[0] << " " << Xf[1] << " " << Xf[2] << endl;
  // Xi and Xf must have the same z-coord
  //ASSERT (Xi[2] != Xf[2]); 

  dx = (Xf[0] - Xi[0])/(n-1);
  dy = (Xf[1] - Xi[1])/(n-1);
  z = Xf[2]; 
  x0 = Xi[0];
  y0 = Xi[1];
  cout << "dx= " << dx << " dy= " << dy << " n= " << n << endl;


  cout << "Create vert and normal" << endl;
  for (i=0;i<n;++i) 
    for (j=0;j<n;++j)
     {
      mesh.vertex().push_back(Point3df(x0 + i*dx, y0 + j*dy,z));
      mesh.normal().push_back(Point3df(0,0,1));
      }  
  cout << "Create poly" << endl;
  k = 0;
  for (i=0;i<n-1;++i)
    { 
      for (j=0;j<n-1;++j)
	{
	  mesh.polygon().push_back(AimsVector<uint,3>(k,k+1+n,k+1));
	  ++k;
	}
      ++k;
    }

  k = 0;
  for (i=0;i<n-1;++i)
    { 
      for (j=0;j<n-1;++j)
	{
	  mesh.polygon().push_back(AimsVector<uint,3>(k,k+n,k+n+1));
	  ++k;
	}
      ++k;
    }
 
  cout << "done \n";
  return(mesh);

}
					





Texture<set<short> > aims::meshdistance::MeshVoronoi2Set( const AimsSurface<3,Void> & mesh, 
							  const Texture<set<short> > & inittex, 
							  const Texture<short> & sulcitex,
							  bool connexity)
{
  Texture<set<short> >			vor = inittex;
  const vector<Point3df>		& vert = mesh.vertex();
  const vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();
  Texture<float>			tex;  
  set<unsigned>::iterator 		in,en;
  set<short>			       	setBack,setFor;

  ASSERT( sulcitex.nItem() == n && inittex.nItem() == n );

  setBack.insert(0);
  setFor.insert(MESHDISTANCE_FORBIDDEN);

  //init sulci wall \n
  for( i=0; i<n; ++i )
    if (sulcitex.item(i) > 0 )
	vor.item(i) = setFor ;//sulci does not belong to the object
   
  // neighbours map 

  map<unsigned, set<unsigned> >	neighbours;
  unsigned			v1, v2, v3;

  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      if(vor.item(v1) !=setFor 
	 && vor.item(v1) != setFor )
	{
	  neighbours[v1].insert( v2 );
	  neighbours[v2].insert( v1 );
	}
      if(vor.item(v1)!=setFor 
	 && vor.item(v1)!=setFor) 
	{
	  neighbours[v1].insert( v3 );
	  neighbours[v3].insert( v1 );
	}
      if(vor.item(v1)!=setFor 
	 && vor.item(v1)!=setFor)  
	{
	  neighbours[v2].insert( v3 );
	  neighbours[v3].insert( v2 );
	}
    }

  //map label_sulci -> edges \n
  map<short,set<unsigned> > labelSulci;
  for (i=0;i<n;++i)
    labelSulci[sulcitex.item(i)].insert(i);

  // init distance texture \n
  for( i=0; i<n; ++i )
    {
      if( inittex.item(i) == setBack )
	tex.push_back( FLT_MAX );
      else 
	if( inittex.item(i) == setFor )
	  tex.push_back( MESHDISTANCE_FORBIDDEN );
	else
	  tex.push_back( 0 );
    }


  multimap<float,unsigned>	front1, front2;
  multimap<float,unsigned>	*cfront = &front1;
  multimap<float,unsigned>::iterator	iv;
  set<unsigned>				neigh;
  set<unsigned>::iterator		iu,eu = neigh.end();
  float					d, d2, l;
  Point3df				pos;
 
  // init first front \n
  for( i=0; i<n; ++i )
    if( tex.item(i) == 0 )
      front1.insert( pair<float,unsigned>( 0, i ) );

  
  //loop until current front is empty
  while( cfront->size() > 0 )
    {
      
      iv=cfront->begin();
      {
        i = (*iv).second;
        d = (*iv).first;

        cfront->erase(iv);
        neigh.erase(i);
        for( in=neighbours[i].begin(), en=neighbours[i].end(); in!=en; ++in )
        {
          d2 = tex.item( *in );
          if (!connexity)
          {
            pos =  vert[i] - vert[*in];
            l = sqrt( pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2] );
          }
          else
            l = 1;
          if( d2 > d + l
              &&  vor.item( *in ) != setFor )
          {
            iu = neigh.find(*in);
            if (iu != eu )
            {
              pair<multimap<float,unsigned>::iterator,
                multimap<float,unsigned>::iterator>
                im = cfront->equal_range(tex.item( *in ) );
              multimap<float,unsigned>::iterator em;
              ++im.second;
              for (em = im.first; em != im.second && em->second != *in ;
                ++em) {}
              cfront->erase(em);
              neigh.erase(iu);
            }
            tex.item( *in ) = d + l;
            neigh.insert( *in );
            cfront->insert(pair<float,unsigned>( tex.item( *in ), *in ) );
            vor.item( *in ) = vor.item( i );
          }
        }
      }
    }

  for (i=0; i<n; ++i)
    if ( vor.item(i) == setFor )
      vor.item(i) = setBack;// sulci become background (usefull for next step of thining)

 
  return (vor);
}






set<pair<short,short> > aims::meshdistance::ModelGraph2Boundaries( const Graph & g, const map<string,short> & traduction  )
{
  set<pair<short,short> >	boundaries;
  const set<Edge*>		& edge = g.edges() ;
  set<Edge*>::const_iterator	ie,ee;
  const Edge			*gyrus;
  Edge::const_iterator		ig;
  const Vertex			*sulcus;
  string			label1,label2;
  map<string,short>::const_iterator	it,et =traduction.end();
  short 			l1;
  bool				pb;

  for (ie=edge.begin(), ee=edge.end(); ie != ee; ++ie)
    {
      gyrus = *ie;
      ig = gyrus->begin(); 
      pb = true;
      sulcus = *ig;
      sulcus->getProperty("label",label1);
      ++ig;
      sulcus = *ig;
      sulcus->getProperty("label",label2);
      it = traduction.find(label1);
      if (it != et)
	{
	  l1 = it->second;
	  it = traduction.find(label2);
	  if (it != et)
	   {
	     boundaries.insert( pair<short,short>(l1,it->second) );	 
	     pb = false;
	   }
	}
      if (pb)
	cerr << label1 << " or " << label2 << " has no translation\n"; 
    }

  return(boundaries);

}










set<set<short> > aims::meshdistance::gyrusHierarchy2setBoundaries(const map<string,short> &trans ,const Graph & g)
{
  set<set<short> > AllowedLabel;
  const set<Edge*>		& edge = g.edges() ;
  set<Edge*>::const_iterator	ie,ee;
  const Edge			*gyrus;
  Edge::const_iterator		ig,eg;
  const Vertex			*sulcus;
  string			sulcus_name;
  map<string,short>::const_iterator	it,et =trans.end();
  short 			sulcus_short;
  set<short>                    setSulci;

  for (ie=edge.begin(), ee=edge.end(); ie != ee; ++ie)
    {
      gyrus = *ie;

      //left side
      setSulci.clear();
      for ( ig = gyrus->begin(), eg = gyrus->end(); ig != eg; ++ig   )
	{
	  sulcus = *ig;
	  sulcus->getProperty("name",sulcus_name);
	  if ( sulcus_name != "unknown" )
	      sulcus_name = sulcus_name + "_left";

	  it = trans.find(sulcus_name);
	  if (it != et)
	    {
	      sulcus_short = it->second;
	      setSulci.insert(sulcus_short);
	    }
	  else
	    cerr << "Can not find the short label corresponding to the sulcus " << sulcus_name << endl;
	}
      AllowedLabel.insert(setSulci);

      //right side
      setSulci.clear();
      for ( ig = gyrus->begin(), eg = gyrus->end(); ig != eg; ++ig   )
	{
	  sulcus = *ig;
	  sulcus->getProperty("name",sulcus_name);
	  if ( sulcus_name != "unknown" )
	      sulcus_name = sulcus_name + "_right";

	  it = trans.find(sulcus_name);
	  if (it != et)
	    {
	      sulcus_short = it->second;
	      setSulci.insert(sulcus_short);
	    }
	  else
	    cerr << "Can not find the short label corresponding to the sulcus " << sulcus_name << endl;
	}
	  AllowedLabel.insert(setSulci);
	

    }

  return(AllowedLabel);

  

}



set<set<short> > aims::meshdistance::GyrusModel2SetOfSetOfSulci(const map<string, set<string> > &gyriAndSulci,const map<string,short> &trans )
{
  
  set<set<short> > AllowedLabel;
  string 	name;
  short		label;
  map<string, set<string> >::const_iterator im,em;
  set<string>::iterator is,es;
  set<short>        labTemp;
  map<string,short>::const_iterator it ,et = trans.end(); 

  for (im = gyriAndSulci.begin(), em = gyriAndSulci.end(); im != em; ++im)
    { 
      for ( is = (im->second).begin(), es = (im->second).end(); is != es; ++ is )
	{
	  name = *is;
	  it = trans.find(name);
	  if (it != et)
	    { 
	      label = it->second;
	      labTemp.insert(label);
	    }
	  else
	    {
	      cerr << "The sulci "<< name << "does not exist in the traduction file " << endl; 
	      ASSERT (0);
	    }
	  
	}
      
      AllowedLabel.insert(labTemp);
      labTemp.clear();
   
    }

  return (AllowedLabel);
}


set<string> aims::meshdistance::GyrusModel2SetOfSulci(const char *model,const std::set<string> &existingName )
{
  
  ifstream 	mf(model);
  set<string > sulci;
  set<string >::iterator is,es = existingName.end() ;
  string 	name,nameLeft,nameRight;
  
  
  while ( mf && !mf.eof() )
    {
      mf >> name ;
     
      if (!mf.eof() && !name.empty()  )
	if (name != "/")
	  {
	    if (name != "unknown")
	      nameRight = name + "_right";
	    else
	      nameRight = name;
	    
	    if (name != "unknown")
	      nameLeft= name + "_left";
	    else
	      nameLeft = name;
	    
	    is = existingName.find(nameLeft);
	    if (is != es)
	      sulci.insert(nameLeft);
	    else
	      cerr << "The sulci " << nameLeft << " does not exist at this level of translation...\n";
	    
	    is = existingName.find(nameRight);
	    if (is != es)
	      sulci.insert(nameRight);
	    else
	      cerr << "The sulci " << nameRight << " does not exist at this level of translation...\n";
	    
	    
	  }
	
    }

  return (sulci);
}


map<string, set<string> > aims::meshdistance::GyrusModel2GyriAndSulci(const char *model)
{
  
  ifstream 	mf(model);
  map<string,set<string > > gyriAndSulci;
  //set<string >::iterator is,es = existingName.end() ;
  string 	name,gyrus,sulcusLeft,sulcusRight;
  bool gyrusBool = true;
  
  while ( mf && !mf.eof() )
    {
      mf >> name ;
     
      if (!mf.eof() && !name.empty()  )
	{	
	  if ( name == "/")
	    if ( gyrusBool )
	       {
		 mf >> gyrus ;
		 gyrusBool = false;
		 mf >> name;
	       }
	    else
	      gyrusBool = true;
	  else
	    {
	      if (name != "unknown")
		sulcusRight = name + "_right";
	      else
		sulcusRight = name;
	    
	      if (name != "unknown")
		sulcusLeft= name + "_left";
	      else
		sulcusLeft = name;
	    
	      //is = existingName.find(sulcusLeft);
	      //if (is != es)
	      gyriAndSulci[gyrus + "_left"].insert(sulcusLeft);
	      //else
	      //cerr << "The sulci " << sulcusLeft << " does not exist at this level of translation...\n";
	    
	      //is = existingName.find(sulcusRight);
	      //if (is != es)		
	      gyriAndSulci[gyrus + "_right"].insert(sulcusRight);
	      //else
	      //cerr << "The sulci " << sulcusRight << " does not exist at this level of translation...\n"; 
	    }
	}
    }

  return (gyriAndSulci);
}

set<string> aims::meshdistance::GyrusModel2SetOfSulci(const map<string, set<string> > &gyriAndSulci,const std::set<string> &existingName )
{
  
  set<string > sulci;
  map<string, set<string> >::const_iterator im,em;
  set<string>::iterator ist,est;
  set<string >::iterator is,es = existingName.end() ;
  string 	name;
  
  for ( im = gyriAndSulci.begin(), em = gyriAndSulci.end(); im != em; ++im   )
  for ( ist = (im->second).begin(), est = (im->second).end(); ist != est; ++ist  ) 
    { 
      name = *ist;
      is = existingName.find(name);
      if (is != es)
	sulci.insert(name);
      else
	cerr << "The sulci " << name << " does not exist at this level of translation...\n";	
    }

  return (sulci);
}



//Instanciation of template functions
namespace aims
{
  namespace meshdistance
  {
    
    template
    std::map<set<short>,float>  SurfaceParcel(const Texture<set<short> > & tex,
					 const AimsSurface<3,Void> & mesh);
    template
    std::map<short,float>  SurfaceParcel(const Texture<short > & tex,
					 const AimsSurface<3,Void> & mesh);
    
    template
    std::map<short,float>  VolumeParcel(const AimsData<short > & vol);

    template
    Texture<set<short> > gyrusSeedDefinition<short>( const AimsSurface<3,Void> & mesh, 
						      const Texture<short> & inittex,
						      const set<short> & setBack, const set<short> & setFor,
						      const set<set<short> > & labelAllowed);

    template
    Texture<set<short> > MeshFrontiereVoronoi<short>( const AimsSurface<3,Void> & mesh, 
						      const Texture<short> & inittex,
						      const set<short> & setBack, const set<short> & setFor);
    template
    Texture<set<short> >
    gyrusSeedDilationInSKIZ<std::set<short> >( const AimsSurface<3,Void> & mesh, 
                             const Texture<std::set<short> > & seed, 
                             const Texture<std::set<short> > & skiz,
                             const std::set<short> & Back, 
                             const std::set<short> & For);

    template
    Texture<short> Frontiere2Texture<short> ( const Texture<set<short> > &tex,
					      const AimsSurface<3,Void> & mesh,
					      const set<short> & setBack, const set<short> & setFor);

    template
    Texture<short> Voronoi2toTexture<short,short>(const Texture<set<short> > & vor, 
						  const AimsSurface<3,Void> & mesh,
						  const set<short> & setBack, const set<short> & setFor);
  }
}
