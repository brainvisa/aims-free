#include <aims/getopt/getopt2.h>
#include <cartodata/volume/volume.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <list>


using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{

  string fileIn;
  list< short > labels;
  string fileOut;
  short background = -1;
  int x, y, z, t;
  bool keepIt;

  AimsApplication app( argc, argv,
     "Select labels from a label data and put it into a new label data into"
     " a new label data" );
  app.addOption( fileIn, "-i", "input label data filename" );
  app.addOptionSeries( labels, "-l",
                       "selected label values", 1 );
  app.addOption( fileOut, "-o", "output label data filename" );
  app.addOption( background, "-b",
                 "background label (default=-1)", 1 );

  try
  {

    app.initialize();

    if ( labels.empty() )
    {

      cerr << "there should be at least 1 label selected" << endl;
      throw;

    }


    Reader< Volume< short > > reader( fileIn );
    VolumeRef< short > data;

    cout << "reading input label data" << flush;
    data.reset( reader.read() );
    cout << endl;

    cout << "selecting labels" << flush;
    list< short >::const_iterator l = labels.begin(), le = labels.end();
    int dx = data->getSizeX(), dy = data->getSizeY(), dz = data->getSizeZ(),
      dt = data->getSizeT();
    for ( t = 0; t < dt; t++ )
      for ( z = 0; z < dz; z++ )
        for ( y = 0; y < dy; y++ )
          for ( x = 0; x < dx; x++ )
          {

            short value = data->at( x, y, z, t );

            l = labels.begin();
            keepIt = false;
            while ( l != le )
            {

              if ( *l == value )
              {

                keepIt = true;

              }
              l ++;

            }
            if ( !keepIt )
            {

              data->at( x, y, z, t ) = background;

            }

          }
    cout << endl;


    cout << "saving selection" << flush;
    Writer< VolumeRef< short > > writer( fileOut );
    writer.write( data );
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


