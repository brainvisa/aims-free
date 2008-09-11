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


#ifndef AIMS_MATH_PPCA_H
#define AIMS_MATH_PPCA_H

#include <aims/def/general.h>
#include <aims/data/fastAllocationData.h>

namespace aims
{

  class ProbabilisticPcaElement 
  {
  public:
    ProbabilisticPcaElement( int nbOfSignificativeEV, double PIj = 1. ) ;
    virtual ~ProbabilisticPcaElement( ) {}
      
    template <class T>
    void doIt( const AimsData<T>& individuals, double distanceRef = 0. ) ;
      
    template <class T>
    void doIt( const std::list< Point3d>& selectedPoints, 
               const AimsData<T>& data, double distanceRef = 0. ) ;
      
    // set a priori class probability
    void setPIj( double PIj ) 
    {
      _PIj = PIj ; 
      if( _computed )
        _lnAddFactor = log( _detCi / ( _PIj * _PIj ) ) ;
    }
      
    inline double distance( const AimsData<double>& individual ) ;

    inline double posteriorProbability( const AimsData<double>& individual, 
					double pX ) ;

//     double normalizedPosteriorProbability( const AimsData<double>& individual, 
// 						  double pX, double normX ) ;

    /* for comparison purposes only, because x prior probability is not taken 
       into account */
    inline double lnPosteriorProbability( const AimsData<double>& individual ) ;
      
    const AimsData<double>& mean() const { return _mean ; }
    double noiseVariance() const
    { return _Sigma2 ; }
      
    template <class T>
    double noiseVariance( const AimsData<T>& individuals, 
                          double& normMean ) const ;
      
    double meanNorm() { return _meanNorm ; }

    const AimsData<double>& eigenValues() { return _EValues ; }
    const AimsData<double>& selectedEigenVectors() { return _EVect ; }

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
      
    AimsFastAllocationData<double> _mean ;
    double _meanMean ;
    double _meanNorm ;
    AimsFastAllocationData<double> _x ;
    AimsFastAllocationData<double> _xT ;
    
    AimsFastAllocationData<double> _Wi ;
    AimsFastAllocationData<double> _invCi ;
    AimsFastAllocationData<double> _EValues ;
    AimsFastAllocationData<double> _EVect ;
    double _Sigma2 ;
      
    double _detCi ;
    double _normFactor ;
    double _lnAddFactor ;
  };
  
  
  template <class T> 
  class ProbabilisticPca
  {
  public:
    ProbabilisticPca( const AimsData<T>& data, 
                      const std::vector< std::list <Point3d> >& classes, 
		      int nbOfSignificantEigenValues, 
                      const std::vector<double>& PIj 
                      = std::vector<double>() ) ;
    ~ProbabilisticPca( ) {}
    
    std::vector<double> 
    posteriorProbabilities( const AimsData<double>& x, double px, 
                            std::vector<double>& maxProbaByClass ) ;
    std::vector<double> 
    andersonScores( const AimsData<double>& x, double px, 
                    std::vector<double>& maxProbaByClass ) ;
    int affectedTo( const AimsData<double>& x ) ;
    
    bool classification( const AimsData<T>& dynamicImage, 
			 const AimsData<byte>& mask,
			 AimsData<short>& segmented ) ;
    bool fuzzyClassification( const AimsData<T>& dynamicImage, 
			      const AimsData<byte>& mask,
			      AimsData<float>& fuzzySegmented,
			      double thresholdOnMaxPercentage = 0., 
			      double andersonScoreThreshold = 0.2,
			      const AimsData<double>& indivPriorProbabilities 
                              = aims::AimsFastAllocationData<double>() ) ;

    
    float posteriorProbability( const AimsData<double>& x, float pX, unsigned int classNb )
    { 
      if( classNb >= _discrElements.size() )
	throw std::runtime_error("Class number exceeds number of classes") ;
      else
	return _discrElements[classNb].posteriorProbability(x, pX) ;
    }
    
    double pX(const AimsData<double>& x ) ;

    short nbOfClasses() const { return _discrElements.size() ; }
    
  private:
    double weight( double norm2, int classe, float tolerance ) ;
    double wienerFilter( double sigma2, double norm2, double factor ) ;
    const std::vector< std::list< Point3d > >& _classes ;
    const AimsData<T>& _data ;
    double _distanceRef ;
    std::vector<double> _PIj ;
    int _nbOfSignificantEigenValues ;
    std::vector<ProbabilisticPcaElement> _discrElements ;
  } ;
}

double 
aims::ProbabilisticPcaElement::distance( const AimsData<double>& x )
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
  
//   for( int t = 0 ; t < indiv.dimX() ; ++t ){
//     _xT(1, t) = indiv(t) - _mean(t) ;
//     _x(t) = indiv(t) - _mean(t) ;
//   }
  
  double distance = 0. ;
  for(int i = 0 ; i < x.dimX() ; ++i ){
    double sum = 0. ;
    for( int j = 0 ; j < x.dimX() ; ++j )
      sum += _invCi(i, j) * ( x(j) - _mean(j) ) ;
    distance += ( x(i) - _mean(i) ) * sum ;
  }
//   double distance = _xT.cross( _invCi.cross( _x ) )(0, 0) ;

  if( distance < 0. )
    std::cout << "Distance =  " << distance << std::endl ;

  return distance - _distanceRef ;
}


double 
aims::ProbabilisticPcaElement::posteriorProbability( const AimsData<double>& x, 
					       double pX )
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
  for(int i = 0 ; i < x.dimX() ; ++i ){
    double sum = 0. ;
    for( int j = 0 ; j < x.dimX() ; ++j )
      sum += _invCi(i, j) * ( x(j) - _mean(j) ) ;
    distance += ( x(i) - _mean(i) ) * sum ;
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
aims::ProbabilisticPcaElement::lnPosteriorProbability( const AimsData<double>& x )
{
  if( ! _computed ){
    std::cerr << "ProbabilisticPcaElement::lnPosteriorProbability : " 
	      << std::endl ;
    throw std::runtime_error( "You must doIt first, parameter not yet " 
			      "computed !") ;
  }

  if( !_valid ) 
    return -std::numeric_limits<double>::max() ;


  double distance = 0. ;
  for(int i = 0 ; i < x.dimX() ; ++i ){
    double sum = 0. ;
    for( int j = 0 ; j < x.dimX() ; ++j )
      sum += _invCi(i, j) * ( x(j) - _mean(j) ) ;
    distance += ( x(i) - _mean(i) ) * sum ;
  }
//   double distance = _xT.cross( _invCi.cross( _x ) )(0, 0) ;

  if( distance < 0. )
    std::cout << "Distance =  " << distance << std::endl ;

//   double distance = xCentered.clone( ).transpose().cross
//     ( _invCi.cross( xCentered ) )(0, 0) ;
  //std::cout << "distance = " << distance << "\tNormFactor = " 
  // << _lnAddFactor << std::endl ;
  return (  _distanceRef - distance  /*/ (x.dimX() - _nbOfSignificantEV )*/ + _lnAddFactor ) ;
}

#endif


