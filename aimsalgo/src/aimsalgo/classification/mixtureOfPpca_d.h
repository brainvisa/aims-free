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


#ifndef MIXTURE_OF_PPCA_D_H
#define MIXTURE_OF_PPCA_D_H

#include <cstdlib>
#include <aims/classification/mixtureOfPpca.h>
#include <aims/io/finder.h>
#include <aims/io/writer.h>
#include <aims/vector/vector.h>
#include <aims/math/gausslu.h>
#include <aims/bucket/bucketMap.h>
#include <cartobase/type/string_conversion.h>
#include <aims/math/svd.h>
#include <aims/data/data.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <math.h>

using namespace std;
using namespace aims;
using namespace carto;

double * PpcaAnalyserElement::_exp = 0 ;

PpcaAnalyserElement::PpcaAnalyserElement( int significantNumberOfVp, bool useOnlyCorrIndiv ) : 
  _useOnlyCorrIndiv( useOnlyCorrIndiv ), _significantNumberOfVp( significantNumberOfVp ), _computed( false ), _energy(0.), _valid(true)
{
  if( !_exp ){
    _exp = new double[75010] ;
    for( int i = 0 ; i < 7510 ; ++i )
      _exp[i] = exp( -i/100. ) ;    
  }
}


template <class T>
void
PpcaAnalyserElement::init( const std::list<int>& selectedIndividuals, double initialPi, 
			   const AimsData<T>& individuals, double noiseRef )
{
  AimsData<T> indivMatrix( max(int(selectedIndividuals.size()), 1), individuals.dimY() ) ;
  list<int>::const_iterator iter( selectedIndividuals.begin() ), 
    last( selectedIndividuals.end() ) ; 
  int i = 0 ;

  while( iter != last ){
    for( int t = 0 ; t < individuals.dimY() ; ++t )
      indivMatrix( i, t ) = individuals( *iter, t ) ;
    ++i ; 
    ++iter ;
  }

  _Pi = initialPi ;
  doIt( indivMatrix, individuals.dimX(), noiseRef ) ;
}


template <class T>
void
PpcaAnalyserElement::doIt( const AimsData<T>& indivMatrix, int totalNbOfIndividuals, double noiseRef )
{
  _computed = true ;
  int nbInd = indivMatrix.dimX() ;
  int nbFrame = indivMatrix.dimY() ;

  AimsData<double> centeredIndivMatrix( nbInd, nbFrame ) ;
  _mean = AimsData<double>( nbFrame ) ;

  for( int ind = 0 ; ind < nbInd ; ++ind )
    for( int t = 0 ; t < nbFrame ; ++t )
      _mean[t] += indivMatrix( ind, t ) ;
  for( int t = 0 ; t < nbFrame ; ++t ){
    _mean[t] /= centeredIndivMatrix.dimX() ;
    for( int ind = 0 ; ind < centeredIndivMatrix.dimX() ; ++ind )
      centeredIndivMatrix( ind, t ) = indivMatrix( ind, t ) - _mean[t] ;
  }
//  cout << "mean = " << _mean << endl ; 
 
  // Matrice des correlations
  AimsData<double> matVarCov( nbFrame, nbFrame ) ;
  int x, y ;
  ForEach2d( matVarCov, x, y ){
    for( int k = 0 ;  k < centeredIndivMatrix.dimX() ; ++k )
      matVarCov(x, y) += centeredIndivMatrix(k, x) * centeredIndivMatrix(k, y) ;
    matVarCov(x, y) /= centeredIndivMatrix.dimX() - 1 ;
  }
  
  // Decomposition SVD 
  AimsSVD<double>  svd;
  svd.setReturnType( AimsSVD<double>::VectorOfSingularValues ) ;
  AimsData<double> eigenValues = svd.doit( matVarCov ) ;  
  svd.sort( matVarCov, eigenValues ) ;  
//  cout << "valeurs propres = " << eigenValues << endl ;

  // Calcul de "noise variance"(somme des valeurs propres non significatives/par leur nb)=sigma^2
  _sigma2 = 0. ;
  for( int i = _significantNumberOfVp ; i < nbFrame ; ++i )
    _sigma2 += eigenValues[i] ;
  _sigma2 /= (double)( nbFrame - _significantNumberOfVp ) ;  
  cout << endl << "sigma2 = " << setprecision(3) << _sigma2 << endl ; 
  
  AimsData<double> selectedEigenVectors( nbFrame, _significantNumberOfVp ) ; //dim dxq
  for( int i = 0 ; i < _significantNumberOfVp ; ++i ){
    for( int t = 0 ; t < nbFrame ; ++t ){
      selectedEigenVectors(t, i) = matVarCov( t, i ) ;
    }
  }
//  cout << "selected EV = " << endl << setprecision(3) << selectedEigenVectors << endl ;

  // Calcul de la "weight" matrice Wi (dim. dxq) 
  AimsData<double> tempWi( _significantNumberOfVp, _significantNumberOfVp ) ;

  for( int i = 0 ; i < _significantNumberOfVp ; ++i )
    tempWi(i, i) = sqrt( eigenValues[i] - _sigma2 ) ; 
  _Wi = selectedEigenVectors.cross( tempWi ) ;  
  
  // calcul de la matrice de covariance Ci = sigma^2I+WWT  
  AimsData<double> Ci = _Wi.cross( _Wi.clone().transpose() ) ;        
  for( int i = 0 ; i < nbFrame ; ++i )
    Ci(i, i) += _sigma2 ;  

  // calcul de detCi et invCi
  double s = 0.000001 ;
  AimsData< double > u = Ci.clone(); // because it is replaced on SVD output
  AimsData< double > v( Ci.dimY(), Ci.dimY() );
  AimsSVD< double > svd2;
  svd2.setReturnType( AimsSVD<double>::MatrixOfSingularValues ) ;
  AimsData< double > w = svd2.doit( u, &v );
  
  // Set the singular values lower than threshold to zero
  double ts = s * w.maximum();
  int i, n = w.dimX();
  double detCi = 1. ;
  for ( i = 0 ; i < n; i++ ){
    if ( w( i, i ) < 0. ){ 
      cout << endl << "pour i = " << i << ", valeur w = " << w( i, i ) << " ! VALEUR NEGATIVE CHANGEE !" << endl ;
      w( i, i ) = - w( i, i ) ;
    }
    detCi *= w( i, i ) / _sigma2 ;
    if ( w( i, i ) > ts )  w( i, i ) = 1.0f / w( i, i );
    else w( i, i ) = 0.0f;
  }
  // Get the SVD inverse of h ( h-1 = V.W-2.V'.H' = V.W-1.U' )
  // for solution of H'.H.x = H'.b  <=>  min(||H.x-b||^2)
  _invCi = v.cross( w.cross(  u.transpose() ) ) ;
//  cout << "invCi = " << endl << _invCi << endl ;

//  cout << "det Ci = " << detCi << endl ;
  _normFactor = 1 / ( pow( detCi, 0.5 ) * pow( _sigma2 / noiseRef, nbFrame / 2. ) ) ;
//  cout << "normFactor avant = " << 1 / pow( detCi, 0.5 ) << endl ;
  cout << "norm factor avec noiseRef = " << _normFactor << endl ;
        
  // calcul de la matrice de covariance posterieure Mi = sigma^2I+WTW et de invMi  
  AimsData<double> Mi = ( _Wi.clone().transpose() ).cross( _Wi ) ;        
  for( int i = 0 ; i < _significantNumberOfVp ; ++i )
    Mi(i, i) += _sigma2 ;  

  u = Mi.clone(); // because it is replaced on SVD output
  v( Mi.dimY(), Mi.dimY() );
  AimsSVD< double > svd3;
  w = svd3.doit( u, &v );
  
  ts = s * w.maximum();
  n = w.dimX();
  for ( i = 0 ; i < n; i++ ){
    if ( w( i, i ) > ts )  w( i, i ) = 1.0f / w( i, i );
    else w( i, i ) = 0.0f;
  }
  _invMi = v.cross( w.cross(  u.transpose() ) ) ;
//  cout << "invMi = " << _invMi << endl ;

  _Rn = AimsData<double>( totalNbOfIndividuals ) ;
}


