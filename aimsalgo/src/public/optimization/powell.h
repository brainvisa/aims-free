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


#ifndef AIMS_OPTIMIZATION_POWELL_H
#define AIMS_OPTIMIZATION_POWELL_H

#include <aims/optimization/optimizer.h>
#include <aims/optimization/objective.h>
#include <aims/vector/vector.h>
#include <algorithm>
#include <list>
#include <math.h>
#include <assert.h>


#define GOLD   1.618034
#define GLIMIT 100.0
#define TINY   1.0e-20
#define SIGN( a, b ) ( (b) >= 0.0 ? fabs( a ) : -fabs( a ) )
#define ITMAX 100
#define TOL 2.0e-4  // tolerance passed to brent
#define ZEPS 1.0e-10
#define CGOLD 0.3819660
#define SQR( a ) ( ( a ) * ( a ) )


//
// class PowellOptimizer
//
template < class T, int D >
class PowellOptimizer : public Optimizer< T, D >
{

  public:
    PowellOptimizer( const ObjectiveFunc< T, D >& func, T error );
    virtual ~PowellOptimizer();

    AimsVector< T, D > doit( const AimsVector< T, D > & pinit,
                         const AimsVector< T, D > & deltaP );

    const AimsVector< T, D >& getDirection( int which ) const;
    const T& getOptCost() const;
    const std::list< T >& getCostEvolution() const;

  private:

    void linmin( AimsVector< T, D >& p, AimsVector< T, D >& xi );
    void mnbrak( T& ax, T& bx, T& cx, T& fa, T& fb, T& fc );
    T brent( T ax, T bx, T cx, T& fb, T& xmin );

    AimsVector< T, D >* _xi;
    T _optCost;
    std::list< T > _costs;
    T _fret;
    AimsVector< T, D > _pcom;
    AimsVector< T, D > _xicom;

};


template < class T, int D >
inline
PowellOptimizer< T, D >::PowellOptimizer( const ObjectiveFunc< T, D >& func,
                                          T error )
  : Optimizer< T, D >( func, error )
{

  _xi = new AimsVector< T, D >[ D ];

}


template < class T, int D >
inline
PowellOptimizer< T, D >::~PowellOptimizer()
{

  delete [] _xi;

}


template < class T, int D >
inline
AimsVector< T, D > 
PowellOptimizer< T, D >::doit( const AimsVector< T, D > & pinit,
			       const AimsVector< T, D > & deltaP )
{

  AimsVector< T, D > p( pinit ), pt, ptt, xit;
  T fp, del, fptt, t; 
  int ibig;

  _optCost = ( T )0;
  _costs.clear();

  for ( int i = 0; i < D; i++ )
    for ( int j = 0; j < D; j++ )
      _xi[ i ][ j ] = ( i == j ? deltaP[ i ] : ( T )0 );

  _fret = this->_func.eval( p );
  
  carto::AttributedObject monitoredParams("monitored_params") ;
  this->_func.fillMonitoredParameters( monitoredParams ) ;
  monitoredParams.setProperty("info", std::string( "first evaluation") ) ;
  this->_probe->test( p, monitoredParams, &_fret) ;
  pt = p;

  for ( int iter = 0; ; iter++ )
  {

    fp = _fret;
    ibig = 0;
    del = ( T )0;
    for ( int i = 0; i < D; i++ )
    {

      xit = _xi[ i ];
      fptt = _fret;
      linmin( p, xit );
      if ( ( T )fabs( fptt - _fret ) > del )
      {

        del = ( T )fabs( fptt - _fret );
        ibig = i;

      }

    }

    this->_func.fillMonitoredParameters( monitoredParams ) ;
    monitoredParams.setProperty("info", std::string( "iteration step") ) ;
    this->_probe->iteration( p, monitoredParams, &_fret) ;
    
    
    if ( 2.0 * fabs( fp - _fret ) <= this->_error * ( fabs( fp ) + fabs( _fret ) ) )
    {

      _optCost = _fret;
      this->_probe->end() ;
      return p;

    }
    assert( iter != 200 );  // iteration maxi
    ptt = p * ( T )2 - pt;
    xit = p - pt;
    pt = p;
    
    fptt = this->_func.eval( ptt );
    this->_func.fillMonitoredParameters( monitoredParams ) ;
    monitoredParams.setProperty( "info", 
			 std::string( "conjugate directions investigation") );
    this->_probe->test( ptt, monitoredParams, &_fret) ;
    if ( fptt < fp )
    {
      t = ( T )( 2.0 * ( fp - 2.0 * _fret + fptt ) * SQR( fp - _fret - del )
	       - del * SQR( fp - fptt ) );
      if ( t < ( T )0 )
      {

         _fret = this->_func.eval( p );
	 this->_func.fillMonitoredParameters( monitoredParams ) ;
	 monitoredParams.setProperty( "info", 
				       std::string( "conjugate directions computing") ) ;
	 this->_probe->test( p, monitoredParams, &_fret) ;
         linmin( p, xit );
         _xi[ ibig ] = _xi[ D - 1 ];
         _xi[ D - 1 ] = xit;

      }

    }
    _costs.push_back( _fret );
  }

  this->_probe->end() ;

  return p;

}


