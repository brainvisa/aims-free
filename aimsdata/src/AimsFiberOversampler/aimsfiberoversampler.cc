#include <aims/fibers/fiberOversampler.h>
#include <aims/getopt/getopt2.h>

using namespace std;
using namespace carto;
using namespace aims;

//////////////////////////////////////////////////////////////////////
//	main
/////////////////////////////////////////////////////////////////////

int main( int argc, const char *argv[] )
{
   int result = EXIT_SUCCESS;

   try
   {
      string fileNameIn;
      string fileNameOut;
      string bundleName;
      string bundlesFile;
      int points = 1;
      bool verbose = false;

      // Building application
      AimsApplication application( argc, argv,
        "Oversample fibers (number of points");
      application.addOption( fileNameIn, "-i", "Bundle File input");
      application.addOption( fileNameOut, "-o", "Bundle File output");
      application.addOption( points, "-points", "number of points to intecalate between 2 points), default = 1", true);

      application.addOption( verbose, "-verbose",
                           "show as much information as possible", true );

      application.initialize();

      BundleReader bundle(fileNameIn);

      rc_ptr< FiberOversampler > fiberOversampler;
      fiberOversampler.reset( new FiberOversampler(points) );

      rc_ptr< BundleWriter > bundleWriter;
      bundleWriter.reset( new BundleWriter() );
      bundleWriter->setFileString(fileNameOut);
      fiberOversampler->addBundleListener( *bundleWriter );

      if (verbose) std::cout << "bunOut: " << fileNameOut << endl;

      bundle.addBundleListener(*fiberOversampler);
      bundle.read();

      if (verbose) cout << "done" << endl; 
   }

   catch( user_interruption &e ) {
   }
   catch( std::exception &e ) {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
      result = EXIT_FAILURE;
  }

  return result;
}
