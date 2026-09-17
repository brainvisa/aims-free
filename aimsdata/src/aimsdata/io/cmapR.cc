// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif
/*
 *  ColorMap reader class
 */

#include <aims/io/cmapR.h>
#include <aims/color/colormap.h>
#include <aims/def/assert.h>

using namespace aims;
using namespace std;


string AimsColorMapReader::removeExtension( const string& name )
{
  string res = name;
  string ext="";
  if ( res.length() > 4 )
    ext = res.substr( int( res.length() - 4 ), 4 );
  if ( ext == ".rgb" )
    res = res.substr( 0, res.length() - 4 );
  return res;
}



void AimsColorMapReader::read( ColorMap& thing )
{
  thing.erase();

  string name = removeExtension( _name ) + ".rgb";
  _is.open( name.c_str() );

  string tmp;

  _is >> tmp;
  thing.setName( tmp );

  size_t size = 0;
  _is >> size;

  AimsRGB item( 0, 0, 0 );
  thing.reserve( size );
  size_t	n;

  for ( n = 0; n < size; n++ )
    thing.push_back( item );

  int tp;

  _is >> tmp;
  ASSERT( tmp == "Red" );
  for ( n = 0; n < size; n++ )
  {
    _is >> tp;
    thing.item( n ).red() = (::byte)tp;
  }

  _is >> tmp;
  ASSERT( tmp == "Green" );
  for ( n = 0; n < size; n++ )
  {
    _is >> tp;
    thing.item( n ).green() = (::byte)tp;
  }

  _is >> tmp;
  ASSERT( tmp == "Blue" );
  for ( n = 0; n < size; n++ )
  {
    _is >> tp;
    thing.item( n ).blue() = (::byte)tp;
  }

  _is.close();
}


AimsColorMapReader&
operator >> ( AimsColorMapReader& reader, ColorMap& thing )
{
  reader.read( thing );
  return reader;
}
