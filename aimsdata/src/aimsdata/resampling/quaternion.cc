/* Copyright (c) 1995-2005 CEA
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


#include <aims/resampling/quaternion.h>
#include <aims/resampling/motion.h>

using namespace aims;


Quaternion::Quaternion() : _vector( 1, 0, 0, 0 )
{
}


Quaternion::Quaternion( const Point4df & q ) : _vector( q )
{
}


Quaternion::Quaternion( const Quaternion & q ) : _vector( q._vector )
{
}


Quaternion::Quaternion( float x, float y, float z, float t )
  : _vector( x, y, z, t )
{
}


Quaternion::~Quaternion()
{
}


Quaternion & Quaternion::operator = ( const Quaternion & q )
{
  if( this != &q )
    _vector = q._vector;
  return( *this );
}


void Quaternion::setVector( const Point4df & vec )
{
  // normalize ...

  _vector = vec;
}


AimsVector<float,16> Quaternion::rotationMatrix() const
{
  AimsVector<float,16>	r;

  // from Advanced animations & rendering techniques, copied code
  float	s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
  s = 2.F / ( _vector[0] * _vector[0] + _vector[1] * _vector[1] 
	      + _vector[2] * _vector[2] + _vector[3] * _vector[3] );
  xs = _vector[0] * s;
  ys = _vector[1] * s;
  zs = _vector[2] * s;
  wx = _vector[3] * xs;
  wy = _vector[3] * ys;
  wz = _vector[3] * zs;
  xx = _vector[0] * xs;
  xy = _vector[0] * ys;
  xz = _vector[0] * zs;
  yy = _vector[1] * ys;
  yz = _vector[1] * zs;
  zz = _vector[2] * zs;

  r[0] = 1.F - ( yy + zz );
  r[4] = xy + wz;
  r[8] = xz - wy;
  r[12] = 0.F;

  r[1] = xy - wz;
  r[5] = 1.F - ( xx + zz );
  r[9] = yz + wx;
  r[13] = 0.F;

  r[2] = xz + wy;
  r[6] = yz - wx;
  r[10] = 1.F - ( xx + yy );
  r[14] = 0.F;

  r[12] = 0;
  r[13] = 0;
  r[14] = 0;
  r[15] = 1;

  return( r );
}


AimsVector<float,16> Quaternion::inverseRotationMatrix() const
{
  AimsVector<float,16>	r;

  float	qxx = _vector[0] * _vector[0];
  float	qyy = _vector[1] * _vector[1];
  float	qzz = _vector[2] * _vector[2];
  float	qxy = _vector[0] * _vector[1];
  float	qzt = _vector[2] * _vector[3];
  float	qzx = _vector[2] * _vector[0];
  float	qyt = _vector[1] * _vector[3];
  float	qyz = _vector[1] * _vector[2];
  float	qxt = _vector[0] * _vector[3];

  r[0] = 1. - 2. * ( qyy + qzz );
  r[4] = 2. * ( qxy + qzt );
  r[8] = 2. * ( qzx - qyt );
  r[12] = 0;

  r[1] = 2. * ( qxy - qzt );
  r[5] = 1. - 2. * ( qzz + qxx );
  r[9] = 2. * ( qyz + qxt );
  r[13] = 0;

  r[2] = 2. * ( qzx + qyt );
  r[6] = 2. * ( qyz - qxt );
  r[10] = 1. - 2. * ( qyy + qxx );
  r[14] = 0;

  r[3] = 0;
  r[7] = 0;
  r[11] = 0;
  r[15] = 1;

  return( r );
}


Point3df Quaternion::apply( const Point3df & p ) const
{
  /*float	a[9] = { _vector[0] * _vector[0] * 2, // 0
		 _vector[1] * _vector[1] * 2, // 1
		 _vector[2] * _vector[2] * 2, // 2
		 _vector[0] * _vector[1] * 2, // 3
		 _vector[0] * _vector[2] * 2, // 4
		 _vector[0] * _vector[3] * 2, // 5
		 _vector[1] * _vector[2] * 2, // 6
		 _vector[1] * _vector[3] * 2, // 7
		 _vector[2] * _vector[3] * 2 }; // 8
  return( Point3df( p[0] * (1. - a[1] + a[2] ) + p[1] * ( a[3] - a[8] ) 
		    + p[2] * ( a[4] + a[7] ), 
		    p[0] * ( a[3] + a[8] ) + p[1] * ( 1. - a[2] - a[0] ) 
		    + p[2] * ( a[6] - a[5] ), 
		    p[0] * ( a[4] - a[7] ) + p[1] * ( a[6] + a[5] ) 
		    + p[2] * ( 1. - a[1] - a[0] ) ) );*/

  AimsVector<float,16>	r = rotationMatrix();
  return( Point3df( r[0] * p[0] + r[4] * p[1] + r[8] * p[2], 
		    r[1] * p[0] + r[5] * p[1] + r[9] * p[2], 
		    r[2] * p[0] + r[6] * p[1] + r[10] * p[2] ) );
}


Point3df Quaternion::applyInverse( const Point3df & p ) const
{
  AimsVector<float,16>	r = rotationMatrix();
  return( Point3df( r[0] * p[0] + r[1] * p[1] + r[2] * p[2], 
		    r[4] * p[0] + r[5] * p[1] + r[6] * p[2], 
		    r[8] * p[0] + r[9] * p[1] + r[10] * p[2] ) );
}


Quaternion & Quaternion::operator *= ( const Quaternion & q )
{
  Point4df		c, d, e;
  const Point4df	& bv = q.vector();
  float			f;

  c = _vector * bv[3];
  d = bv * _vector[3];
  e = cross( bv, _vector );
  f = _vector[3] * bv[3] - dot( _vector, bv );
  _vector = c + d + e;
  _vector[3] = f;
  norm();

  return( *this );
}


