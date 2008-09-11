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



#include <aims/classification/softDecSimilarityCompAnalysis.h>
#include <aims/io/finder.h>
#include <aims/io/writer.h>
#include <aims/vector/vector.h>
#include <aims/math/gausslu.h>
#include <aims/math/random.h>
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




SoftDecisionSimilarComponent::SoftDecisionSimilarComponent( int nbClasses, int nbVar ) : 
  _nbClasses(nbClasses), _nbVar( nbVar ), _isInit(false)
{
  
}

SoftDecisionSimilarComponent::~SoftDecisionSimilarComponent() {}

void 
SoftDecisionSimilarComponent::init()
{
  _isInit = true ;
  _pk = vector<double>( _nbClasses, 1. / _nbClasses ) ;
  _ek = vector< AimsData<double> >( _nbClasses ) ;
  
  _newpk = vector<double>( _nbClasses, 1. / _nbClasses ) ;
  _newek = vector< AimsData<double> >( _nbClasses ) ;

  for( int k = 0 ; k < _nbClasses ; ++k ){
    _ek[k] = AimsData<double>( _nbVar ) ;
    _newek[k] = AimsData<double>( _nbVar ) ;
    double norm2 = 0. ;
    for( int t = 0 ; t < _nbVar ; ++t ){
      _ek[k](t) = UniformRandom() ;
      _newek[k](t) = _ek[k](t) ;
      norm2 += _ek[k](t) * _ek[k](t) ;
    }
    for( int t = 0 ; t < _nbVar ; ++t ){
      _ek[k](t) /= sqrt(norm2) ;
      _newek[k](t) /= sqrt(norm2) ;
    }
  }
}

double 
SoftDecisionSimilarComponent::doIt( const AimsData<float>& indivMatrix )
{
  if( indivMatrix.dimY() != _nbVar ){
    cerr << "Incoherent input indiv matrix" << endl ;
    throw runtime_error("Incoherent input indiv matrix") ;
  }
  _nbInd = indivMatrix.dimX() ;
  _An = vector<double>( _nbInd, 1. ) ;
  _newAn = vector<double>( _nbInd, 1. ) ;
  _valids = vector<bool>( _nbInd ) ;
  
  _corrNbInd = 0 ;
  for(int ind = 0 ; ind < _nbInd ; ++ind ){
    _valids[ind] = false ;
    for( int t = 0 ; t < _nbVar ; ++t )
      if( indivMatrix(ind, t) != 0. )
	_valids[ind] = true ;
    if(_valids[ind] )
      ++_corrNbInd ;
  }
  
  for( int ind = 0 ; ind < _nbInd ; ++ind ){
    if( !_valids[ind] )
      continue ;
    double num = 0., den = 0. ;
    for( int k = 0 ; k < _nbClasses ; ++k ){
      double proj = projection( indivMatrix, ind, k, _newek[k] ) ;
      //cout << "proj = " << proj << endl ;
      num += proj * proj ;
      double noise = 0. ;
      for( int t = 0 ; t < _nbVar ; ++t ){
	double val = indivMatrix(ind, t) - proj * _newek[k](t) ;
	noise += val * val ;
      }
      den += noise ;
    }
  
    _An[ind] = _nbVar * num / den ;
  }
  
  bool notFinished = true ;
  int iter = 0 ;
  _Rnk = AimsData<double>(indivMatrix.dimX(), _nbClasses ) ;
  double lnL = 0. ;
  cout << endl ;
  while( notFinished ){
    expectationStep( indivMatrix ) ;
    maximisationStep( indivMatrix ) ;
    notFinished = !stopCriterion( 1.e-4 ) ;
    lnL =  lnLikelyhood(indivMatrix) ;
    cout << "\riteration " << iter << "lnKikelyhood = " << lnL << endl ;
    if( lnL > 0. || isnan( lnL ) )
      return 0. ;
    ++iter ;
    if (iter > 50 )
      notFinished = false ;
  }
  cout << "lnKikelyhood = " << lnL << endl ;
  return lnL ;
}


