#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/tiffR.h>
#include <aims/io/writer.h>
#include <list>
#include <string>


using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{

  string fileIn;
  string fileOut;

  AimsApplication app( argc, argv,
     " Lit des données Tiff" );
  app.addOption( fileIn, "-i", "input data filename" );
  app.addOption( fileOut, "-o", "output label data filename", true );

  try
  {

    app.initialize();


    TiffReader<  ushort > reader( fileIn );
    AimsData<  ushort > data;

    reader >> data;


    cout << "DBG TIFF " << data.dimX() << " "
	 << data.dimY() << " "
	 << data.dimZ() << " "
	 << data.dimT() << endl;

    cout  << "DBG TIFF " << data(10, 10, 0,0) << endl;
    cout  << "DBG TIFF " << data(1000, 100, 0,0) << endl;
    cout  << "DBG TIFF " << data(1200, 500, 0,0) << endl;
    cout  << "DBG TIFF " << data(2000, 4000, 0,0) << endl;


    Writer< AimsData<ushort> > w("essai");
    w << data;
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

