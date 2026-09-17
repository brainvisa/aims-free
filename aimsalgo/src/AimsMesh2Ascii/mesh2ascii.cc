
#include <cstdlib>
#include <aims/mesh/surface.h>
#include <aims/io/reader.h>

using namespace aims;
using namespace std;


void usage( char* name )
{
  cerr << "usage : " << name << " filein.mesh fileout.txt\n";
  exit( 1 );
}


int main( int argc, char** argv )
{
  if( argc != 3 )
    usage( argv[0] );

  char	*filein = argv[1];
  char	*fileout = argv[2];
  Reader<AimsSurfaceTriangle> sr( filein );
  AimsSurfaceTriangle	s;
  sr >> s;

  const vector<Point3df>	& vert = s.vertex();
  unsigned	i, nv = vert.size();
  ofstream	of( fileout );
  if( !of )
    {
      cerr << "can't open " << fileout << "for writing\n";
      exit( 1 );
    }
  for( i=0; i<nv; ++i )
    {
      const Point3df	& p = vert[i];
      of << p[0] << " " << p[1] << " " << p[2] << endl;
    }
}


