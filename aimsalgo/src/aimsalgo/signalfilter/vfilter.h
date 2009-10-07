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


#ifndef AIMS_SIGNALFILTER_VFILTER_H
#define AIMS_SIGNALFILTER_VFILTER_H

#include <aims/data/data.h>


/** The template class to perform a V-filter.
*/
template <class T>
class AimsVFilter
{
public:

  enum VFilterType
  {
    NonOptimized,
    Optimized
  };

  AimsVFilter( int s=2, VFilterType t=Optimized ) : _size( s ), _type( t )  { }
  virtual ~AimsVFilter() { }

  AimsData< T > doit( AimsData< T >& );

private:

  AimsData< T > optimized( AimsData< T >& );
  AimsData< T > nonOptimized( AimsData< T >& );

  int _size;
  VFilterType _type;
};


template< class T > inline
AimsData< T > AimsVFilter< T >::doit( AimsData< T >& d )
{
  AimsData< T > res;

  switch( _type )
    {
    case NonOptimized:
      res = nonOptimized( d );
      break;
    case Optimized:
      res = optimized( d );
      break;
    }
    
  return res;
}


// This is the non-optimized version!
template< class T > inline
AimsData< T > AimsVFilter< T >::nonOptimized( AimsData< T >& d )
{
  int x, y, z, t, i, j, k, index;
  double sum, sum2, varmin, var[8], moy[8];
  T temp;

  int tm1 = _size - 1;
  double t3 = (double)( _size * _size * _size );

  int dx = d.dimX();
  int dy = d.dimY();
  int dz = d.dimZ();
  int dt = d.dimT();

  AimsData< T > res( dx, dy, dz, dt );
  res.setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ(), d.sizeT() );

  for ( t=0; t<dt; t++ )
    for ( z=0; z<dz; z++ )
      for ( y=0; y<dy; y++ )
        for ( x=0; x<dx; x++ )
    	  {
      	    // Initialization
	    for ( i=0; i<8; i++ )
	      {
		var[i] = 1.0e20;
		moy[i] = 0.0;
	      }

	    // First cube (-1,-1,-1)
	    if ( ( x >= tm1 ) && ( y >= tm1 ) && ( z >= tm1 ) )
	      {
		sum = sum2 = 0.0;
		for ( k=z-tm1; k<z+1; k++ )
		  for ( j=y-tm1; j<y+1; j++ )
		    for ( i=x-tm1; i<x+1; i++ )
		      {
			temp = d( i, j, k, t );
			sum += (double)temp;
			sum2 += (double)( temp * temp );
	              }

		var[ 0 ] = sum2 - ( sum * sum ) / t3;
		moy[ 0 ] = sum / t3;
	      }

	    // Second cube (-1,1,-1)
	    if ( ( x >= tm1 ) && ( y < dy-tm1 ) && ( z >= tm1 ) )
	      {
		sum = sum2 = 0.0;
		for ( k=z-tm1; k<z+1; k++ )
		  for ( j=y; j<y+_size; j++ )
		    for ( i=x-tm1; i<x+1; i++ )
		      {
			temp = d( i, j, k, t );
			sum += (double)temp;
			sum2 += (double)( temp * temp );
		      }

		var[ 1 ] = sum2 - ( sum * sum ) / t3;
		moy[ 1 ] = sum / t3;
	      }

	    // Third cube (1,1,-1)
	    if ( ( x < dx-tm1 ) && ( y < dy-tm1 ) && ( z >= tm1 ) )
	      {
		sum = sum2 = 0.0;
		for ( k=z-tm1; k<z+1; k++ )
		  for ( j=y; j<y+_size; j++ )
		    for ( i=x; i<x+_size; i++ )
		      {
			temp = d( i, j, k, t );
			sum += (double)temp;
			sum2 += (double)( temp * temp );
		      }
		      
		var[ 2 ] = sum2 - ( sum * sum ) / t3;
		moy[ 2 ] = sum / t3;
	      }

	    // Fourth cube (1,-1,-1)
	    if ( ( x < dx-tm1 ) && ( y >= tm1 ) && ( z >= tm1 ) )
	      {
		sum = sum2 = 0.0;
		for ( k=z-tm1; k<z+1; k++ )
		  for ( j=y-tm1; j<y+1; j++ )
		    for ( i=x; i<x+_size; i++ )
		      {
			temp = d( i, j, k, t );
			sum += (double)temp;
			sum2 += (double)( temp * temp );
		      }
		      
		var[ 3 ] = sum2 - ( sum * sum ) / t3;
		moy[ 3 ] = sum / t3;
	      }

	    // Fifth cube (-1,-1,1)
	    if ( ( x >= tm1 ) && ( y >= tm1 ) && ( z < dz-tm1 ) )
	      {
		sum = sum2 = 0.0;
		for ( k=z; k<z+_size; k++ )
		  for ( j=y-tm1; j<y+1; j++ )
		    for ( i=x-tm1; i<x+1; i++ )
		      {
			temp = d( i, j, k, t );
			sum += (double)temp;
			sum2 += (double)( temp * temp );
		      }
		      
		var[ 4 ] = sum2 - ( sum * sum ) / t3;
		moy[ 4 ] = sum / t3;
	      }

	    // Sixth cube (-1,1,1)
	    if ( ( x >= tm1 ) && ( y < dy-tm1 ) && ( z < dz-tm1 ) )
	      {
		sum = sum2 = 0.0;
		for ( k=z; k<z+_size; k++ )
		  for ( j=y; j<y+_size; j++ )
		    for ( i=x-tm1; i<x+1; i++ )
		      {
			temp = d( i, j, k, t );
			sum += (double)temp;
			sum2 += (double)( temp * temp );
		      }
		      
		var[ 5 ] = sum2 - ( sum * sum ) / t3;
		moy[ 5 ] = sum / t3;
	      }

	    // Seventh cube (1,1,1)
	    if ( ( x < dx-tm1 ) && ( y < dy-tm1 ) && ( z < dz-tm1 ) )
	      {
		sum = sum2 = 0.0;
		for ( k=z; k<z+_size; k++ )
		  for ( j=y; j<y+_size; j++ )
		    for ( i=x; i<x+_size; i++ )
		      {
			temp = d( i, j, k, t );
			sum += (double)temp;
			sum2 += (double)( temp * temp );
		      }
		      
		var[ 6 ] = sum2 - ( sum * sum ) / t3;
		moy[ 6 ] = sum / t3;
	      }

	    // Eighth cube (1,-1,1)
	    if ( ( x < dx-tm1 ) && ( y >= tm1 ) && ( z < dz-tm1 ) )
	      {
		sum = sum2 = 0.0;
		for ( k=z; k<z+_size; k++ )
		  for ( j=y-tm1; j<y+1; j++ )
		    for ( i=x; i<x+_size; i++ )
		      {
			temp = d( i, j, k, t );
			sum += (double)temp;
			sum2 += (double)( temp * temp );
		      }
		      
		var[ 7 ] = sum2 - ( sum * sum ) / t3;
		moy[ 7 ] = sum / t3;
	      }

	    /* Recherche du meilleur cube */
	    index = 0;
	    varmin = var[ 0 ];
	    for ( i=1; i<8; i++ )
	      if ( var[ i ] < varmin )
		{
		  varmin = var[ i ];
		  index = i;
		}

	    /* Affectation du point */
	    res( x, y, z, t ) = (T)moy[ index ];
    	  }

  return res;
}


