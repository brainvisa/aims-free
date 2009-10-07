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

#include <aims/io/trmheader.h>
#include <aims/io/motionR.h>
#include <cartobase/exception/ioexcept.h>

using namespace aims;
using namespace carto;
using namespace std;


string TrmHeader::filename() const
{
  if( _name.length() >= 4 && _name.substr( _name.length() - 4, 4 ) == ".trm" )
    return( _name );
  return( _name + ".trm" );
}


void TrmHeader::read( size_t * )
{
  string fileName = filename();

  // simple test: try to read all...
  MotionReader	mr( fileName );
  Motion	m;
  mr.read( m );
    
  setProperty( "file_type", string( "TRM" ) );
  setProperty( "object_type", string( "Motion" ) );
  setProperty( "data_type", string( "VOID" ) );

  // add meta-info to header
  readMinf( removeExtension( _name ) + extension() + ".minf" );
}


void TrmHeader::write()
{
  string fileName = filename();
  /*  ofstream os( fileName.c_str() );
  if( !os )
  io_error::launchErrnoExcept( fileName ); */

  // write meta-info header
  setProperty( "file_type", string( "TRM" ) );
  setProperty( "object_type", string( "Motion" ) );
  setProperty( "data_type", string( "VOID" ) );

  writeMinf( removeExtension( _name ) + extension() + ".minf" );
}


set<string> TrmHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".trm" );
  return exts;
}


