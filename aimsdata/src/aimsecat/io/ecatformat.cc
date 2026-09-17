// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/ecatfinderformat.h>
#include <aims/io/ecatheader.h>
#include <aims/io/finder.h>

using namespace aims;
using namespace std;

bool FinderEcatFormat::check( const string & filename, Finder & f ) const
{
  //cout << "FinderEcatFormat::check( " << filename << " , ...)\n";
  EcatHeader *hdr = new EcatHeader( filename );
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      //cout << "ECAT failed, continuing\n";
      delete hdr;
      return( false );
    }

  //cout << "ECAT header read\n";
  f.setObjectType( "Volume" );
  f.setFormat( "ECAT" );
  vector<string>	vt;
  string		dt;
  hdr->getProperty( "data_type", dt );
  hdr->getProperty( "possible_data_types", vt );
  f.setDataType( dt );
  f.setPossibleDataTypes( vt );
  f.setHeader( hdr );

  return( true );
}


