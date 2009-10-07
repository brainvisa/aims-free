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


#ifndef AIMS_MOMENT_MOMENT_H
#define AIMS_MOMENT_MOMENT_H

#include <fstream>

#include <aims/data/data.h>
#include <aims/mesh/surface.h>
#include <aims/bucket/bucket.h>
#include <aims/math/eigen.h>


template< class T > class Moment;


template< class T >
std::ostream& operator << ( std::ostream&, const Moment< T >& );


template< class T >
class Moment
{
  public:
  
    enum Operation
    {
      mSub = -1,
      mAdd = 1
    };

    enum MomentId
    {
      m000 = 0,
    
      m100 = 0,
      m010 = 1,
      m001 = 2,
    
      m200 = 0,
      m020 = 1,
      m002 = 2,
      m110 = 3,
      m101 = 4,
      m011 = 5,
    
      m300 = 0,
      m030 = 1,
      m003 = 2,
      m210 = 3,
      m201 = 4,
      m120 = 5,
      m021 = 6,
      m102 = 7,
      m012 = 8,
      m111 = 9
    };

    /// doesn't necessarily clear all moments to 0. Call clear() to be sure
    Moment();
    Moment( const Moment< T >& );
    virtual ~Moment() { }

    virtual void clear();
    double cx() const { return _cx; }
    double cy() const { return _cy; }
    double cz() const { return _cz; }
    double ct() const { return _ct; }
    void setcx( double x ) { _cx = x; }
    void setcy( double x ) { _cy = x; }
    void setcz( double x ) { _cz = x; }
    void setct( double x ) { _ct = x; }
    
    double *gravity() { return &_g[ 0 ]; }
    const double *gravity() const { return &_g[ 0 ]; }
    
    double& sum() { return _sum; }
    const double& sum() const { return _sum; }
    double& m0() { return _m0; }
    const double& m0() const { return _m0; }
    
    double *m1() { return &_m1[ 0 ]; }
    const double *m1() const { return &_m1[ 0 ]; }
    double *m2() { return &_m2[ 0 ]; }
    const double *m2() const { return &_m2[ 0 ]; }
    double *m3() { return &_m3[ 0 ]; }
    const double *m3() const { return &_m3[ 0 ]; }
    
    const AimsData< double >& eigenValue() const { return _eigenValue; }
    const AimsData< double >& eigenVector() const { return _eigenVector; }
  
    virtual void update( Point3df&, int );
    virtual void update( AimsVector<double,3>&, int );
    virtual void update( double, double, double, int );
    
    virtual void doit( AimsData< T >&, T, int ) { }
    virtual void doit( AimsSurfaceTriangle& ) { }
    virtual void doit( const aims::BucketMap<Void> & ) { }

    virtual void orientation();

    Moment<T> & operator += ( const Moment<T> & );
    Moment<T> operator + ( const Moment<T> & );
    
  protected:
  
    double _cx;
    double _cy;
    double _cz;
    double _ct;
    
    double _g[ 3 ];
    
    double _sum;
    double _m0;
    
    double _m1[ 3 ];
    double _m2[ 6 ];
    double _m3[ 10 ];
    
    AimsData< double > _eigenValue;
    AimsData< double > _eigenVector;
};


template< class T > inline 
Moment< T >::Moment() : _cx( 1.0 ), _cy( 1.0 ), _cz( 1.0 ), _ct( 1.0 ),
                        _sum( 0.0 ), _m0( 0.0 )
{
}


template< class T > inline
Moment< T >::Moment( const Moment< T >& other )
            : _cx( other.cx() ), _cy( other.cy() ), _cz( other.cz() ),
	      _ct( other.ct() ), _sum( other.sum() ), _m0( other.m0() ),
	      _eigenValue( other.eigenValue() ), 
	      _eigenVector( other.eigenVector() )
{
  int i;
  
  for ( i=0; i<3; i++ ) 
    {
      _g[ i ] = other.gravity()[ i ];
      _m1[ i ] = other.m1()[ i ];
    }
  for ( i=0; i<6; i++ ) _m2[ i ] = other.m2()[ i ];
  for ( i=0; i<9; i++ ) _m3[ i ] = other.m3()[ i ];
}


template< class T > inline
void Moment< T >::clear()
{
  _m0 = 0;
  _m1[0] = 0;
  _m1[1] = 0;
  _m1[2] = 0;
  _m2[0] = 0;
  _m2[1] = 0;
  _m2[2] = 0;
  _m2[3] = 0;
  _m2[4] = 0;
  _m2[5] = 0;
  _m3[0] = 0;
  _m3[1] = 0;
  _m3[2] = 0;
  _m3[3] = 0;
  _m3[4] = 0;
  _m3[5] = 0;
  _m3[6] = 0;
  _m3[7] = 0;
  _m3[8] = 0;
  _m3[9] = 0;
  _sum = 0;
  _g[0] = 0;
  _g[1] = 0;
  _g[2] = 0;
  // cx..ct ?
}


