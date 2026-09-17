// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/giftifinderformat.h>
#include <aims/io/giftiheader.h>
#include <aims/io/finder.h>

using namespace aims;
using namespace std;

bool FinderGiftiFormat::check( const string & filename, Finder & f ) const
{
  GiftiHeader *hdr = new GiftiHeader( filename );
  bool ok = true;
  try
  {
    ok = hdr->read();
  }
  catch( exception & e )
  {
    ok = false;
  }
  if( !ok )
  {
    delete hdr;
    return false;
  }

  string ot;
  hdr->getProperty( "object_type", ot );
  f.setObjectType( ot );
  f.setFormat( "GIFTI" );

  string dt;
  hdr->getProperty( "data_type", dt );
  if( dt.empty() )
    dt = "VOID";
  f.setDataType( dt );
  vector<string> vt;
  hdr->getProperty( "possible_data_types", vt );
  f.setPossibleDataTypes( vt );
  f.setHeader( hdr );

  return( true );
}
