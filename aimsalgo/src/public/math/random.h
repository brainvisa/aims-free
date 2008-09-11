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


#ifndef AIMS_MATH_RANDOM_H
#define AIMS_MATH_RANDOM_H

#include <aims/config/aimsalgopub_config.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <complex>

/**@name Basic random generators*/ 
//@{
/// Uniform distribution between 0.0 and 1.0
AIMSALGOPUB_API double UniformRandom();
/// Normal gaussian distribution with mean = 0.0 and sigma = 1.0
AIMSALGOPUB_API double NormalRandom();
//@}

/**@name Uniform random generator*/
//@{
/// generate uniform random number between min and max
template <class T>
T UniformRandom(const T &min,const T &max);
///
AIMSALGOPUB_API 
std::complex<float>  UniformRandom(const std::complex<float>  &min,
				   const std::complex<float>  &max);
///
AIMSALGOPUB_API 
std::complex<double> UniformRandom(const std::complex<double> &min,
				   const std::complex<double> &max);
//@}


/**@name Gaussian random generator*/
//@{
/// generate gaussian random number with mean,sigma
template <class T>
T GaussianRandom(const T &mean,const T &sigma);
///
AIMSALGOPUB_API 
std::complex<float>  GaussianRandom(const std::complex<float>  &mean,
				    const std::complex<float>  &sigma);
///
AIMSALGOPUB_API 
std::complex<double> GaussianRandom(const std::complex<double> &mean,
				    const std::complex<double> &sigma);
//@}


template <class T> class AimsData;

/**@name Random functions*/
//@{
/// generate a random list of int between 0 and (size-1)
AIMSALGOPUB_API AimsData<int> AimsRandomList( int size );
//@}

template <class T> inline
T UniformRandom(const T &min,const T &max)
{ return((T) ((double)min+(double)(max-min)*(double)UniformRandom()));
}


inline
short UniformRandom( const short& min, const short& max )
{
  return min + short( UniformRandom() * (max-min+0.99) );
}


inline
unsigned short
UniformRandom( const unsigned short& min, const unsigned short& max )
{
  return min + (unsigned short)( UniformRandom() * (max-min+0.99) );
}


inline
int UniformRandom( const int& min, const int& max )
{
  return min + int( UniformRandom() * (max-min+0.99) );
}


inline
unsigned int
UniformRandom( const unsigned int& min, const unsigned int& max )
{
  return min + (unsigned int)( UniformRandom() * (max-min+0.99) );
}


inline
long UniformRandom( const long& min, const long& max )
{
  return min + long( UniformRandom() * (max-min+0.99) );
}


inline
unsigned long
UniformRandom( const unsigned long& min, const unsigned long& max )
{
  return min + (unsigned long)( UniformRandom() * (max-min+0.99) );
}


template <class T> inline
T GaussianRandom(const T &mean,const T &sigma)
{ return((T)((double)mean + (double)sigma * NormalRandom()));
}





/// Nombres aleatoires sous forme de classe.  F. Poupon le 03/08/2000


/** @name Uniform random numbers */
template< class T >
class UniformDeviates
{
public:

  /** Constructor and destructors */
  //@{
  /// constructor
  UniformDeviates() { }
  /// destructor
  virtual ~UniformDeviates() { }
  //@}

  /** 'Minimal' random number generator of Park and Miller. \\
      This function is adapted from the Numerical Recipes in C. \\
      Returns a uniform random deviate between 0.0 and 1.0. Set or
      reset 'idum' to any integer value to initialize the sequence. \\
      @param idum must not be altered between calls for successive
      deviates in a sequence.
   */
  T ran0( int *idum=NULL );

  /** 'Minimal' random number generator of Park and Miller with
      Bays-Durham shuffle and added safeguards. \\
      This function is adapted from the Numerical Recipes in C. \\
      Returns a uniform random deviate between 0.0 and 1.0 (exclusive of
      the endpoint values). Call with 'idum' a negative integer to initialize;
      thereafter, do not alter 'idum' between successive deviates in a
      sequence.
   */
  T ran1( int *idum=NULL );