template <class T>
bool
PpcaAnalyserElement::newStep1( const AimsData<T>& indivMatrix, bool useOnlyCorrIndiv )
{
  int nbInd = indivMatrix.dimX() ;
  int nbFrame = indivMatrix.dimY() ;

/////////////////////////////////////////////////////
//  calcul pour tous les individus de p(tn|i) 
//  + CALCUL DE LA CORRELATION ENTRE CHAQUE INDIV ET LE VECTEUR MOYEN

  AimsData<double> indivn( nbFrame ) ;
  AimsData<double> centeredIndivn( nbFrame ) ;
  _dist = AimsData<double>( nbInd ) ;
  _pTni = AimsData<double>( nbInd ) ;

  double sumInd, sumMean, indMean, meanMean, corr = 0. ;
  int count = 0 ;
  AimsData<double> antiCorr( nbInd, nbFrame ) ;
  AimsData<double> Corr( nbInd, nbFrame ) ;
  int nbIndTaken = 0 ;

  //float invDimNormFactor = 1. / ( nbFrame - _significantNumberOfVp ) ;
  
  for( int n = 0 ; n < nbInd ; ++n ){
    sumInd = 0. ;
    sumMean = 0. ;
    indMean = 0. ;
    meanMean = 0. ;
    corr = 0. ;

    for( int t = 0 ; t < nbFrame ; ++t ){
      indivn[t] = indivMatrix(n, t) ;
      centeredIndivn[t] = indivMatrix(n, t) - _mean[t] ;
      sumInd += indivn[t] ;
      sumMean += _mean[t] ;
    }
    // AJOUT 0411 calcul de la corrélation entre chaque individu et le vecteur moyen
    if( useOnlyCorrIndiv == 1 ){
      indMean = sumInd / (double)nbFrame ;
      meanMean = sumMean / (double)nbFrame ;
      for( int t = 0 ; t < nbFrame ; ++t )
	corr += ( indivn[t] - indMean ) * ( _mean[t] - meanMean ) ;
      
      if( corr < 0. ){
	// probabilité nulle si anti-corrélé
/* 	for( int t = 0 ; t < nbFrame ; ++t )       */
/* 	  antiCorr( nbAntiCorr, t ) = indivn[t] ; */
/* 	++nbAntiCorr ; */
	_pTni[n] = 0. ;
	++count ;
      } else {
	// calcul de p(tn|i)
/* 	for( int t = 0 ; t < nbFrame ; ++t )       */
/* 	  Corr( nbCorr, t ) = indivn[t] ; */
/* 	++nbCorr ; */
	_dist[n] = ( centeredIndivn.clone().transpose() ).cross( _invCi.cross( centeredIndivn ) )(0, 0) /** invDimNormFactor*/ ;
	if( _dist[n] > 1500 )
	  _pTni[n] = 0 ;
	else
	  _pTni[n] = /*pow( 2*M_PI, - nbFrame/2. ) * */ _normFactor * _exp[ int(0.5 * _dist[n] * 100. + 0.000001 ) ] ;
      }

    } else {
      _dist[n] = ( centeredIndivn.clone().transpose() ).cross( _invCi.cross( centeredIndivn ) )(0, 0) /** invDimNormFactor*/ ; 
      if( _dist[n] > 1500 )
	_pTni[n] = 0 ;
      else
	_pTni[n] = /*pow( 2*M_PI, - nbFrame/2. ) * */ _normFactor * _exp[ int(0.5 * _dist[n] * 100. + 0.000001) ] ;
    }
    if( _pTni[n] > 0. )
      ++nbIndTaken ;
  }

/*   Writer < AimsData<double> > wr( "correlated.ima" ) ; */
/*   wr.write( Corr ) ; */
/*   Writer < AimsData<double> > wr2( "anti-correlated.ima" ) ; */
/*   wr2.write( antiCorr ) ; */
/*   Writer < AimsData<double> > wr3( "mean.ima" ) ; */
/*   wr3.write( _mean ) ; */
  
/*   double sumPtni = 0., prodPtni = 0., meanPtni = 0. , varPtni = 0. ; */
/*   // moyenne et variance des pTni */
/*   for( int n = 0 ; n < nbInd ; ++n ){ */
/*     sumPtni += _pTni[n] ; */
/*     prodPtni += _pTni[n] * _pTni[n] ; */
/*   }    */
/*   meanPtni = sumPtni / nbInd ; */
/*    varPtni = prodPtni / nbInd ; */
/*    cout << ( _useOnlyCorrIndiv ? " anticorreles = " : "" ) << ( _useOnlyCorrIndiv ? toString(count) : "" )  */
/*         << " - moyenne et variance des p(tn|i) = "  */
/*         << meanPtni << " - " << varPtni << endl ; */
  if( nbIndTaken < indivMatrix.dimY() * 2 )
    _valid = false ;
  else
    _valid = true  ;
  return _valid ;
}


