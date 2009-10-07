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
#include <aims/scalespace/meshBlob.h>
#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/writer.h>
#include <aims/data/pheader.h>
#include <aims/def/path.h>
#include <aims/utility/converter_texture.h> 
#include <aims/getopt/getopt.h>
#include <graph/graph/graph.h>
#include <graph/graph/gwriter.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/object/sreader.h>
#include <cartobase/stream/directory.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace aims;
using namespace carto;
using namespace std;

 

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsTexture2Primal       -i[nput] <input_texture>                        ",
  "                         -v[olume] <input_volume>                        ",
  "                         -M[esh]  <input mesh>                           ",
  "                         -F[lat]  <input inflated_mesh>                  ",
  "                         -b[egin] <begin_scale>                          ",
  "                         -e[nd] <end_scale>                              ",
  "                         -d[t] <scalespace sample step>                  ",
  "                         --log <logarithm scale>                         ",
  "                         [-H[max]    <texture_threshold> ]               ",
  "                         [-p[primal] <output_primal>]                    ",
  "                         [-g[raph]   <output_graph>]                     ",
  "                         -B[lobs]   <output_blobs_texture>               ",
  "                         -o[utputss]   <output_scalespace_textur>        ",
  "                         [-g[raph]   <output_graph>]                     ",
  "                         [-s[urface] <surface_type>]                     ",
  "                         [-G[row]    <grow_mode>]                        ",
  "                         [-t[ranslation] <coef_translation>]             ",
  "                         [-m[indt] <min scalespace sample step>]         ",
  "                         [-W[max] <Weight threshold>                  ",
  "                         [-S[moothstep] <smooth step for the edp>]       ",
  "                         [-c[onvexity] ]                                 ",
  "                         [-h[elp]]                                       ",
  "-------------------------------------------------------------------------",
  " Build the primal sketch of a texture using 2D geodesic diffusion        ",
  "-------------------------------------------------------------------------",
  "      begin_scale                 :  First scale in the scale space      ",
  "      end_scale                   :  Last  scale in the scale space      ",
  "      scale space sample step     :  Max sample step in the scale space  ",
  "      min scale space sample step :  Min sample step in the scale space  ",
  "                                     <default = scale space sample step> ",
  "      Smooth step for the edp     :  Time step for te edp (depending from the triangulation)",
  "                                     <default = automatic>               ",
  "      Weight  threshold         :    (Absolute) threshold for the laplacian weight estimate",
  "                                     <default = 0.1>                       ",
  "      Texture threshold           :  (Absolute) threshold for the textures values",
  "                                     <default = auto (max of the texture)>",
  "      Convexity                   :  Grey level blob correspond to:      ",
  "                                         -local minima (convexity = -1)  ",
  "                                         -local maxima (convexity =  1)  ",
  " 					<default = -1>                      ",
  "      output_primal               :  Make also the corresponding primal sketch graph ",
  "                                     (Scale Space Blob graph) <default = not used>           ",
  "      output_graph                :  Make also the corresponding  graph  ",
  "                                     (Grey Level Blob graph)  <default = not used>           ",
  "      surface_type                :  Tore (default) or surface           ",
  "      coef_translation            :  Link between the scale and the distance to the mesh",
  "      grow_mode                   :  Mesh growing mode along scales: scale (default),",
  "                                     translate, or pushnormal            ",

  "-------------------------------------------------------------------------",
END_USAGE

//
// Usage
//
void Usage( void )
{
  AimsUsage( usage );
}
 


