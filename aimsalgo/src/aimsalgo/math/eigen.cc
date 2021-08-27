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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cartodata/volume/volume.h>
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
using namespace carto;
using namespace aims;

template < class T >
VolumeRef< T > AimsEigen< T >::doit( VolumeRef< T > mat, VolumeRef< T > *wwi )
{
  ASSERT( mat.getSizeZ() == 1 && mat.getSizeT() == 1 );
  ASSERT( mat.getSizeX() == mat.getSizeY() );
  VolumeRef< T > res;

  int n = mat.getSizeX();

  VolumeRef< T > wr( n, 1, 1, 1,
                     AllocatorContext(
                        &carto::MemoryAllocator::singleton() ) );
  VolumeRef< T > wi( n, 1, 1, 1,
                     AllocatorContext(
                      &carto::MemoryAllocator::singleton() ) );

  bool isSymmetric = true;
  int i, j;

  for ( j=0; j<n && isSymmetric; j++ )
    for ( i=0; i<j && isSymmetric; i++ )
      if ( mat( i, j ) != mat( j, i ) )  isSymmetric = false;

  if ( isSymmetric ) // Symmetric matrix
    {
      VolumeRef< T > e( n, 1, 1, 1,
                        AllocatorContext(
                          &carto::MemoryAllocator::singleton() ) );

      HouseholderTridiag< T > tred;
      DecompositionTQLI< T > tqli;

      tred.doit( mat, wr, e );
      tqli.doit( wr, e, mat );
    }
  else // Non-symmetric matrix
    {
      VolumeRef< T > scale( n, 1, 1, 1,
                            AllocatorContext(
                              &carto::MemoryAllocator::singleton() ) );
      VolumeRef< short > iscale( n, 1, 1, 1,
                                 AllocatorContext(
                                  &carto::MemoryAllocator::singleton() ) );

      Balancing< T > balanc;
      HessenbergReduction< T > elmhes;
      HessenbergAccumulation< T > eltran;
      HessenbergQR< T > hqr;
      BackBalancing< T > balbak;

      mat = balanc.doit( mat, &scale );
      mat = elmhes.doit( mat, &iscale );
      VolumeRef< T > zz = eltran.doit( mat, &iscale );
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


template VolumeRef< float >
AimsEigen< float >::doit( VolumeRef< float > mat, VolumeRef< float > *wwi );

template VolumeRef< double >
AimsEigen< double >::doit( VolumeRef< double > mat, VolumeRef< double > *wwi );


template < class T >
void AimsEigen< T >::sort( VolumeRef< T > evec, VolumeRef< T > wr,
                           VolumeRef< T > *wi)
{
  int i, j, k;
  T p, sum=(T)0;
  VolumeRef< T > wwr, wwi;

  ASSERT( evec.getSizeZ() == 1 && evec.getSizeT() == 1 );
  ASSERT( wr.getSizeZ() == 1 && wr.getSizeT() == 1 );

  if ( wr.getSizeY() > 1 )
    {
      wwr = undiag( wr );
      if ( wi )  wwi = undiag( *wi );
    }
  else
    {
      wwr = wr;
      if ( wi )  wwi = *wi;
    }

  int n = wr.getSizeX();

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
AimsEigen< float >::sort( VolumeRef< float > evec, VolumeRef< float > wr,
                          VolumeRef< float > *wi );

template void
AimsEigen< double >::sort( VolumeRef< double > evec, VolumeRef< double > wr,
                           VolumeRef< double > *wi );
