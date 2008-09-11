/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <cartodata/wip/io/bundleformatchecker.h>
#include <cartobase/object/pythonreader.h>
#include <cartobase/wip/io/minfXML2.h>
#include <cartobase/wip/io/datasourceinfo.h>
#include <cartobase/object/property.h>
#include <cartobase/datasource/datasource.h>

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


