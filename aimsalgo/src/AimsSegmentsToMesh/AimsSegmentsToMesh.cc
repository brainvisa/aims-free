
#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/surface.h>
#include <aims/vector/vector.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/defaultItemR.h>

using namespace aims;
using namespace carto;
using namespace std;


int main(int argc, const char *argv[])
{
  string fileIn;
  Writer< AimsSurfaceTriangle > writer;
  int verbose = 0;
  float cylinderRadius = 1;

  AimsApplication app( argc, argv,
                       "Take a list of segments (pairs of points) and build a "
                       "3D composed of cylinders. Each cylinder joins two "
                       "points of a segment." );
  app.addOption( fileIn, "-i", "Series of pairs of 3D points" );
  app.addOption( writer, "-o", "output mesh file" );
  app.addOption( cylinderRadius, "-w", "Width of the cylinder (default 1)",
                 true );
  app.addOption( verbose, "-v", "display information on standard error output",
                 true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--width", "-w" );
  app.alias( "--verbose", "-v" );

  try
  {
    app.initialize();

    AimsSurfaceTriangle result;

    // Read segments
    vector< Point3df > points;
    {
      if ( verbose ) cout << "Read " << fileIn << flush;
      ifstream in( fileIn.c_str() );
      DefaultAsciiItemReader< Point3df > pointReader;
      size_t segmentCount;
      in >> segmentCount;
      points.resize( 2 * segmentCount );
      for( size_t i = 0; i < points.size(); ++i ) {
        pointReader.read( in, points[ i ] );
      }
      if ( verbose ) cout << " done" << endl;
    }

    // Build cylinders
    for( size_t i = 0; i < points.size(); i += 2 ) {
      Point3df v( points[i+1] - points[ i ] );
      v /= v.norm();
      Point3df vBorders[ 4 ];
      if ( v[ 0 ] ) {
        vBorders[ 0 ] = crossed( v, Point3df( 0, 1, 0 ) );
      } else {
        vBorders[ 0 ] = crossed( v, Point3df( 1, 0, 0 ) );
      }
      vBorders[ 0 ] *= cylinderRadius / vBorders[ 0 ].norm();
      vBorders[ 1 ] = crossed( v, vBorders[ 0 ] );
      vBorders[ 1 ] *= cylinderRadius / vBorders[ 1 ].norm();
      vBorders[ 2 ] = crossed( v, vBorders[ 1 ] );
      vBorders[ 2 ] *= cylinderRadius / vBorders[ 2 ].norm();
      vBorders[ 3 ] = crossed( v, vBorders[ 2 ] );
      vBorders[ 3 ] *= cylinderRadius / vBorders[ 3 ].norm();

      // Add points
      size_t p = result.vertex().size();
      for( int j = 0; j < 4; ++j ) {
        result.vertex().push_back( points[ i ] + vBorders[ j ] );
        result.normal().push_back( vBorders[ j ] / vBorders[ j ].norm() );
      }
      for( int j = 0; j < 4; ++j ) {
        result.vertex().push_back( points[ i + 1 ] + vBorders[ j ] );
        result.normal().push_back( vBorders[ j ] / vBorders[ j ].norm() );
      }

      // Add triangles
      result.polygon().push_back( AimsVector<uint,3>( p + 3, p + 1, p ) );
      result.polygon().push_back( AimsVector<uint,3>( p + 3, p + 2, p + 1 ) );

      result.polygon().push_back( AimsVector<uint,3>( p + 4, p + 5, p + 7 ) );
      result.polygon().push_back( AimsVector<uint,3>( p + 5, p + 6, p + 7 ) );

      for( int j = 0; j < 4; ++j ) {
        result.polygon().push_back( AimsVector<uint,3>( p + (j+1) % 4 + 4,
                                                        p + j + 4,
                                                        p + j ) );
        result.polygon().push_back( AimsVector<uint,3>( p + j,
                                                        p + ( j + 1 ) % 4,
                                                        p + (j+1) % 4 + 4 ) );
      }
    }

    // Write result
    if ( verbose ) cout << "Write " << writer.fileName() << flush;
    writer.write( result );
    if ( verbose ) cout << " done" << endl;

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
