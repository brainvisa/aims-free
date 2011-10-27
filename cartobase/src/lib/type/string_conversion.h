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

#ifndef CARTOBASE_TYPE_STRING_CONVERSION_H
#define CARTOBASE_TYPE_STRING_CONVERSION_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/stream/sstream.h>
#include <string>
#include <vector>
#include <set>

namespace carto
{


////////////////////
//  toString< T > //
////////////////////

template < typename T >
inline
std::string toString( const T& object )
{

  std::ostringstream stream;
  stream << object;
  return stream.str();

}


///////////////////////
//  toString< char > //
///////////////////////

template <>
inline std::string toString( const char& object )
{
  return toString<int>( object );
}


/*! Split a \c string with in parts separated by a substring
  \param text \c string to be splitted
  \param sep \c string to be used as separator
*/
std::vector< std::string > split( const std::string &text,
				  const std::string &sep );

std::vector< std::string > split( const std::string &text,
  const std::set<std::string> &sep );

/*! Concatenate a series of \c string in a single \c string.
  \param pieces Series of \c string to be joined
  \param sep \c string to be inserted between the \c string to be joined
*/
std::string join( const std::vector< std::string > &pieces,
		  const std::string &sep );

std::string stringLower( const std::string & );
std::string stringUpper( const std::string & );

/*! Return a copy of the \c string \param s with leading and trailing
    whitespace removed.
    If \c char \param c is given, remove leading and trailing \param c characters instead of whitespace.
*/
std::string stringStrip(const std::string& str, char c = ' ');

////////////////////////////////
//  toString< unsigned char > //
////////////////////////////////

template <>
inline std::string toString( const unsigned char& object )
{
  return toString<int>( object );
}


//////////////////////////////
//  toString< signed char > //
//////////////////////////////

template <>
inline std::string toString( const signed char& object )
{
  return toString<int>( object );
}



////////////////////
//  stringTo< T > //
////////////////////

template < typename T >
void stringTo( const std::string& value, T& result );

// default implementation
template < typename T >
inline
void stringTo( const std::string& value, T& result )
{

#if __GNUC__ == 2 && __GNUC_MINOR__ - 0 <= 95

  std::istringstream stream( value.c_str() );

#else

  std::istringstream stream( value );

#endif
  stream >> result;

}


/////////////////////////
//  stringTo< string > //
/////////////////////////

template <>
inline
void stringTo< std::string >( const std::string& value, std::string &result )
{

  result = value;

}


///////////////////////
//  stringTo< long > //
///////////////////////

template <>
void stringTo< long >( const std::string& value,
                       long& result );


////////////////////////////////
//  stringTo< unsigned long > //
////////////////////////////////

template <>
void stringTo< unsigned long >( const std::string& value,
                                unsigned long& result );


//////////////////////
//  stringTo< int > //
//////////////////////

template <>
void stringTo< int >( const std::string& value, int& result );


///////////////////////////////
//  stringTo< unsigned int > //
///////////////////////////////

template <>
void stringTo< unsigned int >( const std::string& value,
                               unsigned int& result );


////////////////////////////////
//  stringTo< unsigned char > //
////////////////////////////////

template <>
void stringTo< unsigned char >( const std::string& value,
                                unsigned char& result );


//////////////////////////////
//  stringTo< signed char > //
//////////////////////////////

template <>
void stringTo< signed char >( const std::string& value,
                              signed char& result );


///////////////////////
//  stringTo< char > //
///////////////////////

template <>
inline
void stringTo< char >( const std::string& value, char& result )
{

#ifdef __CHAR_UNSIGNED__

  stringTo< unsigned char >( value,
                             reinterpret_cast< unsigned char& >( result ) );

#else

  stringTo< signed char >( value, reinterpret_cast< signed char& >( result ) );

#endif

}


/////////////////////////////////
//  stringTo< unsigned short > //
/////////////////////////////////

template <>
void stringTo< unsigned short >( const std::string& value,
                                 unsigned short& result );


////////////////////////
//  stringTo< short > //
////////////////////////

template <>
void stringTo< short >( const std::string& value,
                        short& result );


/////////////////////////
//  stringTo< double > //
/////////////////////////

template <>
void stringTo< double >( const std::string& value,
                         double& result );


////////////////////////
//  stringTo< float > //
////////////////////////

template <>
void stringTo< float >( const std::string& value, float& result );


///////////////////////
//  stringTo< bool > //
///////////////////////

template <>
void stringTo< bool >( const std::string& value, bool& result );


} // namespace carto


#endif
