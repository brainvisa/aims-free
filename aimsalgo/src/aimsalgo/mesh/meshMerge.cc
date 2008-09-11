/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#include <aims/mesh/meshMerge.h>
#include <aims/math/mathelem.h>

using namespace aims;
using namespace std;


AimsSurfaceTriangle aims::createCylinder( const Point3df & pos1, 
					  const Point3df & pos2, float radius, 
					  unsigned n, bool closed )
{
  AimsSurfaceTriangle           surf;
  AimsSurface<3,Void>		& s = surf[0];
  vector<Point3df>		& vert = s.vertex();
  vector<Point3df>		& norm = s.normal();
  vector< AimsVector<uint,3> >	& poly = s.polygon();
  Point3df			v = pos1, d = pos1 - pos2, n1, n2, dp;
  unsigned			i;
  float				t, p, c, si, r;
  unsigned			p1, p2, p3, p4;

  if ( n == 0 )
    {
      n = (unsigned) ( ( radius + 5 ) * 0.5 );
      if( n < 6 )
	n = 6;
    }
  p = M_PI * 2 / n;

  r = 1. / sqrt( d[0] * d[0] + d[1] * d[1] + d[2] * d[2] );
  d[0] *= r;
  d[1] *= r;
  d[2] *= r;

  //	normals to d
  n1 = Point3df( d[1] * d[2], d[0] * d[2], -d[0] * d[1] );
  t = 1. / sqrt( n1[0] * n1[0] + n1[1] * n1[1] + n1[2] * n1[2] );
  n1[0] *= t;
  n1[1] *= t;
  n1[2] *= t;
  n2 = Point3df( d[1] * n1[2] - d[2] * n1[1], d[2] * n1[0] - d[0] * n1[2], 
		 d[0] * n1[1] - d[1] * n1[0] );

  for( i=0; i<n; ++i )
    {
      t = ((float) i) * p;
      c = cos( t );
      si = sin( t );
      dp[0] = c * n1[0] + si * n2[0];
      dp[1] = c * n1[1] + si * n2[1];
      dp[2] = c * n1[2] + si * n2[2];
      v[0] = pos1[0] + radius * dp[0];
      v[1] = pos1[1] + radius * dp[1];
      v[2] = pos1[2] + radius * dp[2];
      vert.push_back( v );
      vert.push_back( v );
      norm.push_back( dp );
      norm.push_back( d );
      v[0] = pos2[0] + radius * dp[0];
      v[1] = pos2[1] + radius * dp[1];
      v[2] = pos2[2] + radius * dp[2];
      vert.push_back( v );
      vert.push_back( v );
      norm.push_back( dp );
      norm.push_back( -d );
    }

  if ( closed )
    {
      //	centers
      vert.push_back( pos1 );
      vert.push_back( pos2 );
      norm.push_back( d );
      norm.push_back( -d );
    }

  for( i=0; i<n; ++i )
    {
      p1 = i*4;
      p2 = i*4+2;
      p3 = (i*4+4) % (n*4);
      p4 = (i*4+6) % (n*4);
      poly.push_back( AimsVector<uint,3>( p1, p2, p3 ) );
      poly.push_back( AimsVector<uint,3>( p2, p4, p3 ) );
    }

  if ( closed )
    {
      //	ends of cylinder
      for( i=0; i<n; ++i )
	poly.push_back( AimsVector<uint,3>( i*4+1, (i*4+5) % (n*4), n*4 ) );
      for( i=0; i<n; ++i )
	poly.push_back( AimsVector<uint,3>( (i*4+7) % (n*4), (i*4+3) % (n*4), 
					     n*4+1 ) );
    }

  return surf;
}


AimsSurfaceTriangle aims::createCube( const Point3df & pos, float size )
{
  AimsSurfaceTriangle		surf;
  AimsSurface<3,Void>		& s = surf[0];
  vector<Point3df>		& vert = s.vertex();
  vector<Point3df>		& norm = s.normal();
  vector< AimsVector<uint,3> >	& poly = s.polygon();
  Point3df			v = pos;
  float				len = 0.58;

  size *= 0.5;
  v[0] -= size;
  v[1] -= size;
  v[2] -= size;
  vert.push_back( v );
  v[0] = pos[0] + size;
  vert.push_back( v );
  v[1] = pos[1] + size;
  vert.push_back( v );
  v[0] = pos[0] - size;
  vert.push_back( v );
  v[2] = pos[2] + size;
  vert.push_back( v );
  v[1] = pos[1] - size;
  vert.push_back( v );
  v[0] = pos[0] + size;
  vert.push_back( v );
  v[1] = pos[1] + size;
  vert.push_back( v );

  norm.push_back( Point3df( -len, -len, -len ) );
  norm.push_back( Point3df( len, -len, -len ) );
  norm.push_back( Point3df( len, len, -len ) );
  norm.push_back( Point3df( -len, len, -len ) );
  norm.push_back( Point3df( -len, len, len ) );
  norm.push_back( Point3df( -len, -len, len ) );
  norm.push_back( Point3df( len, -len, len ) );
  norm.push_back( Point3df( len, len, len ) );

  poly.push_back( AimsVector<uint,3>( 0, 2, 1 ) );
  poly.push_back( AimsVector<uint,3>( 0, 3, 2 ) );
  poly.push_back( AimsVector<uint,3>( 0, 1, 5 ) );
  poly.push_back( AimsVector<uint,3>( 1, 6, 5 ) );
  poly.push_back( AimsVector<uint,3>( 1, 2, 7 ) );
  poly.push_back( AimsVector<uint,3>( 1, 7, 6 ) );
  poly.push_back( AimsVector<uint,3>( 2, 3, 4 ) );
  poly.push_back( AimsVector<uint,3>( 2, 4, 7 ) );
  poly.push_back( AimsVector<uint,3>( 0, 5, 3 ) );
  poly.push_back( AimsVector<uint,3>( 3, 5, 4 ) );
  poly.push_back( AimsVector<uint,3>( 4, 5, 6 ) );
  poly.push_back( AimsVector<uint,3>( 6, 7, 4 ) );

  return surf;
}


