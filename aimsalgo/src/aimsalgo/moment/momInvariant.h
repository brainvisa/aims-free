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


#ifndef AIMS_MOMENT_MOMINVARIANT_H
#define AIMS_MOMENT_MOMINVARIANT_H

#include <aims/moment/moment.h>
#include <math.h>
#include <complex>


template< class T > class MomentInvariant;


template< class T > 
std::ostream& operator << ( std::ostream&, const MomentInvariant< T >& );


template< class T >
class MomentInvariant
{
  public:
 
    enum InvariantId
    {
      I00_2 = 0,
      I22_2 = 1,
      I222_3 = 2,
      I11_3 = 3,
      I33_3 = 4,
      I3111_3 = 5,
      I3131_3 = 6,
      I3331_3 = 7,
      I3333_3 = 8,
      I112_23 = 9,
      I123_23 = 10,
      I233_23 = 11
    };
  
    MomentInvariant( Moment< T > *m=0 );
    MomentInvariant( const MomentInvariant< T >& );
    virtual ~MomentInvariant() { }

    Moment< T >& moment() const { return *_mom; }
    
    const double *invariant() const { return &_inv[ 0 ]; }

    void update( double, double, double, int dir=(int)Moment< T >::mAdd );

    void doit( AimsData< T >&, T, int dir=(int)Moment< T >::mAdd );
    void doit( AimsSurfaceTriangle& );
    void doit( Moment< T > * );
       
  private:

    void init();
    void toComplex();
    void toInvariant();
  
    Moment< T > *_mom;

    double _inv[ 12 ];
    
    std::complex< double > _n0;
    std::complex< double > _n1[ 3 ];
    std::complex< double > _n2[ 5 ];
    std::complex< double > _n3[ 7 ];
        
    double _c00;
    double _c10;
    double _c11;
    double _c20;
    double _c22;
    double _c30;
    double _c31;
    double _c32;
    double _c33;
    
    double _cc22;
    double _cc222;
    double _cc11;
    double _cc33;
    double _ce;
    std::complex< double > _p1;
    std::complex< double > _p2;
    std::complex< double > _p3;
    std::complex< double > _p4;
    std::complex< double > _p5;
    std::complex< double > _p6;
    std::complex< double > _p7;
    std::complex< double > _p8;
    std::complex< double > _q1;
    std::complex< double > _q2;
    std::complex< double > _q4;
    std::complex< double > _q5;
    std::complex< double > _r1;
    std::complex< double > _r2;
    
    std::complex< double > _de;
};


template< class T > inline
MomentInvariant< T >::MomentInvariant( Moment< T > *mom )
{
  _mom = mom;
  init();
}


template< class T > inline
MomentInvariant< T >::MomentInvariant( const MomentInvariant< T >& other ) 
{
  _mom = &other.moment();

  init();
  
  for ( int i=0; i<12; i++ ) _inv[ i ] = other.invariant()[ i ];
}


template< class T > inline
void MomentInvariant< T >::init()
{
  // Constants for complex moment computation
  _c00 = 2.0 * sqrt( M_PI ) / 3.0;

  _c10 = 2.0 * sqrt( 3.0 * M_PI ) / 5.0;
  _c11 = sqrt( 6.0 * M_PI ) / 5.0;

  _c20 = 2.0 * sqrt( M_PI / 5.0 ) / 3.0;
  _c22 = sqrt( 2.0 * M_PI / 15.0 );

  _c30 = 2.0 * sqrt( M_PI / 7.0 ) / 5.0;
  _c31 = sqrt( 3.0 * M_PI / 7.0 ) / 5.0;
  _c32 = sqrt( 6.0 * M_PI / 35.0 );
  _c33 = sqrt( M_PI / 35.0 );
  
  // Constants for moment invariant computation
  _cc22 = 16.0 * M_PI / 45.0 / sqrt( 5.0 );
  _cc222 = 32.0 * M_PI * sqrt( 2.0 * M_PI / 7.0 ) / 675.0;
  _cc11 = -4.0 * M_PI * sqrt( 3.0 ) / 25.0;
  _cc33 = -16.0 * M_PI / 175.0 / sqrt( 7.0 );

  _ce = 1.0 / sqrt( 5.0 );

  _p1 = std::complex< double >( sqrt( 10.0 / 21.0 ), 0.0 );
  _p2 = std::complex< double >( -2.0 * sqrt( 5.0 / 21.0 ), 0.0 );
  _p3 = std::complex< double >( sqrt( 2.0 / 7.0 ), 0.0 );
  _p4 = std::complex< double >( 5.0 / sqrt( 21.0 ), 0.0 );
  _p5 = std::complex< double >( -sqrt( 5.0 / 7.0 ), 0.0 );
  _p6 = std::complex< double >( sqrt( 2.0 / 21.0 ), 0.0 );
  _p7 = std::complex< double >( -sqrt( 3.0 / 7.0 ), 0.0 );
  _p8 = std::complex< double >( 2.0 / sqrt( 21.0 ), 0.0 );
  
  _q1 = std::complex< double >( -sqrt( 5.0 / 21.0 ), 0.0 );
  _q2 = std::complex< double >( 1.0 / sqrt( 21.0 ), 0.0 );
  _q4 = std::complex< double >( 1.0 / sqrt( 7.0 ), 0.0 );
  _q5 = std::complex< double >( -2.0 * sqrt( 2.0 / 21.0 ), 0.0 );
  
  _r1 = std::complex< double >( sqrt( 2.0 ), 0.0 );
  _r2 = std::complex< double >( sqrt( 2.0 / 3.0 ), 0.0 );
  
  _de = std::complex< double >( 2.0, 0.0 );
}


