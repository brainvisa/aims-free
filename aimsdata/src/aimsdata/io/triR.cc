// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  Tri reader class
 */
#include <aims/io/triR.h>
#include <aims/io/triheader.h>
#include <cartobase/exception/ioexcept.h>

using namespace aims;
using namespace carto;
using namespace std;


void TriReader::read( AimsSurfaceTriangle& thing, int frame )
{
  if( frame >= 0 )
    cerr << "Warning : .tri single frame reading not implemented yet -\n" 
	 << "reading whole mesh\n";

  TriHeader	hdr( _name );
  size_t	offset;
  if( !hdr.read( &offset ) )
    io_error::launchErrnoExcept( hdr.filename() );

  ifstream	is( hdr.filename().c_str() );
  if( !is )
    io_error::launchErrnoExcept( hdr.filename() );
#ifdef _WIN32
  /* apparently windows can't properly tellg/seekg on ascii streams. 
     It's mysterious. */
  string tstr;
  is >> tstr >> tstr;
#else
  is.seekg( offset );
#endif
  if( !is )
    io_error::launchErrnoExcept( hdr.filename() );

  //	cleanup surface
  thing.erase();

  int nVertex = 0;
  hdr.getProperty( "vertex_number", nVertex );

  thing.vertex().reserve( nVertex );
  thing.normal().reserve( nVertex );

  Point3df vertex, normal;
  int		n;

  //cout << "TRI: reading vertices\n";
  for( n = 0; n < nVertex; ++n )
  {
    is >> vertex[ 0 ] >> vertex[ 1 ] >> vertex[ 2 ]
       >> normal[ 0 ] >> normal[ 1 ] >> normal[ 2 ];
    thing.vertex().push_back( vertex );
    thing.normal().push_back( normal );
  }
  /*cout << "TRI: vertices OK\n";
  cout << "first vertex: " << *thing.vertex().begin() << endl;
  cout << "last vertex: " << *thing.vertex().rbegin() << endl;*/

  string	tmp;
  is >> tmp;
  if( tmp != "-" )
    throw corrupt_stream_error( hdr.filename() );
  int nTriangle = 0;
  is >> nTriangle;
  is >> tmp;
  is >> tmp;

  thing.polygon().reserve( nTriangle );

  //cout << "TRI: reading triangles\n";

  AimsVector< uint, 3 > triangle;
  for ( n = 0; n < nTriangle; n++ )
  {
    is >> triangle[ 0 ] >> triangle[ 1 ] >> triangle[ 2 ];
    thing.polygon().push_back( triangle );
  }
  //cout << "TRI: triangles OK\n";

  if( thing.normal().size() != thing.vertex().size() )
    thing.updateNormals();

  thing.setHeader( hdr );
}
