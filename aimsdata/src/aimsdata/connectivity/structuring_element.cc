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

//--- aims -------------------------------------------------------------------
#include <aims/connectivity/structuring_element.h>
#include <aims/vector/vector.h>                                     // Point3d
//--- carto ------------------------------------------------------------------
#include <cartobase/smart/rcptr.h>                           // smart pointers
//--- std --------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>
//----------------------------------------------------------------------------

using namespace aims;
using namespace aims::strel;
using namespace carto;
using namespace std;

//============================================================================
// STRUCTURING ELEMENT: BASE CLASS
//============================================================================

vector<int> StructuringElement::getAmplitude(
  const Point3d & origin
) const
{
  const_iterator i, e = end();
  vector<int> amplitude(6,0);
  for( i=begin(); i!=e; ++i )
  {
    amplitude[0] = abs( min( -amplitude[0], origin[0] + (*i)[0] ) );
    amplitude[1] = abs( max(  amplitude[1], origin[0] + (*i)[0] ) );
    amplitude[2] = abs( min( -amplitude[2], origin[1] + (*i)[1] ) );
    amplitude[3] = abs( max(  amplitude[3], origin[1] + (*i)[1] ) );
    amplitude[4] = abs( min( -amplitude[4], origin[2] + (*i)[2] ) );
    amplitude[5] = abs( max(  amplitude[5], origin[2] + (*i)[2] ) );
  }
  return amplitude;
}

namespace aims {
  namespace strel {
    StructuringElementRef none()
    {
      static StructuringElementRef se;
      return se;
    }
  }
}

//============================================================================
// SHAPE: INTERFACE
//============================================================================

void Shape::setParameters(
  const vector<double> & amplitude,
  const bool usecenter
)
{
  Point3d origin(0,0,0);
  setParameters( origin, amplitude, usecenter );
}

void Shape::setParameters(
  const Point3d & origin,
  const double amplitude,
  const bool usecenter
)
{
  vector<double> vamplitude(3, amplitude);
  setParameters( origin, vamplitude, usecenter );
}

void Shape::setParameters(
  const double amplitude,
  const bool usecenter
)
{
  Point3d origin(0,0,0);
  vector<double> vamplitude(3, amplitude);
  setParameters( origin, vamplitude, usecenter );
}


//============================================================================
// SHAPE: FACTORY
//============================================================================

void ShapeFactory::init()
{
  static bool initialized = false;
  if( !initialized )
  {
    initialized = true;
    registerShape( "cube", Cube() );
    registerShape( "squarexy", SquareXY() );
    registerShape( "squarexz", SquareXZ() );
    registerShape( "squareyz", SquareYZ() );
    registerShape( "sphere", Sphere() );
    registerShape( "diskxy", DiskXY() );
    registerShape( "diskxz", DiskXZ() );
    registerShape( "diskyz", DiskYZ() );
    registerShape( "cross", Cross() );
    registerShape( "crossxy", CrossXY() );
    registerShape( "crossxz", CrossXZ() );
    registerShape( "crossyz", CrossYZ() );
    registerShape( "diagonalcross", DiagonalCross() );
    registerShape( "diagonalcrossxy", DiagonalCrossXY() );
    registerShape( "diagonalcrossxz", DiagonalCrossXZ() );
    registerShape( "diagonalcrossyz", DiagonalCrossYZ() );
    registerShape( "circlexy", CircleXY() );
    registerShape( "clockwisecirclexy", ClockWiseCircleXY() );
  }
}

map<string,rc_ptr<Shape> > & ShapeFactory::_map()
{
  static map<string,rc_ptr<Shape> > m;
  return m;
}

void ShapeFactory::registerShape(
  const string & type,
  const Shape & strel
)
{
  init();
  _map()[ type ] = rc_ptr<Shape>( strel.clone() );
}