template< class T > inline
void MomentInvariant< T >::update( double x, double y, double z, int dir )
{
  if ( _mom )
    {
      _mom->update( x, y, z, dir );
      toComplex();
      toInvariant();
    }
}


template< class T > inline
void MomentInvariant< T >::doit( AimsData< T >& d, T label, int dir )
{
  if (_mom )
    {
      _mom->doit( d, label, dir );
      toComplex();
      toInvariant();
    }
}


template< class T > inline
void MomentInvariant< T >::doit( AimsSurfaceTriangle& surfTri )
{
  if ( _mom )
    {
      _mom->doit( surfTri );
      toComplex();
      toInvariant();
    }
}


template< class T > inline
void MomentInvariant< T >::doit( Moment< T > *m )
{
  if ( m )
    {
      _mom = m;
      toComplex();
      toInvariant();
    }
}


template< class T > inline
void MomentInvariant< T >::toComplex()
{
  _n0 = std::complex< double >( _c00 * ( _mom->m2()[ 0 ] + _mom->m2()[ 1 ] +
                                    _mom->m2()[ 2 ] ) , 0.0 );
				    
  _n1[ 0 ] = std::complex< double >( _c11 * ( _mom->m3()[ 0 ] + _mom->m3()[ 5 ] +
                                         _mom->m3()[ 7 ] ),
                                -_c11 * ( _mom->m3()[ 1 ] + _mom->m3()[ 3 ] + 
				          _mom->m3()[ 8 ] ) );
  _n1[ 1 ] = std::complex< double >( _c10 * ( _mom->m3()[ 2 ] + _mom->m3()[ 4 ] +
                                         _mom->m3()[ 6 ] ), 0.0 );
  _n1[ 2 ] = std::complex< double >( -_n1[ 0 ].real(), _n1[ 0 ].imag() );
  
  _n2[ 0 ] = std::complex< double >( _c22 * ( _mom->m2()[ 0 ] - _mom->m2()[ 1 ] ), 
                                -2.0 * _c22 * _mom->m2()[ 3 ] );
  _n2[ 1 ] = std::complex< double >( 2.0 * _c22 * _mom->m2()[ 4 ], 
                                -2.0 * _c22 * _mom->m2()[ 5 ] );
  _n2[ 2 ] = std::complex< double >( _c20 * ( 2.0 * _mom->m2()[ 2 ] - 
                                         _mom->m2()[ 0 ] - _mom->m2()[ 1 ] ), 
			        0.0 );
  _n2[ 3 ] = std::complex< double >( -_n2[ 1 ].real(), _n2[ 1 ].imag() );
  _n2[ 4 ] = conj( _n2[ 0 ] );
  
  _n3[ 0 ] = std::complex< double >( _c33 * ( _mom->m3()[ 0 ] - 
                                         3.0 * _mom->m3()[ 5 ] ), 
			        _c33 * ( _mom->m3()[ 1 ] - 
                                         3.0 * _mom->m3()[ 3 ] ) );
  _n3[ 1 ] = std::complex< double >( _c32 * ( _mom->m3()[ 4 ] - _mom->m3()[ 6 ] ),
                                -2.0 * _c32 * _mom->m3()[ 9 ] );
  _n3[ 2 ] = std::complex< double >( -_c31 * ( _mom->m3()[ 0 ] + _mom->m3()[ 5 ] - 
                                          4.0 * _mom->m3()[ 7 ] ), 
			        _c31 * ( _mom->m3()[ 1 ] + _mom->m3()[ 3 ] - 
                                          4.0 * _mom->m3()[ 8 ] ) );
  _n3[ 3 ] = std::complex< double >( _c30 * ( 2.0 * _mom->m3()[ 2 ] - 
                                         3.0 * ( _mom->m3()[ 4 ] + 
					         _mom->m3()[ 6 ] ) ), 0.0 );
  _n3[ 4 ] = std::complex< double >( -_n3[ 2 ].real(), _n3[ 2 ].imag() );
  _n3[ 5 ] = conj( _n3[ 1 ] );
  _n3[ 6 ] = std::complex< double >( -_n3[ 0 ].real(), _n3[ 0 ].imag() );
}


