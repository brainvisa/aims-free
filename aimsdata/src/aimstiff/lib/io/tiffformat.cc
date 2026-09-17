#include <aims/io/tifffinderformat.h>
#include <aims/io/tiffheader.h>
#include <aims/io/finder.h>

using namespace aims;
using namespace std;

bool FinderTiffFormat::check( const string & filename, Finder & f ) const
{
  TiffHeader *hdr = new TiffHeader( filename );
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      delete hdr;
      return( false );
    }
  f.setObjectType( "Volume" );
  f.setFormat( "TIFF" );
  f.setDataType( hdr->dataType() );
  f.setPossibleDataTypes( hdr->possibleDataTypes() );
  f.setHeader( hdr );

  return( true );
}


