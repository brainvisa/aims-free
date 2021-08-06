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
