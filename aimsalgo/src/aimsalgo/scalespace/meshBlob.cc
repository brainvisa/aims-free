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


#include <aims/scalespace/meshBlob.h>
#include <aims/vector/vector.h>
#include <aims/io/writer.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/mesh/surfacegen.h>
#include <aims/graph/graphmanip.h>
#include <aims/io/datatypecode.h>
#include <graph/graph/graph.h>
#include <stdio.h>
#include <float.h>


using namespace aims;
using namespace carto;
using namespace std;


namespace aims
{
  struct ScaleSpace_local_Blob
  {
    ScaleSpace_local_Blob() : growing( true ) {}

    set<unsigned>	vertex;
    bool		growing;
  };
}


Texture<int> 
ScaleSpace::meshBlobExtract( const AimsSurface<3,Void> & mesh, 
			     const Texture<float> & inittex, 
			     map<int, BlobDescriptor> & limits )
{
  Texture<int>				tex;
  const vector<Point3df>		& vert = mesh.vertex();
  const vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();
  map<int,ScaleSpace_local_Blob>	blobs;
  map<int,ScaleSpace_local_Blob>::iterator	ila, ela = blobs.end();

  if( inittex.nItem() != n )
    {
      cerr << "meshBlobExtract : unmatching vertex / texture numbers : " 
           << n << " vertices / " << inittex.nItem() << " textures\n";
      ASSERT( 0 ); // oui c'est pas beau.
    }
    
  tex.reserve( n );

  // neighbours map

  map<unsigned, set<unsigned> >	neighbours;
  multimap<float, unsigned> intensitynode; 
  unsigned			v1, v2, v3;
  const int BOTTOM = 0;

   // init texture
  for( i=0; i<n; ++i )
    {
      tex.push_back( BOTTOM );
      intensitynode.insert( pair<float, unsigned>( inittex.item(i), i ) );
    }
   
  //Detect  connectivity
  cout << "Creating connectivity\n"; 
  
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

  short label = 0;

  set<unsigned>::iterator			in, fn;
  multimap<float,unsigned>::reverse_iterator	im, fm;
  float 					intensity;
  unsigned 					vertex, v;
  bool						flag;
  int						label_neighbours, b;

  unsigned nsup, nbifurc = 0, nsaddle = 0;

  cout << "Extracting blobs\n";
 
  for ( im = intensitynode.rbegin(), fm=intensitynode.rend();im!=fm; ++im) 
    {
      intensity=(*im).first;
      vertex=(*im).second;

      //How many higher neighbours ? 
      set<unsigned>			vertex_sup;

      for( in=neighbours[vertex].begin(), fn=neighbours[vertex].end(); 
	   in!=fn; ++in )
	{
	  if ( inittex.item(*in) > intensity ) 
	    vertex_sup.insert(*in);
	}
      nsup = vertex_sup.size();

      flag=false;
      if( nsup == 0 )	// local max : new blob
	{
	  for( ila=blobs.begin(), label=1; ila!=ela && (*ila).first == label; 
	       ++ila, ++label ) {}		// find free label num
	  ScaleSpace_local_Blob	& blob = blobs[ label ];  // create new blob

	  tex.item(vertex) = label;
	  blob.vertex.insert( vertex );
	  limits[label].maximum = vertex;
	  flag=true;
	}

      else if( nsup == 1 && tex.item(*vertex_sup.begin() ) == BOTTOM )
	// growing bottom
	{
	  tex.item(vertex) = BOTTOM;
	  flag=true;
	}

      else if( nsup > 1 )
	{ 
	  set<int>		blob_neighbours, ptblobs, realblobs;
	  unsigned		bottom = 0;
	  set<int>::iterator	jn,jfn;
	  int			b2;

	  for( in=vertex_sup.begin(), fn=vertex_sup.end(); in!=fn; ++in )
	    {
	      b = tex.item(*in);
	      blob_neighbours.insert( b );
	      if( b == BOTTOM || !blobs[b].growing )
		bottom = 1;
	      else if( blobs[b].vertex.size() == 1 )
		ptblobs.insert( b );
	      else
		realblobs.insert( b );
	    }

	  // merge single point blobs with others (bigger)
	  if( bottom )
	    b = BOTTOM;
	  else if( !realblobs.empty() )
	    b = *realblobs.begin();
	  else	// only several 1-point blobs
	    {
	      b = *ptblobs.begin();
	      ptblobs.erase( b );
	      realblobs.insert( b );	// one of them will grow
	    }
	  for( jn=ptblobs.begin(), jfn=ptblobs.end(); jn!=jfn; ++jn )
	    {
	      b2 = *jn;
	      v = *blobs[b2].vertex.begin();
	      tex.item( v ) = b;
	      blobs.erase( b2 );
	      if( b != BOTTOM )
		blobs[b].vertex.insert( v );
	      blob_neighbours.erase( b2 );
	    }

	  // saddle point
	  if( blob_neighbours.size() > 1 )
	    {
	      set<int>	nbl;

	      tex.item(vertex) = BOTTOM;

	      for( jn=blob_neighbours.begin(), jfn=blob_neighbours.end(); 
		   jn!=jfn; ++jn ) 
		if( (b = *jn) >= 1 )	// real blob (not bottom)
		  {
		    nbl.insert( b );
		    if( blobs[b].growing )
		      {
			blobs[b].growing = false;
			limits[b].minimum = vertex;
			limits[b].saddle = Bottom;
		      }
		    //else
		    //cout << "stopping already stopped blob " << *jn << "\n";
		  }
	      
	      
	      // at least 2 real blobs : junction
	      bool	bottomflag = false;
	      unsigned	blobinit = 0;
	      if( nbl.size() >= 2 )
		for ( jn=blob_neighbours.begin(), jfn=blob_neighbours.end(); 
		      jn!=jfn; ++jn ) 
		  if (*jn >= 1)
		    {
		      BlobDescriptor	&bd = limits[*jn];
		      bd.saddle = Junction;
		      if (!bottomflag)
			{
			  blobinit = *jn;
			  bottomflag = true;
			  bd.neighbour = BOTTOM;
			}
		      else
			bd.neighbour = blobinit;
		      
		    }
	      //cout << "saddle point, type : " << (nbl.size() >= 2) << endl;
	      flag=true;
	      nbifurc += (nbl.size() >= 2);
	      ++nsaddle;
	    }
	  
	}
      if( !flag )	// nothing done : grow
	{
	  label_neighbours = (int) tex.item( *vertex_sup.begin() );
	  if( label_neighbours != BOTTOM && blobs[label_neighbours].growing )
	    {
	      tex.item(vertex) = label_neighbours;
	      blobs[label_neighbours].vertex.insert( vertex );
	    }
	  else tex.item(vertex) = BOTTOM;
	}
    }


  //AC Volume and surface of the blobs




  cout << "Nb of blobs         : " << label<< endl;
  cout << "Nb of saddle points : " << nsaddle << endl;
  cout << "Nb of saddle bifurcations : " << nbifurc << endl;

  return( tex );
}


