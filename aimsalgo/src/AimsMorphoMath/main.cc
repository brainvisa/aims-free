
#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/io/writer.h>
#include <aims/io/reader.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/process.h>
#include <aims/morphology/operatormorpho.h>


using namespace aims;
using namespace carto;
using namespace std;


class Morpho : public Process
{
public:
  Morpho();
  virtual ~Morpho();

  template <typename T> static bool morpho( Process & p, const string & fileIn,
                                            Finder & f );

  float radius;
  string fileOut, mode;
  bool use_chamfer;
  float chamfer_factor;
  Point3d chamfer_mask_size;
  bool bin;
};


Morpho::Morpho() : Process(), radius(1.), use_chamfer( true ),
  chamfer_factor( 50.F ), chamfer_mask_size( 3, 3, 3 ), bin( false )
{
  registerProcessType( "Volume", "S8",     &morpho<int8_t> );
  registerProcessType( "Volume", "U8",     &morpho<uint8_t> );
  registerProcessType( "Volume", "U16",    &morpho<uint16_t> );
  registerProcessType( "Volume", "S16",    &morpho<int16_t> );
  registerProcessType( "Volume", "S32",    &morpho<int32_t> );
  registerProcessType( "Volume", "U32",    &morpho<uint32_t> );
  registerProcessType( "Volume", "FLOAT",  &morpho<float> );
  registerProcessType( "Volume", "DOUBLE", &morpho<double> );
}


Morpho::~Morpho()
{
}

template<typename T>
bool Morpho::morpho( Process & p, const string & fileIn, Finder & f )
{
  Morpho &m = (Morpho &) p;

  MorphoGreyLevel<T> mgl;
  mgl.setChamferBinaryMorphoEnabled( m.use_chamfer );
  mgl.setChamferFactor( m.chamfer_factor );
  mgl.setChamferMaskSize( m.chamfer_mask_size );

  carto::VolumeRef<T> dataIn, dataOut;
  Reader< carto::Volume<T> > r( fileIn );
  string                format = f.format();
  dataIn.reset( r.read( mgl.neededBorderWidth(), &format ) );

  if( m.bin )
  {
    AimsThreshold<T, T> th( AIMS_DIFFER, 0 );
    dataIn = th.bin( dataIn );
  }

  // Specific operation
  int choice;
  if(m.mode=="ero") choice = 0;
  if(m.mode=="dil") choice = 1;
  if(m.mode=="clo") choice = 2;
  if(m.mode=="ope") choice = 3;

  switch ( choice )
    {
    case 0 :
      cout << "\nMorphological operator: EROSION" << endl << endl;
      dataOut = mgl.doErosion( dataIn, m.radius );
      break;

    case 1 :
      cout << "\nMorphological operator: DILATION" << endl << endl;
      dataOut = mgl.doDilation( dataIn, m.radius );
      break;

    case 2 :
      cout << "\nMorphological operator: CLOSING" << endl << endl;
      dataOut = mgl.doClosing( dataIn, m.radius );
      break;

    case 3 :
      cout << "\nMorphological operator: OPENING" << endl << endl;
      dataOut = mgl.doOpening( dataIn, m.radius );
      break;
    }

  // Ecriture du fichier
  Writer< AimsData< T > > w( m.fileOut );
  cout << "Writing result..." << endl;
  w.write( dataOut );
  cout << "End of the process." << endl;

  return true;
}

int main( int argc, const char **argv )

{
  int result = EXIT_SUCCESS;

  Morpho proc;
  ProcessInput        fileIn( proc );
  string fileOut;
  string mode;
  float radius;
  bool dont_use_chamfer = false;
  int   xmask = 3, ymask = 3, zmask = 3;
  float factor = 50;
  bool bin = false;

  AimsApplication application( argc, argv,
    "Morphological operators (erosion, dilation, closing, opening)"
    "\nfor both binary and gray level intensity images" );
  application.addOption( fileIn, "-i", "Input image", 1 );
  application.addOption( radius, "-r", "Radius parameter (in mm)", 1 );
  application.addOption( mode, "-m", "Mode that can be:\n\n"
                         "ero --> erosion\n"
                         "dil --> dilation\n"
                         "clo --> closing\n"
                         "ope --> opening", 1 );
  application.addOption( fileOut, "-o", "Output image", 1 );
  application.addOption( dont_use_chamfer, "-c",
    "do not use chamfer binary mathematical morphology (default: use it when "
    "available)", true );
  application.addOption( bin, "-b",
    "binarize: binarize the input image so that fast binary morphomath will "
    "be used preferably", true );
  application.addOption( xmask, "-x",
    "X size of the chamfer distance mask (unused with grey level morpho) "
    "[default=3]", true );
  application.addOption( ymask, "-y",
    "Y size of the chamfer distance mask (unused with grey level morpho) "
    "[default=3]", true );
  application.addOption( zmask, "-z",
    "Z size of the chamfer distance mask (unused with grey level morpho) "
    "[default=3]", true );
  application.addOption( factor, "-f",
    "chamfer multiplication factor (unused with grey level morpho) "
    "[default=50]", true );

  try
    {

      application.initialize();

      proc.radius = radius;
      proc.fileOut = fileOut;
      proc.mode = mode;
      proc.use_chamfer = !dont_use_chamfer;
      proc.chamfer_factor = factor;
      proc.chamfer_mask_size = Point3d( xmask, ymask, zmask );
      proc.bin = bin;

      if( !proc.execute( fileIn.filename ) )
        cout << "Couldn't process file - aborted\n";

    }
  catch( user_interruption &e ) {}
  catch( std::exception &e )
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
      result = EXIT_FAILURE;
    }
  return result;
}

