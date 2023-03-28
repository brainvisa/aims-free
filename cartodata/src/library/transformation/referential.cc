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

#include <cartodata/transformation/referential.h>
#include <cartobase/uuid/uuid.h>
#include <cartobase/object/property.h>

using namespace carto;
using namespace std;


Referential::Referential()
  : RCObject(), _orientation( 3, 0 ), _header( Object::value( PropertySet() ) )
{
  UUID uuid;
  uuid.generate();
  _uuid = uuid.toString();
  _lpi_uuid = _uuid;

  // the default is LPI for AIMS library.
  _orientation[0] = L;
  _orientation[1] = P;
  _orientation[2] = I;

  PropertySet & ps = _header->value<PropertySet>();
  ps.addBuiltinProperty( "uuid", _uuid );
  ps.addBuiltinProperty( "lpi_uuid", _lpi_uuid );
  ps.addBuiltinProperty( "axes_orientation", _orientation );
}


Referential::Referential( const Referential & ref )
  : RCObject(), _uuid( ref._uuid ), _lpi_uuid( ref._lpi_uuid),
  _orientation( ref._orientation ), _header( Object::value( PropertySet() ) )
{
  PropertySet & ps = _header->value<PropertySet>();
  ps.addBuiltinProperty( "uuid", _uuid );
  ps.addBuiltinProperty( "lpi_uuid", _lpi_uuid );
  ps.addBuiltinProperty( "axes_orientation", _orientation );
}


Referential::Referential( Object ref )
  : RCObject(), _orientation( 3, 0 ), _header( Object::value( PropertySet() ) )
{
  PropertySet & ps = _header->value<PropertySet>();
  ps.addBuiltinProperty( "uuid", _uuid );
  ps.addBuiltinProperty( "lpi_uuid", _lpi_uuid );
  ps.addBuiltinProperty( "axes_orientation", _orientation );

  _header->copyProperties( ref );
}


Referential::~Referential()
{
}


Referential & Referential::operator = ( const Referential & ref )
{
//   _uuid = ref._uuid;
//   _lpi_uuid = ref._lpi_uuid;
//   _orientation = ref._orientation;

  _header = Object::value( PropertySet() );

  PropertySet & ps = _header->value<PropertySet>();
  ps.addBuiltinProperty( "uuid", _uuid );
  ps.addBuiltinProperty( "lpi_uuid", _lpi_uuid );
  ps.addBuiltinProperty( "axes_orientation", _orientation );

  ps.copyProperties( ref.header() );

  return *this;
}


string Referential::orientationStr( Orientation orient )
{
  static map<Orientation, string> orient_map;
  if( orient_map.empty() )
  {
    orient_map[Undefined] = "Undefined";
    orient_map[L] = "L";
    orient_map[R] = "R";
    orient_map[A] = "A";
    orient_map[P] = "P";
    orient_map[S] = "S";
    orient_map[I] = "I";
  }

  map<Orientation, string>::const_iterator i = orient_map.find( orient );
  if( i == orient_map.end() )
    return "Undefined";

  return i->second;
}


Referential::Orientation
Referential::orientationCode( const std::string & orient )
{
  static map<string, Orientation> orient_map;
  if( orient_map.empty() )
  {
    orient_map["Undefined"] = Undefined;
    orient_map["L"] = L;
    orient_map["R"] = R;
    orient_map["A"] = A;
    orient_map["P"] = P;
    orient_map["S"] = S;
    orient_map["I"] = I;
  }

  map<string, Orientation>::const_iterator i = orient_map.find( orient );
  if( i == orient_map.end() )
    return Undefined;

  return i->second;
}


vector<int> Referential::orientationVector( const std::string & orient )
{
  vector<int> orient_vec( 3, 0 );

  if( orient.length() < 3 )
    return orient_vec;

  char s[2] = "L";
  s[0] = orient[0];
  orient_vec[0] = orientationCode( s );
  s[0] = orient[1];
  orient_vec[1] = orientationCode( s );
  s[0] = orient[2];
  orient_vec[2] = orientationCode( s );

  return orient_vec;
}


void Referential::setAxisTransform( AffineTransformation3dBase & tr,
                                    Orientation from_code, Orientation code,
                                    const std::vector<float> & tsl )
{
  int from_ax = 0, to_ax = 0;
  float from_or = 1, to_or = 1, mul = 1;

  switch( from_code )
  {
    case L:
      from_ax = 0;
      from_or = 1;
      break;
    case R:
      from_ax = 0;
      from_or = -1;
      break;
    case A:
      from_ax = 1;
      from_or = -1;
      break;
    case P:
      from_ax = 1;
      from_or = 1;
      break;
    case S:
      from_ax = 2;
      from_or = -1;
      break;
    case I:
      from_ax = 2;
      from_or = 1;
      break;
    default:
      break;
  }
  switch( code )
  {
    case L:
      to_ax = 0;
      to_or = 1;
      break;
    case R:
      to_ax = 0;
      to_or = -1;
      break;
    case A:
      to_ax = 1;
      to_or = -1;
      break;
    case P:
      to_ax = 1;
      to_or = 1;
      break;
    case S:
      to_ax = 2;
      to_or = -1;
      break;
    case I:
      to_ax = 2;
      to_or = 1;
      break;
    default:
      break;
  }
  mul = from_or * to_or;
  tr.matrix()( from_ax, to_ax ) = mul;
  if( mul < 0 && !tsl.empty() )
    tr.matrix()( from_ax, 3 ) = tsl[to_ax];
}


std::string Referential::orientationStr() const
{
  if( _orientation.size() < 3 )
    return "Undefined";

  return orientationStr( Orientation( _orientation[0] ) )
    + orientationStr( Orientation( _orientation[1] ) )
    + orientationStr( Orientation( _orientation[2] ) );
}


rc_ptr<Transformation3d>
Referential::toOrientation( const std::string & orient,
                            const std::vector<float> & transl ) const
{
  return toOrientation( orientationVector( orient ), transl );
}


rc_ptr<Transformation3d>
Referential::toOrientation( const vector<int> & orient,
                            const std::vector<float> & transl ) const
{
  rc_ptr<Transformation3d> trans;
  if( orient.size() < 3 )
  {
    cerr << "Undefined orientation\n";
    return trans;
  }

  AffineTransformation3dBase *tr = new AffineTransformation3dBase;
  trans.reset( tr );

  tr->matrix()( 0, 0 ) = 0;
  tr->matrix()( 1, 1 ) = 0;
  tr->matrix()( 2, 2 ) = 0;

  int axis;
  for( axis=0; axis<3; ++axis )
  {
    Orientation code = Orientation( orient[axis] );
    Orientation from_code = Orientation( _orientation[axis] );
    setAxisTransform( *tr, from_code, code, transl );
  }

  return trans;
}


void Referential::setUuid( const std::string & uuid )
{
  _uuid = uuid;
}


void Referential::setOrientation( const std::string & orient )
{
  _orientation = vector<int>( 3, 0 );
  if( orient == "Undefined" )
    return;
  if( orient.size() < 3 )
  {
    cerr << "Invalid orientation string: " << orient << endl;
    return;
  }
  _orientation = orientationVector( orient );
}


void Referential::setOrientation( const std::vector<int> & orient )
{
  if( orient.size() < 3 )
  {
    cerr << "Invalid orientation vector\n";
    return;
  }
  _orientation = orient;
}


void Referential::setLpiReferential( const std::string & lpi_uuid )
{
  _lpi_uuid = lpi_uuid;
}