// This is the optimized version!
template< class T > inline
AimsData< T > AimsVFilter< T >::optimized( AimsData< T >& d )
{
  int i, j, k, x, y, z, loc;
  double somme, carre, somme1, carre1, vars, *pvar, moyenne, temp;
  int dx = d.dimX();
  int dy = d.dimY();
  int dz = d.dimZ();
  int tm1 = _size - 1;
  double size2 = (double)( _size * _size * _size );

  AimsData< T > vf( dx, dy, dz );
  vf.setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ() );

  AimsData< double > table( dx, dy, _size );

  for ( z=_size; z--; )
    for ( y=dy; y--; )
      for ( x=dx; x--; )  table( x, y, z ) = -1.0;

  for (z=0; z+tm1<dz; z++ )
    {
      i = ( z + tm1 ) % _size;

      // Current table slice initialization
      for ( y=0; y<dy; y++ )
	for ( x=0; x<dx; x++ )  table( x, y, i ) = -1.0;

      // Compute the first cube for each slice
      somme = carre = 0.0;
      for ( k=z; k<z+_size; k++ )
	for ( j=0; j<_size; j++ )
	  for ( i=0; i<_size; i++ )
	    {
	      temp = (double)d( i, j, k );
	      somme += temp;
	      carre += temp * temp;
	    }

      vars = carre - somme * somme / size2;
      moyenne = somme / size2;
      somme1 = somme;
      carre1 = carre;

      pvar = &table( 0, 0, z % _size );
      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	{
	  vf( 0, 0, z ) = (T)moyenne;
	  *pvar = vars;
	}
      pvar = &table( 0, tm1, z % _size );
      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	{ 
	  vf( 0, tm1, z ) = (T)moyenne;
	  *pvar = vars;
	}
      pvar = &table( tm1, 0, z % _size );
      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	{ 
	  vf( tm1, 0, z ) = (T)moyenne;
	  *pvar = vars;
	}
      pvar = &table( tm1, tm1, z % _size );
      if ( (vars < *pvar ) || ( *pvar == -1.0 ) )
	{ 
	  vf( tm1, tm1, z ) = (T)moyenne;
	  *pvar = vars;
	}
      pvar = &table( 0, 0, ( z + tm1 ) % _size );
      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	{ 
	  vf( 0, 0, z + tm1 ) = (T)moyenne;
	  *pvar = vars;
	}
      pvar = &table( 0, tm1, ( z + tm1 ) % _size );
      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	{ 
	  vf( 0, tm1, z + tm1 ) = (T)moyenne;
	  *pvar = vars;
	}
      pvar = &table( tm1, 0, ( z + tm1 ) % _size );
      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	{ 
	  vf( tm1, 0, z + tm1 ) = (T)moyenne;
	  *pvar = vars;
	}
      vf( tm1, tm1, z + tm1 ) = (T)moyenne;
      table( tm1, tm1, ( z + tm1 ) % _size ) = vars;

      // First X scan
      for ( x=1; x+tm1<dx; x++ )
	{
	  for ( k=z; k<z+_size; k++ )
	    for ( j=0; j<_size; j++ )
	      { 
		loc = x-1; 
		temp = (double)d( loc, j, k );
		somme -= temp;
		carre -= temp * temp;
		loc = x + tm1;
		temp = (double)d( loc, j, k );
		somme += temp;
		carre += temp * temp;
	      }
	  vars = carre - somme * somme / size2;
	  moyenne = somme / size2;

	  pvar = &table( x, 0, z % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( x, 0, z ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( x, tm1, z % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( x, tm1, z ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( x + tm1, 0, z % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( x + tm1, 0, z ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( x + tm1, tm1, z % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( x + tm1, tm1, z ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( x, 0, ( z + tm1 ) % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( x, 0, z + tm1 ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( x, tm1, ( z + tm1 ) % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( x, tm1, z + tm1 ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( x + tm1, 0, ( z + tm1 ) % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) ) 
	    { 
	      vf( x + tm1, 0, z + tm1 ) = (T)moyenne;
	      *pvar = vars;
	    }
	  vf( x + tm1, tm1, z + tm1 ) = (T)moyenne;
	  table( x + tm1, tm1, ( z + tm1 ) % _size ) = vars;
	}

      // Computation loop
      for ( y=1; y+tm1<dy; y++ )
	{
	  somme = somme1;
	  carre = carre1;

	  /*** Compute the new cube for each Y ***/
	  for ( k=z; k<z+_size; k++)
	    for ( i=0; i<_size; i++)
	      { 
		loc = y - 1;
		temp = (double)d( i, loc, k );
		somme -= temp;
		carre -= temp * temp;
		loc = y + tm1;
		temp = (double)d( i, loc, k );
		somme += temp;
		carre += temp * temp;          
	      }

	  vars = carre - somme * somme / size2;
	  moyenne = somme / size2;
	  somme1 = somme;
	  carre1 = carre;

	  pvar = &table( 0, y, z % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( 0, y, z ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( 0, y + tm1, z % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( 0, y + tm1, z ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( tm1, y, z % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( tm1, y, z ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( tm1, y + tm1, z % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( tm1, y + tm1, z ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( 0, y, ( z + tm1 ) % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( 0, y, z + tm1 ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( 0, y + tm1, ( z + tm1 ) % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
	    { 
	      vf( 0, y + tm1, z + tm1 ) = (T)moyenne;
	      *pvar = vars;
	    }
	  pvar = &table( tm1, y, ( z + tm1 ) % _size );
	  if ( ( vars < *pvar ) || ( *pvar == -1.0 ) ) 
	    { 
	      vf( tm1, y, z + tm1 ) = (T)moyenne;
	      *pvar = vars;
	    }
	  vf( tm1, y + tm1, z + tm1 ) = (T)moyenne;
	  table( tm1, y + tm1, ( z + tm1 ) % _size ) = vars;      

	  // Main X scan
	  for ( x=1; x+tm1<dx; x++ )
	    { 
	      for ( k=z; k<z+_size; k++ )
		for ( j=y; j<y+_size; j++ )
		  { 
		    loc = x - 1; 
		    temp = (double)d( loc, j, k);
		    somme -= temp;
		    carre -= temp * temp;
		    loc = x + tm1;
		    temp = (double)d( loc, j, k);
		    somme += temp;
		    carre += temp * temp;
		  }

	      vars = carre - somme * somme / size2;
	      moyenne = somme / size2;

	      pvar = &table( x, y, z % _size );
	      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
		{
		  vf( x, y, z ) = (T)moyenne;
		  *pvar = vars;
		}
	      pvar = &table( x, y + tm1, z % _size );
	      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
		{ 
		  vf( x, y + tm1, z ) = (T)moyenne;
		  *pvar = vars;
		}
	      pvar = &table( x + tm1, y, z % _size );
	      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
		{ 
		  vf( x + tm1, y, z ) = (T)moyenne;
		  *pvar = vars;
		}
	      pvar = &table( x + tm1, y + tm1,  z % _size );
	      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
		{ 
		  vf( x + tm1, y + tm1, z ) = (T)moyenne;
		  *pvar = vars;
		}
	      pvar = &table( x, y, ( z + tm1 ) % _size );
	      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
		{ 
		  vf( x, y, z + tm1 ) = (T)moyenne;
		  *pvar = vars;
		}
	      pvar = &table( x, y + tm1, ( z + tm1 ) % _size );
	      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) )
		{ 
		  vf( x, y + tm1, z + tm1 ) = (T)moyenne;
		  *pvar = vars;
		}
	      pvar = &table( x + tm1, y, ( z + tm1 ) % _size );
	      if ( ( vars < *pvar ) || ( *pvar == -1.0 ) ) 
		{ 
		  vf( x + tm1, y, z + tm1 ) = (T)moyenne;
		  *pvar = vars;
		}
	      vf( x + tm1, y + tm1, z + tm1 ) = (T)moyenne;
	      table( x + tm1, y + tm1, ( z + tm1 ) % _size ) = vars;
	    }
	}
    }

  return vf;
}

#endif