int ScaleSpace::blobToGraph( Graph & gr, const Texture<int> & blobs, 
			     const AimsSurfaceTriangle & mesh,const AimsSurfaceTriangle & infl_mesh, 
			     const Texture<float> & values, float scale, 
			     map<int, BlobDescriptor> & limits, 
			     int startindex )
{
  unsigned	BOTTOM = 0, ii,i, n = blobs.nItem();
  float		tmax = -1;
  map<int, set <unsigned> >  		nodeblobs;
  char					num[10];
  int		index = 0;

  for( i=0; i<n; ++i )
    {
      if( blobs.item(i) !=0 ) nodeblobs[(int)blobs.item(i)].insert(i);
      if( blobs.item( i ) > tmax )
	tmax = blobs.item( i );
    }
  cout << "nb of nodes : " << tmax << endl;

  unsigned			nn = (unsigned) tmax;
  map<unsigned, Vertex *>	vmap;
  map<unsigned, Vertex *>::iterator ivmap,evmap;
  Vertex			*v;
  vector<float>			pos;

  pos.push_back( 0 );
  pos.push_back( 0 );
  pos.push_back( 0 );

  string	saddleType[] = { "Junction", "Bottom" };
  set<unsigned>::iterator	             	evset;
  float                                      	surfaceBlob,volumeBlob;  
  Point3df                                   	zero( 0.0, 0.0, 0.0 ),AB,AC,H,CH;

  for( i=1; i<nn+1; ++i )
    {
      v = gr.addVertex( "cluster" );
      v->setProperty( "index", index++ + startindex );
      vmap[i] = v;
      v->setProperty( "name", string( "unknown" ) );
      set<unsigned>  & vset=nodeblobs[i];
      v->setProperty( "vertexset", vset );
      v->setProperty( "scale", scale );
      sprintf( num, "%d", (int) (10 * scale) );
      v->setProperty( "name", string( "blob_scale_" ) + num );
      if( limits.find( i ) != limits.end() )
	{
	  v->setProperty( "maximum", values.item( limits[i].maximum ) );
	  const Point3df	& p1 = infl_mesh.vertex()[ limits[i].maximum ];
	  pos[0] = p1[0];
	  pos[1] = p1[1];
	  pos[2] = p1[2];
	  v->setProperty( "maximum_position", pos );
	  v->setProperty( "minimum", values.item( limits[i].minimum )  );
	  const Point3df	& p2 = infl_mesh.vertex()[ limits[i].minimum ];
	  pos[0] = p2[0];
	  pos[1] = p2[1];
	  pos[2] = p2[2];
	  v->setProperty( "minimum_position", pos );
	  v->setProperty( "blob_saddle_type", 
			   saddleType[ limits[i].saddle ] );
	}
      else cerr << "Blob without descriptor (index: " << i << ")\n";
      
      // blob surface and volume calculations
      const vector< AimsVector< uint,3> >              & poly = mesh.polygon();
      const vector< Point3df >				& vertex = mesh.vertex();
    
      surfaceBlob=0; volumeBlob=0; 
      evset = vset.end();
      for( ii=0, n=poly.size(); ii!=n; ++ii )
	if( vset.find( poly[ii][0] ) != evset 
	    && vset.find( poly[ii][1] ) != evset 
	    && vset.find( poly[ii][2] ) != evset )
	  {
	    const AimsVector<uint,3>	& pol = poly[ii];
	    AB = vertex[ pol[ 1 ] ] - vertex[ pol[ 0 ] ];
	    AC = vertex[ pol[ 2 ] ] - vertex[ pol[ 0 ] ];
	    H  = vertex[ pol[ 0 ] ] + ( AB.dot( AC ) / norm2( AB ) ) * AB;
	    CH = H - vertex[ pol[ 2 ] ];
	    if ( AB != zero )
	      surfaceBlob += norm( CH ) * norm( AB ) / 2;
	    
	    volumeBlob += values.item( pol[ 1 ] ) 
	      + values.item( pol[ 2 ] ) + values.item( pol[ 0 ] ) 
	      - 3 * values.item( limits[i].minimum );
	  } 
      v->setProperty( "surface", surfaceBlob );
      v->setProperty( "volume", volumeBlob );
      

      }
  //!! relation en double
  for (ivmap=vmap.begin(), evmap = vmap.end(); ivmap != evmap; ++ivmap)
    {
      BlobDescriptor	&bd = limits[ivmap->first];
      if (bd.neighbour != BOTTOM)
	{
	  Vertex	*v2 = vmap[bd.neighbour];
	  if( !v2 )
	    cerr << "Link to null vertex\n";
	  else
	    if( ivmap->second->edgesTo( v2 ).empty() )
	      gr.addEdge(ivmap->second,v2,"blob_saddle_link");
	}
    }


  gr.setProperty( "ClusterArg_VERSION", string( "1.0" ) );
  gr.setProperty( "maxscale", scale);

  return( index );
}

