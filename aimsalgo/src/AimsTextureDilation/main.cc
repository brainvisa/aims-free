
#include <aims/distancemap/meshmorphomat.h>
#include <aims/data/data_g.h>
#include <aims/math/math_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>

typedef float float3[3];

using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;


template<typename T> bool
texdilate( Process &, const string &, Finder & );


class TextureDilation : public Process
{
public:
  TextureDilation( AimsSurfaceTriangle &, float, int, const string &,short,short);
  virtual ~TextureDilation();

  AimsSurfaceTriangle	& surface;
  float			dilatesize;
  int			connexityFlag;
  string		outtexfile;
  short 		back,forbidden;
};


TextureDilation::TextureDilation( AimsSurfaceTriangle & m, float cs, 
                                  int cf, const string & of,short ba,
                                  short forb)
  : surface( m ), dilatesize( cs ), connexityFlag( cf ), outtexfile( of ),
    back(ba), forbidden(forb)
{
  registerProcessType( "Texture", "FLOAT", &texdilate<float> );
  registerProcessType( "Texture", "S16", &texdilate<short> );
}


TextureDilation::~TextureDilation()
{
}


int main( int argc, const char** argv )
{
  string meshfile, intexfile, outtexfile;
  bool connexityFlag = false;
  float dilatesize;
  int back = 0, forbidden = -1;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv,
    "Dilation of an image (binary or grey level) painted on a triangulation.\n"
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
  app.addOption( dilatesize, "-s", "dilation size (in mm)" );
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

    if (connexityFlag )
      cout << "Size of the dilation : " << dilatesize << " connections"
        << endl;
    else
      cout << "Size of the dilation : " << dilatesize << "mm" << endl;

    //
    // read triangulation
    //
    cout << "reading triangulation   : " << flush;
    AimsSurfaceTriangle surface;
    Reader<AimsSurfaceTriangle> surfaceR( meshfile );
    surfaceR >> surface;
    cout << "done" << endl;

    TextureDilation	proc( surface, dilatesize, connexityFlag,
                              outtexfile, back, forbidden);
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
void dilatation( TextureDilation & tp, TimeTexture<T> & inpTex, 
		 TimeTexture<T> & outTex );


template<typename T> bool
texdilate( Process & p, const string & intexfile, Finder & )
{
  TextureDilation	& tp = (TextureDilation &) p;

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

  dilatation( tp, inpTex, outTex );

  cout << "writing texture : " << flush;
  Writer<TimeTexture<T> >	texW( tp.outtexfile );
  texW << outTex;
  cout << "done" << endl;

  return true;
}


template<>
void dilatation( TextureDilation & tp, TimeTexture<float> & inpTex, 
		 TimeTexture<float> & outTex )
{
  cout << "Grey-level dilation\n";
  cout << "Dilation..." << flush;
  outTex[0] = MeshGreyDilation( tp.surface[0], inpTex[0], 
  			    (unsigned) tp.dilatesize );
  cout << "done\n";
}


template<>
void dilatation( TextureDilation & tp, TimeTexture<short> & inpTex, 
		 TimeTexture<short> & outTex )
{
  
  cout << "Binar dilation\n";
  if (tp.connexityFlag)
    cout << " metric : connexity";
  else
    cout << " metric : euclidean";
  outTex[0] = MeshDilation(tp.surface[0], inpTex[0], tp.back , tp.forbidden ,tp.dilatesize,tp.connexityFlag);

}
