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


#ifndef AIMS_MATH_PPCA_H
#define AIMS_MATH_PPCA_H

#include <aims/def/general.h>
#include <cartodata/volume/volume.h>

namespace aims
{

  class ProbabilisticPcaElement 
  {
  public:
    ProbabilisticPcaElement( int nbOfSignificativeEV, double PIj = 1. ) ;
    virtual ~ProbabilisticPcaElement( ) {}
      
    template <class T>
    void doIt( const carto::rc_ptr<carto::Volume<T> > & individuals,
               double distanceRef = 0. );
      
    template <class T>
    void doIt( const std::list< Point3d>& selectedPoints, 
               const carto::rc_ptr<carto::Volume<T> > & data,
               double distanceRef = 0. );
      
    // set a priori class probability
    void setPIj( double PIj ) 
    {
      _PIj = PIj ; 
      if( _computed )
        _lnAddFactor = log( _detCi / ( _PIj * _PIj ) ) ;
    }
      
    inline double distance(
      const carto::rc_ptr<carto::Volume<double> > & individual );

    inline double posteriorProbability(
      const carto::rc_ptr<carto::Volume<double> > & individual, double pX );

//     double normalizedPosteriorProbability( const carto::rc_ptr<carto::Volume<double> > & individual,
// 						  double pX, double normX ) ;

    /* for comparison purposes only, because x prior probability is not taken 
       into account */
    inline double lnPosteriorProbability(
      const carto::rc_ptr<carto::Volume<double> > & individual );
      
    const carto::VolumeRef<double>& mean() const { return _mean ; }
    double noiseVariance() const
    { return _Sigma2 ; }
      
    template <class T>
    double noiseVariance( const carto::rc_ptr<carto::Volume<T> > & individuals,
                          double& normMean ) const ;
      
    double meanNorm() { return _meanNorm ; }

    const carto::VolumeRef<double>& eigenValues() { return _EValues ; }
    const carto::VolumeRef<double>& selectedEigenVectors() { return _EVect ; }

    double normFactor() const { return _normFactor ; }

  protected:  
    static double * _exp ;
    inline double exponential( double x )
    {
      if( x < -750 )
        return 0. ;
      int approx = int( x * 10.) ;
      double res = (x * 10. - approx) ;
      return ( 1. - res ) * _exp[approx] + res * _exp[approx+1] ;
    }
    double _distanceRef ;
    bool _computed ;
    bool _valid ;
    int _nbOfSignificantEV ;
    double _PIj ;
      
    carto::VolumeRef<double> _mean ;
    double _meanMean ;
    double _meanNorm ;
    carto::VolumeRef<double> _x ;
    carto::VolumeRef<double> _xT ;
    
    carto::VolumeRef<double> _Wi ;
    carto::VolumeRef<double> _invCi ;
    carto::VolumeRef<double> _EValues ;
    carto::VolumeRef<double> _EVect ;
    double _Sigma2 ;
      
    double _detCi ;
    double _normFactor ;
    double _lnAddFactor ;
  };
  
  
  template <class T> 
  class ProbabilisticPca
  {
  public:
    ProbabilisticPca( const carto::rc_ptr<carto::Volume<T> > & data,
                      const std::vector< std::list <Point3d> >& classes, 
                      int nbOfSignificantEigenValues,
                      const std::vector<double>& PIj 
                      = std::vector<double>() ) ;
    ~ProbabilisticPca( ) {}
    
    std::vector<double> 
    posteriorProbabilities( const carto::rc_ptr<carto::Volume<double> >& x,
                            double px,
                            std::vector<double>& maxProbaByClass ) ;
    std::vector<double> 
    andersonScores( const carto::rc_ptr<carto::Volume<double> > & x, double px,
                    std::vector<double>& maxProbaByClass ) ;
    int affectedTo( const carto::rc_ptr<carto::Volume<double> > & x ) ;
    
    bool classification( const carto::rc_ptr<carto::Volume<T> > & dynamicImage,
                         const carto::rc_ptr<carto::Volume<byte> >& mask,
                         carto::rc_ptr<carto::Volume<short> >& segmented );
    bool fuzzyClassification(
      const carto::rc_ptr<carto::Volume<T> > & dynamicImage,
      const carto::rc_ptr<carto::Volume<byte> > & mask,
      carto::rc_ptr<carto::Volume<float> > & fuzzySegmented,
      double thresholdOnMaxPercentage = 0.,
      double andersonScoreThreshold = 0.2,
      const carto::rc_ptr<carto::Volume<double> > & indivPriorProbabilities
                  = carto::VolumeRef<double>() ) ;

    
    float posteriorProbability(
      const carto::rc_ptr<carto::Volume<double> > & x, float pX,
      unsigned int classNb )
    { 
      if( classNb >= _discrElements.size() )
        throw std::runtime_error("Class number exceeds number of classes") ;
      else
        return _discrElements[classNb].posteriorProbability(x, pX) ;
    }
    