set<string> ShapeFactory::shapes()
{
  init();
  set<string> s;
  map<string,rc_ptr<Shape> >::const_iterator i, e = _map().end();
  for( i=_map().begin(); i!=e; ++i )
    s.insert( i->first );
  return( s );
}

Shape* ShapeFactory::create(
  const string & type,
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  init();
  map<string,rc_ptr<Shape> >::const_iterator i;
  i = _map().find( type );
  if( i == _map().end() )
    return( 0 );
  Shape * new_shape = i->second->clone();
  new_shape->setParameters( origin, amplitude, usecenter );
  return new_shape;
}

Shape* ShapeFactory::create(
  const string & type,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  Point3d origin(0,0,0);
  return create( type, origin, amplitude, usecenter );
}

Shape* ShapeFactory::create(
  const string & type,
  const Point3d & origin,
  const double amplitude,
  const bool usecenter
)
{
  vector<double> vamplitude( 3, amplitude );
  return create( type, origin, vamplitude, usecenter );
}

Shape* ShapeFactory::create(
  const string & type,
  const double amplitude,
  const bool usecenter
)
{
  Point3d origin(0,0,0);
  vector<double> vamplitude( 3, amplitude );
  return create( type, origin, vamplitude, usecenter );
}

//============================================================================
// CONNECTIVITY: INTERFACE
//============================================================================

void Connectivity::setVectorFromMatrix(
  const Point3d & origin,
  const bool usecenter
)
{
  Connectivity::Matrix3x3x3Const m = getMatrix();
  _vector.clear();
  for( int z=0; z<3; ++z )
    for( int y=0; y<3; ++y )
      for( int x=0; x<3; ++x )
          if( (m[z][y][x]) )
            _vector.push_back( origin + Point3d( x-1, y-1, z-1 ) );
  if( usecenter )
    _vector.push_back( origin );
}

//============================================================================
// CONNECTIVITY: FACTORY
//============================================================================

void ConnectivityFactory::init()
{
  static bool initialized = false;
  if( !initialized )
  {
    initialized = true;
    registerConnectivity( "4xy", Connectivity4XY() );
    registerConnectivity( "4xz", Connectivity4XZ() );
    registerConnectivity( "4yz", Connectivity4YZ() );
    registerConnectivity( "4xydiag", Connectivity4XYDiag() );
    registerConnectivity( "4xzdiag", Connectivity4XZDiag() );
    registerConnectivity( "4yzdiag", Connectivity4YZDiag() );
    registerConnectivity( "6", Connectivity6XYZ() );
    registerConnectivity( "8xy", Connectivity8XY() );
    registerConnectivity( "8xz", Connectivity8XZ() );
    registerConnectivity( "8yz", Connectivity8YZ() );
    registerConnectivity( "8xyz", Connectivity8XYZ() );
    registerConnectivity( "18", Connectivity18XYZ() );
    registerConnectivity( "26", Connectivity26XYZ() );
    registerConnectivity( "9xyz-", Connectivity9XYZMinus() );
    registerConnectivity( "9xyz+", Connectivity9XYZPlus() );
    registerConnectivity( "9xzy-", Connectivity9XZYMinus() );
    registerConnectivity( "9xzy+", Connectivity9XZYPlus() );
    registerConnectivity( "9yzx-", Connectivity9YZXMinus() );
    registerConnectivity( "9yzx+", Connectivity9YZXPlus() );
    registerConnectivity( "5x-y", Connectivity5XMinusY() );
    registerConnectivity( "5x+y", Connectivity5XPlusY() );
    registerConnectivity( "5xy-", Connectivity5XYMinus() );
    registerConnectivity( "5xy+", Connectivity5XYPlus() );
    registerConnectivity( "5x-z", Connectivity5XMinusZ() );
    registerConnectivity( "5x+z", Connectivity5XPlusZ() );
    registerConnectivity( "5xz-", Connectivity5XZMinus() );
    registerConnectivity( "5xz+", Connectivity5XZPlus() );
    registerConnectivity( "5y-z", Connectivity5YMinusZ() );
    registerConnectivity( "5y+z", Connectivity5YPlusZ() );
    registerConnectivity( "5yz-", Connectivity5YZMinus() );
    registerConnectivity( "5yz+", Connectivity5YZPlus() );
  }
}

