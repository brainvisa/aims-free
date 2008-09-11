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


#include <aims/data/data.h>
#include <aims/optimization/levmrq.h>
#include <aims/io/writer.h>

using namespace std;

template < class T >
LMFunction< T > *LevenbergMarquardt< T >::doit( AimsData< T >& x,
						AimsData< T >& y,
						AimsData< T > *sig,
						AimsData< int > *ia,
						AimsData< T > *covar )
{
  ASSERT( x.dimY() == 1 && x.dimZ() == 1 && x.dimT() == 1 );
  ASSERT( y.dimY() == 1 && y.dimZ() == 1 && y.dimT() == 1 );
  ASSERT( x.dimX() == y.dimX() );

  int i, itst;
  T alambda, chisq, ochisq;
  bool ok = true, status = true;

  int n = x.dimX();
  int ma = lmFonc->param().size();

  AimsData< T > tsig( n );
  AimsData< int > tia( ma );
  AimsData< T > tcov( ma, ma );
  AimsData< T > alpha( ma, ma );

  if ( sig )
    {
      ASSERT( sig->dimY() == 1 && sig->dimZ() == 1 && sig->dimT() == 1);
      ASSERT( sig->dimX() == x.dimX() );

      tsig = *sig;
    }
  else
    for ( i=0; i<n; i++ )  tsig( i ) = (T)1;

  if ( ia )
    {
      ASSERT( ia->dimY() == 1 && ia->dimZ() == 1 && ia->dimT() == 1 );
      ASSERT( ia->dimX() == ma );

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
LevenbergMarquardt< float >::doit( AimsData< float >& x, AimsData< float >& y,
				   AimsData< float > *sig, 
				   AimsData< int > *ia,
				   AimsData< float > *covar );


template LMFunction< double > *
LevenbergMarquardt< double >::doit( AimsData< double >& x, 
				    AimsData< double >& y,
				    AimsData< double > *sig, 
				    AimsData< int > *ia,
				    AimsData< double > *covar );


template< class T >
bool LevenbergMarquardt< T >::mrqmin( AimsData< T >& x, AimsData< T >& y,
				      AimsData< T >& sig, AimsData< int >& ia,
				      T *chisq, T *alambda, 
				      AimsData< T >& covar, 
				      AimsData< T >& alpha )
{
  int j, k, l, m;
  int mfit;

  int ma = ia.dimX();

  /*static*/ T ochisq;

  /*static*/ vector< T > atry;

  /*static*/ AimsData< T > beta;
  /*static*/ AimsData< T > da;
  /*static*/ AimsData< T > oneda;

  atry = vector< T >( ma );
  beta = AimsData< T >( ma );
  da = AimsData< T >( ma );

  for ( mfit=0, j=0; j<ma; j++ )
    if ( ia( j ) )  mfit++;

  oneda = AimsData< T >( mfit );

  atry = lmFonc->param();

  mrqcof( x, y, sig, ia, chisq, alpha, beta );
  ochisq = (*chisq);

  if ( *alambda < (T)0 )
    {
      std::cerr << "init " << std::endl ;

      *alambda = (T)0.001;

//       std::cerr << "init " << std::endl ;
//       atry = vector< T >( ma );
//       beta = AimsData< T >( ma );
//       da = AimsData< T >( ma );
// 
//       for ( mfit=0, j=0; j<ma; j++ )
// 	if ( ia( j ) )  mfit++;
// 
//       oneda = AimsData< T >( mfit );
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
      if ( ia( l ) )
	{
	  for ( j++, k=-1, m=0; m<ma; m++ )
	    {
	      if ( ia( m ) )
		{
		  k++;
		  covar( j, k ) = alpha( j, k );
		}
	    }
	  covar( j, j ) = alpha( j, j ) * ( (T)1 + (*alambda) );
	  oneda( j ) = beta( j );
	}
    }
  
//   aims::Writer< AimsData<T> > wriCov( "covar.ima") ;
//   wriCov.write( covar ) ;
//   
//   aims::Writer< AimsData<T> > wriOD( "oneda.ima") ;
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
        if ( ia( l ) )  lmFonc->param()[ l ] += da( ++j );
      
      mrqcof( x, y, sig, ia, chisq, covar, da );
      
      if ( *chisq < ochisq )
	{
	  *alambda *= (T)0.1;
	  std::cout << "alamda decreased from " << *alambda << " to " << *alambda / 10 << endl ;
	  ochisq = (*chisq);
          
	  for ( j=-1, l=0; l<ma; l++ )
	    {
	      if ( ia( l ) )
                {
		  for ( j++, k=-1, m=0; m<ma; m++ )
		    {
		      if ( ia( m ) )
			{
			  k++;
			  alpha( j, k ) = covar( j, k );
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
LevenbergMarquardt< float >::mrqmin( AimsData< float >& x, 
				     AimsData< float >& y,
				     AimsData< float >& sig, 
				     AimsData< int >& ia,
				     float *chisq, float *alambda, 
				     AimsData< float >& covar,
				     AimsData< float >& alpha );


template bool
LevenbergMarquardt< double >::mrqmin( AimsData< double >& x, 
				      AimsData< double >& y,
				      AimsData< double >& sig, 
				      AimsData< int >& ia,
				      double *chisq, double *alambda, 
				      AimsData< double >& covar,
				      AimsData< double >& beta );


template< class T >
void LevenbergMarquardt< T >::mrqcof( AimsData< T >& x, AimsData< T >& y,
				      AimsData< T >& sig, AimsData< int >& ia,
				      T *chisq, AimsData< T >& alpha, 
				      AimsData< T >& beta )
{
  int i, j, k, l, m, mfit=0;
  T ymod, wt, sig2i, dy;

  int n = x.dimX();
  int ma = ia.dimX();

  for ( j=0; j<ma; j++ )
    if ( ia( j ) )  mfit++;

  for ( j=0; j<mfit; j++ )
    {
      for ( k=0; k<=j; k++ )  alpha( j, k ) = (T)0;

      beta( j ) = (T)0;
    }
  
  *chisq = (T)0;

  for ( i=0; i<n; i++ )
    {
      //std::cerr << "" << << std::endl ;
      ymod = lmFonc->eval( x( i ) );
      sig2i = (T)1 / ( sig( i ) * sig( i ) );
      dy = y( i ) - ymod;

      for ( j=-1, l=0; l<ma; l++ )
	{
	  if ( ia( l ) )
	    {
	      wt = lmFonc->derivative()[ l ] * sig2i;

	      for ( j++, k=-1, m=0; m<=l; m++ )
		if ( ia( m ) )  
		  alpha( j, ++k ) += wt * lmFonc->derivative()[ m ];

	      beta( j ) += dy * wt;
	    }
	}

      *chisq += dy * dy * sig2i;
    }

  for ( j=1; j<mfit; j++ )
    for ( k=0; k<j; k++ )  alpha( k, j ) = alpha( j, k );
}


template void
LevenbergMarquardt< float >::mrqcof( AimsData< float >& x, 
				     AimsData< float >& y,
				     AimsData< float >& sig, 
				     AimsData< int >& ia,
				     float *chisq, AimsData< float >& alpha,
				     AimsData< float >& beta );


template void
LevenbergMarquardt< double >::mrqcof( AimsData< double >& x, 
				      AimsData< double >& y,
				      AimsData< double >& sig, 
				      AimsData< int >& ia,
				      double *chisq, AimsData< double >& alpha,
				      AimsData< double >& beta );
