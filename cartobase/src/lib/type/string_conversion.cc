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

#include <cartobase/type/string_conversion.h>
#include <cartobase/exception/format.h>
#include <float.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>

using namespace std;

namespace carto
{



///////////////////////
//  stringTo< long > //
///////////////////////

template <>
void stringTo< long >( const string& value, long& result )
{

  char *endptr;
  result = strtol( value.c_str(), &endptr, 10 );
  if ( endptr && *endptr != '\0' )
  {

    throw invalid_integer( value );

  }

}


////////////////////////////////
//  stringTo< unsigned long > //
////////////////////////////////

template <>
void stringTo< unsigned long >( const string& value, unsigned long& result )
{

  char *endptr;
  result = strtoul( value.c_str(), &endptr, 10 );
  if ( endptr && *endptr != '\0' )
  {

    throw invalid_positive_integer( value );

  }

}


////////////////////////////////
//  stringTo< unsigned char > //
////////////////////////////////

template <>
void stringTo< unsigned char >( const string& value, unsigned char& result )
{

  unsigned long r;
  stringTo< unsigned long >( value, r );
  if ( r > UCHAR_MAX )
  {

    throw out_of_range_error( value );

  }
  result = ( unsigned char )r;

}


//////////////////////////////
//  stringTo< signed char > //
//////////////////////////////

template <>
void stringTo< signed char >( const string& value, signed char& result )
{

  long r;
  stringTo< long >( value, r );
  if ( r > SCHAR_MAX || r < SCHAR_MIN )
  {

    throw out_of_range_error( value );

  }
  result = ( signed char ) r;

}


/////////////////////////////////
//  stringTo< unsigned short > //
/////////////////////////////////

template <>
void stringTo< unsigned short >( const string& value, unsigned short& result )
{

  unsigned long r;
  stringTo< unsigned long >( value, r );
  if ( r > USHRT_MAX )
  {

    throw out_of_range_error( value );

  }
  result = ( unsigned short ) r;

}


////////////////////////
//  stringTo< short > //
////////////////////////

template <>
void stringTo< short >( const string& value, short& result )
{

  long r;
  stringTo< long >( value, r );
  if ( r > SHRT_MAX || r < SHRT_MIN )
  {

    throw out_of_range_error( value );

  }
  result = ( short )r;

}


///////////////////////////////
//  stringTo< unsigned int > //
///////////////////////////////

template <>
void stringTo< unsigned int >( const string& value, unsigned int& result )
{

  unsigned long r;
  stringTo< unsigned long >( value, r );
  if ( r > UINT_MAX )
  {

    throw out_of_range_error( value );

  }
  result = ( unsigned int )r;

}


//////////////////////
//  stringTo< int > //
//////////////////////

template <>
void stringTo< int >( const string& value, int& result )
{

  long r;
  stringTo< long >( value, r );
  if ( r > INT_MAX || r < INT_MIN )
  {

    throw out_of_range_error( value );

  }
  result = ( int )r;

}


/////////////////////////
//  stringTo< double > //
/////////////////////////

template <>
void stringTo< double >( const string& value, double& result )
{
  /* note
  Why re-code the strtod() libC function ?
  Because the dot character may be interpreted differently according to the
  locale in strtod(), ie ',' be expected instead of '.', and standard
  numbers not be recognized correctly.
  We *always* use the '.' character as decimal point because it is *standard* !
  However here we also accept ','.
  */
  const char *ptr = value.c_str();
  bool started = false;
  bool ended = false;
  bool neg = false;
  bool ex = false;
  bool firste = true;
  int  expo = 0;
  bool dot = false;
  char c;
  double mmul = 10.;
  double cmul = 1.;
  bool hexa = false;
  while( !ended )
  {
    c = *ptr;
    switch( c )
    {
      case '\0':
      case '\n':
      case ' ':
      case '\t':
        ended = true;
        break;

      case '-':
        if( ex )
          if( firste )
            neg = true;
          else
            throw invalid_number( value );
        else if( started && !neg )
          throw invalid_number( value );
        else
          neg = true;
        break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if( !started )
        {
          result = c - '0';
          started = true;
        }
        else if( !ex )
        {
          result = result * mmul + ( c - '0' ) * cmul;
          if( dot )
            cmul *= 0.1;
        }
        else if( firste )
        {
          expo = c - '0';
          firste = false;
        }
        else
          expo = expo * 10 + c - '0';
        break;

      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'F':
        if( hexa )
        {
          if( !started )
          {
            result = c - 'A';
            started = true;
          }
          else
            result = result * 16 + ( c - 'A' );
        }
        else
          throw invalid_number( value );
        break;

      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'f':
        if( hexa )
        {
          if( !started )
          {
            result = c - 'a';
            started = true;
          }
          else
            result = result * 16 + ( c - 'a' );
        }
        else
          throw invalid_number( value );
        break;

      case 'e':
      case 'E':
        if( hexa )
        {
          if( !started )
          {
            result = 14;
            started = true;
          }
          else
            result = result * 16 + 14 * cmul;
        }
        else if( !ex )
        {
          if( !started )
            throw invalid_number( value );
          ex = true;
          if( neg )
          {
            result = -result;
            neg = false;
          }
        }
        else
          throw invalid_number( value );
        break;

      case '.':
      case ',':
        if( ex || dot || hexa )
          throw invalid_number( value );
        else
        {
          if( !started )
          {
            result = 0.;
            started = true;
          }
          mmul = 1.;
          cmul = 0.1;
          dot = true;
        }
        break;

      case '+':
        if( ex )
        {
          if( !firste )
            throw invalid_number( value );
        }
        else if( started )
          throw invalid_number( value );
        break;

      case 'x':
      case 'X':
        if( hexa || !started || ex || dot || result != 0 )
          throw invalid_number( value );
        hexa = true;
        mmul = 16;
        cmul = 1.;
        break;

      default:
          throw invalid_number( value );
    }
    ++ptr;
  }
  if( !started || ( ex && firste ) )
    throw invalid_number( value );
  if( neg )
  {
    if( ex )
      expo = -expo;
    else
      result = -result;
  }
  if( ex )
    result = result * exp( expo * ::log( 10. ) );

  /*
  char* endptr;
  result =  strtod( value.c_str(), &endptr );
  if( result == 0 && endptr == value.c_str() )
  {

    throw invalid_number( value );

  }
  */

}


////////////////////////
//  stringTo< float > //
////////////////////////

template <>
void stringTo< float >( const string& value, float& result )
{

  double r;
  stringTo< double >( value, r );
  if ( r > FLT_MAX || r < -FLT_MAX )
  {

    throw out_of_range_error( value );

  }
  result = ( float )r;

}


///////////////////////
//  stringTo< bool > //
///////////////////////

template <>
void stringTo< bool >( const string& value, bool& result )
{

  if ( value == "true" || value == "TRUE" || value == "True" )
  {

    result = true;

  }
  else if ( value == "false" || value == "FALSE" || value == "False" )
  {

    result = false;

  }
  else
    {
      try
        {
          double	x = 0;
          stringTo( value, x );
          result = ( x != 0 );
          return;
        }
      catch( ... )
        {
        }
      throw invalid_boolean( value );
    }
}


//-----------------------------------------------------------------------------
std::vector<std::string> split( const std::string &text,
				const std::string &sep )
{
  vector<string> result;
  string current;

  for( unsigned i = 0; i != text.size(); ++i ) {
    if ( text[ i ] == sep[ 0 ] ) {
      unsigned j;
      for( j = 1; j != sep.size() && i+j < text.size() &&
        text[i+j] == sep[j]; ++j ) {}
      if ( j == sep.size() ) {
        if( !current.empty() )
          result.push_back( current );
        current = "";
      }
    } else {
      current += text[ i ];
    }
  }
  if( !current.empty() )
    result.push_back( current );
  return result;
}


std::vector<std::string> split( const std::string &text,
  const std::set<std::string> &sep )
{
  vector<string> result;
  string current;
  set<string>::const_iterator isep, esep = sep.end();

  for( unsigned i = 0; i != text.size(); ++i )
  {
    for( isep=sep.begin(); isep!=esep; ++isep )
      if ( text[ i ] == (*isep)[ 0 ] )
      {
        unsigned j;
        for( j = 1; j != isep->size() && i+j < text.size() &&
          text[i+j] == (*isep)[j]; ++j ) {}
        if ( j == isep->size() )
        {
          if( !current.empty() )
            result.push_back( current );
          current = "";
          break;
        }
      }
    if( isep == esep )
    {
      current += text[ i ];
    }
  }
  if( !current.empty() )
    result.push_back( current );
  return result;
}


//-----------------------------------------------------------------------------
std::string join( const std::vector< std::string > &pieces,
		  const std::string &sep )
{
  string result;
  vector< string >::const_iterator i = pieces.begin();
  if ( i != pieces.end() ) {
    result += *i;
    while( ++i != pieces.end() ) {
      result += sep + *i;
    }
  }
  return result;
}


string stringLower( const string & s )
{
  string	r;
  r.reserve( s.length() );
  string::const_iterator	i, e = s.end();
  for( i=s.begin(); i!=e; ++i )
    if( *i >= 'A' && *i <= 'Z' )
      r += *i - 'A' + 'a';
    else
      r += *i;
  return r;
}


string stringUpper( const string & s )
{
  string	r;
  r.reserve( s.length() );
  string::const_iterator	i, e = s.end();
  for( i=s.begin(); i!=e; ++i )
    if( *i >= 'a' && *i <= 'z' )
      r += *i - 'a' + 'A';
    else
      r += *i;
  return r;
}

string stringStrip( const string& s, char c)
{

  // Search the string for the last character before the last strip characters.
  // Remove the strip characters at the end of the string, based on the found index.
  // Next, search for the first character different from trim character and remove everything before it.

  string r;
  r.reserve( s.length() );
  r.assign(s);

  string::size_type pos = r.find_last_not_of(c);
  if(pos != std::string::npos) {
    r.erase(pos + 1);
    pos = r.find_first_not_of(c);
    if(pos != std::string::npos) r.erase(0, pos);
  }
  else r.erase(r.begin(), r.end());

  return r;
}

}