template < class T, int D >
inline
const AimsVector< T, D >& 
PowellOptimizer< T, D >::getDirection( int which ) const
{

  return _xi[ which ];

}


template < class T, int D >
inline
const T& PowellOptimizer< T, D >::getOptCost() const
{

  return _optCost;

}


template < class T, int D >
inline
const std::list< T >& PowellOptimizer< T, D >::getCostEvolution() const
{

  return _costs;

}


template < class T, int D >
inline
void PowellOptimizer< T, D >::linmin( AimsVector<T,D>& p, AimsVector<T,D>& xi )
{

  T xx, xmin, fx, fb, fa, bx, ax;

  _pcom = p;
  _xicom = xi;
  ax = ( T )0;
  xx = ( T )1;
  fa = _fret;
  mnbrak( ax, xx, bx, fa, fx, fb );
  _fret = brent( ax, xx, bx, fx, xmin );
  xi *= xmin;
  p += xi;

}


template < class T, int D >
inline
void PowellOptimizer< T, D >::mnbrak( T& ax, T& bx, T& cx, T& fa, T& fb, T& fc )
{

  T ulim, u, r, q, fu, tmp;

  fb = this->_func.eval( _pcom + _xicom * bx );

  carto::AttributedObject monitoredParams("monitored_params") ;
  this->_func.fillMonitoredParameters( monitoredParams ) ;
  monitoredParams.setProperty("info", std::string( "mnbrak_bx") ) ;
  this->_probe->test( _pcom + _xicom * bx, monitoredParams, &fb) ;

  if ( fb > fa )
  {

    tmp = ax;
    ax = bx;
    bx = tmp;
    tmp = fb;
    fb = fa;
    fa = tmp;

  }
  cx = bx + ( T )GOLD * ( bx - ax );
  fc = this->_func.eval( _pcom + _xicom * cx );

  this->_func.fillMonitoredParameters( monitoredParams ) ;
  monitoredParams.setProperty("info", std::string( "mnbrak_cx") ) ;
  this->_probe->test( _pcom + _xicom * cx, monitoredParams, &fc) ;


  while ( fb > fc )
  {

    r = ( bx - ax ) * ( fb - fc );
    q = ( bx - cx ) * ( fb - fa );
    u = bx - ( ( bx - cx ) * q - ( bx - ax ) * r ) /
             ( 2.0 * SIGN( std::max( ( T )fabs( q - r ), ( T )TINY ) , 
			   q - r ) );
    ulim = bx + ( T )GLIMIT * ( cx - bx );
    if ( ( bx - u ) * ( u - cx ) > ( T )0 )
    {

      fu = this->_func.eval( _pcom + _xicom * u );

      this->_func.fillMonitoredParameters( monitoredParams ) ;
      monitoredParams.setProperty("info", std::string( "mnbrak_u") ) ;
      this->_probe->test( _pcom + _xicom * u, monitoredParams, &fu) ;

      if ( fu < fc )
      {

        ax = bx;
        bx = u;
        fa = fb;
        fb = fu;
        return;

      }
      else if ( fu > fb )
      {

        cx = u;
        fc = fu;
        return;

      }
      u = cx + ( T )GOLD * ( cx - bx );
      fu = this->_func.eval( _pcom + _xicom * u );

      this->_func.fillMonitoredParameters( monitoredParams ) ;
      monitoredParams.setProperty("info", std::string( "mnbrak_u") ) ;
      this->_probe->test( _pcom + _xicom * u, monitoredParams, &fu) ;

    }
    else if ( ( cx - u ) * ( u - ulim ) > ( T )0 )
    {

      fu = this->_func.eval( _pcom + _xicom * u );

      this->_func.fillMonitoredParameters( monitoredParams ) ;
      monitoredParams.setProperty("info", std::string( "mnbrak_u") ) ;
      this->_probe->test( _pcom + _xicom * u, monitoredParams, &fu) ;

      if ( fu < fc )
      {

        bx = cx;
        cx = u;
        u = cx + ( T )GOLD * ( cx - bx );
        fb = fc;
        fc = fu;
        fu = this->_func.eval( _pcom + _xicom * u );

      }

    }
    else if ( ( u - ulim ) * ( ulim - cx ) >= ( T )0 )
    {

      u = ulim;
      fu = this->_func.eval( _pcom + _xicom * u );

      this->_func.fillMonitoredParameters( monitoredParams ) ;
      monitoredParams.setProperty("info", std::string( "mnbrak_u") ) ;
      this->_probe->test( _pcom + _xicom * u, monitoredParams, &fu) ;

    }
    else
    {

      u = cx + ( T )GOLD * ( cx - bx );
      fu = this->_func.eval( _pcom + _xicom * u );

      this->_func.fillMonitoredParameters( monitoredParams ) ;
      monitoredParams.setProperty("info", std::string( "mnbrak_u") ) ;
      this->_probe->test( _pcom + _xicom * u, monitoredParams, &fu) ;
      

    }
    ax = bx;
    bx = cx;
    cx = u;
    fa = fb;
    fb = fc;
    fc = fu;

  }

}


