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


#ifndef AIMS_MATH_DISCRIMINANTANALYSIS_H
#define AIMS_MATH_DISCRIMINANTANALYSIS_H

#include <aims/def/general.h>
#include <vector>
#include <aims/data/fastAllocationData.h>

namespace aims
{

  class DiscriminantAnalysisElement 
  {
  public:
    DiscriminantAnalysisElement( int significantEV = -1, double PIj = 1. ) ;
    ~DiscriminantAnalysisElement( ) {}
      
    template <class T>
    void doIt( const AimsData<T>& individuals ) ;
      
    template <class T>
    void doIt( const std::list< Point3d>& selectedPoints, 
               const AimsData<T>& data ) ;
      
    // set a priori class probability
    void setPIj( double PIj )
    { 
      _PIj = PIj ; 
      if( _computed )
        _lnAddFactor = -log( _detVarCov / ( _PIj * _PIj ) ) ; 
    }
      
    double posteriorProbability( const AimsData<double>& individual, 
                                 double pX ) const ;

    /* for comparison purposes only, because x prior probability is not taken 
       into account */
    double lnPosteriorProbability( const AimsData<double>& individual ) const ;
    double distance( const AimsData<double>& x ) const ;
  
    const AimsData<double>& mean() const ;
    bool computed() const {return _computed ; }
       
  protected:  
    int _significantEV ;
    bool _computed ;
    double _dataScaleFactor ;
    double _probaScaleFactor ;
    double _PIj ;
      
    AimsFastAllocationData<double> _mean ;
      
    AimsFastAllocationData<double> _invVarCov ;

    double _detVarCov ;
    double _normFactor ;
    double _lnAddFactor ;
      
    // Temporary
    std::vector<Point3d> _indivPosition ;

    /*   vector<float> _projectionVector ; */
    /*   AimsData<float> _projectionMatrix ; */
    /*   AimsData<float> _errorMatrix ; */
    /*   vector<float> _eigenValues ; */
    /*   AimsData<float> _eigenVectors ; */
    /*   AimsData<float> _selectedEigenVectors ; */
    /*   AimsData<float> _selectedEigenVectorsTr ; */
      
    /*   int _significantNumberOfVp ; */
    /*   float _minimalInertia ; */
  };

  
  template <class T> 
  class DiscriminantAnalysis
  {
  public:
    DiscriminantAnalysis( const AimsData<T>& data, 
                          const std::vector< std::list <Point3d> >& classes, 
			  int significantEV = -1, 
			  const std::vector<double>& PIj 
                          = std::vector<double>() ) ;
    ~DiscriminantAnalysis( ) {}
    
    std::vector<double> posteriorProbabilities( const AimsData<double>& x, 
                                                double px ) ;
    std::vector<double> andersonScores( const AimsData<double>& x ) ;
    
    int affectedTo( const AimsData<double>& x ) ;
    
    bool classification( const AimsData<T>& dynamicImage,
                         const AimsData<byte>& mask,
                         AimsData<short>& segmented ) ;
    bool fuzzyClassification( const AimsData<T>& dynamicImage, 
                              const AimsData<byte>& mask,
                              AimsData<float>& fuzzySegmented, 
                              const AimsData<double> &indivPriorProbabilities =
                              AimsData<double>() ) ;
    
  private:
    int _significantEV ;
    const std::vector< std::list< Point3d > >& _classes ;
    const AimsData<T>& _data ;
    std::vector<double> _PIj ;
    
    std::vector<DiscriminantAnalysisElement> _discrElements ;
  } ;
}

#endif