map<string,rc_ptr<Connectivity> > & ConnectivityFactory::_map()
{
  static map<string,rc_ptr<Connectivity> > m;
  return m;
}

void ConnectivityFactory::registerConnectivity(
  const string & type,
  const Connectivity & strel
)
{
  init();
  _map()[ type ] = rc_ptr<Connectivity>( strel.clone() );
}

set<string> ConnectivityFactory::connectivities()
{
  init();
  set<string> s;
  map<string,rc_ptr<Connectivity> >::const_iterator i, e = _map().end();
  for( i=_map().begin(); i!=e; ++i )
    s.insert( i->first );
  return( s );
}

Connectivity* ConnectivityFactory::create(
  const string & type,
  const Point3d & origin,
  const bool usecenter
)
{
  init();
  map<string,rc_ptr<Connectivity> >::const_iterator i;
  i = _map().find( type );
  if( i == _map().end() )
    return( 0 );
  Connectivity * new_con = i->second->clone();
  new_con->setVectorFromMatrix( origin, usecenter );
  return new_con;
}

Connectivity* ConnectivityFactory::create(
  const string & type,
  const bool usecenter
)
{
  return create( type, Point3d(0,0,0), usecenter );
}

//============================================================================
// SHAPE: DERIVED CLASSES
//============================================================================
void Cube::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );
  if( vamplitude.size() < 3 )
    vamplitude.push_back( vamplitude[1] );

  for(int k = - (int)amplitude[2]; k <= (int)amplitude[2]; ++k)
    for(int j = - (int)amplitude[1]; j <= (int)amplitude[1]; ++j)
      for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
        if( usecenter || ( i != 0 ) || ( j != 0 ) || ( k != 0 ) )
          _vector.push_back( Point3d(i, j, k) + origin );
}

void SquareXY::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

  for(int j = - (int)amplitude[1]; j <= (int)amplitude[1]; ++j)
    for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
      if( usecenter || ( i != 0 ) || ( j != 0 ) )
        _vector.push_back( Point3d(i, j, 0) + origin );
}

void SquareXZ::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

  for(int k = - (int)amplitude[1]; k <= (int)amplitude[1]; ++k)
    for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
      if( usecenter || ( i != 0 ) || ( k != 0 ) )
        _vector.push_back( Point3d(i, 0, k) + origin );
}

void SquareYZ::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

  for(int k = - (int)amplitude[1]; k <= (int)amplitude[1]; ++k)
    for(int j = - (int)amplitude[0]; j <= (int)amplitude[0]; ++j)
      if( usecenter || ( j != 0 ) || ( k != 0 ) )
        _vector.push_back( Point3d(0, j, k) + origin );
}

void Sphere::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );
  if( vamplitude.size() < 3 )
    vamplitude.push_back( vamplitude[1] );

  for(int k = - (int)amplitude[2]; k <= (int)amplitude[2]; ++k)
    for(int j = - (int)amplitude[1]; j <= (int)amplitude[1]; ++j)
      for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
        if( ( usecenter || ( i != 0 ) || ( j != 0 ) || ( k != 0 ) ) &&
            sqrt( pow((float)i,2)/pow((float)amplitude[0],2) + 
                  pow((float)j,2)/pow((float)amplitude[1],2) + 
                  pow((float)k,2)/pow((float)amplitude[2],2) ) <= 1 )
          _vector.push_back( Point3d(i, j, k) + origin );
}

void DiskXY::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

  for(int j = - (int)amplitude[1]; j <= (int)amplitude[1]; ++j)
    for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
      if( ( usecenter || ( i != 0 ) || ( j != 0 ) ) &&
          sqrt( pow((float)i, 2) / pow((float)amplitude[0], 2) + 
                pow((float)j, 2) / pow((float)amplitude[1], 2) ) <= 1 )
        _vector.push_back( Point3d(i, j, 0) + origin );
}

