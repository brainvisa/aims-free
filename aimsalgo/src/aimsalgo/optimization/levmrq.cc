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

#include <cstdlib>
#include <cartodata/volume/volume.h>
#include <aims/optimization/levmrq.h>
#include <aims/io/writer.h>

using namespace std;
using namespace carto;

template < class T >
LMFunction< T > *LevenbergMarquardt< T >::doit( rc_ptr<Volume< T > >& x,
                                                rc_ptr<Volume< T > >& y,
                                                rc_ptr<Volume< T > > *sig,
                                                rc_ptr<Volume< int > > *ia,
                                                rc_ptr<Volume< T > > *covar )
{
  ASSERT( x->getSizeY() == 1 && x->getSizeZ() == 1 && x->getSizeT() == 1 );
  ASSERT( y->getSizeY() == 1 && y->getSizeZ() == 1 && y->getSizeT() == 1 );
  ASSERT( x->getSizeX() == y->getSizeX() );

  int i, itst;
  T alambda, chisq, ochisq;
  bool ok = true, status = true;

  int n = x->getSizeX();
  int ma = lmFonc->param().size();

  VolumeRef< T > tsig( n );
  VolumeRef< int > tia( ma );
  VolumeRef< T > tcov( ma, ma );
  VolumeRef< T > alpha( ma, ma );

  if ( sig )
    {
      ASSERT( (*sig)->getSizeY() == 1 && (*sig)->getSizeZ() == 1
              && (*sig)->getSizeT() == 1);
      ASSERT( (*sig)->getSizeX() == x->getSizeX() );

      tsig = *sig;
    }
  else
    for ( i=0; i<n; i++ )  tsig( i ) = (T)1;

  if ( ia )
    {
      ASSERT( (*ia)->getSizeY() == 1 && (*ia)->getSizeZ() == 1
              && (*ia)->getSizeT() == 1 );
      ASSERT( (*ia)->getSizeX() == ma );

      tia = *ia;
    }
  else 
    for ( i=0; i<ma; i++ )  tia( i ) = 1;

  itst = 0;
  alambda = (T)(-1);
  chisq = ochisq = (T)0;

  status = mrqmin( x, y, tsig, tia, &chisq, &alambda, tcov, alpha );
  if ( !status )  return (LMFunction< T > *)0;

  while ( ok )
    {
      ochisq = chisq;

      status = mrqmin( x, y, tsig, tia, &chisq, &alambda, tcov, alpha );
      if ( !status )  return (LMFunction< T > *)0;
      
      std::cout << "chisq = " << chisq << " vs " << ochisq << std::endl ;
      
      if ( chisq > ochisq )  itst = 0;
      else if ( (T)fabs( ochisq - chisq ) < (T)0.1 )  itst++;

      if ( itst >= 4 )
	{
	  alambda = (T)0;

	  status = mrqmin( x, y, tsig, tia, &chisq, &alambda, tcov, alpha );
	  if ( !status )  return (LMFunction< T > *)0;

	  ok = false;
	}
    }

  if ( covar )  *covar = tcov;

  return lmFonc;
}


template LMFunction< float > *
LevenbergMarquardt< float >::doit( rc_ptr<Volume< float > >& x,
                                   rc_ptr<Volume< float > >& y,
                                   rc_ptr<Volume< float > > *sig,
                                   rc_ptr<Volume< int > > *ia,
                                   rc_ptr<Volume< float > > *covar );


template LMFunction< double > *
LevenbergMarquardt< double >::doit( rc_ptr<Volume< double > >& x,
                                    rc_ptr<Volume< double > >& y,
                                    rc_ptr<Volume< double > > *sig,
                                    rc_ptr<Volume< int > > *ia,
                                    rc_ptr<Volume< double > > *covar );


