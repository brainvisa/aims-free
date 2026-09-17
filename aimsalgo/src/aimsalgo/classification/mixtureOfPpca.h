

#ifndef MIXTURE_OF_PPCA_H
#define MIXTURE_OF_PPCA_H

#include <cartodata/volume/volume.h>
#include <vector>
#include <string.h>

/* This code seems to be used... nowhere...
   Anyway it is not currently working: namespace problems will prevent
   compiling.
*/

namespace aims
{

class PpcaAnalyserElement 
{
 public:
  PpcaAnalyserElement( int significantNumberOfVp, bool useOnlyCorrIndiv = false ) ;
  ~PpcaAnalyserElement() {}
  
  template <class T>
  void init( const std::list<int>& selectedIndividuals, double initialPi, 
             const carto::rc_ptr<carto::Volume<T> >& individuals,
             double noiseRef = 1. ) ;
  template <class T>
  bool newStep1( const carto::rc_ptr<carto::Volume<T> >& indivMatrix,
                 bool useOnlyCorrIndiv ) ;
  template <class T>
  double newStep2( carto::rc_ptr<carto::Volume<double> > pTn,
                   const carto::rc_ptr<carto::Volume<T> >& indivMatrix,
                   double noiseRef = 1. ) ;
  
  const std::vector<double>& mean() const ;

  double getPi() { return _Pi ; }
  double getSigma2() const { return _sigma2 ; }
  double getEnergy() const { return _energy ; }
  double getSumDiff2Rni() const { return _sumDiff2Rni ; }
  const carto::VolumeRef<double>& getRn() { return _Rn ; }
  const carto::VolumeRef<double>& getDist() { return _dist ; }
  const carto::VolumeRef<double>& getPtni() { return _pTni ; }
  const carto::VolumeRef<double>& getMean() { return _mean ; }
  const carto::VolumeRef<double>& getInvCi() { return _invCi ; }
  int getExplosionIndex() { return _explosionIndex ; }
  bool isValid( ) const { return _valid ; }
  
 private:  
  // tabulation exponentielle
  static double * _exp ;
  
  template <class T>
    void doIt( const carto::rc_ptr<carto::Volume<T> >& individuals,
               int totalNbOfIndividuals, double noiseRef = 1. ) ;
  
  bool _useOnlyCorrIndiv ;            // mis � true par doIt
  int _significantNumberOfVp ;        // nb de valeurs propres significatives
  bool _computed ;                    // mis � true par doIt
  double _Pi ;
  carto::VolumeRef<double> _dist ;
  carto::VolumeRef<double> _pTni ;
  carto::VolumeRef<double> _Rn ;
  carto::VolumeRef<double> _mean ;       // meme dim que tn
  carto::VolumeRef<double> _Wi ;         // dim dxq
  carto::VolumeRef<double> _invMi ;      // dim qxq
  carto::VolumeRef<double> _invCi ;      //   idem
  double _sigma2 ;
  double _normFactor ;
  int _explosionIndex ;
  double _sumDiff2Rni ;          // somme des (Rni - previousRni) au carr� pour un i donn�
  
  double _energy ;
  bool _valid ;
} ;



template <class T>
class MixtureOfPPCA 
{
 public:
  MixtureOfPPCA( int nbOfClasses, int significantNumberOfVp, int maxNbOfIterations, 
		 const carto::rc_ptr<carto::Volume<T> >& individuals, const std::vector< Point3d > indPosVector,
		 const std::vector< std::list <int> >& initialClasses,
		 const std::string & fileOut,  int runNb, int iterationToUseOnlyCorrelatedIndiv = false ) ;
  ~MixtureOfPPCA() {}
  
  bool doIt() ;
//  const std::vector< std::vector< std::list< int > > >& doIt() ;

  double pTnComputation() ;
  bool distMatrixComputation() ;
  carto::VolumeRef<double> getRni() ;
  double distComputation() ;
  bool classesVisualisation( int nbOfIterations, const std::string & fileOut, bool theEnd ) ;
  
 private:
  int _nbOfClasses ;
  bool _valid ;
  int _significantNumberOfEigenValues ;
  int _maxNbOfIterations ;
  const carto::VolumeRef<T> _individuals ;
  const std::vector<Point3d> _indPosVector ;
  const std::string _fileOut ;
  int _runNb ;
  double _noiseRef ;
  std::vector< PpcaAnalyserElement > _elements ;
  carto::VolumeRef<double> _pTn ;
  std::vector<double> _sigma2init ;
  std::vector< std::list< int > > _finalClasses ;
  carto::VolumeRef<double> _distToClasses ;
  carto::VolumeRef<double> _sigma2Matrix ;
  std::list< int > _nullPtnIndiv ;
  double _logLikelihood ;
  int _nbOfRejected ;

  int _itToUseOnlyCorrelatedIndiv ;
} ;

}

#endif
