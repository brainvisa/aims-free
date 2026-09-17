#include <aims/io/plyfinderformat.h>
#include <aims/io/plyheader.h>
#include <aims/io/finder.h>

using namespace aims;
using namespace std;

bool FinderPlyFormat::check( const string & filename, Finder & f ) const
{
  PlyHeader *hdr = new PlyHeader( filename );
  try
    {
      if( !hdr->read() )
        return false;
    }
  catch( exception & e )
    {
      delete hdr;
      return false;
    }
  f.setFormat( "PLY" );
  string		dt;
  string		ot;
  vector<string>	pdt;
  hdr->getProperty( "object_type", ot );
  hdr->getProperty( "data_type", dt );
  hdr->getProperty( "possible_data_types", pdt );
  f.setDataType( dt );
  f.setObjectType( ot );
  f.setPossibleDataTypes( pdt );
  f.setHeader( hdr );

  return( true );
}