template< class T > inline
void Moment< T >::update( Point3df& pt, int dir )
{
  update( (double)pt[ 0 ], (double)pt[ 1 ], (double)pt[ 2 ], dir );
}


template< class T > inline
void Moment<T>::update( AimsVector<double,3>& pt, int dir )
{
  update( pt[ 0 ], pt[ 1 ], pt[ 2 ], dir );
}


template< class T > inline
void Moment< T >::update( double, double, double, int )
{
  orientation();
}


template< class T > inline
void Moment< T >::orientation()
{
  _eigenVector = AimsData< double >( 3, 3 );
  
  _eigenVector( 0, 0 ) = _m2[ 0 ];
  _eigenVector( 1, 1 ) = _m2[ 1 ];
  _eigenVector( 2, 2 ) = _m2[ 2 ];
  _eigenVector( 0, 1 ) = _eigenVector( 1, 0 ) = _m2[ 3 ];
  _eigenVector( 0, 2 ) = _eigenVector( 2, 0 ) = _m2[ 4 ];
  _eigenVector( 1, 2 ) = _eigenVector( 2, 1 ) = _m2[ 5 ];
  
  AimsEigen< double > eigen;
  _eigenValue = eigen.doit( _eigenVector );
  eigen.sort( _eigenVector, _eigenValue );
}


template< class T > inline
std::ostream& operator << ( std::ostream& out , const Moment< T >& mom )
{
  int i;

  out << "Image geometry: (" << mom.cx() << "x" << mom.cy() << "x" << mom.cz();
  out << ")" << std::endl << "Volume :  " << mom.m0() << "mm3" << std::endl;
  out << "Number of points : " << mom.sum() << std::endl;
  out << "Volume (mm3) : " << mom.m0() << std::endl;
  out << "Order 1 moments : ";
  for ( i=0; i<3; i++ )  out << mom.m1()[ i ] << " ";
  out << std::endl << "Order 2 moments : ";
  for ( i=0; i<6; i++ )  out << mom.m2()[ i ] << " ";
  out << std::endl << "Order 3 moments : ";
  for ( i=0; i<10; i++ )  out << mom.m3()[ i ] << " ";
  out << std::endl << "Center of Mass :  (" << mom.gravity()[ 0 ] << ",";
  out << mom.gravity()[ 1 ] << "," << mom.gravity()[ 2 ] << ")" << std::endl;
  out << "Orientation :" << std::endl << "    Inerty :  ";
  out << mom.eigenValue()( 0, 0 ) << " ; " << mom.eigenValue()( 1, 1 );
  out << " ; " << mom.eigenValue()( 2, 2 );
  out << std::endl << "    V1 :  (" << mom.eigenVector()( 0, 0 ) << ",";
  out << mom.eigenVector()( 1, 0 ) << "," << mom.eigenVector()( 2, 0 ) << ")";
  out << std::endl << "    V2 :  (" << mom.eigenVector()( 0, 1 ) << ",";
  out << mom.eigenVector()( 1, 1 ) << "," << mom.eigenVector()( 2, 1 ) << ")";
  out << std::endl << "    V3 :  (" << mom.eigenVector()( 0, 2 ) << ",";
  out << mom.eigenVector()( 1, 2 ) << "," << mom.eigenVector()( 2, 2 ) << ")";
  
  return out;
}