void 
SoftDecisionSimilarComponent::expectationStep( const AimsData<float>& indivMatrix ) 
{
  vector<double> val( _nbVar ) ;
  for( int ind = 0 ; ind < _nbInd ; ++ind ){
    if( !_valids[ind] )
      continue ;
    double maxLnProb = -_An[0] * (1. - similarity(indivMatrix, ind, 0) ) ;
    double normFact = 0. ;
    for( int k = 0 ; k < _nbClasses ; ++k ){
      val[k] = -_An[ind] * (1. - similarity(indivMatrix, ind, k) )  ;
      if( val[k] > maxLnProb )
	maxLnProb = val[k] ;
    }
    bool nonNullRnk = false ;
    for( int k = 0 ; k < _nbClasses ; ++k ){
      double value =  exp( val[k] - maxLnProb ) * _pk[k] ;
      _Rnk( ind, k ) = value ;
      if( value != 0. )
	nonNullRnk = true ;
      normFact += value ;
    }
    
    if( normFact != 0. )
      for( int k = 0 ; k < _nbClasses ; ++k )
	_Rnk( ind, k ) /= normFact ;
  }
}

void 
SoftDecisionSimilarComponent::maximisationStep( const AimsData<float>& indivMatrix )
{
  _newpk = std::vector<double>( _nbClasses, 0. ) ;
  _newek = std::vector< AimsData<double> >( _nbClasses ) ; 
  for( int k = 0 ; k < _nbClasses ; ++k )
    _newek[k] = AimsData<double>( _nbVar ) ;
  _newAn = std::vector<double>( _nbInd, 0. ) ;
  
  // maximization step : pk
  for( int k = 0 ; k < _nbClasses ; ++k ){
    for( int ind = 0 ; ind < _nbInd ; ++ind )
      if( _valids[ind]){
	_newpk[k] += _Rnk( ind, k ) ;
      }
    
    _newpk[k] /= _corrNbInd ;
  }
  
  // maximization step : ek
  for( int ind = 0 ; ind < _nbInd ; ++ind ){
    if( _valids[ind] ){
      double normInd = 0. ;
      for( int t = 0 ; t < _nbVar ; ++t )
	normInd += indivMatrix(ind, t) * indivMatrix(ind, t) ;
      normInd = sqrt( normInd ) ;
      if( normInd != 0. )
	for( int k = 0 ; k < _nbClasses ; ++k )
	  for( int t = 0 ; t < _nbVar ; ++t ){
	    _newek[k](t) += _Rnk(ind, k) * _An[ind] / normInd * indivMatrix(ind, t) ;
	  }
    }
  }
  for( int k = 0 ; k < _nbClasses ; ++k ){
    double norm = 0. ;
    for( int t = 0 ; t < _nbVar ; ++t ){
      norm += _newek[k](t) * _newek[k](t) ;
    }
    if( norm == 0. )
      cerr << "Bad ek estimation :  null vect " << endl ;
    else{
      norm = sqrt(norm) ;
      for( int t = 0 ; t < _nbVar ; ++t )
	_newek[k](t) /= norm ;
      
      //cout << "For " << k << " norm = " << norm << endl ;
    }
  }

  AimsData<double> eks( _nbClasses, _nbVar ) ;
  for( int k = 0 ; k < _nbClasses ; ++k )
    for( int t = 0 ; t < _nbVar ; ++t ){
      eks(k, t) = _newek[k](t) ;
    }
  Writer< AimsData<double> > wriEks( "ek.ima") ;
  wriEks.write( eks ) ;


  // maximization step : An
  for( int ind = 0 ; ind < _nbInd ; ++ind ){
    if( !_valids[ind] )
      continue ;
    double num = 0., den = 0. ;
    bool nonNullRnk = false ;
    for( int k = 0 ; k < _nbClasses ; ++k ){
      double proj = projection( indivMatrix, ind, k, _newek[k] ) ;
      //cout << "proj = " << proj << endl ;
      num += _Rnk(ind, k) * proj * proj ;
      double noise = 0. ;
      for( int t = 0 ; t < _nbVar ; ++t ){
	double val = indivMatrix(ind, t) - proj * _newek[k](t) ;
	noise += val * val ;
      }
      //noise /= _nbVar ;
      //cout << "noise = " << noise << endl ;
      den += _Rnk(ind, k) * noise ;
      //cout << "ind " <<ind << " : an = " << _An[ind] << endl ;
      if( _Rnk(ind,k) != 0. )
	nonNullRnk = true ;
    }
    _newAn[ind] = (_nbVar - 1) * num / den ;
    //cout << "ind " << ind << " : " << num << " / " << den << " = " << _An[ind] << endl ;
  }
}