template <class T>
double
PpcaAnalyserElement::newStep2( AimsData<double> pTn, const AimsData<T>& indivMatrix, 
			       double noiseRef )
{
  bool explicitComputing = true ;

  int nbInd = indivMatrix.dimX() ;
  int nbFrame = indivMatrix.dimY() ;
   _energy = 0. ;

  ////////////////////////////////////////////
  //  _Rn[i] = probabilityOfTnKnownI * Pi[i] / pTn ;

  
//  _Rn = AimsData<double>( nbInd ) ; 
  double previousRn ;
  int nbIndTemp = 0 ;        // nb d'individus dont p(tn)!=0
//  double previousRn = 0. ;
  _sumDiff2Rni = 0. ;

  //cout << "Rn computing begin   ------  " ;
  for( int n = 0 ; n < nbInd ; ++n ){
    previousRn = _Rn[n] ;
    if( pTn[n] != 0. ){        // on ignore les points problematiques
      _Rn[n] = _pTni[n] * _Pi / pTn[n] ;
      //_energy += 1 - 4. * (_Rn[n] - 0.5) *  (_Rn[n] - 0.5) ;
      ++nbIndTemp ;
    }
    else
      _Rn[n] = 0. ;
    
    // if( n < 10 && _Rn[n] != 0.) cout << "n = " << n << " - nouveau/ancien Rn = " << _Rn[n] << " " << previousRn << endl ;
    //_sumDiff2Rni += pow( _Rn[n] - previousRn, 2. ) ;
  }
  //_energy /= nbInd ;
  //_sumDiff2Rni /= nbInd ;
  
  // cout << "Rn computing end" << endl ;
  
  cout << /*endl <<*/ "critere pour la classe = " << setprecision(9) << _sumDiff2Rni << " - "  ; 


  //////////////////////////////////////////////////
  // _Pi = 1/nbInd * sum(Rni)

  _Pi = 0. ; 
  for( int n = 0 ; n < nbInd ; ++n )
    _Pi += _Rn[n] ;
  _Pi /= nbIndTemp ;
  cout << "Pi = " << setprecision(3) << _Pi << endl ; 

  double nbIndClass = _Pi * nbIndTemp ;
  cout << "'nb d'individus appartenant' a la classe = " << nbIndClass << endl ;


  ////////////////////////////////////////////////
  // _mean = sum(Rni*tn)/sum(Rni)

  double sumRn = 0. ;
  AimsData<double> sumRnTn( nbFrame ) ;
  int nullRniNb = 0, nearNullRni = 0 ;
  for( int n = 0 ; n < nbInd ; ++n ){
      sumRn += _Rn[n] ;
      if( _Rn[n] == 0. )
        ++nullRniNb ;
      if( _Rn[n] < 10e-30 )
        ++nearNullRni ;
      for( int t = 0 ; t < nbFrame ; ++t ) 
	sumRnTn[t] += _Rn[n] * indivMatrix(n, t) ;
  }   
  
  cout << "Nombre de RNi nulls : " << nullRniNb << "et de Rni presque nulls" << nearNullRni << endl ;
  for( int t = 0 ; t < nbFrame ; ++t )  
    _mean[t] = sumRnTn[t] / sumRn ;
//  cout << "mean = " << _mean << endl ; 

  
  ////////////////////////////////////////////////
  // Si = 1/Pi*nbInd sum( Rni (tn-mui) (tn-mui)T )

  AimsData<double> centeredIndiv( nbFrame ) ;
  AimsData<double> Si( nbFrame, nbFrame ) ; 
  
/*   for( int n = 0 ; n < nbInd ; ++n ) */
/*       for( int t = 0 ; t < nbFrame ; ++t ) */
/* 	centeredIndivMatrix(n, t) = indivMatrix(n, t) - _mean[t] ; */
  
  int x, y ;
  for( int n = 0 ;  n < nbInd ; ++n )
    if( _Rn[n] != 0. ){
      for( int t = 0 ; t < nbFrame ; ++t )
	centeredIndiv(t) = indivMatrix(n, t) - _mean[t] ;
      ForEach2d( Si, x, y )
	Si(x, y) += _Rn[n] * centeredIndiv(x) * centeredIndiv(y) ;
    }
  ForEach2d( Si, x, y )
    Si(x, y) /= _Pi * nbIndTemp ;


  //------------------- CALCUL EXPLICITE ---------------------------------
  
  double detCi = 1. ;

  if( explicitComputing ){
    // Decomposition SVD 
    
    //cout << "Compute Si svd begin" << endl ;
    AimsSVD<double>  svd;
    svd.setReturnType( AimsSVD<double>::VectorOfSingularValues ) ;
    AimsData<double> eigenValues = svd.doit( Si ) ;  
    svd.sort( Si, eigenValues ) ;  
    
    //cout << "Compute Si svd end" << endl ;
    
    // Calcul de "noise variance"(somme des valeurs propres non significatives/par leur nb)=sigma^2
    _sigma2 = 0. ;
    for( int i = _significantNumberOfVp ; i < nbFrame ; ++i )
      _sigma2 += eigenValues[i] ;
    _sigma2 /= (double)( nbFrame - _significantNumberOfVp ) ;  
    cout << "sigma2 = " << _sigma2 << " (explicite)" << " - " ;

    AimsData<double> selectedEigenVectors( nbFrame, _significantNumberOfVp ) ; //dim dxq
    for( int i = 0 ; i < _significantNumberOfVp ; ++i )
      for( int t = 0 ; t < nbFrame ; ++t )
	selectedEigenVectors(t, i) = Si( t, i ) ;
        
    // Calcul de la "weight" matrice Wi (dim. dxq) 
    AimsData<double> tempWi( _significantNumberOfVp, _significantNumberOfVp ) ;
    
    for( int i = 0 ; i < _significantNumberOfVp ; ++i )
      tempWi(i, i) = sqrt( eigenValues[i] - _sigma2 ) ; 
   _Wi = selectedEigenVectors.cross( tempWi ) ;  
    
    // calcul de la matrice de covariance Ci = sigma^2I+WWT  
    AimsData<double> Ci = _Wi.cross( _Wi.clone().transpose() ) ;        
    for( int i = 0 ; i < nbFrame ; ++i )
      Ci(i, i) += _sigma2 ;  

    // calcul de detCi et invCi
    double s = 0.000001 ;
    AimsData< double > u = Ci.clone(); // because it is replaced on SVD output
    AimsData< double > v( Ci.dimY(), Ci.dimY() );
    AimsSVD< double > svd2;
    svd2.setReturnType( AimsSVD<double>::MatrixOfSingularValues ) ;
    AimsData< double > w = svd2.doit( u, &v );
    
    // Set the singular values lower than threshold to zero
    double ts = s * w.maximum();
    int i, n = w.dimX();
    for ( i = 0 ; i < n; i++ ){
      if ( w( i, i ) < 0. ){ 
	cout << endl << "pour i = " << i << ",valeur w = " << w( i, i ) << "! VALEUR NEGATIVE CHANGEE!" << endl ;
	w( i, i ) = - w( i, i ) ;
      }
      detCi *= w( i, i ) / _sigma2 ;
      if ( w( i, i ) > ts )  w( i, i ) = 1.0f / w( i, i );
      else w( i, i ) = 0.0f;
    }
    // Get the SVD inverse of h ( h-1 = V.W-2.V'.H' = V.W-1.U' )
    // for solution of H'.H.x = H'.b  <=>  min(||H.x-b||^2)
    _invCi = v.cross( w.cross(  u.transpose() ) ) ;
    
  } else {

    //---------------------Iterative computing----------------------------------

  ////////////////////////////////////////////////
  // _Wi = SiWi (sigma2I + Mi-1 WiT Si Wi)-1     avec Mi = sigma2I + WiT*Wi

  AimsData<double> newWi = _invMi.cross( ( _Wi.clone().transpose() ).cross( Si.cross( _Wi ) ) ) ;
  for( int i = 0 ; i < newWi.dimX() ; ++i ) 
    newWi(i, i) += _sigma2 ;

  double s = 0.000001 ;
  AimsData<double> u = newWi.clone(); // because it is replaced on SVD output
  AimsData<double> v( newWi.dimY(), newWi.dimY() );
  AimsSVD<double> svd1;
  svd1.setReturnType( AimsSVD<double>::MatrixOfSingularValues ) ;
  AimsData<double> w = svd1.doit( u, &v );
  double ts = s * w.maximum();
  int i, n = w.dimX();

  for ( i = 0 ; i < n ; i++ ){
    if ( w( i, i ) < 0. ){ 
      cout << endl << "pour i = " << i << ", w = " << w( i, i ) << " ! VALEUR NEGATIVE CHANGEE !" << endl ;
      w( i, i ) = - w( i, i ) ;
    }  
    if ( w( i, i ) > ts )  w( i, i ) = 1.0f / w( i, i );
    else w( i, i ) = 0.0f;
  }
  newWi = v.cross( w.cross( u.transpose() ) ) ;
  
  newWi = Si.cross( _Wi.cross( newWi ) ) ;
//  _Wi = newWi ;


  //////////////////////////////////////////
  // Mi = _sigmai2I + WiT Wi & invMi

/*   AimsData<double> Mi = ( newWi.clone().transpose() ).cross( newWi ) ;         */
/*   for( int i = 0 ; i < Mi.dimX() ; ++i ) */
/*     Mi(i, i) += _sigma2 ; */
  
/*   AimsData<double> u2 = Mi.clone(); */
/*   AimsData<double> v2( Mi.dimY(), Mi.dimY() ) ;   */
/*   AimsSVD< double > svd2 ; */
/*   svd2.setReturnType( AimsSVD<double>::MatrixOfSingularValues ) ; */
/*   AimsData<double> w2 = svd2.doit( u2, &v2 ) ;  */
/*   ts = s * w2.maximum(); */
/*   n = w2.dimX(); */

/*   for( i = 0 ; i < n ; i++ ){ */
/*     if ( w2( i, i ) < 0. ){  */
/*       cout << endl << "pour i = " << i << ", w2 = " << w2( i, i ) << " ! VALEUR NEGATIVE CHANGEE !" << endl ; */
/*       w2( i, i ) = - w2( i, i ) ; */
/*     }  */
/*     if ( w2( i, i ) > ts )  w2( i, i ) = 1.0f / w2( i, i ) ; */
/*     else w2( i, i ) = 0.0f; */
/*   } */
/*   _invMi = v2.cross( w2.cross( u2.transpose() ) ) ; */


  //////////////////////////////////////////
  // _sigmai2 = 1/d tr(Si - Si Wi Mi-1 WiT)

  _sigma2 = 0. ;
  AimsData<double> temp = Si.cross( _Wi.cross( _invMi.cross( newWi.clone().transpose() ) ) ) ;

  for( int i = 0 ; i < temp.dimX() ; ++i ) 
    _sigma2 += ( Si(i, i) - temp(i, i) ) ;
  _sigma2 /= nbFrame ;
  cout << "sigma2 = " << _sigma2 << " - " ;
  if( _sigma2 < 0. ){
    cout << "VALEUR NEGATIVE CHANGEE ! " << endl ;
    _sigma2 = - _sigma2 ;
  }
    
  // essai
  _Wi = newWi ;


  //////////////////////////////////////////
  // Mi = _sigmai2I + WiT Wi & invMi

  AimsData<double> Mi = ( _Wi.clone().transpose() ).cross( _Wi ) ;        
  for( int i = 0 ; i < Mi.dimX() ; ++i )
    Mi(i, i) += _sigma2 ;
  
  AimsData<double> u2 = Mi.clone();
  AimsData<double> v2( Mi.dimY(), Mi.dimY() ) ; 
  AimsSVD< double > svd2 ;
  svd2.setReturnType( AimsSVD<double>::MatrixOfSingularValues ) ;
  AimsData<double> w2 = svd2.doit( u2, &v2 ) ; 
  ts = s * w2.maximum();
  n = w2.dimX();
  for( i = 0 ; i < n ; i++ ){
    if ( w2( i, i ) < 0. ){ 
      cout << endl << "pour i = " << i << ", w2 = " << w2( i, i ) << " ! VALEUR NEGATIVE CHANGEE !" << endl ;
      w2( i, i ) = - w2( i, i ) ;
    }
    if ( w2( i, i ) > ts )  w2( i, i ) = 1.0f / w2( i, i ) ;
    else w2( i, i ) = 0.0f;
  }
  _invMi = v2.cross( w2.cross( u2.transpose() ) ) ;

  
  //////////////////////////////////////////////////
  // Ci = sigmai2 + Wi WiT  --> Ci-1 & det(Ci)

  AimsData<double> Ci = _Wi.cross( _Wi.clone().transpose() ) ;        
  for( int i = 0 ; i < nbFrame ; ++i )
    Ci(i, i) += _sigma2 ;
    
  // verification Ci pas de valeurs negatives & symetrique
  bool sym = true ;
  for( i = 0 ; i < Ci.dimX() ; i++ ){
    if( Ci(i, i) < 0 ){ 
      cout << "pour i = " << i << ", Ci = " << Ci( i, i ) << " ! VALEUR NEGATIVE CHANGEE !" << endl ;
      Ci( i, i ) = - Ci( i, i ) ;
    }
    for( int j = 0 ; j < Ci.dimY() ; j++ ){
      if( Ci(i,j) != Ci(j,i) ){
	sym = false ;
	cout << "pour i,j = " << i << "," << j << " Ci(i,j) = " << Ci(i,j) << ",Ci(j,i) = " << Ci(j,i) << endl ; 
      }
    }
  }
  if( sym == false ) cout << "--> Ci pas symétrique !" << endl ;


  AimsData<double> u3 = Ci.clone();
  AimsData<double> v3( Ci.dimY(), Ci.dimY() );
  AimsSVD< double > svd3 ;
  svd3.setReturnType( AimsSVD<double>::MatrixOfSingularValues ) ;
  AimsData<double> w3 = svd3.doit( u3, &v3 );
  ts = s * w3.maximum();
  n = w3.dimX();
  bool saveCi = false ;

//  cout << endl << "valeurs diagonale w3 = " ;
  for( i = 0 ; i < n ; i++ ){
//    cout << w3( i, i ) << " " ;
    if ( w3( i, i ) < 0. ){ 
      saveCi = true ;
      cout << endl << "pour i = " << i << ", valeur w3 = " << w3( i, i ) << " ! VALEUR NEGATIVE CHANGEE !" << endl ;
      w3( i, i ) = - w3( i, i ) ;
    }
    if( _sigma2 == 0. )
      detCi *= w3( i, i ) ;
    else
      detCi *= w3( i, i ) / _sigma2 ;
    if ( w3( i, i ) > ts )  w3( i, i ) = 1.0f / w3( i, i );
    else w3( i, i ) = 0.0f;
  }
  _invCi = v3.cross( w3.cross( u3.transpose() ) ) ;
//  cout << "_invCi = " << endl << _invCi << endl ;

  // sauvegarde de Ci dans un fichier
/*   if( saveCi == true ){ */
/*     ofstream ci_file, diff_file ; */
/*     ci_file.open( "Ci_file.txt" ) ; */
/*     w3_file.open( "w3_file.txt" ) ; */
/*     diff_file.open( "diff_file.txt" ) ; */

/*     bool newLine ; */
/*     cout << "dimensions de Ci = " << Ci.dimX() << " " << Ci.dimY() << endl ; */
/*     for( int j = 0 ; j < Ci.dimY() ; j++ ){ */
/*       newLine = true ; */
/*       for( int i = 0 ; i < Ci.dimX() ; i++ ){ */
/* 	if( newLine == true && j != 0 ) ci_file << endl ; */
/* 	ci_file << Ci( i, j ) << " " ; */
/* 	newLine = false ; */
/*       } */
/*     }   */
/*     ci_file << endl ; */
/*     ci_file.close() ; */

/*     // verification  */
/*     AimsData<double> prod = u3.cross( w3.cross( v3.clone().transpose()  ) ) ; */
/*     for( int j = 0 ; j < Ci.dimY() ; j++ ){ */
/*       newLine = true ; */
/*       for( int i = 0 ; i < Ci.dimX() ; i++ ){ */
/* 	if( newLine == true && j != 0 ) diff_file << endl ; */
/* 	diff_file << Ci(i,j) - prod(i,j) << " " ; */
/* 	newLine = false ; */
/*       } */
/*     }   */
/*     diff_file << endl ; */
/*     diff_file.close() ; */
/*   } */

    }
  
  // AJOUT 12/11: calcul de l'énergie pour la mixture i Ei = nbOfInd * sigma2
  //_energy = nbIndTemp * _sigma2 ;
  cout << "energy = " << _energy ;
    
  if( _sigma2 == 0. )
    _normFactor = 1 / pow( detCi, 0.5 ) ;
  else
    _normFactor = 1 / ( pow( detCi, 0.5 ) * pow( _sigma2 / noiseRef, nbFrame / 2. ) ) ;
  cout << " - detCi = " << detCi /*<< " - normFactor sans noiseRef = " << 1 / pow( detCi, 0.5 )*/ 
	 << " - normFactor = " << _normFactor << endl ;
 
  return _sigma2 ;
}

