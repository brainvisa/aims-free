
#ifndef AIMS_MATH_PCA_H
#define AIMS_MATH_PCA_H

#include <aims/def/general.h>
#include <cartodata/volume/volume.h>
#include <vector>


class AimsPCA 
{
public:
  AimsPCA( int significantNumberOfVp, bool normalize = false, bool center = true ) ;
  ~AimsPCA() {}
  
  template <class T>
    void doIt( const carto::rc_ptr<carto::Volume<T> > & individuals ) ;
  
  template <class T>
    void doIt( const std::list< Point3d>& selectedPoints, const carto::rc_ptr<carto::Volume<T> > & data ) ;
  
  
  float noiseVariance( float& meanNorm ) ;
  double totalVariance( float& meanNorm ) const ;
    
  float relativeUnreconstructedVariance( float& meanNorm ) ;
  void setSignificantNumberOfVp( int significantNumberOfVp ) ;
  void setMinimalInertia( float inertiaLimit ) ;
  
  float minimalInertia() const ;
  float significantNumberOfVp() const ;

  float significantInertia() ;
  float noiseInertia() ;
  float unreconstructedVariance( ) ;
  float relativeUnreconstructedVariance( ) ;
  carto::VolumeRef<float> projection(
    const carto::rc_ptr<carto::Volume<float> > & individual ) ;
  float reconstructionError2(
    const carto::rc_ptr<carto::Volume<float> > & individual ) ;
  
  const carto::VolumeRef<float>& projectionMatrix() ;
  const carto::VolumeRef<float>& reconstructionErrorMatrix() ;
  
  const std::vector<float>& eigenValues() const ;
  const carto::VolumeRef<float>& eigenVectors() const ;
  carto::VolumeRef<float> selectedEigenVectors() const ;
  
  const std::vector<float>& mean() const ;
  const std::vector<float>& var() const ;
  
  bool valid() const { return _validPca ; }
  
protected:
  void computeErrorAndProjMatrices() ;
  bool _validPca ;
  bool _computed ;
  bool _matricesComputed ;
  bool _center ;
  bool _normalize ;
  std::vector<float> _mean ;
  std::vector<float> _var ;
  
  std::vector<float> _projectionVector ;
  carto::VolumeRef<float> _projectionMatrix ;
  carto::VolumeRef<float> _errorMatrix ;
  std::vector<float> _eigenValues ;
  carto::VolumeRef<float> _eigenVectors ;
  carto::VolumeRef<float> _selectedEigenVectors ;
  carto::VolumeRef<float> _selectedEigenVectorsTr ;
  
  int _significantNumberOfVp ;
  float _minimalInertia ;
};

#endif
