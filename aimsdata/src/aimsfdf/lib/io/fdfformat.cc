// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/fdffinderformat.h>
#include <aims/io/fdfheader.h>
#include <aims/io/finder.h>

using namespace aims;
using namespace std;

bool FinderFdfFormat::check( const string & filename, Finder & f ) const
{
  FdfHeader *hdr = new FdfHeader( filename );
  int	status = 0;
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      // cerr << " FinderFdfFormat::check exception: " << e.what() << endl;
      delete hdr;
      return( false );
    }
  if( status < 0 )
    {
      // cerr << " FinderFdfFormat::check status = " << status << endl;
      delete hdr;
      return( false );
    }

  f.setObjectType( "Volume" );
  f.setFormat( "FDF" );
  f.setDataType( hdr->dataType() );
  f.setPossibleDataTypes( hdr->possibleDataTypes() );
  f.setHeader( hdr );

  return( true );
}