template <class T>
double
MixtureOfPPCA<T>::pTnComputation() 
{
  int nbInd = _individuals.dimX() ;
  int nbIndTemp = 0 ;
  double sumPtn = 0., prodPtn = 0., meanPtn = 0. , varPtn = 0. ;
  AimsData<double> pTni( _individuals.dimX() ) ;
  double Pi = 0. ;

  _nullPtnIndiv.clear() ;

  _pTn = 0 ;
  cout << endl ;
  for( int n = 0 ; n < nbInd ; ++n ){
    for( int c = 0 ; c < _nbOfClasses ; ++c ){
      Pi = _elements[c].getPi() ;
      pTni = _elements[c].getPtni() ;
      _pTn[n] +=  pTni[n] * Pi ;
    } 
    if( _pTn[n] != 0 ) 
      ++nbIndTemp ;
    else
      _nullPtnIndiv.push_back( n ) ;
  }
  
  _nbOfRejected = nbInd - nbIndTemp ;

  cout << "nb d'individus pris en compte = " << nbIndTemp << " - nb d'individus rejetes = " 
       << nbInd - nbIndTemp << endl ;
  
  // moyenne et variance des pTn
  double oldSumLnPTn = _logLikelihood ;
  _logLikelihood = 0. ;
  for( int n = 0 ; n < nbInd ; ++n ){
    if( _pTn[n] != 0. )
      _logLikelihood += log( _pTn[n] ) ;
    else
      _logLikelihood += -100000000. ;
    sumPtn += _pTn[n] ;
    prodPtn += _pTn[n] * _pTn[n] ;
  }
  meanPtn = sumPtn / nbIndTemp ;
  varPtn = prodPtn / nbIndTemp ;
  cout << endl << "CRITERIUM TO MAXIMIZE : LOG LIKELIHOOD = " << _logLikelihood << " and difference = " 
  	<< (_logLikelihood - oldSumLnPTn)/oldSumLnPTn << " for " 
  	<< nbInd - nbIndTemp << " rejections" << endl ;
  cout << "moyenne et variance des p(tn) = " << meanPtn << " - " << varPtn << endl ;
  
  return (_logLikelihood - oldSumLnPTn)/oldSumLnPTn ;
}