AimsSurfaceTriangle 
aims::createIcosahedron( const Point3df & pos, float size )
{
  AimsSurfaceTriangle		surf;
  AimsSurface<3,Void>		& s = surf[0];
  vector<Point3df>		& vert = s.vertex();
  vector<Point3df>		& norm = s.normal();
  vector< AimsVector<uint,3> >	& poly = s.polygon();
  static const float		X = .525731112119133606;
  static const float		Z = .850650808352039932;
  float				x = size * X;
  float				z = size * Z;

  vert.reserve( 12 );
  norm.reserve( 12 );
  poly.reserve( 20 );

  vert.push_back( pos + Point3df( -x,0.0,z ) );
  vert.push_back( pos + Point3df( x,0.0,z ) );
  vert.push_back( pos + Point3df( -x,0.0,-z ) );
  vert.push_back( pos + Point3df( x,0.0,-z ) );
  vert.push_back( pos + Point3df( 0.0,z,x ) );
  vert.push_back( pos + Point3df( 0.0,z,-x ) );
  vert.push_back( pos + Point3df( 0.0,-z,x ) );
  vert.push_back( pos + Point3df( 0.0,-z,-x ) );
  vert.push_back( pos + Point3df( z,x,0.0 ) );
  vert.push_back( pos + Point3df( -z,x,0.0 ) );
  vert.push_back( pos + Point3df( z,-x,0.0 ) );
  vert.push_back( pos + Point3df( -z,-x,0.0 ) );

  norm.push_back( Point3df( -X,0.0,Z ) );
  norm.push_back( Point3df( X,0.0,Z ) );
  norm.push_back( Point3df( -X,0.0,-Z ) );
  norm.push_back( Point3df( X,0.0,-Z ) );
  norm.push_back( Point3df( 0.0,Z,X ) );
  norm.push_back( Point3df( 0.0,Z,-X ) );
  norm.push_back( Point3df( 0.0,-Z,X ) );
  norm.push_back( Point3df( 0.0,-Z,-X ) );
  norm.push_back( Point3df( Z,X,0.0 ) );
  norm.push_back( Point3df( -Z,X,0.0 ) );
  norm.push_back( Point3df( Z,-X,0.0 ) );
  norm.push_back( Point3df( -Z,-X,0.0 ) );

  poly.push_back( AimsVector<uint,3>( 1,4,0 ) );
  poly.push_back( AimsVector<uint,3>( 4,9,0 ) );
  poly.push_back( AimsVector<uint,3>( 4,5,9 ) );
  poly.push_back( AimsVector<uint,3>( 8,5,4 ) );
  poly.push_back( AimsVector<uint,3>( 1,8,4 ) );
  poly.push_back( AimsVector<uint,3>( 1,10,8 ) );
  poly.push_back( AimsVector<uint,3>( 10,3,8 ) );
  poly.push_back( AimsVector<uint,3>( 8,3,5 ) );
  poly.push_back( AimsVector<uint,3>( 3,2,5 ) );
  poly.push_back( AimsVector<uint,3>( 3,7,2 ) );
  poly.push_back( AimsVector<uint,3>( 3,10,7 ) );
  poly.push_back( AimsVector<uint,3>( 10,6,7 ) );
  poly.push_back( AimsVector<uint,3>( 6,11,7 ) );
  poly.push_back( AimsVector<uint,3>( 6,0,11 ) );
  poly.push_back( AimsVector<uint,3>( 6,1,0 ) );
  poly.push_back( AimsVector<uint,3>( 10,1,6 ) );
  poly.push_back( AimsVector<uint,3>( 11,0,9 ) );
  poly.push_back( AimsVector<uint,3>( 2,11,9 ) );
  poly.push_back( AimsVector<uint,3>( 5,2,9 ) );
  poly.push_back( AimsVector<uint,3>( 11,2,7 ) );

  return surf;
}
