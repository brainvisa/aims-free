#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/rgb/rgb.h>


using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  string fileInR, fileInG, fileInB, fileOut;

  AimsApplication app( argc, argv,
                  "Merge 3 AimsData<byte> files to an AimsData<AimsRGB>" );
  app.addOption( fileInR, "-r", "input U8 data R channel" );
  app.addOption( fileInG, "-g", "input U8 data G channel" );
  app.addOption( fileInB, "-b", "input U8 data B channel" );
  app.addOption( fileOut, "-o", "output data" );

  try
  {

    app.initialize();

    Reader< AimsData< ::byte > > reader;
    AimsData< ::byte > dataR, dataG, dataB;

    cout << "reading R channel";
    reader.setFileName( fileInR );
    reader >> dataR;
    cout << endl;

    cout << "reading G channel";
    reader.setFileName( fileInG );
    reader >> dataG;
    cout << endl;

    cout << "reading B channel";
    reader.setFileName( fileInB );
    reader >> dataB;
    cout << endl;

    if ( !( dataR.dimX() == dataG.dimX() &&
            dataG.dimX() == dataB.dimX() &&
            dataR.dimY() == dataG.dimY() &&
            dataG.dimY() == dataB.dimY() &&
            dataR.dimZ() == dataG.dimZ() &&
            dataG.dimZ() == dataB.dimZ() &&
            dataR.sizeX() == dataG.sizeX() &&
            dataG.sizeX() == dataB.sizeX() &&
            dataR.sizeY() == dataG.sizeY() &&
            dataG.sizeY() == dataB.sizeY() &&
            dataR.sizeZ() == dataG.sizeZ() &&
            dataG.sizeZ() == dataB.sizeZ() ) )
    {

      cerr << "R/G/B channel do not have same dim and size" << endl;
      throw;
      
    }


    cout << "building RGB data";
    int x, y, z, t;
    AimsData< AimsRGB > dataRgb( dataR.dimX(),
                                 dataR.dimY(),
                                 dataR.dimZ(),
                                 dataR.dimT() );
    dataRgb.setSizeX( dataR.sizeX() );
    dataRgb.setSizeY( dataR.sizeY() );
    dataRgb.setSizeZ( dataR.sizeZ() );
    dataRgb.setSizeT( dataR.sizeT() );
    cout << endl;

    for ( t = 0; t < dataRgb.dimT(); t++ )
      for ( z = 0; z < dataRgb.dimZ(); z++ )
        for ( y = 0; y < dataRgb.dimY(); y++ )
          for ( x = 0; x < dataRgb.dimX(); x++ )
            dataRgb( x, y, z, t ) = AimsRGB( dataR( x, y, z, t ),
                                             dataG( x, y, z, t ),
                                             dataB( x, y, z, t ) );
    cout << endl;

    cout << "saving RGB data";
    Writer< AimsData< AimsRGB > > writer( fileOut);
    writer.write(dataRgb);
    cout << endl;

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


