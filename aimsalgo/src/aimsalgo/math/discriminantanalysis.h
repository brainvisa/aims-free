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
    void doIt( const carto::rc_ptr<carto::Volume<T> > & individuals ) ;
      
    template <class T>
    void doIt( const std::list< Point3d>& selectedPoints, 
               const carto::rc_ptr<carto::Volume<T> >& data ) ;
      
    // set a priori class probability
    void setPIj( double PIj )
    { 
      _PIj = PIj ; 
      if( _computed )
        _lnAddFactor = -log( _detVarCov / ( _PIj * _PIj ) ) ; 
    }
      
    double posteriorProbability( const carto::rc_ptr<carto::Volume<double> >& individual,
                                 double pX ) const ;

    /* for comparison purposes only, because x prior probability is not taken 
       into account */
    double lnPosteriorProbability( const carto::rc_ptr<carto::Volume<double> >& individual ) const ;
    double distance( const carto::rc_ptr<carto::Volume<double> >& x ) const ;
  
    const carto::VolumeRef<double>& mean() const ;
    bool computed() const {return _computed ; }
       
  protected:  
    int _significantEV ;
    bool _computed ;
    double _dataScaleFactor ;
    double _probaScaleFactor ;
    double _PIj ;
      
    carto::VolumeRef<double> _mean ;
      
    carto::VolumeRef<double> _invVarCov ;

    double _detVarCov ;
    double _normFactor ;
    double _lnAddFactor ;
      
    // Temporary
    std::vector<Point3d> _indivPosition ;

  };

  
  template <class T> 
  class DiscriminantAnalysis
  {
  public:
    DiscriminantAnalysis( const carto::rc_ptr<carto::Volume<T> >& data,
                          const std::vector< std::list <Point3d> >& classes, 
			  int significantEV = -1, 
			  const std::vector<double>& PIj 
                          = std::vector<double>() ) ;
    ~DiscriminantAnalysis( ) {}
    
    std::vector<double> posteriorProbabilities( const carto::rc_ptr<carto::Volume<double> >& x,
                                                double px ) ;
    std::vector<double> andersonScores(
      const carto::rc_ptr<carto::Volume<double> >& x ) ;
    
    int affectedTo( const carto::rc_ptr<carto::Volume<double> >& x ) ;
    
    bool classification( const carto::rc_ptr<carto::Volume<T> >& dynamicImage,
                         const carto::rc_ptr<carto::Volume<byte> >& mask,
                         carto::rc_ptr<carto::Volume<short> >& segmented ) ;
    bool fuzzyClassification( const carto::rc_ptr<carto::Volume<T> >& dynamicImage,
                              const carto::rc_ptr<carto::Volume<byte> >& mask,
                              carto::rc_ptr<carto::Volume<float> >& fuzzySegmented,
                              const carto::rc_ptr<carto::Volume<double> > &indivPriorProbabilities =
                              carto::rc_ptr<carto::Volume<double> >() ) ;
    
  private:
    int _significantEV ;
    const std::vector< std::list< Point3d > >& _classes ;
    const carto::VolumeRef<T> _data ;
    std::vector<double> _PIj ;
    
    std::vector<DiscriminantAnalysisElement> _discrElements ;
  } ;
}

#endif
