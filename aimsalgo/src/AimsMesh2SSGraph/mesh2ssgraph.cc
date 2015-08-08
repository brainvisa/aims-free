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
#include <graph/graph/graph.h>
#include <cartobase/config/paths.h>
#include <aims/data/pheader.h>
#include <graph/graph/gwriter.h>
#include <cartobase/object/sreader.h>
#include <cartobase/stream/directory.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>


using namespace aims;
using namespace carto;
using namespace std;


void usage( const string & pname )
{
  cerr << pname << " [-p prim.arg] [-s surftype] [-b] [-t trans] "
       << "[-g growmode] in.mesh inflate.mesh outgraph.arg irm.ima textures...\n";
  cerr << "Makes a primal sketch graph from a mesh and a set of smoothed " 
       << "textures\n\n";
  cerr << "Options :\n";
  cerr << "-p    prim.arg : make also the corresponding primal sketch graph\n";
  cerr << "-s    surftype : tore (default) or surface\n";
  cerr << "-b             : blobs are already extracted, don't search for "
       << "them\n";
  cerr << "                 (textures are labels)\n";
  cerr << "-t    trans    : initial translation of extracted surfaces " 
       << "(default: 0)\n";
  cerr << "-g    growmode : mesh growing mode along scales: scale (default), " 
       << "translate, or pushnormal\n";
  exit(1);
}


int main( int argc, char** argv )
{
  if( argc < 5 )
    usage( argv[0] );

  string	stype, growmode, primal;
  bool		noblob = false, errorflg = false;
  int		c;
  float		depl0 = 0;

  while( ( c = getopt( argc, argv, "bstgp" ) ) != EOF )
    switch( c )
      {
      case 'b':
	noblob = true;
	break;
      case 's':
	stype = argv[ optind++ ];
	break;
      case 't':
	sscanf( argv[ optind++], "%f", &depl0 );
	break;
      case 'g':
	growmode = argv[ optind++ ];
	break;
      case 'p':
	primal = argv[ optind++ ];
	break;	
      case 'h':
      case '?':
      default:
	errorflg = true;
      }
  if( errorflg )
    usage( argv[0] );

  ScaleSpace::MeshShape		mshape = ScaleSpace::Tore;
  ScaleSpace::MeshGrowingMode	mgrow = ScaleSpace::Scale;
  ScaleSpace			ss;
  unsigned			i, n = argc - optind - 3;
  char				*texname;
  Graph				g( "ClusterArg" );
  string			gname = argv[optind + 1], 
    volfile = argv[optind + 2];

  if( stype == "surface" )
    mshape = ScaleSpace::Surface;
  else if( !stype.empty() && stype != "tore" )
    {
      cerr << "Invalid surface type - reread the help...\n\n";
      usage( argv[0] );
    }

  if( growmode == "translate" )
    mgrow = ScaleSpace::Translate;
  else if( growmode == "pushnormal" )
    mgrow = ScaleSpace::PushNormal;
  else if( !growmode.empty() && growmode != "scale" )
    {
      cerr << "Invalid growing mode - reread the help...\n\n";
      usage( argv[0] );
    }

  //
  // read triangulation
  //
  string	meshfile = argv[optind];

  cout << "reading triangulations   : " << flush;
  AimsSurfaceTriangle surface;
  Reader<AimsSurfaceTriangle> triR( meshfile );
  triR >> surface;
  cout << "done" << endl;
  
  string	inflate_meshfile = argv[optind];

  AimsSurfaceTriangle infl_surface;
  Reader<AimsSurfaceTriangle> triRR( inflate_meshfile );
  triRR >> infl_surface;
  cout << "done" << endl;
  //
  // read volume
  //
  Finder	f;
  f.check( volfile );
  const PythonHeader	*hd = dynamic_cast<const PythonHeader *>( f.header() );
  ASSERT( hd );
  vector<float>	vs = hd->getProperty( "voxel_size" )->value<vector<float> >();
  cout << "voxel size : " << vs[0] << ", " << vs[1] << ", " << vs[2] << endl;
  g.setProperty( "voxel_size", vs );
  vector<int>	bbmin, bbmax;
  bbmin.push_back( 0 );
  bbmin.push_back( 0 );
  bbmin.push_back( 0 );
  g.setProperty( "boundingbox_min", bbmin );
  hd->getProperty( "volume_dimension", bbmax );
  g.setProperty( "boundingbox_max", bbmax );


  float	scale;
  bool	first = true;
  int	index = 0;

  cout << "nb of textures : " << n << endl;

  for( i=0; i<n; ++i )
    {
      scale = i;
      texname = argv[optind + i + 3];
      TimeTexture<float>	tex;
      cout << "reading texture " << texname << "..." << endl;
      try
	{
	  Reader<Texture1d>	texR( texname );
	  texR >> tex;
	}
      catch( exception & e )
	{
	  cerr << e.what() << endl;
	  exit( 1 );
	}

      Graph					h( "ClusterArg" );
      map<int, ScaleSpace::BlobDescriptor>	limits;

      Texture<int>	tblob;
      if( noblob )
	{
	  unsigned	i, n = tex[0].nItem();
	  tblob.reserve( n );
	  for( i=0; i<n; ++i )
	    tblob.push_back( (int)( tex[0].item( i ) + 0.5f ) );
	}
      else
	tblob = ss.meshBlobExtract( surface[0], tex[0], limits );
      if( first )
	{
	  index +=
	    ss.blobToGraph( g, tblob, surface, infl_surface,tex[0], scale, limits, index );
	  first = false;
	}
      else
	{
	  index += 
	    ss.blobToGraph( h, tblob, surface, infl_surface, tex[0], scale, limits, index );

	  ScaleSpace::MergeCode	ret = ss.mergeGraphs( g, h );
	  if( ret )
	    {
	      cout << "graph merging in scalespace failed ( " << (int) ret 
		   << " )...\n";
	      //exit( 1 );
	    }
	}
    }

  string	datname = gname;
  vector<int>	color;

  string::size_type	pos = datname.rfind( '.' );
  if( pos != string::npos )
    datname.replace( pos+1, datname.length() - pos -1, "data" );
  else
    datname += ".data";

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
  ss.createSubMeshes( g, surface[0], 1, depl0, mshape, mgrow );

  SyntaxSet	s;
  try
    {
      SyntaxReader	sr( Paths::findResourceFile(
        "nomenclature/syntax/graph.stx", "aims" ) );
      sr >> s;
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      exit( 1 );
    }

  if( !primal.empty() )
    {
      cout << "making primal sketch...\n";
      Graph	psk( "ClusterArg" );
      ss.grayLevel2PrimalSketch( g, psk );
      psk.setProperty( "voxel_size", vs );
      psk.setProperty( "boundingbox_min", bbmin );
      psk.setProperty( "boundingbox_max", bbmax );
      try
	{
	  cout << "saving primal sketch " << primal << "...\n";
	  GraphWriter	gw( primal, s );
	  gw << psk;
	}
      catch( exception & e )
	{
	  cerr << e.what() << endl;
	}
    }

  try
    {
      cout << "saving gray-level blobs graph...\n";
      GraphWriter	gw( gname, s );
      gw << g;
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      exit( 1 );
    }

  cout << "done" << endl;
  return( 0 );
}
