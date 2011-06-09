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

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/stream/cuifstream.h>

using namespace carto;
using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

cuifstream::cuifstream()
  : istream(0), _counter(0), _uncomment(0)
{
}


cuifstream::cuifstream(const char* name, cuifstream::openmode mode)
  : istream(0), _base( name, mode ), _enableuncomment( true )
{
  init_buffers();
}


cuifstream::~cuifstream()
{
  destroy_buffers();
}


void
cuifstream::init_buffers()
{
  iostate s = _base.rdstate();
  rdbuf(_base.rdbuf());
  _counter = new fistreambuf<CounterExtractor>(rdbuf());
  rdbuf(_counter);
  if( _enableuncomment )
  {
    _uncomment = new fistreambuf<UncommentExtractor>(rdbuf());
    rdbuf(_uncomment);
  }
  _base.setstate( s );
  setstate( s );
}


void
cuifstream::destroy_buffers()
{
  delete _uncomment;
  _uncomment = 0;
  delete _counter;
  _counter = 0;
}


void
cuifstream::open(const char* filename, cuifstream::openmode mode)
{
  destroy_buffers();
  istream::clear();
  _base.clear();
  _base.open(filename, mode);
  init_buffers();
}


void
cuifstream::close()
{
  _base.close();
  destroy_buffers();
}


bool
cuifstream::operator!() const
{
  return _base.operator!() || istream::operator!();
}


bool
cuifstream::is_open() const
{
  return _base.is_open();
}


void cuifstream::enableUncomment( bool x )
{
  _enableuncomment = x;
  istream::clear();
  _base.clear();
  destroy_buffers();
  init_buffers();
}


bool
cuifstream::eof() const
{
#if __GNUC__ == 4 && __GNUC_MINOR__ == 6
  // in gcc 4.6, the eof bit does never get set; fail does however.
  return _base.eof() || istream::eof() || istream::fail();
#else
  return _base.eof() || istream::eof();
#endif
}