template <class T>
double 
MixtureOfPPCA<T>::distComputation() 
{
  AimsData<double> mean( _individuals.dimY() ) ;
  AimsData<double> invCi( _individuals.dimY(), _individuals.dimY() ) ; 
  AimsData<double> centeredIndivn( _individuals.dimY() ) ;
  AimsData<double> dist(1, 1) ;  
  AimsData<double> Rn( _individuals.dimX() ) ; 
  AimsData<double> Rni( _individuals.dimX(), _nbOfClasses ) ; 
  AimsData<int> results( _individuals.dimX() ) ; 
  double max = 0., global_dist = 0. ;
  
  for( int c = 0 ; c < _nbOfClasses ; ++c ){
    Rn = _elements[c].getRn() ;
    for( int ind = 0 ; ind < _individuals.dimX() ; ++ind )
      Rni(ind, c) = Rn[ind] ;
  }  

  for( int ind = 0 ; ind < _individuals.dimX() ; ++ind ){
    max = Rni( ind, 0 ) ;
    results[ind] = 1 ;
    for( int c = 1 ; c < _nbOfClasses ; ++c ){
      if( Rni(ind, c) > max ){ 
	max = Rni( ind, c ) ;
	results[ind] = c + 1 ;
      }
    }
    mean = _elements[results[ind]-1].getMean() ;
    invCi = _elements[results[ind]-1].getInvCi() ;
    for( int t = 0 ; t < _individuals.dimY() ; ++t )
      centeredIndivn[t] = _individuals(ind, t) - mean[t] ;
    dist = ( centeredIndivn.clone().transpose() ).cross( invCi.cross( centeredIndivn ) ) ;
    global_dist += dist(0,0) ;
  }

  return global_dist ;
}