ScaleSpace::MergeCode ScaleSpace::mergeGraphs( Graph & g, Graph & h )
{
  cout << "merging graphs (" << g.order() << " / " << h.order() 
       << " nodes) ...\n";
  float 		scalemin,scalemax,scale,maxscale=0,scaleh;
  Graph::iterator 	ig,ige,ih,ihe;
  set <unsigned> 	blobg,blobh,blobinter;
  set <Vertex*>         nodemin,nodemax,nodeneigh;
  set <Vertex*>::iterator      in,ine;
  Vertex::iterator      iv,ive,iw,iwe;
  g.getProperty("maxscale", maxscale );
  map <Vertex*, Vertex*> mgh;
  bool  apparitionflag, jamflag;
  Edge::iterator  ie,iee,ic,ice;
  int                   jammin,jammax,wmin,wmax;
  bool			pbup = false, pbdown = false;

  h.getProperty("maxscale",scaleh);
  scalemin=scalemax=scaleh;

  //Determine closest greater and lower scale in g (scalemin and scalemax)

  for (ig=g.begin(),ige=g.end(); ig!=ige; ++ig)
    {
      (*ig)->getProperty("scale",scale);
      
      if ( scalemin == scaleh && scale < scalemin ) 
	scalemin = scale;
     
      if ( scalemax == scaleh && scale > scalemax ) 
	scalemax = scale;
 
      if ( scale > scalemin && scale < scaleh )
	scalemin = scale;

      if ( scale > scaleh && scale < scalemax )
	scalemax = scale;
    }

  //cout << "current scale : " << scaleh << ", inf : " << scalemin << ", sup : " 
  //   << scalemax << endl;

  //Determine node of scale scalemin or scalemax

  bool	problem = false;
  for (ig=g.begin(),ige=g.end(); ig!=ige; ++ig)
    {
      (*ig)->getProperty("scale",scale);
      if( scale == scaleh )
	problem = true;
      if ( scale == scalemin )
	nodemin.insert( *ig );
      if ( scale == scalemax )
	nodemax.insert( *ig );
    }
  if( problem )
    cerr << "Problem: current scale is already in the graph\n";
  //cout << "nodemin : " << nodemin.size() << " nodes\n";
  //cout << "nodemax : " << nodemax.size() << " nodes\n";
 
  //Erase links between nodes of scale scalemin and scalemax

  unsigned	nrem = 0;	// debug
  bool	inc;
  for ( in=nodemax.begin(), ine=nodemax.end(); in!=ine; ++in  )
    {
      iv=(*in)->begin();
      ive=(*in)->end();
      while( iv!=ive )
	{
	  inc = true;
	  for ( ie=(*iv)->begin(), iee=(*iv)->end(); ie!=iee; ++ie )
	    {
	      if ( (*ie)->getProperty("scale",scale) && scale == scalemin ) 
		{
		  iw=iv;
		  ++iv;
		  inc = false;
		  //cout << "remove edge\n";
		  ++nrem;
		  g.removeEdge(*iw);
		  break;
		}
	    }
	  if( inc )
	    ++iv;
	}
    }

  //cout << "removed " << nrem << " links\n";
  
  //	DEBUG ---
  for( in=nodemax.begin(), ine=nodemax.end(); in!=ine; ++in  )
    {
      iv=(*in)->begin();
      ive=(*in)->end();
      while( iv!=ive )
	{
	  for ( ie=(*iv)->begin(), iee=(*iv)->end(); ie!=iee; ++ie )
	    {
	      if ( (*ie)->getProperty("scale",scale) && scale == scalemin ) 
		{
		  cout << "still a link between scalemin & scalemax !\n";
		}
	    }
	  ++iv;
	}
    }
  //	END DEBUG ---

  //Make links between ex-nodes of g and h     
  Vertex		*v;
  float			scaletmp;	// debug
  unsigned		nadd1 = 0, nadd2 = 0;	// debug
  unsigned             MIN_OVERLAP = 1; //min nb of common nodes for spatial overlap criterion
  map <Vertex *, Vertex *>  vertexTranslation;

  for ( ih=h.begin(), ihe=h.end(); ih!=ihe; ++ih )
    {
      apparitionflag=true;
      jamflag=false;
      jammin=jammax=0;
      v=g.cloneVertex(*ih);
      vertexTranslation[*ih] = v;
      v->getProperty( "scale", scale );	// debug
      if ( (*ih)->getProperty("vertexset",blobh ) )
	{	    
	  for ( in=nodemin.begin(), ine=nodemin.end(); in!=ine; ++in  )
	    { 
	      if ( (*in)->getProperty("vertexset",blobg ) )
		{
		  blobinter.clear();
		  insert_iterator<set<unsigned> >	ii(blobinter, 
							   blobinter.begin() );
		  set_intersection( blobh.begin(),blobh.end(),
				    blobg.begin(),blobg.end(),
				    ii );
		  //if ( !blobinter.empty() ) //spatial overlap criterion for defining chain of GLB
		  if ( blobinter.size() > MIN_OVERLAP ) //spatial overlap criterion for defining chain of GLB
		    {
		      /*(*in)->getProperty( "scale", scaletmp );
		      cout << "add edge min (" << scaletmp << ") - current (" 
		      << scale << ")\n";*/
		      ++nadd1;
		      g.addEdge(v,*in,"scalebloblink");
		      apparitionflag=false;
		      ++jammin;
		    }
		}
	    }
	  v->setProperty("apparitionflag",(int)apparitionflag);
	  pbdown |= apparitionflag;
	 
	  for ( in=nodemax.begin(), ine=nodemax.end(); in!=ine; ++in  )
	    { 
	      if ( (*in)->getProperty("vertexset",blobg ) )
		{
		  blobinter.clear();
		  insert_iterator<set<unsigned> >	ii(blobinter, 
							   blobinter.begin() );
		  set_intersection( blobh.begin(),blobh.end(),
				    blobg.begin(),blobg.end(),
				    ii );
		  //if ( !blobinter.empty() )
		  if ( blobinter.size() > MIN_OVERLAP ) //spatial overlap criterion for defining chain of GLB
		    {
		      (*in)->getProperty( "scale", scaletmp );
		      /*cout << "add edge current (" << scale << ") - max (" 
			<< scaletmp << ")\n";*/
		      ++nadd2;
		      g.addEdge(v,*in,"scalebloblink");
		      ++jammax;
		    }
		}
	    }
	}
      v->setProperty("jamflag_down",(int) ( jammin > 2 ) );
      v->setProperty("jamflag_up",(int) ( jammax > 2 ) );
      pbdown|=( jammin > 2 );
      pbup|=( jammax > 2 );
    }
  /*cout << "added " << nadd1 << " linkw btw scales " << scalemin << " and " 
       << scaleh << endl;
  cout << "added " << nadd2 << " linkw btw scales " << scaleh << " and " 
  << scalemax << endl;*/
  
  
  //Copy the (ex) relations (of h) into g
  /*
  Edge::iterator      ei;
  Vertex * ti,*te;
  Edge *   ed;
  for (ivt = vertexTranslation.begin(), evt = vertexTranslation.end(); ivt != evt ; ++ivt)
    for ( iv = (ivt->first)->begin(), ive = (ivt->first)->end(); iv != ive; ++iv )
      {
	ei = (*iv)->begin();
	if (*ei == ivt->first )
	  ++ ei;
	ti = ivt -> second;
	te = vertexTranslation[*ei];
	assert( ti );
	assert( te );
	if ( ti->edgesTo(te).size() == 0)
	  {
	    ed = g.addEdge(ti,te,(*iv)->getSyntax() );
	    ed->copyProperties(**iv);
	  }

      }
  */

  //Flags

  for ( in = nodemax.begin(), ine = nodemax.end(); in!=ine; ++in )
    {
      jammax=0;wmax=0;
      for ( iv = (*in)->begin(), ive = (*in)->end(); iv!=ive;  ++iv  )
	{
	  for ( ie = (*iv)->begin(), iee = (*iv)->end(); ie!=iee; ++ie )
	    {
	      if ( (*ie)->getProperty("scale",scale) && scale == scaleh )
		{
		  ++jammax;
		  for ( iw = (*ie)->begin(), iwe = (*ie)->end(); iw != iwe; ++iw)
		    {
		      for (ic = (*iw)->begin(), ice = (*iw)->end(); ic!= ice; ++ic)
			{
			  if ( (*ic)->getProperty("scale",scale) && scale == scalemax ) ++wmax;
			}
		    }
		}
	    }
	  
	}
      (*in)->setProperty("apparitionflag",(int) ( jammax == 0 ) );
      (*in)->setProperty("jamflag_down",(int) ( jammax > 2 ) );
      (*in)->setProperty("wflag_down",(int) ( wmax > 2 ) );
      pbup|= ( jammax == 0 ) | ( jammax > 2 ) | ( wmax > 2 );
    }

 for ( in = nodemin.begin(), ine = nodemin.end(); in!=ine; ++in )
    {
      jammin=0;wmin=0;
      for ( iv = (*in)->begin(), ive = (*in)->end(); iv!=ive;  ++iv  )
	{
	  for ( ie = (*iv)->begin(), iee = (*iv)->end(); ie!=iee; ++ie )
	    {
	      if ( (*ie)->getProperty("scale",scale) && scale == scaleh )
		{
		  ++jammin;
		  for ( iw = (*ie)->begin(), iwe = (*ie)->end(); iw != iwe; ++iw)
		    {
		      for (ic = (*iw)->begin(), ice = (*iw)->end(); ic!= ice; ++ic)
			{
			  if ( (*ic)->getProperty("scale",scale) && scale == scalemin ) ++wmin;
			}
		    }
		}
	    }
	}
      (*in)->setProperty("apparitionflag",(int) ( jammin == 0 ) );
      (*in)->setProperty("jamflag_up",(int) ( jammin > 2 ) );
      (*in)->setProperty("wflag_up",(int) ( wmin > 2 ) );
      pbdown|= ( jammin == 0 ) |  ( jammin > 2 ) | ( wmin > 2 );
    }



  


 if ( scaleh > maxscale ) g.setProperty("maxscale", scaleh);
  cout << "done\n";
  return( (MergeCode) (pbup | (pbdown*2) ) );
}