Quaternion & Quaternion::operator += ( const Quaternion & q )
{
  return( operator *= ( q ) );
}


Quaternion aims::operator * ( const Quaternion & a, const Quaternion & b )
{
  Quaternion		q;
  Point4df		c, d, e;
  const Point4df	& av = a.vector(), & bv = b.vector();

  c = av * bv[3];
  d = bv * av[3];
  e = Quaternion::cross( bv, av );
  q._vector = c + d + e;
  q._vector[3] = av[3] * bv[3] - Quaternion::dot( av, bv );
  q.norm();

  return( q );
}


Quaternion aims::operator + ( const Quaternion & a, const Quaternion & b )
{
  return( a * b );
}


float Quaternion::dot( const Point4df & a, const Point4df & b )
{
  return( a[0] * b[0] + a[1] * b[1] + a[2] * b[2] );
}


void Quaternion::norm()
{
  float	n =  sqrt( _vector[0] * _vector[0] + _vector[1] * _vector[1] 
		   + _vector[2] * _vector[2] + _vector[3] * _vector[3] );
  _vector /= n;
}


Point3df Quaternion::norm( const Point3df & x )
{
  float	n = 1. / sqrt( x[0] * x[0] + x[1] * x[1] + x[2] * x[2] );
  return( x * n );
}


Quaternion Quaternion::normalized() const
{
  float	n = 1. / (_vector[0] * _vector[0] + _vector[1] * _vector[1] 
		  + _vector[2] * _vector[2] + _vector[3] * _vector[3] );
  return( _vector * n );
}


void Quaternion::fromAxis( const Point3df & c, float phi )
{
  Point3df	a = norm( c ) * (float) sin( phi * 0.5 );
  _vector[0] = a[0];
  _vector[1] = a[1];
  _vector[2] = a[2];
  _vector[3] = cos( phi * 0.5 );
}


Point3df Quaternion::axis() const
{
  float		n = sqrt( _vector[0] * _vector[0] 
			  + _vector[1] * _vector[1] 
			  + _vector[2] * _vector[2] );
  if( n == 0 )
    return( Point3df( 0, 0, 1 ) );

  n = 1. / n;
  if( _vector[3] < 0 )	// negative angle
    n *= -1;
  return( Point3df( _vector[0] * n, _vector[1] * n, _vector[2] * n ) );
}


float Quaternion::angle() const
{
  return( acos( _vector[3] ) * 2 );
}


Point4df Quaternion::cross( const Point4df & v1, const Point4df & v2 )
{
  Point4df	result;
  result[0] = v1[1] * v2[2] - v1[2] * v2[1];
  result[1] = v1[2] * v2[0] - v1[0] * v2[2];
  result[2] = v1[0] * v2[1] - v1[1] * v2[0];
  result[3] = 0;
  return result;
}


void Quaternion::buildFromMatrix( const float* m )
{
  // from Advanced animations and rendering techniques, 
  // A. Watt, M. Watt, Addison-Wesley, ACM press
  float	tr, s;

  tr = m[0] + m[5] + m[10];
  if( tr > 0 )
    {
      s = sqrt( tr + 1. );
      _vector[3] = s * 0.5;
      s = 0.5 / s;
      _vector[0] = ( m[6] - m[9] ) * s;
      _vector[1] = ( m[8] - m[2] ) * s;
      _vector[2] = ( m[1] - m[4] ) * s;
    }
  else
    {
      int	i = 0, j, k;

      if( m[5] > m[0] )
	i = 1;
      if( m[10] > m[i+i*4] )
	i = 2;
      j = (i+1) % 3;
      k = (j+1) % 3;
      s = sqrt( m[i+i*4] - ( m[j+j*4] + m[k+k*4] ) + 1. );
      _vector[i] = s * 0.5;
      s = 0.5 / s;
      _vector[3] = ( m[j*4+k] - m[k*4+j] ) * s;
      _vector[j] = ( m[i*4+j] + m[j*4+i] ) * s;
      _vector[k] = ( m[i*4+k] + m[k*4+i] ) * s;
    }
}


void Quaternion::buildFromMotion( const Motion & m )
{
  /* I reimplement buildFromMatrix() to speedup,
  and avoid matrix copy/transposition */

  Point4df      vec;
  float tr, s;

  const AimsData<float> & rotation = m.rotation();

  tr = rotation(0,0) + rotation(1,1) + rotation(2,2);
  if( tr > 0 )
  {
    s = sqrt( tr + 1. );
    _vector[3] = s * 0.5;
    s = 0.5 / s;
    _vector[0] = ( rotation(1,2) - rotation(2,1) ) * s;
    _vector[1] = ( rotation(2,0) - rotation(0,2) ) * s;
    _vector[2] = ( rotation(0,1) - rotation(1,0) ) * s;
  }
  else
  {
    int       i = 0, j, k;

    if( rotation(1,1) > rotation(0,0) )
      i = 1;
    if( rotation(2,2) > rotation(i,i) )
      i = 2;
    j = (i+1) % 3;
    k = (j+1) % 3;
    s = sqrt( rotation(i,i) - ( rotation(j,j) + rotation(k,k) ) + 1. );
    _vector[i] = s * 0.5;
    s = 0.5 / s;
    _vector[3] = ( rotation(j,k) - rotation(k,j) ) * s;
    _vector[j] = ( rotation(i,j) - rotation(j,i) ) * s;
    _vector[k] = ( rotation(i,k) - rotation(k,i) ) * s;
  }
}


Quaternion Quaternion::inverse() const
{
  return( Quaternion( Point4df( -_vector[0], -_vector[1], -_vector[2], 
				_vector[3] ) ) );
}
