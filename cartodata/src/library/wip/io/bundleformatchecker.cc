#ifdef USE_SOMA_IO
  ERROR_FILE_SHOULDNT_BE_INCLUDED;
#endif

#include <cartodata/wip/io/bundleformatchecker.h>
#include <soma-io/object/pythonreader.h>
#include <soma-io/io/minfXML2.h>
#include <soma-io/io/datasourceinfo.h>
#include <soma-io/datasource/datasource.h>
#include <cartobase/object/property.h>

using namespace carto;
using namespace std;


namespace
{
  bool initMe()
  {
    vector<string>	exts;
    exts.push_back( "bundles" );
    DataSourceInfo::registerFormat( "PYTHONBUNDLE", 
                                    new PythonBundleFormatChecker, exts );
    DataSourceInfo::registerFormat( "XMLBUNDLE", 
                                    new XMLBundleFormatChecker, exts );

    return true;
  }

  bool	_dummy = initMe();
}


PythonBundleFormatChecker::~PythonBundleFormatChecker()
{
}


Object PythonBundleFormatChecker::check( DataSource & ds, 
                                         DataSourceInfo & ) const
{
  rc_ptr<DataSource>	pds( &ds );
  Object		hdr = Object::value( PropertySet() );
  try
    {
      PythonReader	pr( pds );
      // cout << "PythonBundleFormatChecker " << ds.url() << endl;;
      pr.read( hdr );
    }
  catch( ... )
    {
      pds.release();
      throw;
    }
  pds.release();
  string		format;
  if( !hdr->getProperty( "format", format ) 
      || format.substr( 0, 8 ) != "bundles_" )
    throw invalid_format_error( ds.url() );
  hdr->setProperty( "object_type", string( "Bundles" ) );
  hdr->setProperty( "data_type", string( "any" ) );
  return hdr;
}


XMLBundleFormatChecker::~XMLBundleFormatChecker()
{
}


Object XMLBundleFormatChecker::check( DataSource & ds, 
                                      DataSourceInfo & ) const
{
  // cout << "XMLBundleFormatChecker " << ds.url() << endl;
  Object		hdr = Object::value( PropertySet() );
  string		format;
  readDictionaryMinfXML( ds, hdr );
  if( !hdr || !hdr->getProperty( "format", format ) 
      || format.substr( 0, 8 ) != "bundles_" )
    throw invalid_format_error( ds.url() );
  hdr->setProperty( "object_type", string( "Bundles" ) );
  hdr->setProperty( "data_type", string( "any" ) );
  return hdr;
}