void ScaleSpace::cleanGraphScale( Graph & g, float scale)
{
 Graph::iterator 	ig,ige,ih;
 float			nodescale;
 ig=g.begin();ige=g.end(); 
 while ( ig!=ige ) 
   {
     if ( (*ig)->getProperty("scale",nodescale) &&  (nodescale==scale) ) 
       {
	 ih=ig;
	 ++ig;
	 g.removeVertex(*ih); 
       }
     else ++ig;
   }
}


void ScaleSpace::createSubMeshes( Graph & g, const AimsSurface<3,Void> & mesh, 
				  float depl, float depl0, MeshShape ms, 
				  MeshGrowingMode gm,bool compressed )
{
  Graph::iterator		ig, eg=g.end();
  set<unsigned>			vset;
  const vector<Point3df>	& vert = mesh.vertex();
  const vector<Point3df>	& norm = mesh.normal();
  const vector<AimsVector<uint,3> >	& poly = mesh.polygon();
  set<unsigned>::iterator	imv, emv;
  unsigned			i, vindex = 0, n;
  float				scale;
  vector<float>			pos;
  Point3df			ppos;
  Point3df			grav( 0, 0, 0 );

  rc_ptr<map<string,map<string,GraphElementCode> > > 
    objmap( new map<string,map<string,GraphElementCode> > );
  g.getProperty("aims_objects_table",objmap);

  //cluster mesh
  {
    GraphElementCode 	&gec = (*objmap)["cluster"]["cluster"];
    gec.id = "cluster";
    gec.attribute = "aims_cluster";
    gec.objectType = "Mesh";
    gec.dataType = "VOID";
    gec.syntax = "cluster";
    gec.local_file_attribute = "cluster_filename";
    gec.global_index_attribute = "cluster_label";
    gec.global_filename = "cluster.mesh";
    if (compressed)
      gec.storageType = GraphElementCode::Global;
    else
      gec.storageType = GraphElementCode::Local;
  }
  //maximum mesh
  {
    GraphElementCode 	&gec = (*objmap)["cluster"]["maximum"];
    gec.id = "maximum";
    gec.attribute = "aims_maximum";
    gec.objectType = "Mesh";
    gec.dataType = "VOID";
    gec.syntax = "cluster";
    gec.local_file_attribute = "maximum_filename";
    gec.global_index_attribute = "maximum_label";
    gec.global_filename = "maximum.mesh";
    if (compressed)
      gec.storageType = GraphElementCode::Global;
    else
      gec.storageType = GraphElementCode::Local;
  }
  //minimum mesh
  {
    GraphElementCode 	&gec = (*objmap)["cluster"]["minimum"];
    gec.id = "minimum";
    gec.attribute = "aims_minimum";
    gec.objectType = "Mesh";
    gec.dataType = "VOID";
    gec.syntax = "cluster";
    gec.local_file_attribute = "minimum_filename";
    gec.global_index_attribute = "minimum_label";
    gec.global_filename = "minimum.mesh";
    if (compressed)
      gec.storageType = GraphElementCode::Global;
    else
      gec.storageType = GraphElementCode::Local;
  }
  //minimum2 mesh
  {
    GraphElementCode 	&gec = (*objmap)["cluster"]["minimum2"];
    gec.id = "minimum2";
    gec.attribute = "aims_minimum2";
    gec.objectType = "Mesh";
    gec.dataType = "VOID";
    gec.syntax = "cluster";
    gec.local_file_attribute = "minimum2_filename";
    gec.global_index_attribute = "minimum2_label";
    gec.global_filename = "minimum2.mesh";
    if (compressed)
      gec.storageType = GraphElementCode::Global;
    else
      gec.storageType = GraphElementCode::Local;
  } 

  g.setProperty("aims_objects_table",objmap);

  vector<int>   vec;
  vec.push_back( 255 );
  vec.push_back( 0 );
  vec.push_back( 0 );
  g.setProperty( "roi_label_maximum", vec );
  vec[0] = 0;
  vec[1] = 255;
  vec[2] = 0;
  g.setProperty( "roi_label_minimum", vec );
  vec[0] = 255;
  vec[1] = 0;
  vec[2] = 192;
  g.setProperty( "roi_label_minimum2", vec );


  if( gm == Scale )
    {
      //	compute gravity center of global mesh
      for( i=0, n=vert.size(); i<n; ++i )
	grav += vert[i];
      grav /= n;
    }

  for( ig=g.begin(); ig!=eg; ++ig )
    if( (*ig)->getProperty( "vertexset", vset ) )
      {
	++vindex;
	scale = 0;
	(*ig)->getProperty( "scale", scale );
	if (scale > 0)
	  scale = log(scale);
	else
	  cerr << "Not able to compute the log of the scale (scale=0) \n";
	AimsSurfaceTriangle	*s = new AimsSurfaceTriangle;
	vector<Point3df>	& nvert = s->vertex();
	vector<Point3df>	& nnorm = s->normal();
	vector<AimsVector<uint,3> >	& npoly = s->polygon();
	map<unsigned, unsigned>	indices;
	unsigned		n = 0;
	set<unsigned>::iterator	evset = vset.end();
	Point3df		mnorm( 0, 0, 0 ), dd, tv;

	for( imv=vset.begin(), emv=vset.end(); imv!=emv; ++imv )
	  {
	    mnorm += norm[*imv];
	    indices[*imv] = n++;
	    switch( gm )
	      {
	      case Scale:
		tv = vert[*imv] - grav;
		tv /= tv.norm();
		dd = ( depl0 + depl * scale ) * tv;
		break;
	      case PushNormal:
		dd = ( depl0 + depl * scale ) * norm[*imv];
	      default:
		dd = Point3df( 0, 0, 0 );
		break;
	      }
	    nvert.push_back( vert[*imv] + dd );
	    nnorm.push_back( norm[*imv] );
	  }

	if( gm == Translate )
	  {
	    float nn = 1. / sqrt( mnorm[0] * mnorm[0] + mnorm[1] * mnorm[1] 
				  + mnorm[2] * mnorm[2] );
	    mnorm *= nn;
	    for( i=0, n=vset.size(); i<n; ++i )
	      nvert[i] += ( depl0 + depl * scale ) * mnorm;
	  }

	for( i=0, n=poly.size(); i!=n; ++i )
	  if( vset.find( poly[i][0] ) != evset 
	      && vset.find( poly[i][1] ) != evset 
	      && vset.find( poly[i][2] ) != evset )
	    npoly.push_back( AimsVector<uint,3>( indices[poly[i][0]], 
						  indices[poly[i][1]], 
						  indices[poly[i][2]] ) );


	if( s->polygon().empty() )
	  delete s;
	else
	  {
	    if( ms == Tore )
	      {
		AimsSurfaceTriangle *s2 = s;
		s = new AimsSurfaceTriangle;
		*s = surface2Tore( *s2 );
		delete s2;
	      }
	    
	    rc_ptr<AimsSurfaceTriangle> rs(s);
	    (*ig)->setProperty("aims_cluster",rs);
	  }

	// important points
	
	if( (*ig)->getProperty( "maximum_position", pos ) )
	  {
	    ppos[0] = pos[0];
	    ppos[1] = pos[1];
	    ppos[2] = pos[2];
	    switch( gm )
	      {
	      case Translate:
		tv = ppos + ( depl0 + depl * scale ) * mnorm;
		break;
	      case Scale:
		tv = ppos - grav;
		tv /= tv.norm();
		tv = ppos + ( depl0 + depl * scale ) * tv;
	      default:
		break;
	      }
	    AimsSurfaceTriangle	*s1 = SurfaceGenerator::cube( tv, 0.5 );

	    rc_ptr<AimsSurfaceTriangle> rs(s1);
	    (*ig)->setProperty("aims_maximum",rs);
	  }
	if( (*ig)->getProperty( "minimum_position", pos ) )
	  {
	    ppos[0] = pos[0];
	    ppos[1] = pos[1];
	    ppos[2] = pos[2];
	    switch( gm )
	      {
	      case Translate:
		tv = ppos + ( depl0 + depl * scale ) * mnorm;
		break;
	      case Scale:
		tv = ppos - grav;
		tv /= tv.norm();
		tv = ppos + ( depl0 + depl * scale ) * tv;
	      default:
		break;
	      }
	    AimsSurfaceTriangle	*s2= SurfaceGenerator::cube( tv, 0.5 );
	    
	    rc_ptr<AimsSurfaceTriangle> rs(s2);
	    
	    string	stype;
	    if( (*ig)->getProperty( "blob_saddle_type", stype ) 
		&& stype == "Junction" )
	      (*ig)->setProperty("aims_minimum2",rs);
	    else
	      (*ig)->setProperty("aims_minimum",rs);
	  }
	
      }
}