void DiskXZ::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

  for(int k = - (int)amplitude[1]; k <= (int)amplitude[1]; ++k)
    for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
      if( ( usecenter || ( i != 0 ) || ( k != 0 ) ) &&
          sqrt( pow((float)i, 2) / pow((float)amplitude[0], 2) + 
                pow((float)k, 2) / pow((float)amplitude[1], 2) ) <= 1 )
        _vector.push_back( Point3d(i, 0, k) + origin );
}

void DiskYZ::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

  for(int k = - (int)amplitude[1]; k <= (int)amplitude[1]; ++k)
    for(int j = - (int)amplitude[0]; j <= (int)amplitude[0]; ++j)
      if( ( usecenter || ( j != 0 ) || ( k != 0 ) ) &&
          sqrt( pow((float)j, 2) / pow((float)amplitude[0], 2) + 
                pow((float)k, 2) / pow((float)amplitude[1], 2) ) <= 1 )
        _vector.push_back( Point3d(0, j, k) + origin );
}

void Cross::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );
  if( vamplitude.size() < 3 )
    vamplitude.push_back( vamplitude[1] );

  for(int k = - (int)amplitude[2]; k <= (int)amplitude[2]; ++k)
    for(int j = - (int)amplitude[1]; j <= (int)amplitude[1]; ++j)
      for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
        if( ( ( i == 0 ) && ( ( j == 0 ) || ( k == 0 ) ) ) || 
            ( ( j == 0 ) && ( k == 0 ) ) )
          if( usecenter || ( i != 0 ) || ( j != 0 ) || ( k != 0 ) )
            _vector.push_back( Point3d(i, j, k) + origin );
}

void CrossXY::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

  for(int j = - (int)amplitude[1]; j <= (int)amplitude[1]; ++j)
    for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
      if( ( i == 0 ) || ( j == 0 ) )
        if( usecenter || ( i != 0 ) || ( j != 0 ) )
          _vector.push_back( Point3d(i, j, 0) + origin );
}

void CrossXZ::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

  for(int k = - (int)amplitude[1]; k <= (int)amplitude[1]; ++k)
    for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
      if( ( i == 0 ) || ( k == 0 ) )
        if( usecenter || ( i != 0 ) || ( k != 0 ) )
          _vector.push_back( Point3d(i, 0, k) + origin );
}

void CrossYZ::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

  for(int k = - (int)amplitude[1]; k <= (int)amplitude[1]; ++k)
    for(int j = - (int)amplitude[0]; j <= (int)amplitude[0]; ++j)
      if( ( j == 0 ) || ( k == 0 ) )
        if( usecenter || ( j != 0 ) || ( k != 0 ) )
          _vector.push_back( Point3d(0, j, k) + origin );
}

void DiagonalCross::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );
  if( vamplitude.size() < 3 )
    vamplitude.push_back( vamplitude[1] );

  for(int k = - (int)amplitude[2]; k <= (int)amplitude[2]; ++k)
    for(int j = - (int)amplitude[1]; j <= (int)amplitude[1]; ++j)
      for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
        if( ( abs(i) == abs(j) ) && ( abs(i) == abs(k) ) )
          if( usecenter || ( i != 0 ) || ( j != 0 ) || ( k != 0 ) )
            _vector.push_back( Point3d(i, j, k) + origin );
}

void DiagonalCrossXY::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

    for(int j = - (int)amplitude[1]; j <= (int)amplitude[1]; ++j)
      for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
        if( abs(i) == abs(j) )
          if( usecenter || ( i != 0 ) || ( j != 0 ) )
            _vector.push_back( Point3d(i, j, 0) + origin );
}