double 
SoftDecisionSimilarComponent::lnLikelyhood( const AimsData<float>& indivMatrix )
{
  //cout << "lnLikely" << endl ;
  double lnLikely = 0. ;
  for( int k = 0 ; k < _nbClasses ; ++k ){
    double lnPk = log( _pk[k] ) ;
    for( int ind = 0 ; ind < _nbInd ; ++ind ){
      if( _valids[ind] )
	lnLikely += _Rnk( ind, k) 
	  * ( lnPk - _An[ind] * ( 1. - similarity( indivMatrix, ind, k ) ) ) ;
    }
  }
  return lnLikely ;
}



bool 
SoftDecisionSimilarComponent::stopCriterion( double threshold )
{
  double meanSqrDiffEk = 0. ;
  
  for( int k = 0 ; k < _nbClasses ; ++k )
    for( int t = 0 ; t < _nbVar ; ++t )
      meanSqrDiffEk += (_newek[k](t) - _ek[k](t) ) * (_newek[k](t) - _ek[k](t) ) ;
  
  meanSqrDiffEk /= _nbClasses ;
  
  double meanSqrDiffAn = 0. ;
  for( int ind = 0 ; ind < _nbInd ; ++ind ){
    if(_valids[ind] ){
      meanSqrDiffAn += (_newAn[ind] - _An[ind] ) * (_newAn[ind] - _An[ind] ) 
	/ (_An[ind] * _An[ind] ) ;
    
//     cout << ind << " An = " << _An[ind] << " && newAn =  " << _newAn[ind] 
// 	 << "   rapp = "<< (_newAn[ind] - _An[ind] ) * (_newAn[ind] - _An[ind] ) 
//       / (_An[ind] * _An[ind] ) << endl ;
    }
  }
  
  
  meanSqrDiffAn /= _corrNbInd ;
  
  double meanSqrDiffPk = 0. ;
  for( int k = 0 ; k < _nbClasses ; ++k )
    meanSqrDiffPk += ( _newpk[k] - _pk[k] ) * ( _newpk[k] - _pk[k] ) 
      / (_pk[k] * _pk[k] ) ;
  
  meanSqrDiffPk /= _nbClasses ;
  
//   cout << "Relarive square parameter differences :" << endl 
//        << "\t ek : " << meanSqrDiffEk << "\t pk : " << meanSqrDiffPk 
//        << "\t An : " << meanSqrDiffAn << endl ;
  
  _pk = _newpk ;
  for( int k = 0 ; k < _nbClasses ; ++k )
    for( int t = 0 ; t < _nbVar ; ++t )
      _ek[k](t) = _newek[k](t) ;
  _An = _newAn ;
    
  cout << "Parameters mean square difference : " << meanSqrDiffEk + meanSqrDiffAn + meanSqrDiffPk << endl ;
  if( meanSqrDiffEk + meanSqrDiffAn + meanSqrDiffPk < 3* threshold )
    return true ;
  
  return false ;
}