template < class T, int D >
inline
T PowellOptimizer< T, D >::brent( T ax, T bx, T cx, T& fb, T& xmin )
{

  T a, b, d, etemp, fu, fv, fw, fx, p, q, r, tol1, tol2, u, v, w, x, xm, e;

  d = ( T )0;
  e = ( T )0;
  a = ( ax < cx ? ax : cx );
  b = ( ax > cx ? ax : cx );
  x = w = v = bx;
  fw = fv = fx = fb;
  for ( int iter = 0; iter < ITMAX; iter++ )
  {

    xm = ( T )( 0.5 * ( a + b ) );
    tol1 = ( T )( TOL * fabs( x ) + ZEPS );
    tol2 = ( T )2 * tol1;
    if ( fabs( x - xm ) <= tol2 - 0.5 * ( b - a ) )
    {

      xmin = x;
      return fx;

    }
    if ( fabs( e ) > tol1 )
    {

      r = ( x - w ) * ( fx - fv );
      q = ( x - v ) * ( fx - fw );
      p = ( x - v ) * q - ( x - w ) * r;
      q = ( T )( 2.0 * ( q - r ) );
      if ( q > ( T )0 )
        p = -p;
      q = ( T )fabs( q );
      etemp = e;
      e = d;
      if ( fabs( p ) >= fabs( 0.5 * q * etemp ) ||
           p <= q * ( a - x ) || p >= q * ( b - x ) )
      {

        e = ( x >= xm ? a - x : b - x );      
        d =( T )CGOLD * e;

      }
      else
      {

        d = p / q;
        u = x + d;
        if ( u - a < tol2 || b - u < tol2 )
          d = SIGN( tol1, xm - x );

      } 

    }
    else
    {

      e = ( x >= xm ? a - x : b - x );
      d = ( T )CGOLD * e;

    }
    u = ( T )( fabs( d ) >= tol1 ? x + d : x + SIGN( tol1, d ) );
    fu = this->_func.eval( _pcom + _xicom * u );

    carto::AttributedObject monitoredParams("monitored_params") ;
    this->_func.fillMonitoredParameters( monitoredParams ) ;
    monitoredParams.setProperty("info", 
                                std::string( "mnbrak_bx") ) ;
    this->_probe->test( _pcom + _xicom * u, monitoredParams, &fu) ;

    if ( fu <= fx )
    {

      if ( u >= x )
        a = x;
      else
        b = x;
      v = w;
      w = x;
      x = u;
      fv = fw;
      fw = fx;
      fx = fu;

    }
    else
    {

      if ( u < x )
        a = u;
      else
        b = u;
      if ( fu <= fw || w == x )
      {

        v = w;
        w = u;
        fv = fw;
        fw = fu;

      }
      else if ( fu <= fv || v == x || v == w )
      {

        v = u;
        fv = fu;

      }

    }

  }
  std::cerr << "brent: too many iterations" << std::endl;
  xmin = x;

  return ( T )-1;
}


#endif
