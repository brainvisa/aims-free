// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  Tri writer class
 */
#include <aims/io/triW.h>
#include <aims/io/triheader.h>
#include <cartobase/exception/file.h>

using namespace aims;
using namespace carto;
using namespace std;

void 
TriWriter::write( const AimsSurfaceTriangle& thing )
{
  TriHeader	hdr( _name );
  string name = hdr.removeExtension( _name ) + ".tri";
  ofstream	os( name.c_str() );
  if( !os )
    throw file_error( name );

  os << "- " << thing.vertex().size() << endl;

  vector<Point3df>::size_type n;

  for ( n = 0; n < thing.vertex().size(); n++ )
    os << thing.vertex()[ n ][ 0 ] << " "
       << thing.vertex()[ n ][ 1 ] << " "
       << thing.vertex()[ n ][ 2 ] << " "
       << thing.normal()[ n ][ 0 ] << " "
       << thing.normal()[ n ][ 1 ] << " "
       << thing.normal()[ n ][ 2 ] << endl;
  os << "- " << thing.polygon().size() << " "
     << thing.polygon().size() << " "
     << thing.polygon().size() << endl;
  for ( n = 0; n < thing.polygon().size(); n++ )
    os << thing.polygon()[ n ][ 0 ] << " "
       << thing.polygon()[ n ][ 1 ] << " "
       << thing.polygon()[ n ][ 2 ] << endl;

  hdr.copy( thing.header() );
  if( hdr.hasProperty( "nb_t_pos" ) )
    hdr.removeProperty( "nb_t_pos" );
  hdr.writeMinf( hdr.filename() + ".minf" );
}


