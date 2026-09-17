
#include <aims/scalespace/meshBlob.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <aims/connectivity/meshcc.h>


using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  string meshfile, intexfile, outtexfile;
  int T = 0;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv, "Extract blobs" );
  app.addOption( meshfile, "-i", "input mesh file" );
  app.alias( "--input", "-i" );
  app.addOption( intexfile, "-t", "object definition" );
  app.alias( "--texture", "-t" );
  app.addOption( outtexfile, "-o", "output texture file (blobs)" );
  app.alias( "--output", "-o" );
  app.addOption( T, "-T", "time point [default = 0]", true );
  app.alias( "--Time", "-T" );
  app.alias( "--time", "-T" );

  try
  {
    app.initialize();

    //
    // read triangulation
    //
    cout << "reading triangulation   : " << flush;
    AimsSurfaceTriangle surface;
    Reader<AimsSurfaceTriangle>	triR( meshfile );
    triR >> surface;
    cout << "done" << endl;

    //
    // read input texture
    //
    cout << "reading texture   : " << flush;
    TimeTexture<float>	inpTex;
    Reader<Texture1d> texR( intexfile );
    texR >> inpTex;
    cout << "done" << endl;

    cout << "mesh vertices : " << surface[0].vertex().size() << endl;
    cout << "mesh polygons : " << surface[0].polygon().size() << endl;
    cout << "texture dim   : " << inpTex[T].nItem() << endl;

    TimeTexture<float>			outTex;
    map<int, ScaleSpace::BlobDescriptor>	limits;

    ScaleSpace	ss;
    Texture<int>	texint = ss.meshBlobExtract( surface[0], inpTex[T],
                                                     limits );

    for( unsigned i=0, n=texint.nItem(); i<n; ++i )
      outTex[0].push_back( texint.item( i ) );


    cout << "writing texture : " << flush;
    Writer<Texture1d>	texW( outtexfile );
    texW.write( outTex );
    cout << "done" << endl;
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}


