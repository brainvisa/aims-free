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


#include <aims/data/fastAllocationData.h>
#include <aims/math/mathelem.h>
#include <aims/math/eigen.h>
#include <aims/math/householder.h>
#include <aims/math/tqli.h>
#include <aims/math/balanc.h>
#include <aims/math/elmhes.h>
#include <aims/math/eltran.h>
#include <aims/math/hqr.h>
#include <aims/math/balbak.h>
#include <math.h>
#include <complex>

using namespace std;
using namespace aims;

template < class T >
AimsData< T > AimsEigen< T >::doit( AimsData< T >& mat, AimsData< T > *wwi )
{
  ASSERT( mat.dimZ() == 1 && mat.dimT() == 1 );
  ASSERT( mat.dimX() == mat.dimY() );
  AimsFastAllocationData< T > res;

  int n = mat.dimX();

  AimsFastAllocationData< T > wr( n );
  AimsFastAllocationData< T > wi( n );

  bool isSymmetric = true;
  int i, j;

  for ( j=0; j<n && isSymmetric; j++ )
    for ( i=0; i<j && isSymmetric; i++ )
      if ( mat( i, j ) != mat( j, i ) )  isSymmetric = false;

  if ( isSymmetric ) // Symmetric matrix
    {
      AimsFastAllocationData< T > e( n );

      HouseholderTridiag< T > tred;
      DecompositionTQLI< T > tqli;

      tred.doit( mat, wr, e );
      tqli.doit( wr, e, mat );
    }
  else // Non-symmetric matrix
    {
      AimsFastAllocationData< T > scale( n );
      AimsFastAllocationData< short > iscale( n );

      Balancing< T > balanc;
      HessenbergReduction< T > elmhes;
      HessenbergAccumulation< T > eltran;
      HessenbergQR< T > hqr;
      BackBalancing< T > balbak;

      mat = balanc.doit( mat, &scale );
      mat = elmhes.doit( mat, &iscale );
      AimsData< T > zz = eltran.doit( mat, &iscale );
      wr = hqr.doit( mat, wi, &zz );
	
      mat = balbak.doit( zz, wr, wi, &scale );
    }

  if ( retType == MatrixOfEigenValues )
    {
      if ( wwi )  *wwi = diag( wi );
      res = diag( wr );
    }
  else 
    {
      if ( wwi )  *wwi = wi;
      res = wr;
    }

  return res;
}


template AimsData< float > 
AimsEigen< float >::doit( AimsData< float >& mat, AimsData< float > *wwi );

template AimsData< double > 
AimsEigen< double >::doit( AimsData< double >& mat, AimsData< double > *wwi );


template < class T >
void AimsEigen< T >::sort( AimsData< T >& evec, AimsData< T >& wr,
			   AimsData< T > *wi)
{
  int i, j, k;
  T p, sum=(T)0;
  AimsFastAllocationData< T > wwr, wwi;

  ASSERT( evec.dimZ() == 1 && evec.dimT() == 1 );
  ASSERT( wr.dimZ() == 1 && wr.dimT() == 1 );

  if ( wr.dimY() > 1 )
    {
      wwr = undiag( wr );
      if ( wi )  wwi = undiag( *wi );
    }
  else
    {
      wwr = wr;
      if ( wi )  wwi = *wi;
    }

  int n = wr.dimX();

  if ( wi )
    {
      for ( i=0; i<n; i++ )  
	sum += wwi( i );
    }

  if ( sum == (T)0 )
    {
      for ( i=0; i<n-1; i++ )
	{
	  k = i;
	  p = wwr( k );

	  for ( j=i+1; j<n; j++ )
	    if ( wwr( j ) >= p )
	      {
		k = j;
		p = wwr( k );
	      }

	  if ( k != i )
	    {
	      wwr( k ) = wwr( i );
	      wwr( i ) = p;

	      for ( j=0; j<n; j++ )
		{
		  p = evec( j, i );
		  evec( j, i ) = evec( j, k );
		  evec( j, k ) = p;
		}
	    }
	}

      if ( retType == MatrixOfEigenValues )
	{
	  wr = diag( wwr );
	  if ( wi )  *wi = diag( wwi );
	}
      else
	{
	  wr = wwr;
	  if ( wi )  *wi = wwi;
	}
    }
}


template void
AimsEigen< float >::sort( AimsData< float >& evec, AimsData< float >& wr,
			  AimsData< float > *wi );

template void
AimsEigen< double >::sort( AimsData< double >& evec, AimsData< double >& wr,
			   AimsData< double > *wi );