template <class T>
bool 
MixtureOfPPCA<T>::classesVisualisation( int nbOfIterations, const string & fileOut, bool theEnd ) 
{
  bool res = true ;

// POUR TOUS LES INDIVIDUS
  double max = 0. ;
  AimsData<double> score( _individuals.dimX(), _nbOfClasses ) ; 
  AimsData<int> results( _individuals.dimX() ) ; 
  vector< list< int > > tempList( _nbOfClasses ) ;

  AimsData<double> Rni = getRni() ;

/*   for( int ind = 0 ; ind < _individuals.dimX() ; ++ind ){ */
/*     sumRn = 0. ; */
/*     for( int c = 0 ; c < _nbOfClasses ; ++c ) */
/*       sumRn += Rni( ind, c ) ; */
/*     for( int c = 0 ; c < _nbOfClasses ; ++c ) */
/*       score( ind, c) = Rni(ind, c) / sumRn ; */
/*   } */

// CALCUL DES CLASSES FINALES
  for( int ind = 0 ; ind < _individuals.dimX() ; ++ind ){
    max = 0. ;
    results[ind] = 0 ;
    for( int c = 0 ; c < _nbOfClasses ; ++c ){
      if( Rni(ind, c) > max ){ 
	max = Rni( ind, c ) ;
	results[ind] = c + 1 ;
      }
    }
    if( results[ind] > 0 )
      _finalClasses[results[ind] - 1].push_back( ind ) ;
  }
 
 
// SAUVEGARDE DES CLASSES toutes les 10 itérations
//  AimsData< short > resultImage( data->dimX(), data->dimY(), data->dimZ() ) ;  // data inconnu ici
//  resultImage.setSizeXYZT( data->sizeX(), data->sizeY(), data->sizeZ() ) ;
  if( nbOfIterations% 30  == 0  || theEnd ){
    AimsData< short > resultImage( 128, 128, 111 ) ;
    AimsData< short > rejected( 128, 128, 111 ) ;
    AimsData< float > rniImage( 128, 128, 111, _nbOfClasses ) ;
    rejected.setSizeXYZT( 0.858086, 0.858086, 2.425 ) ;
    rejected = 0 ;
    rniImage.setSizeXYZT( 0.858086, 0.858086, 2.425 ) ;
    rniImage = 0. ;
    resultImage.setSizeXYZT( 0.858086, 0.858086, 2.425 ) ;
    resultImage = 0 ;
    Point3d theVoxel ;
    // recuperer le x,y,z du voxel dont la valeur est *iter
    for( short c = 0 ; c < _nbOfClasses ; ++c ){
      list<int>::const_iterator iter( _finalClasses[c].begin() ), 
	last( _finalClasses[c].end() ) ;
      while( iter != last ){
	theVoxel = _indPosVector[*iter] ;
	resultImage( theVoxel ) = c + 1 ;
	rniImage(theVoxel[0], theVoxel[1], theVoxel[2], c) = Rni( *iter, c ) ;
	++iter ;
      }
    }

    list<int>::const_iterator it( _nullPtnIndiv.begin() ), 
      lt( _nullPtnIndiv.end() ) ;
    while( it != lt ){
      theVoxel = _indPosVector[*it] ;
      rejected( theVoxel ) = 1 ;
      ++it ;
    }
    
    string iter_nb = carto::toString( nbOfIterations ) ;  
    Writer< AimsData< short > > writer( "iter" + iter_nb + "_classes_" + fileOut ) ;
    writer.write( resultImage ) ;

    Writer< AimsData< short > > writer3( "iter" + iter_nb + "_rejected__" + fileOut ) ;
    writer3.write( rejected ) ;
    
    Writer< AimsData< float > > writer2( "iter" + iter_nb + "_rni.ima" ) ;
    writer2.write( rniImage ) ;
  }
  
  cout << endl << "Nb d'individus a l'iteration " << nbOfIterations << " ds chaque classe = " ;
  for( int i = 0 ; i < _nbOfClasses ; ++i )
    cout << _finalClasses[i].size() << " "  ;
  cout << endl ;

  // A LA DERNIERE ITERATION
  if( theEnd ){

    // sauvegarder la matrice des individus _individuals (AimsData) et les classes finales (1 bucketMap)
    Writer< AimsData<T> > writerMatrix( "indivMatrix" ) ;
    Finder f ;
    string fformat = f.format() ;
    if( !writerMatrix.write( _individuals, false, &fformat ) )
      return( false ) ;

//    AimsBucket< int > bClasses ;
    AimsBucket< short > bClasses ;
    int count ;
    
    for( int c = 0 ; c < _nbOfClasses ; ++c ){
      count = 0 ;
      list<int>::const_iterator iter( _finalClasses[c].begin() ), 
	last( _finalClasses[c].end() ) ;
      while( iter != last ){
//	AimsBucketItem< int > item ;
	AimsBucketItem< short > item ;
	item.location() = Point3d( count, count, count ) ;
	item.value() = *iter ;
	bClasses[c].push_back( item ) ;
	++iter ;
	++count ;
      }
    }
//    Writer< AimsBucket< int > > writerClasses( fileOut + "_finalClasses" ) ;
    Writer< AimsBucket< short > > writerClasses( fileOut + "_finalClasses" ) ;
    writerClasses.write( bClasses, true, &fformat ) ;
    bClasses.clear() ;
  
    AimsData<double> classCorrelationMatrix( _nbOfClasses, _nbOfClasses  ) ;
    vector<double> classesNorm( _nbOfClasses, 0. ) ;
    multimap<double, Point2d> fusion ;
    multimap<double, int> split ;
    
    for( int i = 0 ; i < _nbOfClasses ; ++i )
      for( int n = 0 ; n < _individuals.dimX() ; ++n ) {
	for( int j = i ; j < _nbOfClasses ; ++j )
	  classCorrelationMatrix(i, j) += Rni( n, i ) * Rni(n, j ) ;
	
	classesNorm[i] += Rni( n, i ) * Rni( n, i ) ;
      }
    
    for( int i = 0 ; i < _nbOfClasses ; ++i ){
      classesNorm[i] = sqrt( classesNorm[i] ) ;
      double meanNorm = 0. ;
      for( int t  = 0 ; t < _elements[i].getMean().dimX() ; ++t )
	meanNorm += _elements[i].getMean()[t] ;
      
      split.insert( pair<double, int>( _elements[i].getSigma2() / meanNorm, i) ) ;
    }
    for( int i = 0 ; i < _nbOfClasses ; ++i )
      for( int j = i ; j < _nbOfClasses ; ++j ){
	if ( classesNorm[i] * classesNorm[j] > 0 )
	  classCorrelationMatrix(i, j) /= classesNorm[i] * classesNorm[j] ;
	else
	  classCorrelationMatrix(i, j) = 0. ;
	
	// To get a cost, we take the opposite of correlation matrix
	classCorrelationMatrix(j, i) = classCorrelationMatrix(i, j) ;
	if( i != j )
	  fusion.insert( pair<double, Point2d> ( classCorrelationMatrix(i, j), Point2d(i, j) ) ) ;
      }
    
    cout << "Classes à fusionner : " << endl ;

    count = 0 ;
    for( multimap<double, Point2d>::reverse_iterator rit = fusion.rbegin() ; rit != fusion.rend() && count < 5 ; ++rit, ++count )
      cout << (rit->second)[0] << " avec " << (rit->second)[1] << " : " << rit->first << endl ;
    
    cout << "Classe à séparer : " << endl ;
    count = 0 ;
    for( multimap<double, int>::reverse_iterator rit = split.rbegin() ; rit != split.rend() && count < 5  ; ++rit, ++count )
      cout << rit->second << " : " << rit->first << endl ;
  }
  return res ;
}


