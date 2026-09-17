

#ifndef SOFT_DECISION_SIMILAR_COMPONENT_H
#define SOFT_DECISION_SIMILAR_COMPONENT_H

#include <cartodata/volume/volume.h>
#include <vector>
#include <string.h>


class SoftDecisionSimilarComponent
{
public:
  SoftDecisionSimilarComponent( int nbClasses, int nbVar ) ;
  ~SoftDecisionSimilarComponent() ;

  void init( ) ;
  
  double doIt( const carto::rc_ptr<carto::Volume<float> >& indivMatrix ) ;
  
  const carto::VolumeRef<double>& getRnk() const
  {
    return _Rnk ;
  }
  
  std::vector<short> getSegmentationResult() const 
  {
    if( (!_isInit) || _An.size() == 0 )
      throw std::runtime_error( "Should doIt before getting result !" ) ;
    
    std::vector<short> segRes( _nbInd ) ;
    
    for( int ind = 0 ; ind < _nbInd ; ++ind ){
      if( _valids[ind ] ){
	short bestClass = 0 ;
	for( int k = 1 ; k < _nbClasses ; ++k ){
	  if( _Rnk(ind, k) > _Rnk(ind, bestClass) )
	    bestClass = k ;
	}
      
	segRes[ind] = bestClass ;
      } else
	segRes[ind] = -1 ;
    }
    
    return segRes ;
  }
  
private:
  int _nbClasses ;
  int _nbVar ;
  int _nbInd ;
  bool _isInit ;
  int _corrNbInd ;
  
  std::vector<short> _labels ;
  std::vector<bool> _valids ;
  
  double lnLikelyhood( const carto::rc_ptr<carto::Volume<float> >& indivMatrix ) ;
  void expectationStep( const carto::rc_ptr<carto::Volume<float> >& indivMatrix ) ;
  void maximisationStep( const carto::rc_ptr<carto::Volume<float> >& indivMatrix ) ;

  bool stopCriterion( double threshold ) ;

  inline double similarity( const carto::rc_ptr<carto::Volume<float> >& indivMatrix, int ind, int k ) ;
  inline double projection( const carto::rc_ptr<carto::Volume<float> >& indivMatrix, int ind, int k,
			    const carto::rc_ptr<carto::Volume<double> >& newek ) ;
  std::vector<double> _pk ; // class weight
  std::vector< carto::VolumeRef<double> > _ek ; // class mean normalized vect
  std::vector<double> _An ;
  std::vector<double> _newpk ; // class weight
  std::vector< carto::VolumeRef<double> > _newek ; // class mean normalized vect
  std::vector<double> _newAn ;

  carto::VolumeRef<double> _Rnk ; // posterior proba of classes regarding to data
} ;

double 
SoftDecisionSimilarComponent::similarity( const carto::rc_ptr<carto::Volume<float> >& indivMatrix, int ind, int k )
{
  double norm = 0., sim = 0. ;
  for( int t = 0 ; t < _nbVar ; ++t ){
    norm += indivMatrix->at( ind, t ) * indivMatrix->at( ind, t ) ;
    sim += indivMatrix->at( ind, t ) * _ek[k](t) ;
  }
  if( norm <= 0. )
    return 0. ;
  return sim / sqrt(norm) ;
}

double 
SoftDecisionSimilarComponent::projection(
  const carto::rc_ptr<carto::Volume<float> >& indivMatrix,
  int ind, int /* k */,
  const carto::rc_ptr<carto::Volume<double> >& newek )
{
  double proj = 0. ;
  for( int t = 0 ; t < _nbVar ; ++t ){
    proj += indivMatrix->at( ind, t ) * newek->at(t) ;
    //std::cout << "proj : " << proj << " with indMat = " << indivMatrix->at( ind, t ) << " && newek(k)(t) = " << newek->at(t) << std::endl ;
  }
  
  return proj ;
}


#endif
