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
  Point3dl chamfer_mask_size;
};


Morpho::Morpho() : Process(), radius(1.), use_chamfer( true ),
  chamfer_factor( 50.F ), chamfer_mask_size( 3, 3, 3 )
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
      proc.chamfer_mask_size = Point3dl( xmask, ymask, zmask );

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

