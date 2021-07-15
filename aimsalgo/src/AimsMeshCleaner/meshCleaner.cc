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
    AimsApplication app( argc, argv, "removingPointsWithHighCurvature");
    app.addOption(r, "-i", "input mesh" );
    app.addOption(w, "-o", "output mesh" );
    app.addOption(maxCurv, "-maxCurv", "maximum unoriented Gaussian curvature allowed in the mesh");
    app.initialize();

    s.reset( r.read() );
  }

  aims::GaussianCurvature *curv = new GaussianCurvature( s );
  aims::GaussianCurvature & mc_aims = *curv;
  rc_ptr<GeometricProperties> gp( curv );
  mc_aims.doNeighbor();

  float gaussianCurvature, meanCurvature, orientedMeanCurvature, orientedGaussianCurvature, voronoiArea;
  std::pair<float, float> principalCurvatures;
  Point3df normal;

  unsigned int count = 0;

    aims::VertexRemover vertexRemover( gp );
    bool flagFinished;
    unsigned int iter = 0;
    do
    {
      flagFinished = true;
      std::cout << "pass " << ++iter << "... " << std::flush;
      size_t i;
      for( list<MeshGraphVertex>::iterator
           i = vertexRemover.geometricProperties().getVertices().begin();
           i != vertexRemover.geometricProperties().getVertices().end(); )
      {
        mc_aims.localProcess(i, gaussianCurvature, meanCurvature,
                        principalCurvatures, orientedMeanCurvature,
                        orientedGaussianCurvature, normal, voronoiArea );
        if( fabs( gaussianCurvature ) >= maxCurv )
        {
          if (vertexRemover(i))
          {
            flagFinished = false;
            ++count;
            continue;
          }
        }
        ++i;
      }
      std::cout << " Removed " << count << " vertices" << std::endl;
    } while (!flagFinished);

  std::cout << "Remove " << count << " points" << std::endl;

  std::cout << "Converting into mesh...\n" << std::flush;
  vertexRemover.geometricProperties().graphToMesh();

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

