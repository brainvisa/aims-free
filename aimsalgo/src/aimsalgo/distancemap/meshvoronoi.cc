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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/distancemap/meshvoronoi_d.h>
#include <graph/graph/graph.h>
#include <fstream>

 
using namespace aims::meshdistance;
using namespace aims;
using namespace std;
using namespace carto;

const short aims::meshdistance::MESHDISTANCE_FORBIDDEN = -1;
const short aims::meshdistance::MESHDISTANCE_UNREACHED = -2;


TimeTexture<short> aims::meshdistance::MeshVoronoiStepbyStep( const AimsSurface<3,Void> & mesh, 
				const Texture<short> & inittex, 
				float dist, bool connexity, bool object )
{
  Texture<short>			vor = inittex;
  Texture<float>			tex;
  const vector<Point3df>		& vert = mesh.vertex();
  const vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();

  ASSERT( inittex.nItem() == n );

  tex.reserve(n);

  // neighbours map

  map<unsigned, set<unsigned> >	neighbours;
  unsigned			v1, v2, v3;

  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      if(inittex.item(v1)!=MESHDISTANCE_FORBIDDEN 
	 && inittex.item(v2)!=MESHDISTANCE_FORBIDDEN)
	{
	  neighbours[v1].insert( v2 );
	  neighbours[v2].insert( v1 );
	}
      if(inittex.item(v1)!=MESHDISTANCE_FORBIDDEN 
	 && inittex.item(v3)!=MESHDISTANCE_FORBIDDEN) 
	{
	  neighbours[v1].insert( v3 );
	  neighbours[v3].insert( v1 );
	}
      if(inittex.item(v2)!=MESHDISTANCE_FORBIDDEN 
	 && inittex.item(v3)!=MESHDISTANCE_FORBIDDEN)  
	{
	  neighbours[v2].insert( v3 );
	  neighbours[v3].insert( v2 );
	}
    }

  // init distance texture
  if (object)
    {
      for( i=0; i<n; ++i )
	{
	  if( inittex.item(i) == 0 )
	    tex.push_back( FLT_MAX );
	  else 
	    if( inittex.item(i) == MESHDISTANCE_FORBIDDEN )
	      tex.push_back( MESHDISTANCE_FORBIDDEN );
	    else
	      tex.push_back( 0 );
	}
    }
  else
    {
      for( i=0; i<n; ++i )
	{
	  if( inittex.item(i) == 0 )
	    tex.push_back( 0  );
	  else if( inittex.item(i) == MESHDISTANCE_FORBIDDEN )
	    tex.push_back( MESHDISTANCE_FORBIDDEN );
	  else
	    tex.push_back( FLT_MAX );
	}
    }
  
  multimap<float,unsigned>	front1, front2;
  multimap<float,unsigned>	*cfront = &front1;
  multimap<float,unsigned>::iterator	iv;
  set<unsigned>				neigh;
  set<unsigned>::iterator		in, fn,iu,eu = neigh.end();
  float					d=0, d2, l;
  Point3df				pos;

  // init first front
  for( i=0; i<n; ++i )
    if( tex.item(i) == 0 )
      front1.insert( pair<float,unsigned>(  0 , i ) );


  TimeTexture<short> 	tvor;
  unsigned		inc = 0, inc2 = 0;
  //	loop until current front is empty
  while( cfront->size() > 0 && d < dist )
    {
      
      iv=cfront->begin();
      
      i = (*iv).second;
      d = (*iv).first;	
      
      cfront->erase(iv);
      neigh.erase(i);  
      for( in=neighbours[i].begin(), fn=neighbours[i].end(); in!=fn; ++in )
      {
        d2 = tex.item( *in );
        pos =  vert[i] - vert[*in];
        if (!connexity)
          l = sqrt( pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2] );
        else
          l = 1;

        if( d2 > d + l )
        {
          iu = neigh.find(*in);
          if (iu != eu)
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

      if (inc % 500 == 0)
      {
      tvor[inc2++] = vor;	
      }
      ++inc;
    }
  tvor[inc2] = vor;

  cout << inc2 <<  endl;
  
  /*
    set<short> slab;
  
    for (i=0;i<n;++i)
    slab.insert( vor.item(i) );
    slab.erase(0);
    slab.erase(MESHDISTANCE_FORBIDDEN);
    cout << "Nb of parcels: " << slab.size() << endl;
    
    slab.clear();
    for (i=0;i<n;++i)
    slab.insert( inittex.item(i) );
    slab.erase(0);
    slab.erase(MESHDISTANCE_FORBIDDEN);
    cout << "Nb of seeds: " << slab.size() << endl;
  */
  
   return( tvor );
}