template <class T>
bool
MixtureOfPPCA<T>::distMatrixComputation() 
{
  bool res = true ;
  int count ;
  double sumDist ;
  AimsData<double> meanDistMatrix( _nbOfClasses, _nbOfClasses ) ;
  multimap< float, Point2d > fScore ;
  multimap< float, Point2d > meanFScore ;

  for( int fromC = 0 ; fromC < _nbOfClasses ; ++fromC ){
    for( int toC = 0 ; toC < _nbOfClasses ; ++toC ){
      count = 0 ;
      sumDist = 0. ;
      list<int>::const_iterator iter( _finalClasses[fromC].begin() ), 
	last( _finalClasses[fromC].end() ) ;
      while( iter != last ){
	sumDist += _distToClasses( *iter, toC ) ;
	++count ;
	++iter ;
      }
      meanDistMatrix( fromC, toC ) = sumDist / count ;     
    }
  }
/*   cout << endl << "matrice des distances moyennes inter classes = "  */
/*        << endl << meanDistMatrix << endl ; */

  // remplissage de la map de fusion fScore
  for( int i = 0 ; i < _nbOfClasses ; ++i )
    for( int j = 0 ; j < _nbOfClasses ; ++j )
      fScore.insert( pair< float, Point2d >( meanDistMatrix(i, j), Point2d(i, j) ) ) ;

  // remplissage de la map meanFScore
  for( int i = 0 ; i < _nbOfClasses ; ++i )
    for( int j = i+1 ; j < _nbOfClasses ; ++j )
      meanFScore.insert( pair< float, Point2d >( ( meanDistMatrix(i, j) + meanDistMatrix(j, i) )/2., 
						 Point2d(i, j) ) ) ;
      
  cout << endl << "elements de la map des scores = " << endl;
  multimap< float, Point2d >::iterator iter( fScore.begin() ), last( fScore.end() ) ;
  while( iter != last ){
    if( (*iter).first > 1.5 && (*iter).first < 100. ){
      cout << "  [" << (*iter).first << ", " << (*iter).second << "]" ;
      cout << "  sigmas = " << _sigma2Matrix[ (*iter).second[0] ] << " et " 
	   << _sigma2Matrix[ (*iter).second[1] ] << endl;
    }
    ++iter ;
  }

  cout << endl << "elements de la map des scores moyens 2 a 2 = " << endl;
  multimap< float, Point2d >::iterator iterM( meanFScore.begin() ), lastM( meanFScore.end() ) ;
  while( iterM != lastM ){
    if( exp( -0.5 * (*iterM).first ) > 0.01 ){
      cout << "  [" << (*iterM).first << ", " << (*iterM).second << "]" ;
      cout << "  exp(-score/2) = " << exp( -0.5 * (*iterM).first ) << endl ;
    }
    ++iterM ;
  }


  return res;
}


template <class T>
MixtureOfPPCA<T>::MixtureOfPPCA( int nbOfClasses, int significantNumberOfVp, 
				 int maxNbOfIterations, 
				 const AimsData<T>& individuals, const std::vector< Point3d > indPosVector, 
				 const std::vector< std::list <int> >& initialClasses,
				 const std::string & fileOut, int runNb, int iterationToUseOnlyCorrelatedIndiv ) : 
  _nbOfClasses( nbOfClasses ), _valid( true ), _significantNumberOfEigenValues( significantNumberOfVp ), _maxNbOfIterations( maxNbOfIterations ), _individuals( individuals ), _indPosVector( indPosVector ), _fileOut( fileOut ), _runNb( runNb ), _itToUseOnlyCorrelatedIndiv(iterationToUseOnlyCorrelatedIndiv)
{
  
  double Pi_init = 1. / (double)_nbOfClasses ;
  _elements.reserve( _nbOfClasses ) ;  
  _pTn = AimsData<double>( _individuals.dimX() ) ;
  _pTn = 1 ;
  _noiseRef = 1. ;
  _sigma2init = vector<double>( _nbOfClasses ) ;

  cout << endl ;
  
  for( int c = 0 ; c < _nbOfClasses ; ++c ){
    if( int( initialClasses[c].size() ) > _individuals.dimY() ) {
      PpcaAnalyserElement el( _significantNumberOfEigenValues ) ;
      cout << endl << "Initialization class " << c << " ..." ;

/*       cout << endl << "numeros des indivs de la classe " << c << " : " ; */
/*       count = 0 ; */
/*       list<int>::const_iterator iter( initialClasses[c].begin() ),  */
/* 	last( initialClasses[c].end() ) ; */
/*       while( iter != last && count < 10 ){ */
/* 	cout << *iter << " " ; */
/* 	++count ; */
/* 	++iter ; */
/*       } */

      // AJOUT: calcul d'un facteur noiseRef = bruit de la classe 0 pour normaliser normFactor
      if( c == 0 ){
	el.init( initialClasses[c], Pi_init, individuals, _noiseRef ) ;
	_noiseRef = el.getSigma2() ;
	cout << "Noise Ref = " << setprecision(3) << _noiseRef << endl ;
      }
      el.init( initialClasses[c], Pi_init, individuals, _noiseRef ) ;
      cout << "Class " << c << " initialized !" << endl ;
      _elements.push_back( el ) ;
    } 
    else
      cerr << "Not enough data for element " << c << endl ;
  }
   
}


