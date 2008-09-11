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


#ifndef AIMS_OPTIMIZATION_LMGAMMA_H
#define AIMS_OPTIMIZATION_LMGAMMA_H

#include <aims/optimization/lmfunc.h>


// Gamma-variate function:
// f(t) = k * ( t - ta ) ^ a * exp( - ( t - ta ) / b )   si t > ta
// f(t) = 0   sinon
template < class T >
class LMGamma : public LMFunction< T >
{
public:

  LMGamma( T k=(T)1.0, T ta=(T)4.0, T a=(T)3.0, T b=(T)1.5 );

  T apply( T );
  T eval( T );
};


template< class T > inline
LMGamma< T >::LMGamma( T k, T ta, T a, T b ) : LMFunction< T >()
{
  this->par.push_back( k );
  this->par.push_back( ta );
  this->par.push_back( a );
  this->par.push_back( b );

  this->der = std::vector< T >( 4 );
}


template< class T > inline
T LMGamma< T >::eval( T x )
{
  if ( x > this->par[ 1 ] )
    {
      // k 
      double k = (double)this->par[ 0 ];
      // a
      double a = (double)this->par[ 2 ];
      // t - ta
      double tta = (double)( x - this->par[ 1 ] );
      // ln( t - ta )
      double ltta = log( tta );
      // ( t - ta ) / b
      double ttab = tta / this->par[ 3 ];
      // ( t - ta ) ^ a * exp( - ( t - ta ) / b )
      // = exp( a * ln( t - ta ) - ( t - ta ) / b )
      double expf = exp( a * ltta - ttab );

      // f
      double f = k * expf;

      // df / dk
      this->der[ 0 ] = (T)expf;

      // df / dta
      this->der[ 1 ] = (T)( k * ( ttab - a ) * exp( ( a - 1 ) * ltta - ttab ) );

      // df / da
      this->der[ 2 ] = (T)( k * ltta * expf );

      // df / db
      double b2 = (double)( this->par[ 3 ] * this->par[ 3 ] );
      this->der[ 3 ] = (T)( k * exp( ( a + 1 ) * ltta - ttab ) / b2 );

      return (T)f;
    }
  else
    {
      this->der[ 0 ] = this->der[ 1 ] = this->der[ 2 ] = this->der[ 3 ] = (T)0;

      return (T)0;
    }
}


template< class T > inline
T LMGamma< T >::apply( T x )
{
  if ( x > this->par[ 1 ] )
    {
      // t - ta
      double tta = (double)( x - this->par[ 1 ] );
      // ln( t - ta )
      double ltta = log( tta );
      // ( t - ta ) / b
      double ttab = tta / this->par[ 3 ];

      return (T)( this->par[ 0 ] * exp( this->par[ 2 ] * ltta - ttab ) );
    }
  else return (T)0;
}

#endif