int main( int argc, char** argv )
{
  
  
  
  char		*stype, *growmode, *primal=0;
  char		*gname=0, *volfile=0,*texname=0,*meshfile=0,*inflate_meshfile=0, *ssfile,*bfile;
  //bool	errorflg = false;
  float		H , depl0 = 10, beginscale = -1, endscale = -1, dt = 0, 
    dtmin = 0, dts = 0.05,sign = -1, depl = 0.5;
  bool  	wgraph = false, logFlag = false ;
  float 	Wmax = 0.1;
  int           dtminnb = 1;

  //
  // Parser of options
  //
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage           ,AIMS_OPT_CALLFUNC,0},
  { 's',"surface"      ,AIMS_OPT_STRING,&stype          ,0                ,0},
  { 't',"trans"        ,AIMS_OPT_FLOAT ,&depl           ,0                ,0},
  { 'G',"Grow"         ,AIMS_OPT_STRING,&growmode       ,0                ,0},
  { 'p',"primal"       ,AIMS_OPT_STRING,&primal         ,0                ,0},
  { 'b',"beginscale"   ,AIMS_OPT_FLOAT ,&beginscale     ,0                ,1},
  { 'e',"endscale"     ,AIMS_OPT_FLOAT ,&endscale       ,0                ,1},
  { 'd',"dt"           ,AIMS_OPT_FLOAT ,&dt             ,0                ,1},
  { 'c',"concavity"    ,AIMS_OPT_FLOAT ,&sign           ,0                ,0},
  { 'H',"Hmax"         ,AIMS_OPT_FLOAT ,&H              ,0                ,0},
  { 'W',"Weightmax"    ,AIMS_OPT_FLOAT ,&Wmax           ,0                ,0},
  { 'm',"mindt"        ,AIMS_OPT_INT   ,&dtminnb        ,0                ,0},
  { 'S',"smoothstep"   ,AIMS_OPT_FLOAT ,&dts            ,0                ,0},
  { 'g',"graph"        ,AIMS_OPT_STRING,&gname          ,0                ,0},
  { 'v',"volume"       ,AIMS_OPT_STRING,&volfile        ,0                ,1},
  { 'o',"outputss"     ,AIMS_OPT_STRING,&ssfile          ,0               ,0},
  { 'B',"Blobs"        ,AIMS_OPT_STRING,&bfile          ,0                ,1},
  { 'i',"input"        ,AIMS_OPT_STRING,&texname        ,0                ,1},
  { ' ',"log"          ,AIMS_OPT_FLAG  ,&logFlag        ,0                ,0},
  { 'M',"mesh"         ,AIMS_OPT_STRING,&meshfile       ,0                ,1},
  { 'F',"flat_mesh"    ,AIMS_OPT_STRING,&inflate_meshfile,0                ,1},
  { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );
 
  ASSERT( !(beginscale < 0 || endscale < 0 || endscale < beginscale || dt <= 0) );

  ScaleSpace::MeshShape		mshape = ScaleSpace::Tore;
  ScaleSpace::MeshGrowingMode	mgrow = ScaleSpace::Scale;
  ScaleSpace			ss;
  Graph				g( "ClusterArg" );

  if (logFlag)
    {
      cout << "Logarithmic scale\n";
      dtmin = (float)pow((double)dt,(double)1/dtminnb);
      cout << "dtmin = " << dtmin << endl;
    }
    else
    {
      cout << "Linear scale\n";
      dtmin = dt / dtminnb;
      cout << "dtmin = " << dtmin << endl;
    }
  if (dtmin == 0)
    dtmin = dt;
  
  if (! (sign ==1 || sign == -1) ) 
    {
      cout << "The concavity parameter (-c ...) must be 1 or -1 \n";
      assert(0);
    }

  if (sign == -1)
    cout << "Sulci studied\n";
  else
    cout << "Gyri studied\n";
  
  if (gname != 0)
    wgraph = true;
  else
    cout << "No graph written \n";
  
  if( stype && string( stype ) == "surface" )
    mshape = ScaleSpace::Surface;
  else if( !stype || string( stype ) != "tore" )
    {
      cerr << "Invalid surface type - reread the help...\n\n";
      assert(0);
    }

  if( growmode && string( growmode ) == "translate" )
    mgrow = ScaleSpace::Translate;
  else if( growmode && string( growmode ) == "pushnormal" )
    mgrow = ScaleSpace::PushNormal;
  else if( !growmode || string( growmode ) != "scale" )
    {
      cerr << "Invalid growing mode - reread the help...\n\n";
      assert(0);

    }

  //
  // read triangulation
  //
  cout << "reading triangulation   : " << flush;
  AimsSurfaceTriangle surface,inflate_surface;
  Reader<AimsSurfaceTriangle>	surfaceR( meshfile );
  assert( surfaceR.read( surface ) );
  Reader<AimsSurfaceTriangle>	surfaceRF( inflate_meshfile );
  assert( surfaceRF.read( inflate_surface ) );
  cout << "done" << endl;

 
  //
  // read volume
  //
  cout << "reading volume info\n";
  Finder	f;
  assert( f.check( volfile ) );
  const PythonHeader	*hd 
  = dynamic_cast<const PythonHeader *>( f.header() );
  assert( hd );
  vector<float>	vs;
  vector<int>	bbmin, bbmax;
  assert( hd->getProperty( "voxel_size", vs ) );
  assert( hd->getProperty( "volume_dimension", bbmax ) );
  cout << "voxel size : " << vs[0] << ", " << vs[1] << ", " << vs[2] << endl;
  g.setProperty( "voxel_size", vs );
  bbmin.push_back( 0 );
  bbmin.push_back( 0 );
  bbmin.push_back( 0 );
  g.setProperty( "boundingbox_min", bbmin );
  g.setProperty( "boundingbox_max", bbmax );

  //	read texture
  TimeTexture<float>	tex;
  cout << "reading texture " << texname << "..." << endl;
  Reader<Texture1d>	texR( texname );
  texR >> tex ;
 
  float	m = 1e38, M = -1e38, x;
  for( unsigned i=0; i<tex.nItem(); ++i )
    {
      x = tex.item(i);
      if( x < m )
	m = x;
      if( x > M )
	M = x;
    }
  cout << "texture : min : " << m << ", max : " << M <<endl;
  
  

  for (unsigned i=0; i < tex.nItem();++i)
    tex.item(i) *= sign;
  

  cout << "mesh size : " << surface.vertex().size() << " vertices\n";
  cout << "texture size : " << tex.nItem() << endl;
  cout << "curvature max :" << H << endl;
  
  //Mesh feature and ordering nodes
  cout << "Estime laplacian coefficients..." << endl ;
  map<unsigned, set< pair<unsigned,float> > > weightLapl;
  weightLapl = AimsMeshWeightFiniteElementLaplacian(surface[0],Wmax);

  //	process
  cout << "processing........\n";
  map<float,Texture<int> >  tex_blobs;
  map<float,Texture<int> >::iterator tbb,tbe;
  map<float,Texture<float> >tex_curv;
  map<float,Texture<float> >::iterator tcb,tce;
  TimeTexture<int> tb;
  TimeTexture<float>   tc;
  
  ss.adaptiveScaleSpace( g, surface,inflate_surface, tex, tex_blobs,tex_curv, 
			 0, beginscale, endscale, dt, dts, dtmin, 
			 H,logFlag,weightLapl);
  
  cout << "\n";

  // Writting the blob texture
  string outtex = texname;
  string::size_type	pos = outtex.rfind( '.' );
  if( pos != string::npos )
    outtex = outtex.substr( 0, pos); 
  string::size_type inc=0;
  for (tbb=tex_blobs.begin(), tbe=tex_blobs.end(); tbb != tbe; ++tbb)
    {
      tb[inc] = tbb->second;
      ++inc;
    }
  inc = 0;
  for (tcb=tex_curv.begin(), tce=tex_curv.end(); tcb != tce; ++tcb)
    {
      tc[inc] = tcb->second;
      ++inc;
    }
  cout << "writing blobs texture : " << flush;
  string outtexfile =outtex + "_blobs";
  Writer<TimeTexture<int> >	texW(bfile );
  texW <<  tb ;
  cout << "done" << endl;
    
  
  //Writting the curvature scalespace texture
   cout << "writing curvature scalespace texture : " << flush;
   Writer<TimeTexture<float> >	tW(ssfile );
   tW <<  tc ;
   cout << "done" << endl;

  //build the graph
   if (wgraph)
     {
       Graph	psk( "ClusterArg" );
       cout << "Meshing..." << flush ;
       ss.createSubMeshes( g, inflate_surface[0], depl, depl0, mshape, mgrow);
       cout << "done"<< endl;
       cout << "Building the primal sketch..."<< flush;
       ss.grayLevel2PrimalSketch( g, psk );
       cout << "done\n";

       //Global parameters 
       //GLB
       string	datname = gname;
       vector<int>	color;
       string::size_type	 pos = datname.rfind( '.' );
       if( pos != string::npos )
	 datname.replace( pos+1, datname.length() - pos -1, "data" );
       else
	 datname += ".data";
       datname = FileUtil::basename( datname );
       g.setProperty( "filename_base", datname );

       g.setProperty( "type.tri", string( "cluster.tri" ) );
       g.setProperty( "cluster.tri", 
		       string( "cluster Tmtktri_filename mesh_filename_maximum "
			       "mesh_filename_minimum mesh_filename_minimum2" ) );
       color.push_back( 255 );
       color.push_back( 0 );
       color.push_back( 0 );
       g.setProperty( "mesh_filename_maximum", color );
       color[0] = 0;
       color[1] = 255;
       color[2] = 0;
       g.setProperty( "mesh_filename_minimum", color );
       color[0] = 255;
       color[1] = 0;
       color[2] = 192;
       g.setProperty( "mesh_filename_minimum2", color );
       Directory	dir( datname );
       dir.mkdir();
       ss.createSubMeshes( g, inflate_surface[0], depl, depl0, mshape, mgrow);
       //PSK
       datname = primal;
       pos = datname.rfind( '.' );
       if( pos != string::npos )
	 datname.replace( pos+1, datname.length() - pos -1, "data" );
       else
	 datname += ".data";
       datname = FileUtil::basename( datname );
       psk.setProperty( "filename_base", datname );

       //bounding boxes

       float	maxscale;
       g.getProperty( "maxscale", maxscale );
       float	grow = mgrow * maxscale + depl0;
       bbmax[0] += (int) ( grow / vs[0] );
       bbmax[1] += (int) ( grow / vs[1] );
       bbmax[2] += (int) ( grow / vs[2] );
       g.setProperty( "boundingbox_max", bbmax );
       psk.setProperty( "voxel_size", vs );
       psk.setProperty( "boundingbox_min", bbmin );
       psk.setProperty( "boundingbox_max", bbmax );

       //write the graph
       try
	 {
	   cout << "saving primal sketch " << primal << "...\n";
	   Writer<Graph>	gw( primal );
	   gw.write( psk );
	 }
       catch( exception & e )
	 {
	   cerr << e.what() << endl;
	 }
       try
	 {
	   cout << "saving glb " << gname << "...\n";
	   Writer<Graph>	gw( gname );
	   gw.write( g );
	 }
       catch( exception & e )
	 {
	   cerr << e.what() << endl;
	   exit( 1 );
	 }

       
	   
	 

     }
  cout << "done" << endl;
  return 0;
}