void DiagonalCrossXZ::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

    for(int k = - (int)amplitude[1]; k <= (int)amplitude[1]; ++k)
      for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
        if( abs(i) == abs(k) )
          if( usecenter || ( i != 0 ) || ( k != 0 ) )
            _vector.push_back( Point3d(i, 0, k) + origin );
}

void DiagonalCrossYZ::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

    for(int k = - (int)amplitude[1]; k <= (int)amplitude[1]; ++k)
      for(int j = - (int)amplitude[0]; j <= (int)amplitude[0]; ++j)
        if( abs(j) == abs(k) )
          if( usecenter || ( j != 0 ) || ( k != 0 ) )
            _vector.push_back( Point3d(0, j, k) + origin );
}

void CircleXY::setParameters(
  const Point3d & origin,
  const vector<double> & amplitude,
  const bool usecenter
)
{
  _vector.clear();
  vector<double> vamplitude = amplitude;
  if( vamplitude.size() < 1 )
    vamplitude.push_back( 1.0 );
  if( vamplitude.size() < 2 )
    vamplitude.push_back( vamplitude[0] );

  for(int j = - (int)amplitude[1]; j <= (int)amplitude[1]; ++j)
    for(int i = - (int)amplitude[0]; i <= (int)amplitude[0]; ++i)
      if( ( usecenter || ( i != 0 ) || ( j != 0 ) ) &&
          ( sqrt( pow((float)i, 2)  + pow((float)j, 2) * ( pow( amplitude[0], 2 ) / pow( amplitude[1], 2 ) ) ) <= amplitude[0] ) &&
          ( sqrt( pow((float)i, 2)  + pow((float)j, 2) * ( pow( amplitude[0], 2 ) / pow( amplitude[1], 2 ) ) ) > amplitude[0] - 1 ) )
        _vector.push_back( Point3d(i, j, 0) + origin );
}

void ClockWiseCircleXY::setParameters(
    const Point3d & origin,
    const vector<double> & amplitude,
    const bool usecenter
)
{
    CircleXY::setParameters(origin, amplitude, usecenter);
    clockwise_order();
}

