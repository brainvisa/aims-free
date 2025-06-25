// includes from AIMS
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/mesh/geometric.h>

using namespace aims;
using namespace carto;
using namespace std;
using namespace aims::meshgraph;


void removingPointsWithHighCurvature(int argc, const char * argv[])
{
  rc_ptr<AimsSurfaceTriangle> s;
  std::cout << "Reading mesh..." << std::flush;
  Writer<AimsSurfaceTriangle> w;
  float maxCurv;
  {
    Reader<AimsSurfaceTriangle> r;
    AimsApplication app( argc, argv,
                         "Remove points with high positive curvature\n"
                         "Points with negative curvature are left untouched.");
    app.addOption(r, "-i", "input mesh" );
    app.addOption(w, "-o", "output mesh" );
    app.addOption(maxCurv, "-maxCurv",
                  "maximum unoriented (but signed) Gaussian curvature allowed "
                  "in the mesh");
    app.initialize();

    s.reset( r.read() );
  }

  VertexRemover::cleanMesh( *s, maxCurv, true );

  std::cout << "Writing mesh..." << std::flush;
  {
    std::cout << "remaining: " << s->vertex().size() << std::endl;
    w.write( *s );
    std::cout << "OK.\n";
  }
}


int main( int argc, const char* argv[] )
{
  try
  {
#ifdef __TIME__
    std::cout << "(Compiled at " << __TIME__ << " on " << __DATE__ << ")" << std::endl;
#endif

    removingPointsWithHighCurvature( argc, argv );
  }
  catch( carto::user_interruption & )
  {
          // Exceptions thrown by command line parser (already handled, simply exit)
  }
  catch( exception & e )
  {
          cerr << e.what() << endl;
  }
}

