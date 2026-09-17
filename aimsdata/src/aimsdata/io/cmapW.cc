// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif
/*
 *  ColorMap writer class
 */
#include <aims/io/cmapW.h>
#include <aims/color/colormap.h>
using aims::ColorMap;
#include <iomanip>

using namespace std;

 
string 
AimsColorMapWriter::removeExtension( const string& name )
{
  string res = name;
  string ext="";
  if ( res.length() > 4 )
    ext = res.substr( int( res.length() - 4 ), 4 );
  if ( ext == ".rgb" )
    res = res.substr( 0, res.length() - 4 );
  return res;
}



void AimsColorMapWriter::write( const ColorMap& thing )
{
  string name = removeExtension( _name ) + ".rgb";
  _os.open( name.c_str() );

  string tmp;

  _os << thing.name() << endl;
  _os << thing.nItem() << endl;

  size_t nLine = thing.nItem() / 19;
  size_t rest = thing.nItem() % 19;

  _os << "Red" << endl;
  size_t	l, c;
  for ( l = 0; l < nLine; l++ )
  {
    _os << " ";
    for ( c = 0; c < 19; c++ )
      _os << " " << setw( 3 ) << thing.item( l * 19 + c ).red();
    _os << endl;
  }
  _os << " ";
  for ( c = thing.nItem() - rest; c < thing.nItem(); c++ )
    _os << " " << setw( 3 ) << thing.item( c ).red();
  _os << endl;


  _os << "Green" << endl;
  for ( l = 0; l < nLine; l++ )
  {
    _os << " ";
    for ( c = 0; c < 19; c++ )
      _os << " " << setw( 3 ) << thing.item( l * 19 + c ).green();
    _os << endl;
  }
  _os << " ";
  for ( c = thing.nItem() - rest; c < thing.nItem(); c++ )
    _os << " " << setw( 3 ) << thing.item( c ).green();
  _os << endl;


  _os << "Blue" << endl;
  for ( l = 0; l < nLine; l++ )
  {
    _os << " ";
    for ( c = 0; c < 19; c++ )
      _os << " " << setw( 3 ) << thing.item( l * 19 + c ).blue();
    _os << endl;
  }
  _os << " ";
  for ( c = thing.nItem() - rest; c < thing.nItem(); c++ )
    _os << " " << setw( 3 ) << thing.item( c ).blue();
  _os << endl;


  _os.close();
}


AimsColorMapWriter&
operator << ( AimsColorMapWriter& writer, const ColorMap& thing )
{
  writer.write( thing );
  return writer;
}
