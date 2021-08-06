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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/motionR.h>
#include <aims/data/pheader.h>
#include <soma-io/utilities/asciidatasourcetraits.h>
#include <soma-io/datasource/filedatasource.h>

using namespace aims;
using namespace carto;
using namespace std;


MotionReader::MotionReader( const string & name )
      :  _name( name )
{
}


MotionReader::~MotionReader()
{
}


void MotionReader::read( Motion& motion )
{
  string                fname = _name;
  FileDataSource	is( fname );
  if( !is.isOpen() )
    is.open( DataSource::Read );
  if( !is.isOpen() )
  {
    if( fname.length() < 4 || fname.substr( fname.length()-4, 4 ) != ".trm" )
    {
      fname += ".trm";
      is = FileDataSource( fname );
      is.open( DataSource::Read );
      if( !is.isOpen() )
        throw file_not_found_error( _name );
    }
    else
      throw file_not_found_error( _name );
  }
  AsciiDataSourceTraits<float>		fir;
  if( !StreamUtil::skip( is, " \t\n\r" ) )
    throw wrong_format_error( _name );
  if( !fir.read( is, motion.matrix()( 0, 3 ) ) )
    throw wrong_format_error( _name );
  if( !StreamUtil::skip( is, " \t\n\r" ) )
    throw wrong_format_error( _name );
  if( !fir.read( is, motion.matrix()( 1, 3 ) ) )
    throw wrong_format_error( _name );
  if( !StreamUtil::skip( is, " \t\n\r" ) )
    throw wrong_format_error( _name  );
  if( !fir.read( is, motion.matrix()( 2, 3 ) ) )
    throw wrong_format_error( _name );
  unsigned	i, j;
  for( i=0; i<3; ++i )
    for( j=0; j<3; ++j )
      {
        if( !StreamUtil::skip( is, " \t\n\r" ) )
          throw wrong_format_error( _name );
        if( !fir.read( is, motion.rotation()( i, j ) ) )
          throw wrong_format_error( _name );
      }
  is.close();

  PythonHeader  *ph = new PythonHeader;
  if( ph->readMinf( fname + ".minf" ) )
    motion.setHeader( ph );
  else
  {
    motion.setHeader( new PythonHeader );
    delete ph;
  }
}

MotionReader& MotionReader::operator >> ( Motion& thing )
{
  this->read( thing );
  return *this;
}
