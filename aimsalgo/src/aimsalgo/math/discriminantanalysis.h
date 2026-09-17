
#ifndef AIMS_MATH_DISCRIMINANTANALYSIS_H
#define AIMS_MATH_DISCRIMINANTANALYSIS_H

#include <aims/def/general.h>
#include <vector>
#include <cartodata/volume/volume.h>

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
