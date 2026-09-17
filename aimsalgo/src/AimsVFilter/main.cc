
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/data/data.h>
#include <aims/getopt/getopt2.h>
#include <aims/signalfilter/vfilter.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  Reader<AimsData<short> > reader;
  Writer<AimsData<short> > writer;
  int msize = 2;
  char *atype = NULL;
  AimsVFilter< short >::VFilterType algo;

  AimsApplication app( argc, argv, "apply a V-Filter to an image" );
  app.addOption( reader, "-i", "input file" );
  app.addOption( writer, "-o", "output file" );
  app.addOption( msize, "-m", "mask size [default=2]", true );
  app.addOption( atype, "-t",
                 "algo type: o -> optimized, n -> non optimized  [default=o]",
                 true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--masksize", "-m" );
  app.alias( "--type", "-t" );

  try
  {
    app.initialize();

    AimsData< short > vol;

    reader.read( vol );

    if ( atype == NULL || string( atype ) == "o" )
      algo = AimsVFilter< short >::Optimized;
    else if ( string( atype ) == "n" )
      algo = AimsVFilter< short >::NonOptimized;
    else
    {
      cerr << "unknown algo type " << atype << endl;
      return EXIT_FAILURE;
    }

    if ( msize < 2 )  msize = 2;

    AimsVFilter< short > vfilter( msize, algo );
    AimsData< short > res = vfilter.doit( vol );

    writer.write( res );

    return EXIT_SUCCESS;
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}
