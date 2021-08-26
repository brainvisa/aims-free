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

#include <aims/math/pca_d.h>

using namespace std;
using namespace aims;
using namespace carto;


AimsPCA::AimsPCA( int significantNumberOfVp, bool normalize, bool center ) : 
  _validPca(true), _computed(false), _matricesComputed(false), 
     _center(center), _normalize(normalize), _significantNumberOfVp(significantNumberOfVp), 
     _minimalInertia(1.0)
{
  
}

float
AimsPCA::noiseVariance( float& meanNorm2 )
{
  if( _eigenValues.empty() )
    throw runtime_error("Youn should run pca first") ;

  double sigma2 = 0. ;
  for( unsigned int i = _significantNumberOfVp  ; i < _eigenValues.size() ; ++i )
    sigma2 += _eigenValues[i] ;
  
  sigma2 /= double(_eigenValues.size() - _significantNumberOfVp ) ;

  meanNorm2 = 0. ;
  for( unsigned int t = 0 ; t < _mean.size() ; ++t )
    meanNorm2 += _mean[t] * _mean[t] ;
  meanNorm2 /= _mean.size() ;
  
  return sigma2 ;
}

double 
AimsPCA::totalVariance( float& meanNorm ) const
{
  if( _eigenValues.empty() )
    throw runtime_error("Youn should run pca first") ;

  double sigma2 = 0. ;
  for( unsigned int i = 0  ; i < _eigenValues.size() ; ++i )
    sigma2 += _eigenValues[i] ;
  
  sigma2 /= double(_eigenValues.size() ) ;

  meanNorm = 0. ;
  for( unsigned int t = 0 ; t < _mean.size() ; ++t )
    meanNorm += _mean[t] * _mean[t] ;
  meanNorm /= _mean.size() ;

  return sigma2 ;
}

void
AimsPCA::setSignificantNumberOfVp( int significantNumberOfVp )
{
  _significantNumberOfVp = significantNumberOfVp ;
  _minimalInertia = 1. ;
}

void
AimsPCA::setMinimalInertia( float minimalInertia )
{
  if( minimalInertia >= 0. && minimalInertia <= 1. ){
    _minimalInertia = minimalInertia ;
    _significantNumberOfVp = -1 ;
  } else
    throw runtime_error( "Wrong range of parameter minimalInertia !") ;
}