static void addComponent( unsigned comp, Vertex* v, 
			  map<Vertex *, unsigned> & labels )
{
  labels[ v ] = comp;
  float	scale, nscale;
  if( !v->getProperty( "scale", scale ) )
    return;

  set<Vertex *>			neigh ; 
  set<Vertex *>::iterator	in, en = neigh.end();
  map<float, set<Vertex *> >	scn;

  Vertex::iterator              iv,ev ;
  Edge::iterator                ie;
  
  for (iv = v->begin(), ev = v->end(); iv != ev; ++iv)
    if ( (*iv)->getSyntax() == "scalebloblink" )
      {
	ie = (*iv)->begin();
	if ( *ie == v )
	  ++ ie;
	neigh.insert(*ie);
      } 
  
  
  for( in=neigh.begin(); in!=en; ++in )
    if( (*in)->getProperty( "scale", nscale ) )
      scn[nscale].insert( *in );

  map<float, set<Vertex *> >::iterator	is, es=scn.end();
  float					scale2;
  Vertex				*v2;
  map<Vertex *, unsigned>::iterator	notdone = labels.end();

  for( is=scn.begin(); is!=es; ++is )
    if( (*is).second.size() == 1 )
      {
	v2 = *(*is).second.begin();
	if( labels.find( v2 ) == notdone )
	  {
	    set<Vertex *>		neigh2 ; 
	    set<Vertex *>::iterator	in2, en2 = neigh2.end();
	    bool			reject = false;

  
	    for (iv = v2->begin(), ev = v2->end(); iv != ev; ++iv)
	      if ( (*iv)->getSyntax() == "scalebloblink" )
		{
		  ie = (*iv)->begin();
		  if ( *ie == v2 )
		    ++ ie;
		  neigh2.insert(*ie);
		} 
  

	    for( in2=neigh2.begin(); in2!=en2; ++in2 )
	      if( (*in2)->getProperty( "scale", scale2 ) && scale2 == scale 
		  && *in2 != v )
		{
		  reject = true;
		  break;
		}
	    if( !reject )
	      addComponent( comp, v2, labels );
	  }
      }
}


float ScaleSpace::getRepresentativeScale(const set<Vertex *> & SSB)
{
  set<Vertex *>::iterator		is, es;
  float scale, scale0 = FLT_MAX;
  for( is=SSB.begin(), es=SSB.end(); is!=es; ++is )
    if ( (*is)->getProperty( "scale", scale ) )
      if ( scale < scale0 )
	scale0 = scale;
  return(scale0);

}

rc_ptr<AimsSurfaceTriangle> ScaleSpace::getRepresentativeBlobMesh(float scale0, const set<Vertex *> & SSB)
{
  set<Vertex *>::iterator		is, es;
  rc_ptr<AimsSurfaceTriangle> rs;
  bool ok = false;
  float scale;
  for( is=SSB.begin(), es=SSB.end(); is!=es; ++is )
    if ( (*is)->getProperty( "scale", scale ) && scale == scale0 )
      {
	ok = true;
	if ( (*is)->getProperty( "aims_cluster", rs)  )
	    break;
      }
    else
      cerr << "Could not extract the GLB mesh\n";

  if (!ok)
    cerr << "The SSB does not contain a GLB at the scale " << scale0 << endl; 

  return(rs);

}

