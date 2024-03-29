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