template< class T >
bool LevenbergMarquardt< T >::mrqmin( rc_ptr<Volume< T > >& x,
                                      rc_ptr<Volume< T > >& y,
                                      rc_ptr<Volume< T > >& sig,
                                      rc_ptr<Volume< int > >& ia,
                                      T *chisq, T *alambda,
                                      rc_ptr<Volume< T > >& covar,
                                      rc_ptr<Volume< T > >& alpha )
{
  int j, k, l, m;
  int mfit;

  int ma = ia->getSizeX();

  /*static*/ T ochisq;

  /*static*/ vector< T > atry;

  /*static*/ VolumeRef< T > beta;
  /*static*/ VolumeRef< T > da;
  /*static*/ VolumeRef< T > oneda;

  atry = vector< T >( ma );
  beta = VolumeRef< T >( ma );
  da = VolumeRef< T >( ma );

  for ( mfit=0, j=0; j<ma; j++ )
    if ( ia->at( j ) )  mfit++;

  oneda = VolumeRef< T >( mfit );

  atry = lmFonc->param();

  mrqcof( x, y, sig, ia, chisq, alpha, beta );
  ochisq = (*chisq);

  if ( *alambda < (T)0 )
    {
      std::cerr << "init " << std::endl ;

      *alambda = (T)0.001;

//       std::cerr << "init " << std::endl ;
//       atry = vector< T >( ma );
//       beta = VolumeRef< T >( ma );
//       da = VolumeRef< T >( ma );
// 
//       for ( mfit=0, j=0; j<ma; j++ )
// 	if ( ia->at( j ) )  mfit++;
// 
//       oneda = VolumeRef< T >( mfit );
// 
//       *alambda = (T)0.001;
// 
//       mrqcof( x, y, sig, ia, chisq, alpha, beta );
// 
//       ochisq = (*chisq);
// 
//       atry = lmFonc->param();

    }

  for ( j=-1, l=0; l<ma; l++ )
    {
      if ( ia->at( l ) )
	{
	  for ( j++, k=-1, m=0; m<ma; m++ )
	    {
	      if ( ia->at( m ) )
		{
		  k++;
		  covar->at( j, k ) = alpha->at( j, k );
		}
	    }
	  covar->at( j, j ) = alpha->at( j, j ) * ( (T)1 + (*alambda) );
	  oneda( j ) = beta( j );
	}
    }
  
//   aims::Writer< VolumeRef<T> > wriCov( "covar.ima") ;
//   wriCov.write( covar ) ;
//   
//   aims::Writer< VolumeRef<T> > wriOD( "oneda.ima") ;
//   wriOD.write( oneda ) ;
  
  bool status = gaussj.doit( covar, oneda, mfit );
  if ( !status )  return false;
  
  for ( j=0; j<mfit; j++ )  da( j ) = oneda( j );
  
  if ( *alambda == (T)0 )
    {
      covsrt.doit( covar, &ia, mfit );
    }
  else
    {
      for ( j=-1, l=0; l<ma; l++ )
        if ( ia->at( l ) )  lmFonc->param()[ l ] += da( ++j );
      
      mrqcof( x, y, sig, ia, chisq, covar, da );
      
      if ( *chisq < ochisq )
	{
	  *alambda *= (T)0.1;
	  std::cout << "alamda decreased from " << *alambda << " to " << *alambda / 10 << endl ;
	  ochisq = (*chisq);
          
	  for ( j=-1, l=0; l<ma; l++ )
	    {
	      if ( ia->at( l ) )
                {
		  for ( j++, k=-1, m=0; m<ma; m++ )
		    {
		      if ( ia->at( m ) )
			{
			  k++;
			  alpha->at( j, k ) = covar->at( j, k );
			}
		    }
                  
		  beta( j ) = da( j );
		  atry = lmFonc->param();
		}
	    }

	}
      else 
	{
	  //std::cout << "alamda increased from " << *alamda << " to " << *alamda * 10 << endl ;
	  *alambda *= (T)10.0;
	  *chisq = ochisq;
	  lmFonc->param() = atry;
	}
    }
  cout << "mrqmin end" << endl ;
  return true;
}


template bool
LevenbergMarquardt< float >::mrqmin( rc_ptr<Volume< float > >& x,
                                     rc_ptr<Volume< float > >& sig,
                                     rc_ptr<Volume< float > >& y,
                                     rc_ptr<Volume< int > >& ia,
                                     float *chisq, float *alambda,
                                     rc_ptr<Volume< float > >& covar,
                                     rc_ptr<Volume< float > >& alpha );


template bool
LevenbergMarquardt< double >::mrqmin( rc_ptr<Volume< double > >& x,
                                      rc_ptr<Volume< double > >& y,
                                      rc_ptr<Volume< double > >& sig,
                                      rc_ptr<Volume< int > >& ia,
                                      double *chisq, double *alambda,
                                      rc_ptr<Volume< double > >& covar,
                                      rc_ptr<Volume< double > >& beta );


template< class T >
void LevenbergMarquardt< T >::mrqcof( rc_ptr<Volume< T > >& x,
                                      rc_ptr<Volume< T > >& y,
                                      rc_ptr<Volume< T > >& sig,
                                      rc_ptr<Volume< int > >& ia,
                                      T *chisq, rc_ptr<Volume< T > >& alpha,
                                      rc_ptr<Volume< T > >& beta )
{
  int i, j, k, l, m, mfit=0;
  T ymod, wt, sig2i, dy;

  int n = x->getSizeX();
  int ma = ia->getSizeX();

  for ( j=0; j<ma; j++ )
    if ( ia->at( j ) )  mfit++;

  for ( j=0; j<mfit; j++ )
    {
      for ( k=0; k<=j; k++ )  alpha->at( j, k ) = (T)0;

      beta->at( j ) = (T)0;
    }
  
  *chisq = (T)0;

  for ( i=0; i<n; i++ )
    {
      //std::cerr << "" << << std::endl ;
      ymod = lmFonc->eval( x->at( i ) );
      sig2i = (T)1 / ( sig->at( i ) * sig->at( i ) );
      dy = y->at( i ) - ymod;

      for ( j=-1, l=0; l<ma; l++ )
	{
	  if ( ia->at( l ) )
	    {
	      wt = lmFonc->derivative()[ l ] * sig2i;

	      for ( j++, k=-1, m=0; m<=l; m++ )
		if ( ia->at( m ) )
		  alpha->at( j, ++k ) += wt * lmFonc->derivative()[ m ];

	      beta->at( j ) += dy * wt;
	    }
	}

      *chisq += dy * dy * sig2i;
    }

  for ( j=1; j<mfit; j++ )
    for ( k=0; k<j; k++ )  alpha->at( k, j ) = alpha->at( j, k );
}


template void
LevenbergMarquardt< float >::mrqcof( rc_ptr<Volume< float > >& x,
				     rc_ptr<Volume< float > >& y,
				     rc_ptr<Volume< float > >& sig,
				     rc_ptr<Volume< int > >& ia,
				     float *chisq,
                                     rc_ptr<Volume< float > >& alpha,
				     rc_ptr<Volume< float > >& beta );


template void
LevenbergMarquardt< double >::mrqcof( rc_ptr<Volume< double > >& x,
				      rc_ptr<Volume< double > >& y,
				      rc_ptr<Volume< double > >& sig,
				      rc_ptr<Volume< int > >& ia,
				      double *chisq,
                                      rc_ptr<Volume< double > >& alpha,
				      rc_ptr<Volume< double > >& beta );