multimap<float, unsigned> ScaleSpace::sortGLB(string key,
					      const map<unsigned, Vertex *> & nodes,
					      const map<unsigned, set<Vertex *> > & scblobs)
{

  multimap<float, unsigned> ordering;
  map<unsigned, Vertex *>::const_iterator in,en = nodes.end() ;
  float					k;
  map<unsigned, std::set<Vertex *> >::const_iterator isb,esb = scblobs.end();
  int					num;
  Vertex 				*v = 0;

  for( isb=scblobs.begin(); isb!=esb; ++isb )
    {
      num = (*isb).first;
      in = nodes.find(num);
      if (in != en)
	v = (*in).second;
      else
	cerr << "Could not find the GLB vertex corresponding to the label " << num << " in the correspondance table."; 
      v->getProperty( key, k ); 
      ordering.insert( pair<float,unsigned>( k, num ) );
    }

  return(ordering);
}


void  ScaleSpace::GLBVertexToSSBVertex(Graph & psketch,
				       const multimap<float, unsigned> &  ordering,
				       const map<unsigned, Vertex *> & nodes,
				       const map<unsigned, set<Vertex *> > & scblobs)
{
  unsigned	num2 = 0;
  float scale0;
  int					num;
  multimap<float, unsigned>::const_reverse_iterator ior,eor;
  map<unsigned, std::set<Vertex *> >::const_iterator isb,esb = scblobs.end();
  set<Vertex *> SSB;
  Vertex *v = 0;
  map<unsigned, Vertex *>::const_iterator in,en = nodes.end() ;

  rc_ptr<map<string,map<string,GraphElementCode> > > 
    objmap( new map<string,map<string,GraphElementCode> > );
  psketch.getProperty("aims_objects_table",objmap);
  GraphElementCode 	&gec = (*objmap)["cluster"]["cluster"];
  gec.id = "cluster";
  gec.attribute = "aims_cluster";
  gec.objectType = "Mesh";
  gec.dataType = "VOID";
  gec.syntax = "cluster";
  gec.local_file_attribute = "cluster_filename";
  gec.global_index_attribute = "cluster_label";
  gec.global_filename = "cluster.mesh";
  gec.storageType = GraphElementCode::Global;
  psketch.setProperty("aims_objects_table",objmap);

  for( ior=ordering.rbegin(), eor = ordering.rend(); ior!=eor; ++ior, ++num2 )
    {
      num = (*ior).second;
      in = nodes.find(num);
      if (in != en)
        v = (*in).second;
      else
      {
        cerr << "Could not find the GLB vertex corresponding to the label "
          << num << " in the correspondance table.";
        ASSERT( v );
      }

      isb = scblobs.find(num);
      if (isb != esb)
	{
	  SSB = isb->second;
	  scale0 = getRepresentativeScale(isb->second); 
	  v->setProperty( "scale", scale0 );
	  rc_ptr<AimsSurfaceTriangle> rs = getRepresentativeBlobMesh(scale0,SSB);
	  if (rs.get() != 0)
	    v->setProperty("aims_cluster",rs);
	}
      else
	cerr << "Could not find the SSB (set of GLB vertex) corresponding to the label " 
	     << num << " in the correspondance table."; 
    }  
}


void  ScaleSpace::setGraphLabelAndName(       const multimap<float, unsigned> &  ordering,
				       const map<unsigned, Vertex *> & nodes,
				       const map<unsigned, set<Vertex *> > & scblobs)
{
  unsigned	num2 = 0;
  int					num;
  multimap<float, unsigned>::const_reverse_iterator ior,eor;
  map<unsigned, std::set<Vertex *> >::const_iterator isb,esb = scblobs.end();
  set<Vertex *> SSB;
  char		label[50];
  Vertex *v = 0;
  set<Vertex *>::iterator		is, es;
  map<unsigned, Vertex *>::const_iterator in,en = nodes.end() ;


  for( ior=ordering.rbegin(), eor = ordering.rend(); ior!=eor; ++ior, ++num2 )
    {
      sprintf( label, "scalespace_%d", num2 );
      num = (*ior).second;
      in = nodes.find(num);
      if (in != en)
        v = (*in).second;
      else
      {
        cerr << "Could not find the GLB vertex corresponding to the label "
          << num << " in the correspondance table.";
        ASSERT( v );
      }
      v->setProperty( "name", string( label ) );

      isb = scblobs.find(num);
      if (isb != esb)
	{
	  SSB = isb->second;
	  for( is= SSB.begin(), es=SSB.end(); is!=es; ++is )
	    (*is)->setProperty( "label", string( label ) );
	}
      else
	cerr << "Could not find the SSB (set of GLB vertex) corresponding to the label " 
	     << num << " in the correspondance table."; 
    }  
}



void  ScaleSpace::GLBEdgeToSSBEdge(Graph & glevel, Graph & psketch, 
		       const map<Vertex *, unsigned> & labels,
		       const map<unsigned, Vertex *> & nodes)
{

  const set<Edge*>		&glevellink = glevel.edges();
  set<Edge*>::const_iterator	bsi,esi = glevellink.end();
  Edge::const_iterator		bei;
  Vertex			*v1,*v2;
  unsigned			l1 = 0,l2 = 0;
  Edge				*E;
  map<Vertex *, unsigned>::const_iterator il,el = labels.end() ;
  map<unsigned, Vertex *>::const_iterator in1,in2,en = nodes.end() ;

  for (bsi = glevellink.begin(); bsi != esi; ++bsi)
    if ( (*bsi)->getSyntax() == "scalebloblink")
      {
	bei = (*bsi)->begin();
	v1 = *bei;
	++bei;
	v2 = *bei; 
	il = labels.find(v1);
	if (il != el)
	  l1 = il->second;
	else
	  cerr << "Could not find the label corresponding to the input vertex\n";
	il = labels.find(v2);
	if (il != el)
	  l2 = il->second;
	else
	  cerr << "Could not find the label corresponding to the input vertex\n";
	if (l1 != l2)
	  {
	    in1 = nodes.find(l1);
	    in2 = nodes.find(l2);
	    if ( in1 != en && in2 != en)
	      {
		E = psketch.addEdge(in1->second,in2->second,"scalebloblink");
		E->copyProperties( Object::reference( (*bsi)->getValue() ));
	      }
	    else
	      cerr << "Could not find the vertex corresponding to the label " << l1 
		   << " or the label " << l2 << endl;
	  }
      }

}
  