template< class T > inline
Moment<T> & Moment<T>::operator += ( const Moment<T> & m )
{
  // un-center m2 and m3 which are centered, contrarily to m1
  // don't ask me why (denis 22/07/2006)
  if( _sum == 0 )
    {
      *this = m;
      return *this;
    }
  else if( m._sum == 0 )
    return *this;

  double	g0 = _m1[0] / _m0;
  double	g1 = _m1[1] / _m0;
  double	g2 = _m1[2] / _m0;

  _m3[0] += _m0*g0*g0*g0 + g0*_m2[0]*3;
  _m3[1] += _m0*g1*g1*g1 + g1*_m2[1]*3;
  _m3[2] += _m0*g2*g2*g2 + g2*_m2[2]*3;
  _m3[3] += _m0*g0*g0*g1 + g0*_m2[3]*2 + g1*_m2[0];
  _m3[4] += _m0*g0*g0*g2 + g0*_m2[4]*2 + g2*_m2[0];
  _m3[5] += _m0*g0*g1*g1 + g0*_m2[1] + g1*_m2[3]*2;
  _m3[6] += _m0*g1*g1*g2 + g1*_m2[5]*2 + g2*_m2[1];
  _m3[7] += _m0*g0*g2*g2 + g0*_m2[2] + g2*_m2[4]*2;
  _m3[8] += _m0*g1*g2*g2 + g1*_m2[2] + g2*_m2[5]*2;
  _m3[9] += _m0*g0*g1*g2 + g0*_m2[5] + g1*_m2[4] +g2*_m2[3];

  _m2[0] += _m0 * g0*g0;
  _m2[1] += _m0 * g1*g1;
  _m2[2] += _m0 * g2*g2;
  _m2[3] += _m0 * g0*g1; 
  _m2[4] += _m0 * g0*g2;
  _m2[5] += _m0 * g1*g2;

  double m20 = m._m2[0];
  double m21 = m._m2[1];
  double m22 = m._m2[2];
  double m23 = m._m2[3];
  double m24 = m._m2[4];
  double m25 = m._m2[5];

  double h0 = m._m1[0] / m._m0;
  double h1 = m._m1[1] / m._m0;
  double h2 = m._m1[2] / m._m0;

  _sum += m._sum;
  _m0 += m._m0;
  _m1[0] += m._m1[0];
  _m1[1] += m._m1[1];
  _m1[2] += m._m1[2];

  _m3[0] += m._m3[0] + m._m0*h0*h0*h0 + h0*m20*3;
  _m3[1] += m._m3[1] + m._m0*h1*h1*h1 + h1*m21*3;
  _m3[2] += m._m3[2] + m._m0*h2*h2*h2 + h2*m22*3;
  _m3[3] += m._m3[3] + m._m0*h0*h0*h1 + h0*m23*2 + h1*m20;
  _m3[4] += m._m3[4] + m._m0*h0*h0*h2 + h0*m24*2 + h2*m20;
  _m3[5] += m._m3[5] + m._m0*h0*h1*h1 + h0*m21 + h1*m23*2;
  _m3[6] += m._m3[6] + m._m0*h1*h1*h2 + h1*m25*2 + h2*m21;
  _m3[7] += m._m3[7] + m._m0*h0*h2*h2 + h0*m22 + h2*m24*2;
  _m3[8] += m._m3[8] + m._m0*h1*h2*h2 + h1*m22 + h2*m25*2;
  _m3[9] += m._m3[9] + m._m0*h0*h1*h2 + h0*m25 + h1*m24 +h2*m23;

  _m2[0] += m._m2[0] + m._m0 * h0*h0;
  _m2[1] += m._m2[1] + m._m0 * h1*h1;
  _m2[2] += m._m2[2] + m._m0 * h2*h2;
  _m2[3] += m._m2[3] + m._m0 * h0*h1;
  _m2[4] += m._m2[4] + m._m0 * h0*h2;
  _m2[5] += m._m2[5] + m._m0 * h1*h2;

  // TODO: check that _cx..._ct match

  _g[0] = _m1[0] / _m0;
  _g[1] = _m1[1] / _m0;
  _g[2] = _m1[2] / _m0;

  g0 = _g[0];
  g1 = _g[1];
  g2 = _g[2];

  // center back m2 and m3
  _m2[0] -= _m0 * g0*g0;
  _m2[1] -= _m0 * g1*g1;
  _m2[2] -= _m0 * g2*g2;
  _m2[3] -= _m0 * g0*g1;
  _m2[4] -= _m0 * g0*g2;
  _m2[5] -= _m0 * g1*g2;

  _m3[0] -= _m0*g0*g0*g0 + g0*_m2[0]*3;
  _m3[1] -= _m0*g1*g1*g1 + g1*_m2[1]*3;
  _m3[2] -= _m0*g2*g2*g2 + g2*_m2[2]*3;
  _m3[3] -= _m0*g0*g0*g1 + g0*_m2[3]*2 + g1*_m2[0];
  _m3[4] -= _m0*g0*g0*g2 + g0*_m2[4]*2 + g2*_m2[0];
  _m3[5] -= _m0*g0*g1*g1 + g0*_m2[1] + g1*_m2[3]*2;
  _m3[6] -= _m0*g1*g1*g2 + g1*_m2[5]*2 + g2*_m2[1];
  _m3[7] -= _m0*g0*g2*g2 + g0*_m2[2] + g2*_m2[4]*2;
  _m3[8] -= _m0*g1*g2*g2 + g1*_m2[2] + g2*_m2[5]*2;
  _m3[9] -= _m0*g0*g1*g2 + g0*_m2[5] + g1*_m2[4] +g2*_m2[3];

  orientation();
  return *this;
}


template< class T > inline
Moment<T> Moment<T>::operator + ( const Moment<T> & m )
{
  Moment<T>	mom( *this );
  mom += m;
  return mom;
}


#endif
