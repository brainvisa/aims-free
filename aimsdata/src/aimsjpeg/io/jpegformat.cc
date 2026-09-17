#include <aims/io/jpegfinderformat.h>
#include <aims/io/jpegheader.h>
#include <aims/io/finder.h>

using namespace aims;
using namespace std;

bool FinderJpegFormat::check( const string & filename, Finder & f ) const
{
  JpegHeader *hdr = new JpegHeader( filename );
  try
  {
    hdr->read();
  }
  catch( exception & e )
  {
    delete hdr;
    throw;
  }
  f.setObjectType( "Volume" );
  f.setFormat( "JPEG" );
  f.setDataType( hdr->dataType() );
  f.setPossibleDataTypes( hdr->possibleDataTypes() );
  f.setHeader( hdr );

  return( true );
}