float 
AimsPCA::significantInertia( )
{ 
  if( !_computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;
      
  float currentInertia = 0., totalInertia = 0. ;
  for( unsigned int t = 0 ; t < _eigenValues.size() ; ++t ){
    totalInertia += _eigenValues[t] ;
    if( _significantNumberOfVp != -1 && int(t) < _significantNumberOfVp )
      currentInertia += _eigenValues[t] ;
  }
  
  if( _significantNumberOfVp != -1 )
    return currentInertia / totalInertia ;

  int t = 0 ;
  while( currentInertia < _minimalInertia ){
    currentInertia += _eigenValues[t] / totalInertia ;
    ++t ;
  }
  return currentInertia ;
}

float 
AimsPCA::relativeUnreconstructedVariance( )
{
  if( !_computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;
      
  float currentInertia = 0., totalInertia = 0. ;
  for( unsigned int t = 0 ; t < _eigenValues.size() ; ++t ){
    totalInertia += _eigenValues[t]  ;
  }
  
  int t = 0 ;
  while( t < _significantNumberOfVp ){
    currentInertia += _eigenValues[t] / totalInertia ;
    ++t ;
  }
  return 1. - currentInertia ;
}

float 
AimsPCA::unreconstructedVariance( )
{
  if( !_computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;
      
  float currentInertia = 0., totalInertia = 0. ;
  for( unsigned int t = 0 ; t < _eigenValues.size() ; ++t ){
    totalInertia += _eigenValues[t]  ;
  }
  
  int t = 0 ;
  while( t < _significantNumberOfVp ){
    currentInertia += _eigenValues[t] ;
    ++t ;
  }
  
  return totalInertia - currentInertia ;
}


void 
AimsPCA::computeErrorAndProjMatrices()
{
  int nbFrame = _eigenVectors->getSizeX() ;
  // Calcul des matrices d'erreurs et de projection
  _projectionMatrix = VolumeRef< float >( nbFrame, nbFrame, 1, 1,
                                          carto::AllocatorContext::fast() );
  _selectedEigenVectors = VolumeRef<float>(
      nbFrame, _significantNumberOfVp, 1, 1, carto::AllocatorContext::fast() );
  _selectedEigenVectorsTr = VolumeRef<float>(
      _significantNumberOfVp, nbFrame, 1, 1, carto::AllocatorContext::fast());
  
  for( int i = 0 ; i < _significantNumberOfVp ; ++i )
    for( int t = 0 ; t < nbFrame ; ++t )
    {
      _selectedEigenVectors(t, i) = _eigenVectors( t, i ) ;
      _selectedEigenVectorsTr(i, t) = _eigenVectors( t, i ) ;
    }
  for(int ord = 0 ; ord < _significantNumberOfVp ; ++ord )
    _projectionMatrix = matrix_product( _selectedEigenVectors,
                                        _selectedEigenVectorsTr );

  _errorMatrix = VolumeRef<float>( nbFrame, nbFrame, 1, 1,
                                   carto::AllocatorContext::fast() );
  for( int t = 0 ; t < nbFrame ; ++t )
    _errorMatrix(t, t) = 1. ;
  
  for(int ord = 0 ; ord < _significantNumberOfVp ; ++ord )
    _errorMatrix = _errorMatrix
      - matrix_product( _projectionMatrix, transpose( _projectionMatrix ) );
  
  _matricesComputed = true ;
}

const VolumeRef<float>&
AimsPCA::projectionMatrix( ) 
{
  if( ! _computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;
    

  if( !_matricesComputed ) 
    computeErrorAndProjMatrices() ;
  return _projectionMatrix ;
}

const VolumeRef<float>&
AimsPCA::reconstructionErrorMatrix( )
{
  if( ! _computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;

  if( !_matricesComputed ) 
    computeErrorAndProjMatrices() ;
  return _errorMatrix ;
}


const vector<float>& 
AimsPCA::mean() const
{
  if( ! _computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;

  return _mean ;
}

const vector<float>& 
AimsPCA::var() const
{
  if( ! _computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;

  return _var ;
}
float 
AimsPCA::noiseInertia()
{
  if( ! _computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;

  return (1. - significantInertia()) ;
}

const vector<float>&  
AimsPCA::eigenValues( ) const
{
  if( ! _computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;
  
  return  _eigenValues ;
}

const VolumeRef<float>&
AimsPCA::eigenVectors( ) const 
{
  if( ! _computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;
  
  return _eigenVectors ;
}

VolumeRef<float>
AimsPCA::selectedEigenVectors( ) const 
{
  if( ! _computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;
  
  VolumeRef<float> selectedEigenVec(
    _eigenVectors->getSizeY(), _significantNumberOfVp, 1, 1,
    carto::AllocatorContext::fast() );

  for( int i = 0 ; i < _significantNumberOfVp ; ++i )
    for( int t = 0 ; t < _eigenVectors->getSizeY(); ++t )
      selectedEigenVec(t, i) = _eigenVectors(t, i) ;
  
  return selectedEigenVec ;
}


VolumeRef<float>
AimsPCA::projection( const rc_ptr<Volume<float> > & individual )
{
  if( ! _computed )
    throw runtime_error( "You must doIt first, parameter not yet computed !") ;

  if( !_matricesComputed ) 
    computeErrorAndProjMatrices() ;
  
  return matrix_product( _selectedEigenVectorsTr,
                         matrix_product( _selectedEigenVectors, individual ) );
}

float 
AimsPCA::reconstructionError2( const rc_ptr<Volume<float> > & individual )
{
  VolumeRef<float> residual
    = VolumeRef<float>( individual ) - projection( individual ) ;
  cout << "After proj " << endl ;
  
  float err2 = 0. ;
  for( int i = 0 ; i < residual->getSizeX() ; ++i )
    err2 += residual(i) * residual(i) ;
  return err2 ;
}

// template class AimsPCA<int8_t> ;
// template class AimsPCA<uint8_t> ;
// template class AimsPCA<int16_t> ;
// template class AimsPCA<uint16_t> ;
// template class AimsPCA<int32_t> ;
// template class AimsPCA<uint32_t> ;
// template class AimsPCA<float> ;
// template class AimsPCA<double> ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// template class AimsPCA<char> ;
#endif
// template class AimsPCA<long> ;
// template class AimsPCA<unsigned long> ;