Texture<short> aims::meshdistance::MeshVoronoi( const AimsSurface<3,Void> & mesh, 
						const Texture<short> & inittex, 
						float dist, bool connexity, bool object )
{
  Texture<short>			vor = inittex;
  Texture<float>			tex;
  const vector<Point3df>		& vert = mesh.vertex();
  const vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();

  ASSERT( inittex.nItem() == n );

  tex.reserve(n);

  // neighbours map

  map<unsigned, set<unsigned> >	neighbours;
  unsigned			v1, v2, v3;

  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      if(inittex.item(v1)!=MESHDISTANCE_FORBIDDEN 
	 && inittex.item(v2)!=MESHDISTANCE_FORBIDDEN)
	{
	  neighbours[v1].insert( v2 );
	  neighbours[v2].insert( v1 );
	}
      if(inittex.item(v1)!=MESHDISTANCE_FORBIDDEN 
	 && inittex.item(v3)!=MESHDISTANCE_FORBIDDEN) 
	{
	  neighbours[v1].insert( v3 );
	  neighbours[v3].insert( v1 );
	}
      if(inittex.item(v2)!=MESHDISTANCE_FORBIDDEN 
	 && inittex.item(v3)!=MESHDISTANCE_FORBIDDEN)  
	{
	  neighbours[v2].insert( v3 );
	  neighbours[v3].insert( v2 );
	}
    }
  

  // init distance texture
  if (object)
    {
      for( i=0; i<n; ++i )
	{
	  if( inittex.item(i) == 0 )
	    tex.push_back( FLT_MAX );
	  else 
	    if( inittex.item(i) == MESHDISTANCE_FORBIDDEN )
	      tex.push_back( MESHDISTANCE_FORBIDDEN );
	    else
	      tex.push_back( 0 );
	}
    }
  else
    {
      for( i=0; i<n; ++i )
	{
	  if( inittex.item(i) == 0 )
	    tex.push_back( 0  );
	  else if( inittex.item(i) == MESHDISTANCE_FORBIDDEN )
	    tex.push_back( MESHDISTANCE_FORBIDDEN );
	  else
	    tex.push_back( FLT_MAX );
	}
    }
  
  multimap<float,unsigned>	front1, front2;
  multimap<float,unsigned>	*cfront = &front1;
  multimap<float,unsigned>::iterator	iv;
  set<unsigned>				neigh;
  set<unsigned>::iterator		in, fn,iu,eu = neigh.end();
  float					d=0, d2, l;
  Point3df				pos;

  // init first front
  for( i=0; i<n; ++i )
    if( tex.item(i) == 0 )//0
      front1.insert( pair<float,unsigned>(  0 , i ) );


  TimeTexture<short> 	tvor;
  //	loop until current front is empty
  while( cfront->size() > 0 && d < dist )
    {
      
      iv=cfront->begin();
      
      i = (*iv).second;
      d = (*iv).first;	
      
      cfront->erase(iv);
      neigh.erase(i);  
      for( in=neighbours[i].begin(), fn=neighbours[i].end(); in!=fn; ++in )
      {
        d2 = tex.item( *in );
        pos =  vert[i] - vert[*in];
        if (!connexity)
          l = sqrt( pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2] );
        else
          l = 1;

        if( d2 > d + l )
        {
          iu = neigh.find(*in);
          if (iu != eu)
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


  /*
    set<short> slab;
  
    for (i=0;i<n;++i)
    slab.insert( vor.item(i) );
    slab.erase(0);
    slab.erase(MESHDISTANCE_FORBIDDEN);
    cout << "Nb of parcels: " << slab.size() << endl;
    
    slab.clear();
    for (i=0;i<n;++i)
    slab.insert( inittex.item(i) );
    slab.erase(0);
    slab.erase(MESHDISTANCE_FORBIDDEN);
    cout << "Nb of seeds: " << slab.size() << endl;
  */
  
 
  return (vor);
}



//Instanciation of template functions
namespace aims
{
  namespace meshdistance
  {
    
    
    template
    Texture<short> MeshVoronoiT<short> ( const AimsSurface<3,Void> & mesh, 
					 const Texture<short> & inittex, 
					 const short & Back, const short & For, 
					 float dist, bool connexity,bool object);

    template
    Texture<float> MeshVoronoiT<float> ( const AimsSurface<3,Void> & mesh, 
					 const Texture<float> & inittex, 
					 const float & Back, const float & For, 
					 float dist, bool connexity,bool object);

    template
    Texture<set<short> > MeshVoronoiT<set<short> >( const AimsSurface<3,Void> & mesh, 
						    const Texture<set<short> > & inittex, 
						    const set<short> & Back, const set<short> & For, 
						    float dist, bool connexity,bool object);

  }
}