    double pX(const carto::rc_ptr<carto::Volume<double> >& x ) ;

    short nbOfClasses() const { return _discrElements.size() ; }
    
  private:
    double weight( double norm2, int classe, float tolerance ) ;
    double wienerFilter( double sigma2, double norm2, double factor ) ;
    const std::vector< std::list< Point3d > >& _classes ;
    const carto::VolumeRef<T> _data;
    double _distanceRef ;
    std::vector<double> _PIj ;
    int _nbOfSignificantEigenValues ;
    std::vector<ProbabilisticPcaElement> _discrElements ;
  } ;
}

double 
aims::ProbabilisticPcaElement::distance(
  const carto::rc_ptr<carto::Volume<double> > & x )
{
  if( ! _computed )
    {
      std::cerr << "ProbabilisticPcaElement::distance : " << std::endl ;
      throw std::runtime_error( "You must doIt first, parameter not yet " 
				"computed !" ) ;
    }
  
  if( !_valid ){
    std::cerr << "Invalid ppca ! "<< std::endl ;
    return 10000000. ;
  }
  
//   for( int t = 0 ; t < indiv->getSizeX() ; ++t ){
//     _xT(1, t) = indiv(t) - _mean(t) ;
//     _x(t) = indiv(t) - _mean(t) ;
//   }
  
  double distance = 0. ;
  for(int i = 0 ; i < x->getSizeX() ; ++i )
  {
    double sum = 0. ;
    for( int j = 0 ; j < x->getSizeX() ; ++j )
      sum += _invCi(i, j) * ( x->at(j) - _mean(j) ) ;
    distance += ( x->at(i) - _mean(i) ) * sum ;
  }
//   double distance = _xT.cross( _invCi.cross( _x ) )(0, 0) ;

  if( distance < 0. )
    std::cout << "Distance =  " << distance << std::endl ;

  return distance - _distanceRef ;
}


double 
aims::ProbabilisticPcaElement::posteriorProbability(
  const carto::rc_ptr<carto::Volume<double> > & x, double pX )
{    
  if( ! _computed )
    {
      std::cerr << "ProbabilisticPcaElement::posteriorProbability : " 
		<< std::endl ;
      throw std::runtime_error( "You must doIt first, parameter not yet " 
				"computed !") ;
    }

  if( !_valid ) 
    return 0. ;
  
  
  double distance = 0. ;
  for(int i = 0 ; i < x->getSizeX() ; ++i ){
    double sum = 0. ;
    for( int j = 0 ; j < x->getSizeX() ; ++j )
      sum += _invCi(i, j) * ( x->at(j) - _mean(j) ) ;
    distance += ( x->at(i) - _mean(i) ) * sum ;
  }
  
  if( distance < 0. )
    std::cout << "Distance =  " << distance << std::endl ;

//   double distance = _xT.cross( _invCi.cross( _x ) )(0, 0)  ;
//   double distance = xCentered.clone( ).transpose().cross
//     ( _invCi.cross( xCentered ) )(0, 0)  ;
  int index = int( 0.5 * (distance - _distanceRef) * 100. 
		   + 0.5 ) ;
  if( index > 100000 )
    return 0. ;
  else if( index < 0. )
    return _normFactor / _exp[ -index ] * _PIj / pX ;
  
  return _normFactor * _exp[ index ] * _PIj / pX ;
}

double 
aims::ProbabilisticPcaElement::lnPosteriorProbability(
  const carto::rc_ptr<carto::Volume<double> > & x )
{
  if( ! _computed )
  {
    std::cerr << "ProbabilisticPcaElement::lnPosteriorProbability : " 
	      << std::endl ;
    throw std::runtime_error( "You must doIt first, parameter not yet " 
			      "computed !") ;
  }

  if( !_valid ) 
    return -std::numeric_limits<double>::max() ;


  double distance = 0. ;
  for(int i = 0 ; i < x->getSizeX() ; ++i )
  {
    double sum = 0. ;
    for( int j = 0 ; j < x->getSizeX() ; ++j )
      sum += _invCi(i, j) * ( x->at(j) - _mean(j) ) ;
    distance += ( x->at(i) - _mean(i) ) * sum ;
  }
//   double distance = _xT.cross( _invCi.cross( _x ) )(0, 0) ;

  if( distance < 0. )
    std::cout << "Distance =  " << distance << std::endl ;

//   double distance = xCentered.clone( ).transpose().cross
//     ( _invCi.cross( xCentered ) )(0, 0) ;
  //std::cout << "distance = " << distance << "\tNormFactor = " 
  // << _lnAddFactor << std::endl ;
  return (  _distanceRef - distance  /*/ (x->getSizeX() - _nbOfSignificantEV )*/ + _lnAddFactor ) ;
}

#endif


