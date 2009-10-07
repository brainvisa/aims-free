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

#include <cartobase/type/byte_order.h>

using namespace std;

namespace carto {

const unsigned int bigEndian = 0x41424344;
const unsigned int littleEndian = 0x44434241;
const unsigned int pdpEndian = 0x43444142;

int byteOrder()
{
  static int result = 0;
  if ( result == 0 ) {
    char *p = reinterpret_cast<char *>( &result );
    p[ 0 ] = 0x41;
    p[ 1 ] = 0x42;
    p[ 2 ] = 0x43;
    p[ 3 ] = 0x44;
  }
  return result;
}

const char * byteOrderString()
{
  static char result[ 5 ] = { 0 };
  
  if ( result[ 0 ] == 0 ) {
    unsigned int x = 0x41424344;
    char *p = reinterpret_cast<char *>( &x );
    result[ 0 ] = p[ 0 ];
    result[ 1 ] = p[ 1 ];
    result[ 2 ] = p[ 2 ];
    result[ 3 ] = p[ 3 ];
    result[ 4 ] = 0;
  }
  return result;
}


int stringToByteOrder( const string &bos )
{
  if ( bos == "ABCD" ) return bigEndian;
  else if ( bos == "DCBA" ) return littleEndian;
  else if ( bos == "CDAB" ) return pdpEndian;
  return 0;
}

ByteSwapper::ByteSwapper( int bo )
{
  _swap = bo != byteOrder();
}


ByteSwapper::ByteSwapper( const string &bos )
{
  _swap = stringToByteOrder( bos ) != byteOrder();
}


template <> 
void ByteSwapper::_doSwap<2>( char *p )
{
  register char tmp = p[ 0 ];
  p[ 0 ] = p[ 1 ];
  p[ 1 ] = tmp;
}


template <> 
void ByteSwapper::_doSwap<4>( char *p )
{
  register char tmp = p[ 0 ];
  p[ 0 ] = p[ 3 ];
  p[ 3 ] = tmp;
  tmp = p[ 1 ];
  p[ 1 ] = p[ 2 ];
  p[ 2 ] = tmp;
}


template <> 
void ByteSwapper::_doSwap<8>( char *p )
{
  register char tmp = p[ 0 ];
  p[ 0 ] = p[ 7 ];
  p[ 7 ] = tmp;
  tmp = p[ 1 ];
  p[ 1 ] = p[ 6 ];
  p[ 6 ] = tmp;
  tmp = p[ 2 ];
  p[ 2 ] = p[ 5 ];
  p[ 5 ] = tmp;
  tmp = p[ 3 ];
  p[ 3 ] = p[ 4 ];
  p[ 4 ] = tmp;
}


template <> 
void ByteSwapper::_doSwap<16>( char *p )
{
  register char tmp;
  for( register int i = 0; i < 8; ++i ) {
    tmp = p[ i ];
    p[ i ] = p[ 15 - i ];
    p[ 15 - i ] = tmp;
  }
}



template <> 
std::istream & ByteSwapper::_swappedRead<2>( std::istream &in, char *p )
{
  in.get( p[ 1 ] );
  in.get( p[ 0 ] );
  return in;
}

template <> 
std::istream & ByteSwapper::_swappedRead<4>( std::istream &in, char *p )
{
  for( register int i = 3; i >= 0; --i ) {
    in.get( p[ i ] );
  }
  return in;
}


template <> 
std::istream & ByteSwapper::_swappedRead<8>( std::istream &in, char *p )
{
  for( register int i = 7; i >= 0; --i ) {
    in.get( p[ i ] );
  }
  return in;
}


template <> 
std::istream & ByteSwapper::_swappedRead<16>( std::istream &in, char *p )
{
  for( register int i = 15; i >= 0; --i ) {
    in.get( p[ i ] );
  }
  return in;
}

template <> 
std::ostream & ByteSwapper::_swappedWrite<2>( std::ostream &out, char *p )
{
  out.put( p[ 1 ] );
  out.put( p[ 0 ] );
  return out;
}

template <> 
std::ostream & ByteSwapper::_swappedWrite<4>( std::ostream &out, char *p )
{
  for( register int i = 3; i >= 0; --i ) {
    out.put( p[ i ] );
  }
  return out;
}


template <> 
std::ostream & ByteSwapper::_swappedWrite<8>( std::ostream &out, char *p )
{
  for( register int i = 7; i >= 0; --i ) {
    out.put( p[ i ] );
  }
  return out;
}


template <> 
std::ostream & ByteSwapper::_swappedWrite<16>( std::ostream &out, char *p )
{
  for( register int i = 15; i >= 0; --i ) {
    out.put( p[ i ] );
  }
  return out;
}

} // namesapce carto
