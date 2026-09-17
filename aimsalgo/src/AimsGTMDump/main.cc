
#include <aims/data/data.h>
#include <aims/getopt/getopt2.h>
#include <aims/roi/roigtm.h>
using aims::RoiGtm;
#include <aims/io/roigtmR.h>
using aims::RoiGtmReader;

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  string filein;

  AimsApplication app( argc, argv, "Dump Gtm" );
  app.addOption( filein, "-i", "origin Roi file (arg format)" );
  app.alias( "--input", "-i" );

  try
  {
    app.initialize();

    // Read GTM from file Arg (Preferred or image)
    RoiGtm rgtm;
    RoiGtmReader rgtmR( filein, RoiGtmReader::read_gtmarg_only );
    rgtmR.read( rgtm );

    rgtm.streamout();

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
