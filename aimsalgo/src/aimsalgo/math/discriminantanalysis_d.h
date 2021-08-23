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
#include <aims/math/svd.h>
#include <aims/math/mathelem.h>
#include <aims/math/random.h>
#include <aims/math/gausslu.h>
#include <aims/math/discriminantanalysis.h>
#include <aims/io/writer.h>

namespace aims
{

DiscriminantAnalysisElement::DiscriminantAnalysisElement( int significantEV, double PIj ) : 
  _significantEV(significantEV), _computed(false), _dataScaleFactor(1.), 
  _probaScaleFactor(1.), _PIj(PIj), _detVarCov(1.), 
  _normFactor(1.), _lnAddFactor(0.)
{
  
}

template <typename T>
void
DiscriminantAnalysisElement::doIt( const std::list< Point3d >& selectedPoints,
				   const AimsData<T>& data )
{
  aims::AimsFastAllocationData<T> individuals( std::max(int(selectedPoints.size()), 1), data.dimT() ) ;
  int i = 0 ;
  std::list< Point3d >::const_iterator iter( selectedPoints.begin() ), last( selectedPoints.end() ) ;
  Point3df mean ;
  
  _indivPosition = std::vector<Point3d>( selectedPoints.size() ) ;
  while( iter != last ){
    _indivPosition[ i ] = *iter ;
    for(int t = 0 ; t < data.dimT() ; ++t )
      individuals( i, t ) = T(data( (*iter)[0], (*iter)[1], (*iter)[2], t ) ) ;
    ++i ; ++iter ;
    //cout << "Indiv " << i << " = " << *iter << endl ;
    mean += Point3df( (*iter)[0], (*iter)[1], (*iter)[2] ) ;
  }
  //cout << "Center = " << mean / float( individuals.dimX() ) << endl ;
  doIt( individuals ) ;
}

template <typename T>
void 
DiscriminantAnalysisElement::doIt( const AimsData<T>& individuals )
{
  _computed = true ;
  int nbFrame = individuals.dimY() ;
  
  aims::AimsFastAllocationData<double> centeredIndivMatrix( individuals.dimX(), individuals.dimY() ) ;
  _mean = aims::AimsFastAllocationData<double>(individuals.dimY() ) ;
  _mean = 0 ;
  T val ;
  for( int ind = 0 ; ind < individuals.dimX() ; ++ind )
    for( int t = 0 ; t < individuals.dimY() ; ++t ){
      val = individuals( ind, t ) ;
      _mean[t] += val ;
    }
  
  //TEMP
  double meanMean = 0. ; 
  //END TEMP

  for( int t = 0 ; t < nbFrame ; ++t ){
    _mean[t] = _mean[t] / centeredIndivMatrix.dimX() ;
    for( int ind = 0 ; ind < centeredIndivMatrix.dimX() ; ++ind ){
      centeredIndivMatrix( ind, t ) = individuals( ind, t ) - _mean[t] ;  
      meanMean += centeredIndivMatrix( ind, t ) ;
    }
  }
    
  // Matrice des correlations
  aims::AimsFastAllocationData<double> matVarCov(centeredIndivMatrix.dimY(), centeredIndivMatrix.dimY() ) ;
  int                 x1, y1;
  ForEach2d( matVarCov, x1, y1 )
    {
      for(int k=0;  k < centeredIndivMatrix.dimX()  ;++k)
	matVarCov(x1, y1) += centeredIndivMatrix(k, x1) * centeredIndivMatrix(k, y1);	
      matVarCov(x1, y1) /= centeredIndivMatrix.dimX() - 1 ;
    }
  
  //Writer< AimsData<double> > wriMatVarCov("matVarCov.ima") ;
  //wriMatVarCov.write( matVarCov ) ;

  aims::AimsFastAllocationData< double > u = matVarCov.clone(); // because it is replaced on SVD output
  aims::AimsFastAllocationData< double > v( matVarCov.dimY(), matVarCov.dimY() );
  
  AimsSVD< double > svd2;
  carto::VolumeRef< double > w = svd2.doit( u.volume(), &v.volume() );
  //svd2.sort(u, w, &v) ;

  AimsData<double> invW( w.getSizeX(), w.getSizeY() ) ;
  
  // JUST FOR SOCIETY FOR MOLECULAR IMAGING
//   int indMax = 0 ;
  double sig2 = 0. ;
//   for( int i = 1 ; i < w.dimX() ; ++i )
//     if( w(i, i) > w(indMax, indMax) )
//       indMax = i ;
  for( int i = _significantEV ; i < w.getSizeX() ; ++i ){
    sig2 +=  w(i, i) ;
  }
  sig2 /= w.getSizeX() - _significantEV ;
  
  std::cout << "signif EV = " << _significantEV << " sig2 = " << sig2 << std::endl ;
  
  double lnSig2 = log(sig2) ;
  double sqrtSig2 = sqrt(sig2) ;
  int i, n = w.getSizeX();
  double lnDetVarCov = 0 ;
  _detVarCov = 1. ;
  for ( i = 0; i < _significantEV ; i++ )
    {
      lnDetVarCov += 0.5 * log( w( i, i ) ) ;
      _detVarCov *= sqrt(w( i, i )) ;
      invW( i, i ) = 1.0f / w( i, i );
    }
  for ( i = _significantEV ; i<n; i++ )
    {
      lnDetVarCov += 0.5 * lnSig2 ;
      _detVarCov *= sqrtSig2 ;
      w( i, i ) = sig2 ;
      invW( i, i ) = 1.0f / sig2 ;
    }

  // JUST FOR SOCIETY FOR MOLECULAR IMAGING END

  // Set the singular values lower than threshold to zero
//   double ts = s * w.maximum();
//   int i, n = w.dimX();
//   _detVarCov = 1. ;
//   for ( i=0; i<n; i++ )
//     {
//       _detVarCov *= w( i, i ) ;
//       if ( w( i, i ) > ts )  w( i, i ) = 1.0f / w( i, i );
//       else w( i, i ) = 0.0f;
//     }
  
  
  //cout << "Det Var Cov = " << _detVarCov << endl ;
  if( _detVarCov != 0. ){
    _invVarCov = (u.clone().transpose()).cross( invW.cross( u ) ) ;
    _normFactor = 1. / _detVarCov ;
    _lnAddFactor = -lnDetVarCov + log( _PIj * _PIj ) ;
    
    std::cout << "normFactor = " << _normFactor << "\t_lnAddFactor = " << _lnAddFactor << std::endl ;
    ForEach2d( matVarCov, x1, y1 )
      if( x1 < y1 ){
	double val = 0.5 * ( _invVarCov(x1, y1) + _invVarCov(y1, x1) ) ;
	_invVarCov(x1, y1) = _invVarCov(y1, x1) = val ;
      }
  } else{ 
    std::cerr << "Bad var cov matrix !" << std::endl ;
    throw std::runtime_error( "Bad var cov matrix !") ;
  }
}

double 
DiscriminantAnalysisElement::posteriorProbability( const AimsData<double>& x, 
						   double ) const
{ 
  if( ! _computed ){
    std::cerr << "You must doIt first, parameter not yet computed !"
      << std::endl ;
    
    throw std::runtime_error( "You must doIt first, parameter not yet computed !") ;
  }
//   aims::AimsFastAllocationData<double> xCentered(x.dimX()) ;
//   for( int t = 0 ; t < x.dimX() ; ++t )
//     xCentered(t) = ( x(t) - _mean(t) ) * _dataScaleFactor ;

  
  double distance = 0. ;
  for(int i = 0 ; i < x.dimX() ; ++i ){
    double sum = 0. ;
    for( int j = 0 ; j < x.dimX() ; ++j )
      sum += _invVarCov(i, j) * ( x(j) - _mean(j) ) ;
    //double term = ( x(i) - _mean(i) ) * sum ;
    distance += ( x(i) - _mean(i) ) * sum ;
  }
  
  return _normFactor * exp( -distance ) ;
}

double 
DiscriminantAnalysisElement::distance( const AimsData<double>& x ) const
{
  if( ! _computed ){
    std::cerr <<"You must doIt first, parameter not yet computed !"
      << std::endl ;
    throw std::runtime_error( "You must doIt first, parameter not yet computed !") ;
  }  
//   cout << "DiscriminantAnalysisElement::distance " << endl ;
//   cout << "inv var cov size = " << _invVarCov.dimX() << " , " <<  _invVarCov.dimY() << endl ;
  double distance = 0. ;
  for(int i = 0 ; i < x.dimX() ; ++i ){
//     cout << "i = " << i << endl ;
    double sum = 0. ;
    for( int j = 0 ; j < x.dimX() ; ++j ){
//       cout << "\tj = " << j << endl ;
      sum += _invVarCov(i, j) * ( x(j) - _mean(j) ) ;
    }
    //double term = ( x(i) - _mean(i) ) * sum ;
    distance += ( x(i) - _mean(i) ) * sum ;
  }
//   cout << "DiscriminantAnalysisElement::distance end " << endl ;
  
  return distance ;
}

double 
DiscriminantAnalysisElement::lnPosteriorProbability( const AimsData<double>& x ) const
{
  if( ! _computed ){
    std::cerr << "You must doIt first, parameter not yet computed !"
      << std::endl ;
    throw std::runtime_error( "You must doIt first, parameter not yet computed !") ;
  }
  double distance = 0. ;
  for(int i = 0 ; i < x.dimX() ; ++i ){
    double sum = 0. ;
    for( int j = 0 ; j < x.dimX() ; ++j )
      sum += _invVarCov(i, j) * ( x(j) - _mean(j) ) ;
    //double term = ( x(i) - _mean(i) ) * sum ;
    distance += ( x(i) - _mean(i) ) * sum ;
  }
  
//   double distanceTest = 0. ;
//   AimsData<double> Ux( x.dimX() ) ;
//   for(int i = 0 ; i < _U.dimX() ; ++i )
//     for( int j = 0 ; j < _U.dimY() ; ++j )
//       Ux(i) += _U(j, i) * (x(j) - _mean(i) ) ;
  
//   for( int i = 0 ; i < Ux.dimX() ; ++i )
//     distanceTest += Ux(i) * Ux(i) * _invEigenValues[i] ;
  
//   double distance = 0. ;
//   for(int i = 0 ; i < x.dimX() ; ++i ){
//     double sum = 0. ;
//     for( int j = 0 ; j < x.dimX() ; ++j )
//       sum += _invVarCov(i, j) * ( x(j) - _mean(j) ) ;
//     distance += ( x(i) - _mean(i) ) * sum ;
//     cout << i << "\txi = " << x(i) << "\tmean i = " << _mean(i) << 
//       " diff = " << x(i) - _mean(i) << " dist = " << distance << endl ;
//   }

  return -distance + _lnAddFactor ;
}


const AimsData<double>& 
DiscriminantAnalysisElement::mean() const
{
  if( ! _computed ){
    std::cerr <<"You must doIt first, parameter not yet computed !"
      << std::endl ;
    throw std::runtime_error( "You must doIt first, parameter not yet computed !") ;
  }
  return _mean ;
}

template <class T>
DiscriminantAnalysis<T>::DiscriminantAnalysis(
  const AimsData<T>& data,
  const std::vector< std::list< Point3d > >& classes,
  int significantEV, const std::vector<double>& PIj )
  : _significantEV(significantEV <= 0 ? data.dimT() : significantEV),
    _classes(classes), _data(data), _PIj(PIj)
{
  if( PIj.size() != 0 )
    if( PIj.size() == classes.size() )
      _PIj = PIj ;
    else{
      std::cerr << "Inconsistant data : number of classes and number of classes prior probabilities are different !" << std::endl ;

      throw std::runtime_error("Inconsistant data : number of classes and number of classes prior probabilities are different !") ;
    }
  else{
    _PIj.reserve( _classes.size() ) ;
    for( unsigned int c = 0 ; c < _classes.size() ; ++c )
      _PIj.push_back( 1./_classes.size() ) ;
  }
  //--------------------------------TEMP---------------------------------------------
/*   AimsData<double> indiv(12000, 3) ; */
/*   Point3dd mean( 56., 32., 100.) ; */
/*   Point3dd sigma( 20., 2., 10.) ; */
  
/*   for( int ind = 0 ; ind < indiv.dimX() ; ++ind ) */
/*     for( int t = 0 ; t < indiv.dimY() ; ++t ) */
/*       indiv(ind, t) = GaussianRandom ( mean[t], sigma[t] ) ; */
  
  //--------------------------------TEMP---------------------------------------------
  _discrElements.reserve( _classes.size() ) ;
  for( unsigned int c = 0 ; c < _classes.size() ; ++c ){
    DiscriminantAnalysisElement el( significantEV, _PIj[c] ) ;
    el.doIt( _classes[c], _data ) ;
/*     el.doIt( indiv ) ; */
    
    std::cout << "Class " << c << " done !" << std::endl ;
    
    _discrElements.push_back( el ) ;
  }
  std::cout << "Discriminant analysis initization completed" << std::endl ;
}

template <class T>
std::vector<double>
DiscriminantAnalysis<T>::posteriorProbabilities( const AimsData<double>& x, double pX )
{
  std::vector<double> res( _classes.size() ) ;
  for( unsigned int c = 0 ; c < _classes.size() ; ++c )
    res[c] = _discrElements[c].posteriorProbability( x, pX ) ;
  return res ;
}

template <class T>
std::vector<double>
DiscriminantAnalysis<T>::andersonScores( const AimsData<double>& x )
{
  std::vector<double> res( _classes.size() ) ;
  double sum = 0. ;
  for( unsigned int c = 0 ; c < _classes.size() ; ++c ){
    res[c] = _discrElements[c].posteriorProbability( x, 1. ) ;
    sum += res[c] ;
  }
  
  for( unsigned int c = 0 ; c < _classes.size() ; ++c )
    res[c] /= sum ;
  
  return res ;
}

template <class T>
int 
DiscriminantAnalysis<T>::affectedTo( const AimsData<double>& x )
{
  double lnP ;
  double lnPMax = _discrElements[0].lnPosteriorProbability( x ) ;
  int indMax = 0 ;
  for( unsigned int c = 0 ; c < _classes.size() ; ++c ){
    lnP = _discrElements[c].lnPosteriorProbability( x ) ;
    if( lnP > lnPMax  ){
      lnPMax = lnP ;
      indMax = c ;
    }
  }

  return indMax ;
}


template <class T>
bool
DiscriminantAnalysis<T>::classification( const AimsData<T>& dynamicImage, 
                                         const AimsData<byte>& mask,
					 AimsData<short>& segmented )
{
  int x, y, z ;
  if( dynamicImage.dimT() != _data.dimT() ) 
    return false ;
  if( segmented.dimX() != _data.dimX() &&  segmented.dimY() != _data.dimY() 
      && segmented.dimZ() != _data.dimZ() )
    {
      segmented = aims::AimsFastAllocationData<short> ( _data.dimX(), _data.dimY(), _data.dimZ() );
      segmented.setSizeXYZT( _data.sizeX(), _data.sizeY(), 
                             _data.sizeZ(), 1.0 ) ;
    }
  
  aims::AimsFastAllocationData<double> indiv( dynamicImage.dimT() ) ;
  ForEach3d( dynamicImage, x, y, z ){
    if( mask(x, y, z) ) {
      std::cout << "x : " << x << ", y : " << y << ", z : " << z << std::endl ;
      for( int t = 0 ; t < _data.dimT() ; ++t )
	indiv(t) = dynamicImage(x, y, z, t ) ;
      segmented(x, y, z) = affectedTo( indiv ) ;
    }
  }
  
  std::cout << "Discriminant analysis classification completed" << std::endl ;

  return true ;
}

template <class T>
bool
DiscriminantAnalysis<T>::fuzzyClassification( const AimsData<T>& dynamicImage, 
					      const AimsData<byte>& mask,
					      AimsData<float>& fuzzySegmented, 
					      const AimsData<double> & 
                                              indivPriorProbabilities )
{
  bool noPriorProba 
    =  ( indivPriorProbabilities.dimX( ) == 1
    && indivPriorProbabilities.dimY( ) == 1
    && indivPriorProbabilities.dimZ( ) == 1 );
  int count = 0 ;
  //double uniformPriorProba ;

  int x, y, z ;
  if( noPriorProba )
    {
      ForEach3d( mask, x, y, z )
        if( mask( x, y, z ) )
          ++count ;
      //uniformPriorProba = 1. / count ;
    }
  if( dynamicImage.dimT() != _data.dimT() ) 
    return false ;
  if( fuzzySegmented.dimX() != _data.dimX() 
      &&  fuzzySegmented.dimY() != _data.dimY() 
      && fuzzySegmented.dimZ() != _data.dimZ() &&
      fuzzySegmented.dimT() != int(_classes.size() ) )
    {
      fuzzySegmented = aims::AimsFastAllocationData<float> ( _data.dimX(), _data.dimY(), 
                                         _data.dimZ(), _classes.size() ) ;
      fuzzySegmented.setSizeXYZT( _data.sizeX(), _data.sizeY(), _data.sizeZ(), 
                                  1.0 ) ;
    }
  
  std::vector<double> probabilityRepartition ;
  aims::AimsFastAllocationData<double> indiv( dynamicImage.dimT() ) ;
  ForEach3d( dynamicImage, x, y, z ){
    if( mask(x, y, z) ) {
      //cout << "x : " << x << ", y : " << y << ", z : " << z << endl ;
      for( int t = 0 ; t < _data.dimT() ; ++t )
	indiv(t) = dynamicImage(x, y, z, t ) ;
      /*       probabilityRepartition = andersonScores( indiv ) ; */
      probabilityRepartition = posteriorProbabilities( indiv, 1. ) ;
      
      for( int c = 0 ; c < fuzzySegmented.dimT() ; ++c )
	fuzzySegmented(x, y, z, c) = probabilityRepartition[c] ;
    }
  }
  
  std::cout << "Discriminant analysis : fuzzy classification completed"
    << std::endl ;
  
  return true ;
}

}
