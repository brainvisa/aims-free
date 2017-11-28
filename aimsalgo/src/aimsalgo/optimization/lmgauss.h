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


#ifndef AIMS_OPTIMIZATION_LMGAUSS_H
#define AIMS_OPTIMIZATION_LMGAUSS_H

#include <aims/optimization/lmfunc.h>


template < class T >
class LMGaussian : public LMFunction< T >
{
public:

  LMGaussian( T k=(T)1.0, T m=(T)0.0, T s=(T)1.0 );

  T apply( T );
  T eval( T );
};


template< class T > inline
LMGaussian< T >::LMGaussian( T k, T m, T s ) : LMFunction< T >()
{
  this->par.push_back( k );
  this->par.push_back( m );
  this->par.push_back( s );

  this->der = std::vector< T >( 3 );
}



template< class T > inline
T LMGaussian< T >::eval( T x )
{
  T arg = ( x - this->par[ 1 ] ) / this->par[ 2 ];
  T ex = (T)exp( -1.0 * (arg * arg ) );
  T fac = (T)2 * this->par[ 0 ] * ex * arg;

  this->der[ 0 ] = ex;
  this->der[ 1 ] = fac / this->par[ 2 ];
  this->der[ 2 ] = fac * arg / this->par[ 2 ];

  return this->par[ 0 ] * ex;
}


template< class T > inline
T LMGaussian< T >::apply( T x )
{
  T arg = ( x - this->par[ 1 ] ) / this->par[ 2 ];

  return this->par[ 0 ] * (T)exp( -1.0 * ( arg * arg ) );
}

#endif