void ClockWiseCircleXY::clockwise_order() {

    // Initialization
    const std::vector<Point3d> & unordered_circle = _vector;
    std::vector<int> amplitude = getAmplitude();
    std::vector<Point3d>::const_iterator it, circle_end = unordered_circle.end();
    std::vector<Point3d> clockwise_circle(unordered_circle.size());
    std::vector<int> quarter_index(4, 0);
    int circle_radius = amplitude[0]; 
    int quarter_size = unordered_circle.size() / 4;
              
    // Reorder per quarter
    for (it = unordered_circle.begin(); it != circle_end; ++it){
        // Compute the difference between the current pixel and the central one
        Point3d p((*it)[0], (*it)[1], (*it)[2]);

        if (((p[0] > 0) && (p[1] < 0)) || ((p[0] == 0) && (p[1] == -circle_radius))) {
            // Top right quarter
            clockwise_circle[quarter_index[0]] = p;
            quarter_index[0]++;
        }
        else if (((p[0] > 0) && (p[1] > 0)) || ((p[0] == circle_radius) && (p[1] == 0))) {
            // Bottom right quarter
            clockwise_circle[quarter_size + quarter_index[1]] = p;
            quarter_index[1]++;
        }
        else if (((p[0] < 0) && (p[1] > 0)) || ((p[0] == 0) && (p[1] == circle_radius))) {
            // Bottom left quarter
            clockwise_circle[quarter_size * 2 + quarter_index[2]] = p;
            quarter_index[2]++;
        }
        else {
            // Top left quarter
            clockwise_circle[quarter_size * 3 + quarter_index[3]] = p;
            quarter_index[3]++;
        }
    }
    
    if (quarter_size > 1) {
        // Reorder bottom right quarter
        int quarter = 1;
        std::vector<Point3d>::iterator sit, send, qit, 
                                    qend = clockwise_circle.begin() 
                                            + (quarter + 1) * quarter_size;
        sit = clockwise_circle.begin() + quarter * quarter_size;
        send = sit;
        for (qit = clockwise_circle.begin() + quarter * quarter_size + 1; 
             qit != qend; ++qit) {
            if ((*qit)[1] == (*sit)[1]) {
                // Same y coordinate
                send = qit;
            }
            else {
                // Sort preceding line and store new line start
//                 std::cout << "quarter " << quarter
//                           << ", reversing " << int(send + 1 - sit) << " elements of "
//                           << "line " << (*sit)[1]
//                           << std::endl << std::flush;
                std::reverse(sit, send + 1);
                sit = qit;
                send = sit;
            }
        }
//         std::cout << "quarter " << quarter
//                   << ", reversing " << int(send + 1 - sit) << " elements of "
//                   << "line " << (*sit)[1]
//                   << std::endl << std::flush;
        std::reverse(sit, send + 1);
        
        // Reorder bottom left quarter
        quarter = 2;
        std::reverse(clockwise_circle.begin() + quarter * quarter_size, 
                     clockwise_circle.begin() + (quarter + 1) * quarter_size);
        
        // Reorder top left quarter
        quarter = 3;
        std::vector<Point3d>::reverse_iterator srit, srend, qrit, 
                                    qrend = clockwise_circle.rbegin() 
                                            + quarter_size;
        srit = clockwise_circle.rbegin();
        srend = srit;
        for (qrit = clockwise_circle.rbegin() + 1; 
             qrit != qrend; ++qrit) {
            if ((*qrit)[1] == (*srit)[1]) {
                // Same y coordinate
                srend = qrit;
            }
            else {
                // Sort preceding line and store new line start
//                 std::cout << "quarter " << quarter
//                           << ", reversing " << int(srend + 1 - srit) << " elements of "
//                           << "line " << (*srit)[1]
//                           << std::endl << std::flush;
                std::reverse(srit, srend + 1);
                srit = qrit;
                srend = srit;
            }
        }
//         std::cout << "quarter " << quarter
//                   << ", reversing " << int(srend + 1 - srit) << " elements of "
//                   << "line " << (*srit)[1]
//                   << std::endl << std::flush;
        std::reverse(srit, srend + 1);
        
        std::reverse(clockwise_circle.begin() + quarter * quarter_size, 
                     clockwise_circle.end());
        
        // Replaces circle vector using the clockwise ordered one
        _vector = clockwise_circle;
    }
}