  /** Long period random number generator of L'Ecuyer with Bays-Derham
      shuffle and added safeguards. \\
      This function is adapted from the Numerical Recipes in C. \\
      Returns a uniform random deviate between 0.0 and 1.0 (exclusive of
      the endpoint values). Call with 'idum' a negative integer to initialize;
      thereafter, do not alter 'idum' between successive deviates in a
      sequence.
   */
  T ran2( int *idum=NULL );

  /** Long period random number generator of Knuth. \\
      This function is adapted from the Numerical Recipes in C. \\
      Returns a uniform random deviate between 0.0 and 1.0. Set 'idum' to
      any negative value to initialize or reinitialize the sequence.
   */
  T ran3( int *idum=NULL );
};


template< class T > inline
T UniformDeviates< T >::ran0( int *idum )
{
  int k;
  T ans;
  int idumm;

  if ( idum == NULL )
  {
    srand( time( NULL ) );
    idumm = - rand();
  }
  else  idumm = *idum;

  idumm ^= 123459876;
  k = idumm / 127773;
  idumm  = 16807 * ( idumm - k * 127773 ) - 2836 * k;

  if ( idumm < 0 )  idumm += 2147483647;

  ans = (T)( ( 1.0 / 2147483647 ) * idumm );
  idumm ^= 123459876;

  if ( idum )  *idum = idumm;

  return ans;
}


template< class T > inline
T UniformDeviates< T >::ran1( int *idum )
{
  int j;
  int k, idumm;
  static int iy = 0;
  static int iv[ 32 ];
  T temp;

  if ( idum == NULL )
    {
      srand( time( NULL ) );
      idumm = - rand();
    }
  else  idumm = *idum;

  if ( idumm <= 0 || !iy )
    {
      if ( -idumm < 1 )  idumm = 1;
      else idumm = -idumm;

      for ( j=39; j>=0; j-- ) 
	{
	  k = idumm / 127773;
	  idumm = 16807 * ( idumm - k * 127773 ) - 2836 * k;

	  if ( idumm < 0 )  idumm += 2147483647;
	  if ( j < 32 )  iv[ j ] = idumm;
	}

      iy = iv[ 0 ];
    }

  k = idumm / 127773;
  idumm = 16807 * ( idumm - k * 127773 ) - 2836 * k;

  if ( idumm < 0 )  idumm += 2147483647;

  j = iy / ( 1 +  2147483646 / 32 );
  iy = iv[ j ];
  iv[ j ] = idumm;

  if ( idum )  *idum = idumm;

  if ( ( temp = (T)( 1.0 * iy / 2147483647 ) ) > (T)( 1.0 - 1.2e-7 ) ) 
    return (T)( 1.0 - 1.2e-7 );
  else return (T)temp;
}


template< class T > inline
T UniformDeviates< T >::ran2( int *idum )
{
  int j;
  int k, idumm;
  static int idum2 = 123456789;
  static int iy = 0;
  static int iv[ 32 ];
  T temp;

  if ( idum == NULL )
    {
      srand( time( NULL ) );
      idumm = - rand();
    }
  else  idumm = *idum;

  if ( idumm  <= 0 ) 
    {
      if ( -idumm < 1 )  idumm = 1;
      else idumm = -idumm;

      idum2 = idumm;

      for ( j = 39; j>=0; j-- ) 
	{
	  k = idumm / 53668;
	  idumm = 40014 * ( idumm - k * 53668 ) - k * 12211;

	  if ( idumm < 0 ) idumm += 2147483563;
	  if ( j < 32 ) iv[ j ] = idumm;
	}

      iy = iv[ 0 ];
    }

  k = idumm / 53668;
  idumm =  40014 * ( idumm - k * 53668 ) - k * 12211;

  if ( idumm < 0 )  idumm += 2147483563;

  k = idum2 / 52774;
  idum2 = 40692 * ( idum2 - k * 52774 ) - k * 3791;

  if ( idum2 < 0 )  idum2 += 2147483399;

  j = iy / ( 1 + 2147483562 / 32 );
  iy = iv[ j ] - idum2;
  iv[ j ] = idumm;
  if ( iy < 1 )  iy += 2147483562;

  if ( idum )  *idum = idumm;

  if ( ( temp = (T)( 1.0 * iy / 2147483563 ) ) > (T)( 1.0 - 1.2e-7 ) ) 
    return (T)( 1.0 - 1.2e-7 );
  else return (T)temp;
}