template <class T>
bool
//const vector< vector< list< int > > >&
MixtureOfPPCA<T>::doIt()
{
  int nbOfIterations = 0 ;
  bool res ;
//  AimsData<double> distToClass( _individuals.dimX() ) ;
//  _distToClasses = AimsData<double>( _individuals.dimX(), _nbOfClasses ) ;
  _finalClasses = vector< list< int > >( _nbOfClasses ) ;
  _sigma2Matrix = AimsData<double>( _nbOfClasses ) ;
  double sumOfEnergies, criteria ;

  AimsData<double> imageOfMean( _nbOfClasses, _maxNbOfIterations, _individuals.dimY() ) ;
  AimsData<double> imageOfSigma2( _nbOfClasses, _maxNbOfIterations ) ;
  AimsData<double> imageOfPi( _nbOfClasses, _maxNbOfIterations ) ;
  AimsData<double> imageOfRniDiff( _nbOfClasses, _maxNbOfIterations ) ;
  AimsData<double> imageOfEnergy( _maxNbOfIterations ) ;
  AimsData<double> imageOfLogLikelihood( _maxNbOfIterations ) ;
  AimsData<double> imageOfLogLikelihoodProgression( _maxNbOfIterations ) ;

  _nbOfRejected = 0 ;
  int oldNbOfRejected = 0 ;
  bool notYetTheEnd = true ;
  
  do{
    for( int i = 0 ; i < _nbOfClasses ; ++i )    
      _finalClasses[i].clear() ;
    cout << endl << "Iteration n°: " << nbOfIterations /*<< endl*/ ;
    
    // 1ERE PARTIE
    cout << endl << "Calcul des p(tn|i) pour chaque classe... " << endl ;
    for( int i = 0 ; i < _nbOfClasses ; ++i ){
      cerr << "Class " << i << ": " ;
      if( _elements[i].isValid() )
	_elements[i].newStep1( _individuals, 0 /*(_itToUseOnlyCorrelatedIndiv < 0 ? -1 : nbOfIterations > _itToUseOnlyCorrelatedIndiv )*/ ) ;
    }

    oldNbOfRejected = _nbOfRejected ;
    
    // INTERMEDIAIRE - calcul de p(tn) = sum( p(tn/i)*p(i) )
    cout << endl << "Calcul des p(tn)..." ;
    imageOfLogLikelihoodProgression(nbOfIterations) = pTnComputation() ;
    imageOfLogLikelihood(nbOfIterations) = _logLikelihood ;
    
    // 2EME PARTIE
    cout << endl << "Calcul des autres parametres pour chaque classe..." ;

    //_noiseRef = _elements[0].newStep2( _pTn, _individuals, 1. ) ;
    criteria = 0. ;
    double oldSumOfEnergies = sumOfEnergies ;
    sumOfEnergies = 0. ;
    for( int i = 0 ; i < _nbOfClasses ; ++i ){
      cout << "Class " << i << ": " ;
      if( _elements[i].isValid() ){
	_elements[i].newStep2( _pTn, _individuals, /*_noiseRef*/1000000. ) ;
	//_sigma2Matrix[i] = _elements[i].getSigma2() ;
	//cout << "ok !" << endl ;
	sumOfEnergies += _elements[i].getEnergy() ;
	criteria += _elements[i].getSumDiff2Rni() ;
	
	//------ AJOUT 12/11 pour visualiser les paramètres sous forme d'AimsData -----
	for( int t = 0 ; t < _individuals.dimY() ; ++t )
	  imageOfMean( i, nbOfIterations, t ) = ( _elements[i].getMean() )[t] ;
	imageOfSigma2( i, nbOfIterations ) = _elements[i].getSigma2() ; 
	imageOfPi( i, nbOfIterations ) = _elements[i].getPi() ;
	imageOfRniDiff( i, nbOfIterations ) = _elements[i].getSumDiff2Rni() ; 
      }
    }

    cout << endl << "Critere d'arret = " << criteria << " or " << (oldSumOfEnergies - sumOfEnergies)/_elements.size() << endl ;
    imageOfEnergy( nbOfIterations ) = sumOfEnergies ; 

/*     Writer < AimsData<double> > wr( "Mean3D.ima" ) ; */
/*     wr.write( imageOfMean ) ; */
/*     Writer < AimsData<double> > wr2( "Sigma2D.ima" ) ; */
/*     wr2.write( imageOfSigma2 ) ; */
/*     Writer < AimsData<double> > wr3( "Pi2D.ima" ) ; */
/*     wr3.write( imageOfPi ) ; */
/*     Writer < AimsData<double> > wr4( "RniDiff2D.ima" ) ; */
/*     wr4.write( imageOfRniDiff ) ; */
/*     Writer < AimsData<double> > wr5( "Energy1D.ima" ) ; */
/*     wr5.write( imageOfEnergy ) ; */
/*     Writer < AimsData<double> > wr6( "logLikelihood.ima" ) ; */
/*     wr6.write( imageOfLogLikelihood ) ; */
/*     Writer < AimsData<double> > wr7( "logLikelihoodProgression.ima" ) ; */
/*     wr7.write( imageOfLogLikelihoodProgression ) ; */
    
    notYetTheEnd = (nbOfIterations < _maxNbOfIterations) && 
      ( (nbOfIterations < 10 ) || 
	!( (oldNbOfRejected == _nbOfRejected) && (fabs( imageOfLogLikelihoodProgression(nbOfIterations) ) < 0.0000001) ) ) ;
    
    // fonction pour calculer et visualiser les classes
    res = classesVisualisation( nbOfIterations, _fileOut, !notYetTheEnd ) ;
    
    ++nbOfIterations ;
  } while( notYetTheEnd ) ;
  
  // matrice des distances de chaque individu à chaque classe
  /*   for( int i = 0 ; i < _nbOfClasses ; ++i ){ */
  /*     distToClass = _elements[i].getDist() ; */
  /*     for( int n = 0 ; n < distToClass.dimX() ; ++n ){ */
  /*       // distToClass déjà au carré - division par la dimension */
      /*       _distToClasses(n, i) = distToClass[n] / _individuals.dimY() ;  */
      /*     } */
      /*   } */
      
      //  _valid = distMatrixComputation() ;
      
      //    return _finalClasses ;
      //    return _nullPtnIndiv ;
  return _valid ;
}



template <class T>
AimsData<double>
MixtureOfPPCA<T>::getRni()
{
  AimsData<double> Rn( _individuals.dimX() ) ; 
  AimsData<double> RnAll( _individuals.dimX(), _nbOfClasses ) ; 
  for( int c = 0 ; c < _nbOfClasses ; ++c ){
    Rn = _elements[c].getRn() ;
    for( int ind = 0 ; ind < _individuals.dimX() ; ++ind )
      RnAll(ind, c) = Rn[ind] ;
  }  

  return RnAll ;
}

#endif