//============================================================================
// CONNECTIVITY: DERIVED CLASSES
//============================================================================
Connectivity::Matrix3x3x3Const Connectivity4XY::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 1, 0 },
                                     { 1, 0, 1 },
                                     { 0, 1, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity4XZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 1, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 1, 0, 1 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 1, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity4YZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 1, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 1, 0 },
                                     { 0, 0, 0 },
                                     { 0, 1, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 1, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity4XYDiag::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 1, 0, 1 },
                                     { 0, 0, 0 },
                                     { 1, 0, 1 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity4XZDiag::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 1, 0, 1 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 1, 0, 1 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity4YZDiag::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 1, 0 },
                                     { 0, 0, 0 },
                                     { 0, 1, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 1, 0 },
                                     { 0, 0, 0 },
                                     { 0, 1, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity6XYZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 1, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 1, 0 },
                                     { 1, 0, 1 },
                                     { 0, 1, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 1, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity8XY::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 1, 1, 1 },
                                     { 1, 0, 1 },
                                     { 1, 1, 1 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity8XZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 1, 1, 1 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 1, 0, 1 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 1, 1, 1 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity8YZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 1, 0 },
                                     { 0, 1, 0 },
                                     { 0, 1, 0 } },
                                   { { 0, 1, 0 },
                                     { 0, 0, 0 },
                                     { 0, 1, 0 } },
                                   { { 0, 1, 0 },
                                     { 0, 1, 0 },
                                     { 0, 1, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity8XYZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 1, 0, 1 },
                                     { 0, 0, 0 },
                                     { 1, 0, 1 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 1, 0, 1 },
                                     { 0, 0, 0 },
                                     { 1, 0, 1 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity18XYZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 1, 0 },
                                     { 1, 1, 1 },
                                     { 0, 1, 0 } },
                                   { { 1, 1, 1 },
                                     { 1, 0, 1 },
                                     { 1, 1, 1 } },
                                   { { 0, 1, 0 },
                                     { 1, 1, 1 },
                                     { 0, 1, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity26XYZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 1, 1, 1 },
                                     { 1, 1, 1 },
                                     { 1, 1, 1 } },
                                   { { 1, 1, 1 },
                                     { 1, 0, 1 },
                                     { 1, 1, 1 } },
                                   { { 1, 1, 1 },
                                     { 1, 1, 1 },
                                     { 1, 1, 1 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity9XYZMinus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 1, 1, 1 },
                                     { 1, 1, 1 },
                                     { 1, 1, 1 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity9XYZPlus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 1, 1, 1 },
                                     { 1, 1, 1 },
                                     { 1, 1, 1 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity9XZYMinus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 1, 1, 1 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 1, 1, 1 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 1, 1, 1 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity9XZYPlus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 1, 1, 1 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 1, 1, 1 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 1, 1, 1 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity9YZXMinus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 1, 0, 0 },
                                     { 1, 0, 0 },
                                     { 1, 0, 0 } },
                                   { { 1, 0, 0 },
                                     { 1, 0, 0 },
                                     { 1, 0, 0 } },
                                   { { 1, 0, 0 },
                                     { 1, 0, 0 },
                                     { 1, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity9YZXPlus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 1 },
                                     { 0, 0, 1 },
                                     { 0, 0, 1 } },
                                   { { 0, 0, 1 },
                                     { 0, 0, 1 },
                                     { 0, 0, 1 } },
                                   { { 0, 0, 1 },
                                     { 0, 0, 1 },
                                     { 0, 0, 1 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5XMinusY::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 1, 1, 0 },
                                     { 1, 0, 0 },
                                     { 1, 1, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5XPlusY::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 1, 1 },
                                     { 0, 0, 1 },
                                     { 0, 1, 1 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5XYMinus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 1, 1, 1 },
                                     { 1, 0, 1 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5XYPlus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 1, 0, 1 },
                                     { 1, 1, 1 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5XMinusZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 1, 1, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 1, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 1, 1, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5XPlusZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 1, 1 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 1 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 1, 1 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5XZMinus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 1, 1, 1 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 1, 0, 1 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5XZPlus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 1, 0, 1 },
                                     { 0, 0, 0 } },
                                   { { 0, 0, 0 },
                                     { 1, 1, 1 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5YMinusZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 1, 0 },
                                     { 0, 1, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 1, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 1, 0 },
                                     { 0, 1, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5YPlusZ::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 1, 0 },
                                     { 0, 1, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 1, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 1, 0 },
                                     { 0, 1, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5YZMinus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 1, 0 },
                                     { 0, 1, 0 },
                                     { 0, 1, 0 } },
                                   { { 0, 1, 0 },
                                     { 0, 0, 0 },
                                     { 0, 1, 0 } },
                                   { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } } };
  return m;
}

Connectivity::Matrix3x3x3Const Connectivity5YZPlus::getMatrix() const
{
  static const bool m[3][3][3] = { { { 0, 0, 0 },
                                     { 0, 0, 0 },
                                     { 0, 0, 0 } },
                                   { { 0, 1, 0 },
                                     { 0, 0, 0 },
                                     { 0, 1, 0 } },
                                   { { 0, 1, 0 },
                                     { 0, 1, 0 },
                                     { 0, 1, 0 } } };
  return m;
}