template< class T > inline
T UniformDeviates< T >::ran3( int *idum )
{
  static int inext, inextp;
  static int ma[ 56 ];
  static int iff = 0;
  int mj, mk, idumm;
  int i, ii, k;

  if ( idum == NULL )
    {
      srand( time( NULL ) );
      idumm = - rand();
    }
  else  idumm = *idum;

  if ( idumm < 0 || iff == 0 )
    {
      iff = 1;
      mj = 161803398 - ( idumm < 0 ? -idumm : idumm );
      mj %= 1000000000;
      ma[ 55 ] = mj;
      mk = 1;

      for ( i=1; i<=54; i++ ) 
	{
	  ii = ( 21 * i ) % 55;
	  ma[ ii ] = mk;
	  mk = mj - mk;
	  if ( mk < 0 )  mk += 1000000000;
	  mj = ma[ ii ];
	}

      for ( k=1; k<=4; k++ )
	for ( i=1; i<=55; i++ ) 
	  {
	    ma[ i ] -= ma[ 1 + ( i + 30 ) % 55 ];

	    if ( ma[ i ] < 0 )  ma[ i ] += 1000000000;
	  }

      inext = 0;
      inextp = 31;
      idumm = 1;
    }

  if ( ++inext == 56 )  inext = 1;
  if ( ++inextp == 56 )  inextp = 1;

  mj = ma[ inext ] - ma[ inextp ];

  if ( mj < 0 )  mj += 1000000000;

  ma[ inext ] = mj;

  if ( idum )  *idum = idumm;

  return (T)( 1.0 * mj / 1000000000 );
}


/** Exponential random numbers */
template< class T >
class ExponentialDeviates
{
public:

  /** Constructor and destructor */
  //@{
  /// constructor
  ExponentialDeviates() { }
  /// destructor
  virtual ~ExponentialDeviates() { }
  //@}

  /** Exponential random number generator. \\
      This function is adapted from the Numerical Recipes in C. \\
      Returns an exponentially distributed, positive, random deviate of
      unit mean, using method ran1(idum) of UniformDeviates as the source of
      uniform deviates.
  */
  T doit( int *idum=NULL );
};


template< class T > inline
T ExponentialDeviates< T >::doit( int *idum )
{
  T dum;
  UniformDeviates< T > udev;

  do  dum = udev.ran1( idum );
  while ( dum == (T)0 );

  return (T)( -log( (double)dum ) );
}


/** Normal random numbers */
template< class T >
class NormalDeviates
{
public:

  /** Constructors and destructors */
  //@{
  /// constructor
  NormalDeviates() { }
  /// destructor
  virtual ~NormalDeviates() { }
  //@}

  /** Normal random numbers generator. \\
      This function is adapted from the Numerical Recipes in C. \\
      Returns a normally distributed deviate with zero mean and unit variance,
      using method ran1(idum) of UniformDeviates as the source of uniform
      deviates.
   */
  T doit( int *idum=NULL );
};


template< class T > inline
T NormalDeviates< T >::doit( int *idum )
{
  static int iset = 0;
  static T gset;
  T fac, rsq, v1, v2;
  UniformDeviates< T > udev;
 
  if  ( iset == 0 )
    {
      do 
	{
	  v1 = (T)( 2.0 * udev.ran1( idum ) - 1.0 );
	  v2 = (T)( 2.0 * udev.ran1( idum ) - 1.0 );
	  rsq = v1 * v1 + v2 * v2;
	} 
      while ( rsq >= (T)1 || rsq == (T)0 );

      fac = (T)sqrt( -2.0 * log( (double)rsq ) / rsq );
      gset = v1 * fac;
      iset = 1;
      return v2 * fac;
    }
  else
    {
      iset = 0;
      return gset;
    }       
}

#endif
