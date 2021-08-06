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
    thing.item( n ).red() = (byte)tp;
  }

  _is >> tmp;
  ASSERT( tmp == "Green" );
  for ( n = 0; n < size; n++ )
  {
    _is >> tp;
    thing.item( n ).green() = (byte)tp;
  }

  _is >> tmp;
  ASSERT( tmp == "Blue" );
  for ( n = 0; n < size; n++ )
  {
    _is >> tp;
    thing.item( n ).blue() = (byte)tp;
  }

  _is.close();
}


AimsColorMapReader&
operator >> ( AimsColorMapReader& reader, ColorMap& thing )
{
  reader.read( thing );
  return reader;
}
