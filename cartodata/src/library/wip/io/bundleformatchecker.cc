/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

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


