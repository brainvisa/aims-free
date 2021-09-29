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
