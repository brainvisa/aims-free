
#include <aims/distancemap/meshmorphomat.h>
#include <aims/math/math_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <iostream>

typedef float float3[3];

using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;

template<typename T> bool
texerode( Process &, const string &, Finder & );


class TextureErosion : public Process
{
public:
  TextureErosion( AimsSurfaceTriangle &, float, int, const string &,
                  short, short);
  virtual ~TextureErosion();

  AimsSurfaceTriangle	& surface;
  float			erodesize;
  int			connexityFlag;
  string		outtexfile;
  short 		back,forbidden;

};


TextureErosion::TextureErosion( AimsSurfaceTriangle & m, float cs, 
                                int cf, const string & of,short ba,
                                short forb )
  : surface( m ), erodesize( cs ), connexityFlag( cf ), outtexfile( of ),
    back(ba), forbidden(forb)
{
  registerProcessType( "Texture", "FLOAT", &texerode<float> );
  registerProcessType( "Texture", "S16", &texerode<short> );
}


TextureErosion::~TextureErosion()
{
}


int main( int argc, const char** argv )
{
  string meshfile, intexfile, outtexfile;
  bool connexityFlag = false, greyFlag = false;
  float erodesize;
  int back = 0, forbidden = -1;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv,
    "Erosion of an image (binary or grey level) painted on a triangulation.\n"
    "Input texture of 'short int' are eroded with binary structuring "
    "element.\n"
    "The 'background label' corresponds to the label of the background "
    "(default = 0).\n"
    "The 'forbidden label' corresponds to the label out of the domain "
    "(default = -1).\n"
    "Input texture of 'float' are eroded with the min function as structuring "
    "element" );
  app.addOption( meshfile, "-i", "input mesh file" );
  app.alias( "--input", "-i" );
  app.addOption( intexfile, "-t", "object definition texture" );
  app.alias( "--texture", "-t" );
  app.addOption( outtexfile, "-o", "output texture file (labelled opening)" );
  app.alias( "--output", "-o" );
  app.addOption( erodesize, "-s", "erosion size (in mm)" );
  app.alias( "--size", "-s" );
  app.addOption( back, "-b", "background label", true );
  app.alias( "--background", "-b" );
  app.addOption( forbidden, "-f", "forbidden label", true );
  app.alias( "--forbidden", "-f" );
  app.addOption( connexityFlag, "--connexity",
    "euclidean or mesh connexity [default=euclidean]", true );
//   app.addOption( greyFlag, "--grey", "default: binary", true );

  try
  {
    app.initialize();

    if (connexityFlag || greyFlag)
      cout << "Size of the erosion : " << erodesize << " connections" << endl;
    else
      cout << "Size of the erosion : " << erodesize << "mm" << endl;

    //
    // read triangulation
    //
    cout << "reading triangulation   : " << flush;
    AimsSurfaceTriangle surface;
    Reader<AimsSurfaceTriangle> surfaceR( meshfile );
    surfaceR >> surface;
    cout << "done" << endl;

    TextureErosion	proc( surface, erodesize, connexityFlag,
                              outtexfile, back, forbidden );
    if( !proc.execute( intexfile ) )
      cerr << "Failed\n";
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


template<typename T>
void erosion( TextureErosion& tp, TimeTexture<T> & inpTex, 
              TimeTexture<T> & outTex );


template<typename T> bool
texerode( Process & p, const string & intexfile, Finder & )
{
  TextureErosion	& tp = (TextureErosion &) p;

  //
  // read input texture
  //
  cout << "reading texture   : " << flush;
  
  TimeTexture<T>	inpTex;
  Reader<TimeTexture<T> > texR( intexfile );
  texR >> inpTex;
 
  cout << "done" << endl;

  cout << "mesh vertices : " << tp.surface[0].vertex().size() << endl;
  cout << "mesh polygons : " << tp.surface[0].polygon().size() << endl;
  cout << "texture dim   : " << inpTex[0].nItem() << endl;
  
  TimeTexture<T>	outTex;

  erosion( tp, inpTex, outTex );

  cout << "writing texture : " << flush;
  Writer<TimeTexture<T> >	texW( tp.outtexfile );
  texW << outTex;
  cout << "done" << endl;

  return( true );
}


template<>
void erosion( TextureErosion & tp, TimeTexture<float> & inpTex, 
		 TimeTexture<float> & outTex )
{
  cout << "Grey-level erosion\n";
  cout << "Erosion..." << flush;
  outTex[0] = MeshGreyErosion( tp.surface[0], inpTex[0], 
				   (unsigned) tp.erodesize );
  cout << "done\n" << flush;
}


template<>
void erosion( TextureErosion & tp, TimeTexture<short> & inpTex, 
		 TimeTexture<short> & outTex )
{
  cout << "Binary erosion\n";
  outTex[0] = MeshErosion(tp.surface[0], inpTex[0] ,tp.back, tp.forbidden,
			  tp.erodesize,tp.connexityFlag);

}


