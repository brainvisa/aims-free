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


#ifndef AIMS_OPTIMIZATION_LM2GAUSS_H
#define AIMS_OPTIMIZATION_LM2GAUSS_H

#include <aims/optimization/lmfunc.h>


template < class T >
class LM2Gaussian : public LMFunction< T >
{
public:

  LM2Gaussian( T k1=(T)1.0, T m1=(T)0.0, T s1=(T)1.0, 
	       T k2=(T)1.0, T m2=(T)0.0, T s2=(T)1.0 );

  T apply( T );
  T eval( T );
};


template< class T > inline
LM2Gaussian< T >::LM2Gaussian( T k1, T m1, T s1, T k2, T m2, T s2 ) 
  : LMFunction< T >()
{
  this->par.push_back( k1 );
  this->par.push_back( m1 );
  this->par.push_back( s1 );
  this->par.push_back( k2 );
  this->par.push_back( m2 );
  this->par.push_back( s2 );

  this->der = std::vector< T >( 6 );
}



template< class T > inline
T LM2Gaussian< T >::eval( T x )
{
  T y = (T)0;

  for ( int i=0; i<6; i+=3 ) 
    {
      T arg = ( x - this->par[ i + 1 ] ) / this->par[ i + 2 ];
      T ex = (T)exp( -1.0 * ( arg * arg ) );
      T fac = (T)2 * this->par[ i ] * ex * arg;

      y += this->par[ i ] * ex;

      this->der[ i ] = ex;
      this->der[ i + 1 ] = fac / this->par[ i + 2 ];
      this->der[ i + 2 ] = fac * arg / this->par[ i + 2 ];
    }

  return y;
}


template< class T > inline
T LM2Gaussian< T >::apply( T x )
{
  T y = (T)0;

  for ( int i=0; i<6; i+=3 ) 
    {
      T arg = ( x - this->par[ i + 1 ] ) / this->par[ i + 2 ];

      y += this->par[ i ] * (T)exp( -1.0 * ( arg * arg ) );
    }

  return y;
}

#endif