/*
void  ScaleSpace::computeSSBVolume(const set<Vertex *>& SSB)
{
  float scale1, scale2, surface1, surface2, volume = 0 ;
  set<Vertex *>::const_iterator		is;
  set<Vertex *>::const_reverse_iterator es;
  Vertex *v1, *v2;
  
  es = SSB.rbegin();
  is = SSB.begin();
  white( is ! = es)
  {
      v1 = *is;
      ++is;
      v2 = *is;
      if ( v1->getProperty("surface",surface1) && v2->getProperty("surface",surface2)
	   && )
	

    }
  

}

*/
void ScaleSpace::grayLevel2PrimalSketch( Graph & glevel, 
					 Graph & psketch )
{
  //	connected components
  Graph::const_iterator			iv, ev = glevel.end();
  map<Vertex *, unsigned>		labels; //glb graph nodes -> ind
  map<Vertex *, unsigned>::iterator	ic, notdone = labels.end();
  Vertex				*v, *blob;
  unsigned				comp = 0;
  int					id;

  for( iv=glevel.begin(); iv!=ev; ++iv )
    if( labels.find( *iv ) == notdone )
      addComponent( comp++, *iv, labels );

  map<unsigned, Vertex *>		nodes; // glb ind -> graph ndoes  
  map<unsigned, Vertex *>::iterator	in, en = nodes.end();
  float					scale, scale2,scale3,ds;
  vector<int>				glb;
  int					num;
  map<unsigned, set<Vertex *> >		scblobs; // ind -> psk nodes

  for( ic=labels.begin(); ic!=notdone; ++ic )
    {
      blob = (*ic).first;
      num = (*ic).second;
      in = nodes.find( num );
      if( in == en )
	{
	  v = psketch.addVertex( "cluster" );
	  nodes[ (*ic).second ] = v;
	}
      else
	v = (*in).second;

      scblobs[num].insert( blob );
      

      if( blob->getProperty( "scale", scale ) )
	{
	  if( !v->getProperty( "minscale", scale2 ) || scale < scale2 )
	    v->setProperty( "minscale", scale );
	  if( !v->getProperty( "maxscale", scale2 ) || scale > scale2 )
	    v->setProperty( "maxscale", scale );
	}
      if( blob->getProperty( "index", id ) )
	{
	  glb.clear();
	  v->getProperty( "gray_level_blobs", glb );
	  glb.push_back( id );
	  v->setProperty( "gray_level_blobs", glb );
	}
     
      v->getProperty( "minscale", scale2 );
      v->getProperty( "maxscale", scale3 );
      ds = scale3 - scale2; 
      v->setProperty("lifetime", ds ); 
    }

  //Sort the SSB according to their lifetime
  multimap<float, unsigned>		ordering;

  ordering = sortGLB("lifetime",nodes,scblobs);  
  setGraphLabelAndName(ordering,nodes,scblobs);
  GLBVertexToSSBVertex(psketch,ordering,nodes,scblobs);
  GLBEdgeToSSBEdge(glevel,psketch,labels,nodes);
  
}


AimsSurfaceTriangle ScaleSpace::surface2Tore( const AimsSurfaceTriangle & s )
{
  const vector<Point3df>  & vert = s.vertex();
  const vector<AimsVector<uint,3> >  & poly = s.polygon();
  unsigned  i, p = poly.size(), t1, t2, t3;
  map<pair<unsigned, unsigned>, unsigned> edges;
  map<pair<unsigned, unsigned>, unsigned>::iterator ie, eie = edges.end();
  AimsSurfaceTriangle      tore;

  /*cout << "input mesh : " << vert.size() << " vertices, " << poly.size() 
    << " polygons\n";*/
  for( i=0; i<p; ++i )
    {
      t1 = poly[i][0];
      t2 = poly[i][1];
      t3 = poly[i][2];

      if( t1 < t2 )
	++edges[ pair<unsigned, unsigned>( t1, t2 ) ];
      else
	++edges[ pair<unsigned, unsigned>( t2, t1 ) ];
      if( t1 < t3 )
	++edges[ pair<unsigned, unsigned>( t1, t3 ) ];
      else
	++edges[ pair<unsigned, unsigned>( t3, t1 ) ];
      if( t2 < t3 )
	++edges[ pair<unsigned, unsigned>( t2, t3 ) ];
      else
	++edges[ pair<unsigned, unsigned>( t3, t2 ) ];
    }

  //cout << "surface2Tore : " << edges.size() << " links\n";
  unsigned		nedge = 0;
  AimsSurfaceTriangle	*msh;

  for( ie=edges.begin(); ie!=eie; ++ie )
    if( (*ie).second == 1 )
      {
	++nedge;
	msh = SurfaceGenerator::cylinder( vert[(*ie).first.first], 
					  vert[(*ie).first.second], 
					  0.3, 0.3, 3, false );
	SurfaceManip::meshMerge( tore, *msh );
	delete msh;
      }
  //cout << "surface2Tore : " << nedge << " cylinders\n";

  return( tore );
}



