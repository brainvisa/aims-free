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

#include <cartobase/wip/io/pythonformatchecker.h>
#include <cartobase/wip/io/datasourceinfo.h>
#include <cartobase/object/property.h>
#include <cartobase/datasource/datasource.h>
// #define CARTO_DEBUG_IO
#ifdef CARTO_DEBUG_IO
#include <iostream>
#endif


using namespace carto;
using namespace std;

namespace
{
  bool initMe()
  {
    vector<string>	exts;
    exts.push_back( "minf" );
    exts.push_back( "py" );
    DataSourceInfo::registerFormat( "PYTHON", new PythonFormatChecker, exts );

    return true;
  }

  bool	_dummy = initMe();
}


Object PythonFormatChecker::check( DataSource & ds, DataSourceInfo & ) const
{
#ifdef CARTO_DEBUG_IO
  cout << "PythonFormatChecker::check " << ds.url() << endl;
#endif

  static const string	sign = "attributes";
  ds.open( DataSource::Read );
  char		c;
  int		i, n = sign.length();

  for( i=0; i<n && ds.isOpen() && sign[i] == (c=(char)ds.getch()); ++i ) {}
  if( ds.isOpen() )
    {
      // rewind
      int	j = i;
      if( i == n )
        --j;
      for( ; j>=0; --j )
        ds.ungetch( sign[j] );
    }
  if( i != n )
    {
      if( !ds.isOpen() )
        io_error::launchErrnoExcept( ds.url() );
      throw wrong_format_error( "not a python MINF file", ds.url() );
    }

  Object hdr = Object::value( PropertySet() );
  hdr->setProperty( "format", string( "PYTHON" ) );
  hdr->setProperty( "object_type", string( "genericobject" ) );
  hdr->setProperty( "data_type", string( "any" ) );

#ifdef CARTO_DEBUG_IO
  cout << "PythonFormatChecker::check " << ds.url() << " OK" << endl;
#endif

  return hdr;
}


