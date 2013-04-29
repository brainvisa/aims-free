
#include <aims/getopt/getopt2.h>
#include <aims/mesh/curv.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  Reader<AimsSurfaceTriangle> mr;
  string outfile;
  AimsApplication app( argc, argv, "Check if a mesh is closed. The program exists with failure state if the input mesh is not closed." );
  app.addOption( mr, "-i", "input mesh" );
  app.alias( "--input", "-i" );
  app.addOption( outfile, "-o", "output bad vertices file", true );
  app.alias( "--output", "-o" );

  try
  {
    app.initialize();
    AimsSurfaceTriangle mesh;
    mr.read( mesh );
    AimsSurfaceTriangle::const_iterator im, em = mesh.end();
    map<uint32_t, map<uint32_t, string> > badvertices;
    ofstream fout;
    ostream *pfout = &fout;
    if( !outfile.empty() )
      fout.open( outfile.c_str(), ios::out );
    else
      pfout = &cout;
    ostream & out = *pfout;

    out << "Timepoint, vertex, status\n";

    for( im=mesh.begin(); im!=em; ++im )
    {
      vector< list<unsigned> > ordered = AimsMeshOrderTriangle( im->second );
      bool first = true;
      uint32_t timepoint = im->first;
      const vector<AimsVector<uint32_t,3> > & poly = im->second.polygon();

      unsigned i, n = ordered.size();
      for( i=0; i<n; ++i )
      {
        const list<unsigned> & tril = ordered[i];
        if( tril.size() == 1 )
        {
          badvertices[im->first][ i ] = "single_triangle_attached_to_vertex";
          out << timepoint << ", " << i << ", " << badvertices[im->first][ i ]
            << "\n";
          continue;
        }
        set<uint32_t> sv;
        const AimsVector<uint32_t,3> & t1 = poly[ *tril.begin() ];
        const AimsVector<uint32_t,3> & t2 = poly[ *tril.rbegin() ];
        sv.insert( t1[0] );
        sv.insert( t1[1] );
        sv.insert( t1[2] );
        sv.insert( t2[0] );
        sv.insert( t2[1] );
        sv.insert( t2[2] );
        if( sv.size() != 4 )
        {
          badvertices[im->first][ i ] = "open_fan";
          out << timepoint << ", " << i << ", " << badvertices[im->first][ i ]
            << "\n";
        }
      }
    }

    if( !badvertices.empty() )
      return EXIT_FAILURE;

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

