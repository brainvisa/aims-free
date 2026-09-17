#include <aims/io/vidafinderformat.h>
#include <aims/io/vidaheader.h>
#include <aims/io/finder.h>

using namespace aims;
using namespace std;

bool FinderVidaFormat::check( const string & filename, Finder & f ) const
{
  string::size_type	pos = filename.rfind( '.' );
  string		ext, fname;

  if( pos == string::npos )
    fname = filename + ".vimg";
  else
    fname = filename;

  VidaHeader *hdr = new VidaHeader( fname );
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
  f.setFormat( "VIDA" );
  vector<string>	vt;
  if( hdr->imageIsProbablyLabel() )
    {	// prefers to be read as short
      f.setDataType( "S16" );
      vt.push_back( "S16" );
      vt.push_back( "FLOAT" );
    }
  else
    {	// prefers to be read as float
      f.setDataType( "FLOAT" );
      vt.push_back( "FLOAT" );
      vt.push_back( "S16" );
    }
  f.setPossibleDataTypes( vt );
  f.setHeader( hdr );

  return( true );
}
