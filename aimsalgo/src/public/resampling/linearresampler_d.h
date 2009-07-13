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


#ifndef AIMS_RESAMPLING_LINEARRESAMPLER_D_H
#define AIMS_RESAMPLING_LINEARRESAMPLER_D_H

#include <cstdlib>
#include <aims/resampling/linearresampler.h>

template < class T >
LinearResampler< T >::LinearResampler()
  : SplineResampler< T >()
{
}


template < class T >
LinearResampler< T >::~LinearResampler()
{
}


template < class T >
int LinearResampler< T >::getOrder() const
{

  return 1;

}


template < class T >
void LinearResampler< T >::doResample(
                                      const AimsData< T >& inVolume,
                                      const Motion& invTransform3d,
                                      const T& outBackground,
                                      const Point3df& outLocation,
                                      T& outValue, int t )
{

  const T	*i = &inVolume( 0, 0, 0, t );
  const T	*pi, *pj;

  Point3df normalizedInLocation;
  normalizedInLocation = invTransform3d.transform( outLocation );

  normalizedInLocation[0] += 0.5;
  normalizedInLocation[1] += 0.5;
  normalizedInLocation[2] += 0.5;

  int x = ( int )normalizedInLocation[0];
  int y = ( int )normalizedInLocation[1];
  int z = ( int )normalizedInLocation[2];

  if ( ( normalizedInLocation[0] < 0.0 ) &&
       ( ( double )x != normalizedInLocation[0] ) )
  {

    -- x;

  }
  if ( ( normalizedInLocation[1] < 0.0 ) &&
       ( ( double )y != normalizedInLocation[1] ) )
  {

    -- y;

  }
  if ( ( normalizedInLocation[2] < 0.0 ) &&
       ( ( double )z != normalizedInLocation[2] ) )
  {

    -- z;

  }

  if ( ( x >= 0 ) && ( x < inVolume.dimX() ) &&
       ( y >= 0 ) && ( y < inVolume.dimY() ) &&
       ( z >= 0 ) && ( z < inVolume.dimZ() ) )
  {

    double weightX0, weightY0, weightX1, weightY1;
    int foldX0, foldY0, foldX1, foldY1;
    double intensity, qi, qj;

    // first y contribution
    normalizedInLocation[1] -= 0.5;
    y = ( int )normalizedInLocation[1];
    if ( ( normalizedInLocation[1] < 0.0 ) &&
         ( ( double )y != normalizedInLocation[1] ) )
    {

      -- y;

    }
    weightY0 = getBSplineWeight( y, normalizedInLocation[1] );
    foldY0 = getFold( y, inVolume.dimY() ) * inVolume.dimX();

    // second y contribution
    ++ y;
    weightY1 = getBSplineWeight( y, normalizedInLocation[1] );
    foldY1 = getFold( y, inVolume.dimY() ) * inVolume.dimX();

    // first x contribution
    normalizedInLocation[0] -= 0.5;
    x = ( int )normalizedInLocation[0];
    if ( ( normalizedInLocation[0] < 0.0 ) &&
         ( ( double )x != normalizedInLocation[0] ) )
    {

      -- x;

    }
    weightX0 = getBSplineWeight( x, normalizedInLocation[0] );
    foldX0 = getFold( x, inVolume.dimX() );

    // second x contribution
    ++ x;
    weightX1 = getBSplineWeight( x, normalizedInLocation[0] );
    foldX1 = getFold( x, inVolume.dimX() );

    if ( inVolume.dimZ() == 1 )
    {

      //summing contributions
      pj = i;
      pi = pj + foldY0;
      qi = weightX0 * ( double )*( pi + foldX0 );
      qi += weightX1 * ( double )*( pi + foldX1 );
      qj = weightY0 * qi;
      pi = pj + foldY1;
      qi = weightX0 * ( double )*( pi + foldX0 );
      qi += weightX1 * ( double )*( pi + foldX1 );
      intensity = qj + weightY1 * qi;

    }
    else
    {

      // first z contribution
      normalizedInLocation[2] -= 0.5;
      z = ( int )normalizedInLocation[2];
      if ( ( normalizedInLocation[2] < 0.0 ) &&
           ( ( double )z != normalizedInLocation[2] ) )
      {

        -- z;

      }
      pj = i + getFold( z, inVolume.dimZ() ) * inVolume.dimX() *
           inVolume.dimY();
      pi = pj + foldY0;
      qi = weightX0 * ( double )*( pi + foldX0 );
      qi += weightX1 * ( double )*( pi + foldX1 );
      qj = weightY0 * qi;
      pi = pj + foldY1;
      qi = weightX0 * ( double )*( pi + foldX0 );
      qi += weightX1 * ( double )*( pi + foldX1 );
      qj += weightY1 * qi;
      intensity = getBSplineWeight( z, normalizedInLocation[2] ) * qj;

      // first z contribution
      ++ z;
      pj = i + getFold( z, inVolume.dimZ() ) * inVolume.dimX() *
           inVolume.dimY();
      pi = pj + foldY0;
      qi = weightX0 * ( double )*( pi + foldX0 );
      qi += weightX1 * ( double )*( pi + foldX1 );
      qj = weightY0 * qi;
      pi = pj + foldY1;
      qi = weightX0 * ( double )*( pi + foldX0 );
      qi += weightX1 * ( double )*( pi + foldX1 );
      qj += weightY1 * qi;
      intensity += getBSplineWeight( z, normalizedInLocation[2] ) * qj;
    }
    outValue = ( T )intensity;

  }
  else
  {

    outValue = outBackground;

  }

}


template < class T >
double LinearResampler< T >::getBSplineWeight( int i, double x ) const
{

  x = std::fabs( x - ( double )i );
  return ( x > 1.0 ) ? 0.0 : 1.0 - x;

}


#endif