int ScaleSpace::adaptiveScaleSpace( Graph & psketch, 
				    const AimsSurfaceTriangle & mesh,const AimsSurfaceTriangle & infl_mesh,
				    const Texture1d & inittex,
				    map<float,Texture<int> > & tex_blobs,
				    map<float,Texture<float> > & tex_curv,
				    int index, float tbegin, float tend, 
				    float dt,float dts, float dtmin,
				    const float HMAX,bool logFlag,
				    const map<unsigned, set< pair<unsigned,float> > > & weightLapl)
{
  set<float>         scales;
  vector<float>      scalesVector;
  Texture1d          outtex,intertex;
  Texture<int>	     texb;
  float              t,scale,x,Tmax,Tmin,mean,estd;
  Graph		     h( "ClusterArg" );
  map<int, ScaleSpace::BlobDescriptor>	limits;
  AimsSurfaceTriangle::const_iterator	is = mesh.find( 0 );
  if( is == mesh.end() )
    {
      cerr << "Mesh has no time 0\n";
      return( 0 );
    }
  const AimsSurface<3,Void>		& mesh0 = (*is).second;

  unsigned           ind = index, inc=0;
  float 	     tOld,DT;

  psketch.getProperty("samplescale",scales);
  //cout << "scales size : " << scales.size() << endl;
  //tex_curv[tbegin] = inittex.begin()->second;
  unsigned incdts = 0;   

  if ( index == 0 )
    {
      scale = tbegin;
      cout << endl << "initial scale : " << scale << endl;
      cout << ind << "-" << inc << endl; 
      ++inc;
      unsigned incdts = 0;   
      bool OK=false;
      while (!OK)
	{
	  try
	    {
	      cout << "smoothing..." << endl;
	      outtex[0]=TextureSmoothing::FiniteElementSmoothing(inittex.find(0)->second,dts,tbegin ,
								 HMAX,weightLapl);
	      cout << endl;
	      OK=true;
	      tex_curv[scale] = outtex[0];
	    
	    }
	  catch(exception & e)
	    {
	      cout << "outtex " << outtex.nItem() << endl;
	      cerr << "The diffusion process diverges. dts= " << dts << "\nI change dts\n";
	      dts /= 2;
	      cout << "dts= " << dts <<endl;
	      ++incdts;
	      assert(incdts < 4);
		
	    }
	}
      cout << "outtex size : " << outtex.nItem() << endl;
      texb = meshBlobExtract( mesh0, outtex[0], limits);
      tex_blobs[ scale ] = texb;
      index = blobToGraph( psketch, texb, mesh, infl_mesh,outtex[0], tbegin, limits, 
			   index );
      cout << "init index : " << index << endl;
      scales.insert(scale);
     
    }
   tOld=tbegin;

   if (logFlag)
     {
       for ( t=tbegin * dt ; t<tend; t*=dt ) //log scale
	 {
	   scale=t;
	   DT = t - tOld;//= tOld*(dt -1) Diffusion time; change with t if log scale
	   scales.insert(t);
	   cout << endl << "scale : " << scale << " from: " << tOld << " DT: " << DT  << endl;
	   cout << ind << "-" << inc << endl; 
	   ++inc;
	   if( outtex.nItem() == 0 )
	     intertex = inittex;
	   else
	     intertex=outtex;
	   bool OK2 = false;
	   while (!OK2)
	     {
	       try
		 {
		   cout << "smoothing..." << endl;
		   outtex[0]=TextureSmoothing::FiniteElementSmoothing(intertex[0],dts,
								      DT ,
								      HMAX,weightLapl);
		   tex_curv[scale] = outtex[0];
		   cout << endl;
		   OK2 = true;
	      
		 }
	       catch( exception &e )
		 {
		   cerr << "The diffusion process diverges. dts= " << dts << "\nI change dts\n";
		   dts /= 2;
		   cout << "dts= " << dts <<endl; 
		   ++incdts;
		   assert(incdts < 4 );
			      
		 }
	     }

	   Tmin = 1e38; Tmax = -1e38; mean = 0; estd = 0;
	   for( unsigned i=0; i<outtex.nItem(); ++i )
	     {
	       x = outtex.item(i); 
	       mean += x;
	       if( x < Tmin )
		 Tmin = x;
	       if( x > Tmax )
		 Tmax = x;
	     }
	   for( unsigned i=0; i<outtex.nItem(); ++i )
	     {
	       x = outtex.item(i); 
	       estd += (x - mean)*(x-mean);
	     }
	   estd = sqrt(estd);
	   estd /= outtex.nItem();
	   mean /= outtex.nItem();
	   cout << "Texture min : " << Tmin << ", max : " << Tmax 
		<< ", mean : " << mean << ", std : " << estd << endl;
 
	   texb = meshBlobExtract( mesh0, outtex[0], limits );
	   tex_blobs[scale] = texb;
	   index += 
	     blobToGraph( h, texb, mesh, infl_mesh, outtex[0], scale, limits, index );
	   MergeCode	ret = mergeGraphs( psketch, h );
	   h.clear();
	   if( ret && sqrt(dt) > dtmin )
	     {
	  
	       if ( ret & 2)//adapScale down 
		 {
		   cout << endl << "Adaptive scaling..." << endl;
		   psketch.setProperty("samplescale",scales);
		   cout << "Down:" << scale    << " -> " << tOld * sqrt(dt) << " from " << tOld <<  endl; 
		   index = adaptiveScaleSpace( psketch, mesh, infl_mesh,intertex,tex_blobs,
					       tex_curv,index, tOld , tOld *pow((double) dt,0.6) , 
					       sqrt(dt), dts, dtmin,HMAX, logFlag,weightLapl);//log scale

		   psketch.getProperty("samplescale",scales);
		 }
	       if ( ret & 1)//adapScale up
		 {
		   cout << endl << "Adaptive scaling..." << endl;
		   psketch.setProperty("samplescale",scales);
		   cout << "Up: " << scale << " -> " << scale * sqrt(dt) << endl; 
		   index  = adaptiveScaleSpace( psketch, mesh, infl_mesh,outtex,tex_blobs,
						tex_curv,index, scale , scale * pow((double) dt,0.6) , 
						sqrt(dt), dts, dtmin,HMAX, logFlag,weightLapl); 
		   psketch.getProperty("samplescale",scales);
		 }
	     }
	   tOld = t;
	 }
     }
   else
     {
       for ( t=tbegin + dt ; t<=tend; t+=dt )//linear scale
	 {
	   scale=t;
	   scales.insert(t);
	   cout << endl << "scale : " << scale << " from: " << tOld << " dt: " << dt   << endl;
	   cout << ind << "-" << inc << endl; 
	   ++inc;
	   if( outtex.nItem() == 0 )
	     intertex = inittex;
	   else
	     intertex=outtex;
      
	   bool OK2 = false;
	   while (!OK2)
	     {
	       try
		 {
		   cout << "smoothing..." << endl;
		   outtex[0]=TextureSmoothing::FiniteElementSmoothing(intertex[0],dts,
								       dt ,HMAX,weightLapl);

		   tex_curv[scale] = outtex[0];
		   cout << endl;
		   OK2 = true;
	      
		 }
	       catch( exception &e )
		 {
		   cerr << "The diffusion process diverges. dts= " << dts << "\nI change dts\n";
		   dts /= 2;
		   cout << "dts= " << dts <<endl; 
		   ++incdts;
		   assert(incdts < 4);
		 }
	     }

	   Tmin = 1e38; Tmax = -1e38;
	   for( unsigned i=0; i<outtex.nItem(); ++i )
	     {
	       x = outtex.item(i); 
	       if( x < Tmin )
		 Tmin = x;
	       if( x > Tmax )
		 Tmax = x;
	     }
	   cout << "Texture min : " << Tmin << ", max : " << Tmax <<endl;
 
	   texb = meshBlobExtract( mesh0, outtex[0], limits );
	   tex_blobs[scale] = texb;
	   index += 
	     blobToGraph( h, texb, mesh, infl_mesh,outtex[0], scale, limits, index );
	   MergeCode	ret = mergeGraphs( psketch, h );
	   h.clear();
	   if( ret && dt > dtmin ) //linear scale
	     {
	  
	       if ( ret & 2)//adapScale down 
		 {
		   cout << endl << "Adaptive scaling..." << endl;
		   psketch.setProperty("samplescale",scales);
		   cout << "Down:" << scale    << " -> " << tOld * sqrt(dt) << " from " << tOld <<  endl; 
		   index  = adaptiveScaleSpace( psketch, mesh,infl_mesh,intertex ,tex_blobs,
						tex_curv,index, scale - dt , scale - dt / 4 , 
						dt / 2, dts, dtmin,HMAX, logFlag,weightLapl); 

		   psketch.getProperty("samplescale",scales);
		 }
	       if ( ret & 1)//adapScale up
		 {
		   cout << endl << "Adaptive scaling..." << endl;
		   psketch.setProperty("samplescale",scales);
		   cout << "Up: " << scale << " -> " << scale * sqrt(dt) << endl; 
		   index  = adaptiveScaleSpace( psketch, mesh,infl_mesh,outtex ,tex_blobs,
						tex_curv,index, scale  , scale + dt *0.6 , 
						dt / 2, dts, dtmin,HMAX, logFlag,weightLapl); 

		   psketch.getProperty("samplescale",scales);
		 }
	     }
	   tOld = t;
	 }
     }
  //cout << "index at adapScale return : " << index << endl;
  set<float>::iterator	isc, esc=scales.end();

  for( isc=scales.begin(); isc!=esc; ++isc )
      scalesVector.push_back( *isc );
    
    psketch.setProperty("scales",scalesVector);
  psketch.setProperty("samplescale",scales);
  return(index);

}
