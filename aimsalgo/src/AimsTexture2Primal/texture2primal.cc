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
#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/writer.h>
#include <aims/data/pheader.h>
#include <aims/def/path.h>
#include <aims/utility/converter_texture.h> 
#include <aims/getopt/getopt2.h>
#include <graph/graph/graph.h>
#include <graph/graph/gwriter.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/object/sreader.h>
#include <cartobase/stream/directory.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  Reader<Texture1d> texR;
  Reader<AimsSurfaceTriangle> surfaceR;
  Reader<AimsSurfaceTriangle> surfaceRF;
  Writer<Graph> gwp;
  Writer<Graph>	gw;
  Writer<TimeTexture<int> > texW;
  Writer<TimeTexture<float> > tW;

  string stype, growmode;
  string volfile;
  //bool	errorflg = false;
  float		H , depl0 = 10, beginscale = -1, endscale = -1, dt = 0, 
    dtmin = 0, dts = 0.05,sign = -1, depl = 0.5;
  bool  	wgraph = false, logFlag = false ;
  float 	Wmax = 0.1;
  int           dtminnb = 1;

  AimsApplication app( argc, argv,
                       "Build the primal sketch of a texture using 2D "
                       "geodesic diffusion" );
  app.addOption( texR, "-i", "input texture" );
  app.addOption( volfile, "-v", "input volume (used for size)" );
  app.addOption( surfaceR, "-M", "input mesh" );
  app.addOption( surfaceRF, "-F", "inflated mesh file" );
  app.addOption( texW, "-B", "output blobs texture" );
  app.addOption( tW, "-o", "output scalespace texture" );
  app.addOption( beginscale, "-b", "First scale in the scale space" );
  app.addOption( endscale, "-e", "Last scale in the scale space" );
  app.addOption( dt, "-d", "Max sample step in the scale space" );
  app.addOption( gw, "-g",
                 "output graph, (Grey Level Blob graph) "
                 "<default = not used>", true );
  app.addOption( gwp, "-p",
                 "output corresponding primal sketch graph (needs also -g) "
                 "(Scale Space Blob graph) <default = not used>", true );
  app.addOption( logFlag, "--log", "use log scale", true );
  app.addOption( H, "-H",
                 "(Absolute) threshold for the textures values "
                 "<default = auto (max of the texture)>)", true );
  app.addOption( stype, "-s", "surface_type: Tore (default) or surface",
                 true );
  app.addOption( growmode, "-G",
                 "Mesh growing mode along scales: scale (default), translate, "
                 "or pushnormal ", true );
  app.addOption( depl, "-t",
                 "Link between the scale and the distance to the mesh", true );
  app.addOption( dtminnb, "-m",
                 "min scale space sample step: Min sample step in the scale "
                 "space, <default = scale space sample step>", true );
  app.addOption( Wmax, "-W",
                 "(Absolute) threshold for the laplacian weight estimate, "
                 "<default = 0.1>", true );
  app.addOption( dts, "-S",
                 "Time step for te edp (depending from the triangulation), "
                 "<default = automatic>", true );
  app.addOption( sign, "-c",
                 "Convexity: Grey level blob correspond to: "
                 "* local minima (convexity = -1),  "
                 "* local maxima (convexity =  1), <default = -1>", true );

  app.alias( "--input", "-i" );
  app.alias( "--volume", "-v" );
  app.alias( "--mesh", "-M" );
  app.alias( "--flat_mesh", "-F" );
  app.alias( "--beginscale", "-b" );
  app.alias( "--endscale", "-e" );
  app.alias( "--dt", "-d" );
  app.alias( "--Hmax", "-H" );
  app.alias( "--primal", "-p" );
  app.alias( "--graph", "-g" );
  app.alias( "--Blobs", "-B" );
  app.alias( "--outputss", "-o" );
  app.alias( "--surface", "-s" );
  app.alias( "--Grow", "-G" );
  app.alias( "--trans", "-t" );
  app.alias( "--translation", "-t" );
  app.alias( "--mindt", "-m" );
  app.alias( "--Weightmax", "-W" );
  app.alias( "--Wmax", "-W" );
  app.alias( "--smoothstep", "-S" );
  app.alias( "--convexity", "-c" );
  app.alias( "--concavity", "-c" );

  try
  {
    app.initialize();
 
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

    if ( !gw.fileName().empty() )
      wgraph = true;
    else
      cout << "No graph written \n";

    if( stype == "surface" )
      mshape = ScaleSpace::Surface;
    else if( !stype.empty() && stype != "tore" )
      {
        cerr << "Invalid surface type - reread the help...\n\n";
        assert(0);
      }

    if( growmode == "translate" )
      mgrow = ScaleSpace::Translate;
    else if( growmode == "pushnormal" )
      mgrow = ScaleSpace::PushNormal;
    else if( !growmode.empty() && growmode != "scale" )
      {
        cerr << "Invalid growing mode - reread the help...\n\n";
        assert(0);
      }

    //
    // read triangulation
    //
    cout << "reading triangulation   : " << flush;
    AimsSurfaceTriangle surface,inflate_surface;
    if( ! surfaceR.read( surface ) )
      throw logic_error( "Internal error: read failed" );
    if( ! surfaceRF.read( inflate_surface ) )
      throw logic_error( "Internal error: read failed" );
    cout << "done" << endl;


    //
    // read volume
    //
    cout << "reading volume info\n";
    Finder	f;
    if( ! f.check( volfile ) )
      throw logic_error( "Internal error: check failed" );
    const PythonHeader	*hd
    = dynamic_cast<const PythonHeader *>( f.header() );
    assert( hd );
    vector<float>	vs;
    vector<int>	bbmin, bbmax;
    if( ! hd->getProperty( "voxel_size", vs ) )
      throw logic_error( "Internal error: getProperty failed" );
    if( ! hd->getProperty( "volume_dimension", bbmax ) )
      throw logic_error( "Internal error: getProperty failed" );
    cout << "voxel size : " << vs[0] << ", " << vs[1] << ", " << vs[2] << endl;
    g.setProperty( "voxel_size", vs );
    bbmin.push_back( 0 );
    bbmin.push_back( 0 );
    bbmin.push_back( 0 );
    g.setProperty( "boundingbox_min", bbmin );
    g.setProperty( "boundingbox_max", bbmax );

    //	read texture
    TimeTexture<float>	tex;
    cout << "reading texture " << texR.fileName() << "..." << endl;
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
    string outtex = texR.fileName();
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
    texW <<  tb ;
    cout << "done" << endl;


    //Writting the curvature scalespace texture
    cout << "writing curvature scalespace texture : " << flush;
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
        string	datname = gw.fileName();
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
        datname = gwp.fileName();
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
            cout << "saving primal sketch " << gwp.fileName() << "...\n";
            gwp.write( psk );
          }
        catch( exception & e )
          {
            cerr << e.what() << endl;
          }
        try
          {
            cout << "saving glb " << gw.fileName() << "...\n";
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
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}
