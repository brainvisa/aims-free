
#include <cstdlib>
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/def/path.h>
#include "activPixel.h"
#include "activPixelR.h"
#include "pix2vol.h"

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  try
    {
      Reader<AimsData<short> >	filein;
      string			pixfile;
      Writer<AimsData<short> >	fileout;
      short label = 255;
      Point3df trans;
      AimsApplication	app( argc, argv, "Convert an Activ pixel list file "
			     "into a volume" );
      trans[0] = trans[1] = trans[2] = 0.0f;
      app.addOption( filein, "-i", "origin file" );
      app.addOption( pixfile, "-p", "activ pixels list file" );
      app.addOption( fileout, "-o", "output image [default=input]", true );
      app.addOption( label, "-l", "filling value [default=255]", true );
      app.addOption( trans[0], "-x", 
		     "value of translation along x axis [default=0.0]", true );
      app.addOption( trans[1], "-y", 
		     "value of translation along y axis [default=0.0]", true );
      app.addOption( trans[2], "-z", 
		     "value of translation along z axis [default=0.0]", true );

      app.initialize();

      if( fileout.fileName().empty() )
	fileout.setFileName( filein.fileName() );

      cout << endl << "Reading template file : " << filein.fileName() << endl;
      AimsData< short >		vol;
      filein.read( vol );

      cout << "Reading pixel list file : " << pixfile << endl;
      AimsActivPixelReader reader( pixfile );
      AimsActivPixel pixels;
      reader >> pixels;

      cout << "Converting to volume..." << endl;
      AimsData< short > res = AimsActivPixel2Volume( vol, pixels, trans, 
						     label );

      cout << "Writing file : " << fileout.fileName() << endl;
      fileout.write( res );
    }
  catch( user_interruption &e )
    {
    }
  catch( std::exception &e )
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