template< class T > inline
void MomentInvariant< T >::toInvariant()
{
  double mo200 = _mom->m2()[ 0 ];
  double mo020 = _mom->m2()[ 1 ];
  double mo002 = _mom->m2()[ 2 ];
  double mo110 = _mom->m2()[ 3 ];
  double mo101 = _mom->m2()[ 4 ];
  double mo011 = _mom->m2()[ 5 ];
  
  double mo300 = _mom->m3()[ 0 ];
  double mo030 = _mom->m3()[ 1 ];
  double mo003 = _mom->m3()[ 2 ];
  double mo210 = _mom->m3()[ 3 ];
  double mo201 = _mom->m3()[ 4 ];
  double mo120 = _mom->m3()[ 5 ];
  double mo021 = _mom->m3()[ 6 ];
  double mo102 = _mom->m3()[ 7 ];
  double mo012 = _mom->m3()[ 8 ];
  double mo111 = _mom->m3()[ 9 ];
  
  double m200_2 = mo200 * mo200;
  double m020_2 = mo020 * mo020;
  double m002_2 = mo002 * mo002;
  double m110_2 = mo110 * mo110;
  double m101_2 = mo101 * mo101;
  double m011_2 = mo011 * mo011;
  
  double m200m020 = mo200 * mo020;
  double m200m002 = mo200 * mo002;
  double m020m002 = mo020 * mo002;

  double m200_3 = m200_2 * mo200;
  double m020_3 = m020_2 * mo020;
  double m002_3 = m002_2 * mo002;
  double m200_2m020 = m200_2 * mo020;
  double m200_2m002 = m200_2 * mo002;
  double m020_2m200 = m020_2 * mo200;
  double m020_2m002 = m020_2 * mo002;
  double m002_2m200 = m002_2 * mo200;
  double m002_2m020 = m002_2 * mo020;
  double m200m110_2 = mo200 * m110_2; 
  double m200m101_2 = mo200 * m101_2;
  double m200m011_2 = mo200 * m011_2;
  double m020m110_2 = mo020 * m110_2;
  double m020m101_2 = mo020 * m101_2;
  double m020m011_2 = mo020 * m011_2;
  double m002m110_2 = mo002 * m110_2;
  double m002m101_2 = mo002 * m101_2;
  double m002m011_2 = mo002 * m011_2;
  double m200m020m002 = m200m020 * mo002;
  double m110m101m011 = mo110 * mo101 * mo011;
  
  double m300_2 = mo300 * mo300;
  double m030_2 = mo030 * mo030;
  double m003_2 = mo003 * mo003;
  double m210_2 = mo210 * mo210;
  double m201_2 = mo201 * mo201;
  double m120_2 = mo120 * mo120;
  double m021_2 = mo021 * mo021;
  double m102_2 = mo102 * mo102;
  double m012_2 = mo012 * mo012;
  double m111_2 = mo111 * mo111;
  
  double m300m120 = mo300 * mo120; 
  double m300m102 = mo300 * mo102;
  double m030m210 = mo030 * mo210;
  double m030m012 = mo030 * mo012;
  double m003m201 = mo003 * mo201;
  double m003m021 = mo003 * mo021;
  double m120m102 = mo120 * mo102;
  double m210m012 = mo210 * mo012;
  double m201m021 = mo201 * mo021;  

  double m0 = _mom->m0();
  double d1 = pow( m0, 5.0 / 3.0 );
  double d2 = pow( m0, 10.0 / 3.0 );
  double d3 = pow( m0, 5.0 );
  double d4 = pow( m0, 4.0 );
  double d5 = pow( m0, 17.0 / 3.0 );
  double d6 = pow( m0, 8.0 );
  
  // Moment invariant of order 2 derived from algebraic calculus
  double I0 = _c00 * ( mo200 + mo020 + mo002 ) / d1;
  double aI0 = fabs( I0 );
  _inv[ 0 ] = I0 * sqrt( aI0 ) / aI0;
  
  double I1 = _cc22 * ( m200_2 + m020_2 + m002_2 + 
                        3.0 * ( m110_2 + m101_2 + m011_2 ) - 
			m200m020 - m200m002 - m020m002 ) / d2;
  double aI1 = fabs( I1 );
  _inv[ 1 ] = I1 * pow( aI1, 0.25 ) / aI1;
  
  double I2 = _cc222 * ( -2.0 * ( m200_3 + m020_3 + m002_3 ) + 
                         3 * ( m200_2m002 + m200_2m020 + m020_2m200 + 
			       m020_2m002 + m002_2m200 + m002_2m020 ) - 
			 9.0 * ( m200m110_2 + m200m101_2 + m020m110_2 + 
			         m020m011_2 + m002m101_2 + m002m011_2 ) + 
		         18.0 * ( m200m011_2 + m020m101_2 + m002m110_2 ) -
                         12.0 * m200m020m002 - 54.0 * m110m101m011 ) / d3;
  double aI2 = fabs( I2 );
  _inv[ 2 ] = I2 * pow( aI2, 1.0 / 6.0 ) / aI2;
  
  // Moment invariant of order 3 derived from quantum mechanic calculus
  double I3 = _cc11 * ( m300_2 + m030_2 + m003_2 + m210_2 + m201_2 + m120_2 + 
                        m021_2 + m102_2 + m012_2 + 
			2.0 * ( m300m120 + m300m102 + m030m210 + m030m012 +
                                m003m201 + m003m021 + m210m012 + m201m021 + 
				m120m102 ) ) / d4;
  double aI3 = fabs( I3 );
  _inv[ 3 ] = I3 * pow( aI3, 1.0 / 6.0 ) / aI3;

  double I4 = _cc33 * ( m300_2 + m030_2 + m003_2 + 
                        6.0 * ( m210_2 + m201_2 + m120_2 + m021_2 + m102_2 + 
			        m012_2 ) + 15.0 * m111_2 - 
			3.0 * ( m300m120 + m300m102 + m030m210 + m030m012 + 
			        m003m201 + m003m021 + m210m012 + m201m021 + 
				m120m102 ) ) / d4;
  double aI4 = fabs( I4 );
  _inv[ 4 ] = I4 * pow( aI4, 1.0 / 6.0 ) / aI4;

  std::complex< double > zeta[ 5 ];
  zeta[ 0 ] = _p1 * _n3[ 0 ] * _n3[ 4 ] + _p2 * _n3[ 1 ] * _n3[ 3 ] + 
              _p3 * _n3[ 2 ] * _n3[ 2 ];
  zeta[ 1 ] = _p4 * _n3[ 0 ] * _n3[ 5 ] + _p5 * _n3[ 1 ] * _n3[ 4 ] + 
              _p6 * _n3[ 2 ] * _n3[ 3 ];
  zeta[ 2 ] = _p4 * _n3[ 0 ] * _n3[ 6 ] + _p7 * _n3[ 2 ] * _n3[ 4 ] + 
              _p8 * _n3[ 3 ] * _n3[ 3 ];
  zeta[ 3 ] = _p4 * _n3[ 1 ] * _n3[ 6 ] + _p5 * _n3[ 2 ] * _n3[ 5 ] + 
              _p6 * _n3[ 3 ] * _n3[ 4 ];
  zeta[ 4 ] = _p1 * _n3[ 2 ] * _n3[ 6 ] + _p2 * _n3[ 3 ] * _n3[ 5 ] + 
              _p3 * _n3[ 4 ] * _n3[ 4 ];
	      
  std::complex< double > xi[ 5 ];
  xi[ 0 ] = _q2 * _n3[ 2 ] * _n1[ 0 ] + _q1 * _n3[ 1 ] * _n1[ 1 ] -
            _p5 * _n3[ 0 ] * _n1[ 2 ];
  xi[ 1 ] = _q4 * _n3[ 3 ] * _n1[ 0 ] + _q5 * _n3[ 2 ] * _n1[ 1 ] +
            _p1 * _n3[ 1 ] * _n1[ 2 ];
  xi[ 2 ] = _p3 * _n3[ 4 ] * _n1[ 0 ] + _p7 * _n3[ 3 ] * _n1[ 1 ] +
            _p3 * _n3[ 2 ] * _n1[ 2 ];
  xi[ 3 ] = _p1 * _n3[ 5 ] * _n1[ 0 ] + _q5 * _n3[ 4 ] * _n1[ 1 ] +
            _q4 * _n3[ 3 ] * _n1[ 2 ];
  xi[ 4 ] = -_p5 * _n3[ 6 ] * _n1[ 0 ] + _q1 * _n3[ 5 ] * _n1[ 1 ] +
            _q2 * _n3[ 4 ] * _n1[ 2 ];
	    
  std::complex< double > nu[ 5 ];
  nu[ 0 ] = _n1[ 0 ] * _n1[ 0 ];
  nu[ 1 ] = _r1 * _n1[ 0 ] * _n1[ 1 ];
  nu[ 2 ] = _r2 * ( _n1[ 0 ] * _n1[ 2 ] + _n1[ 1 ] * _n1[ 1 ] );
  nu[ 3 ] = _r1 * _n1[ 1 ] * _n1[ 2 ];
  nu[ 4 ] = _n1[ 2 ] * _n1[ 2 ];
  
  std::complex< double > dI5 = xi[ 4 ] * nu[ 0 ] - xi[ 3 ] * nu[ 1 ] + 
                          xi[ 2 ] * nu[ 2 ] - xi[ 1 ] * nu[ 3 ] +
			  xi[ 0 ] * nu[ 4 ];
  double I5 = _ce * dI5.real() / d6;
  double aI5 = fabs( I5 );
  _inv[ 5 ] = I5 * pow( aI5, 1.0 / 12.0 ) / aI5;
  
  std::complex< double > dI6 = _de * xi[ 0 ] * xi[ 4 ] - _de * xi[ 1 ] * xi[ 3 ] +
                          xi[ 2 ] * xi[ 2 ];
  double I6 = _ce * dI6.real() / d6;
  double aI6 = fabs( I6 );
  _inv[ 6 ] = I6 * pow( aI6, 1.0 / 12.0 ) / aI6;
  
  std::complex< double > dI7 = zeta[ 4 ] * xi[ 0 ] - zeta[ 3 ] * xi[ 1 ] +
                          zeta[ 2 ] * xi[ 2 ] - zeta[ 1 ] * xi[ 3 ] +
			  zeta[ 0 ] * xi[ 4 ];
  double I7 = _ce * dI7.real() / d6;
  double aI7 = fabs( I7 );
  _inv[ 7 ] = I7 * pow( aI7, 1.0 / 12.0 ) / aI7;
  
  std::complex< double > dI8 = _de * zeta[ 0 ] * zeta[ 4 ] - 
                          _de * zeta[ 1 ] * zeta[ 3 ] + zeta[ 2 ] * zeta[ 2 ];
  double I8 = _ce * dI8.real() / d6;
  double aI8 = fabs( I8 );
  _inv[ 8 ] = I8 * pow( aI8, 1.0 / 12.0 ) / aI8;
  
  std::complex< double > dI9 = nu[ 4 ] * _n2[ 0 ] - nu[ 3 ] * _n2[ 1 ] +
                          nu[ 2 ] * _n2[ 2 ] - nu[ 1 ] * _n2[ 3 ] +
			  nu[ 0 ] * _n2[ 4 ];
  double I9 = _ce * dI9.real() / d5;
  double aI9 = fabs( I9 );
  _inv[ 9 ] = I9 * pow( aI9, 0.125 ) / aI9;
  
  std::complex< double > dI10 = xi[ 4 ] * _n2[ 0 ] - xi[ 3 ] * _n2[ 1 ] +
                           xi[ 2 ] * _n2[ 2 ] - xi[ 1 ] * _n2[ 3 ] +
			   xi[ 0 ] * _n2[ 4 ];
  double I10 = _ce * dI10.real() / d5;
  double aI10 = fabs( I10 );
  _inv[ 10 ] = I10 * pow( aI10, 0.125 ) / aI10;
  
  std::complex< double > dI11 = zeta[ 4 ] * _n2[ 0 ] - zeta[ 3 ] * _n2[ 1 ] +
                           zeta[ 2 ] * _n2[ 2 ] - zeta[ 1 ] * _n2[ 3 ] +
			   zeta[ 0 ] * _n2[ 4 ];
  double I11 = _ce * dI11.real() / d5;
  double aI11 = fabs( I11 );
  _inv[ 11 ] = I11 * pow( aI11, 0.125 ) / aI11;
}


template< class T > inline
std::ostream& operator << ( std::ostream& out, 
			    const MomentInvariant< T >& mom )
{
  for ( int i=0; i<12; i++ )  out << mom.invariant()[ i ] << " ";

  return out;
}

#endif
