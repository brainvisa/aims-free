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


#include <cartobase/stream/attachable_cuifstream.h>

using namespace carto;
using namespace std;


attachable_cuifstream::attachable_cuifstream()
  : cuifstream(), _attached( 0 )
{
}


attachable_cuifstream::attachable_cuifstream( const char *name ) 
  : cuifstream( name ), _attached( 0 )
{
}


attachable_cuifstream::attachable_cuifstream( istream & s )
  : cuifstream(), _attached( &s )
{
  init_buffers();
}


attachable_cuifstream::~attachable_cuifstream()
{
  close();
}


void attachable_cuifstream::attach( istream & s, int line_num )
{
  //if( !_attached )
  close();
    //else
    //destroy_buffers();
  _attached = &s;
  init_buffers();
  setstate( _attached->rdstate() );
  _counter->extractor().reset( line_num );
}


void attachable_cuifstream::close()
{
  cuifstream::close();
  _attached = 0;
}


void attachable_cuifstream::init_buffers()
{
  if( _attached )
    {
      rdbuf( _attached->rdbuf() );
      _counter = new fistreambuf<CounterExtractor>( rdbuf() );
      rdbuf( _counter );
      _uncomment = new fistreambuf<UncommentExtractor>( rdbuf() );
      rdbuf( _uncomment );
    }
  else
    cuifstream::init_buffers();
}


void attachable_cuifstream::destroy_buffers()
{
  if( _attached )
    {
      _attached->setstate( rdstate() );
      _attached = 0;
    }
  cuifstream::destroy_buffers();
}


bool attachable_cuifstream::operator!() const
{
  if( _attached )
    return _attached->operator!() || istream::operator!();
  else
    return( cuifstream::operator!() );
}


bool attachable_cuifstream::is_open() const
{
  if( _attached )
    {
      ifstream	*fs = dynamic_cast<ifstream *>( _attached );
      if( fs )
	return fs->is_open();
      return( true );	// non-file streams are considered always open
    }
  else
    return( cuifstream::is_open() );
}


