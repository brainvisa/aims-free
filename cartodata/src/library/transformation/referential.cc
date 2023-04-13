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


Referential::Referential( unsigned ndim )
  : RCObject(), _orientation( ndim, 0 ),
    _header( Object::value( PropertySet() ) )
{
  UUID uuid;
  uuid.generate();
  _uuid = uuid.toString();
  _lpi_uuid = _uuid;

  // the default is LPI for AIMS library.
  string orient = "LPITUVWXYZ";
  _orientation = orientationVector( orient.substr( 0, ndim ) );

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


void Referential::ensureOrder( unsigned ndim )
{
  if( _orientation.size() != ndim )
  {
    unsigned i, n = _orientation.size(), j;
    _orientation.resize( ndim );
    set<int> used;
    for( i=0; i<n; ++i )
      if( _orientation[i] != Undefined )
        used.insert( ( _orientation[i] - 1 ) / 2 );

    for( i=n, j=0; i<ndim; ++j )
    {
      if( used.find( j ) == used.end() )
      {
        _orientation[i] = j * 2 + 1;
        ++i;
      }
    }
  }
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
    orient_map[T] = "T";
    orient_map[t] = "t";
    orient_map[U] = "U";
    orient_map[u] = "u";
    orient_map[V] = "V";
    orient_map[v] = "v";
    orient_map[W] = "W";
    orient_map[w] = "w";
    orient_map[X] = "X";
    orient_map[x] = "x";
    orient_map[Y] = "Y";
    orient_map[y] = "y";
    orient_map[Z] = "Z";
    orient_map[z] = "z";
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
    orient_map["T"] = T;
    orient_map["t"] = t;
    orient_map["U"] = U;
    orient_map["u"] = u;
    orient_map["V"] = V;
    orient_map["v"] = v;
    orient_map["W"] = W;
    orient_map["w"] = w;
    orient_map["X"] = X;
    orient_map["x"] = x;
    orient_map["Y"] = Y;
    orient_map["y"] = y;
    orient_map["Z"] = Z;
    orient_map["z"] = z;
  }

  map<string, Orientation>::const_iterator i = orient_map.find( orient );
  if( i == orient_map.end() )
    return Undefined;

  return i->second;
}


vector<int> Referential::orientationVector( const std::string & orient,
                                            unsigned dim )
{
  unsigned i, n = orient.length();
  vector<int> orient_vec( n, 0 );
  char s[2] = "L";

  for( i=0; i<n; ++i )
  {
    s[0] = orient[i];
    orient_vec[i] = orientationCode( s );
  }
  if( n < dim )
  {
    set<int> used;
    vector<int> def_orient( dim, 0 );
    for( i=0; i<dim; ++i )
      def_orient[i] = i * 2 + 1;
    for( i=0; i<n; ++i )
      used.insert( ( orient_vec[i] - 1 ) / 2 );
    orient_vec.reserve( dim );
    i = 0;
    while( orient_vec.size() < dim )
    {
      if( used.find( ( def_orient[i] - 1 ) / 2 ) == used.end() )
        orient_vec.push_back( def_orient[i] );
      ++i;
    }
  }

  return orient_vec;
}


void Referential::setAxisTransform( AffineTransformationBase & tr,
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
    case T:
      from_ax = 3;
      from_or = 1;
      break;
    case t:
      from_ax = 3;
      from_or = -1;
      break;
    case U:
      from_ax = 4;
      from_or = 1;
      break;
    case u:
      from_ax = 4;
      from_or = -1;
      break;
    case V:
      from_ax = 5;
      from_or = 1;
      break;
    case v:
      from_ax = 5;
      from_or = -1;
      break;
    case W:
      from_ax = 6;
      from_or = 1;
      break;
    case w:
      from_ax = 6;
      from_or = -1;
      break;
    case X:
      from_ax = 7;
      from_or = 1;
      break;
    case x:
      from_ax = 7;
      from_or = -1;
      break;
    case Y:
      from_ax = 8;
      from_or = 1;
      break;
    case y:
      from_ax = 8;
      from_or = -1;
      break;
    case Z:
      from_ax = 9;
      from_or = 1;
      break;
    case z:
      from_ax = 9;
      from_or = -1;
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
    case T:
      to_ax = 3;
      to_or = 1;
      break;
    case t:
      to_ax = 3;
      to_or = -1;
      break;
    case U:
      to_ax = 4;
      to_or = 1;
      break;
    case u:
      to_ax = 4;
      to_or = -1;
      break;
    case V:
      to_ax = 5;
      to_or = 1;
      break;
    case v:
      to_ax = 5;
      to_or = -1;
      break;
    case W:
      to_ax = 6;
      to_or = 1;
      break;
    case w:
      to_ax = 6;
      to_or = -1;
      break;
    case X:
      to_ax = 7;
      to_or = 1;
      break;
    case x:
      to_ax = 7;
      to_or = -1;
      break;
    case Y:
      to_ax = 8;
      to_or = 1;
      break;
    case y:
      to_ax = 8;
      to_or = -1;
      break;
    case Z:
      to_ax = 9;
      to_or = 1;
      break;
    case z:
      to_ax = 9;
      to_or = -1;
      break;
    default:
      break;
  }
  mul = from_or * to_or;
  tr.matrix()( from_ax, to_ax ) = mul;
  if( mul < 0 && !tsl.empty() )
    tr.matrix()( from_ax, tr.matrix().ncols - 1 ) = tsl[to_ax];
}


std::string Referential::orientationStr() const
{
  if( _orientation.size() < 3 )
    return "Undefined";

  string orient;
  unsigned i, n = _orientation.size();
  for( i=0; i<n; ++i )
    orient += orientationStr( Orientation( _orientation[i] ) );

  return orient;
}


std::string Referential::orientationStr( const string & orient ) const
{
  return orientationStr( orientationVector( orient, _orientation.size() ) );
}


std::string Referential::orientationStr( const vector<int> & orient )
{
  unsigned i, n = orient.size();
  string orientv;
  for( i=0; i<n; ++i )
    orientv += orientationStr( Orientation( orient[i] ) );

  return orientv;
}


rc_ptr<Transformation>
Referential::toOrientation( const std::string & orient,
                            const std::vector<float> & transl,
                            bool allow_resize ) const
{
  int n = 0;
  if( !allow_resize )
    n = _orientation.size();
  return toOrientation( orientationVector( orient, n ), transl );
}


rc_ptr<Transformation>
Referential::toOrientation( const vector<int> & orient,
                            const std::vector<float> & transl ) const
{
  rc_ptr<Transformation> trans;
  if( orient.size() < 3 )
  {
    cerr << "Undefined orientation\n";
    return trans;
  }

  AffineTransformationBase *tr
    = new AffineTransformationBase( _orientation.size() );
  trans.reset( tr );

  int axis, n = _orientation.size();
  for( axis=0; axis<n; ++axis)
    tr->matrix()( axis, axis ) = 0;

  for( axis=0; axis<n; ++axis )
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


void Referential::setOrientation( const std::string & orient,
                                  bool allow_resize )
{
  if( orient == "Undefined" )
    return;
  if( orient.size() < 3 )
  {
    cerr << "Invalid orientation string: " << orient << endl;
    return;
  }
  unsigned n = 0;
  if( !allow_resize )
    n = _orientation.size();
  _orientation = orientationVector( orient, n );
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


