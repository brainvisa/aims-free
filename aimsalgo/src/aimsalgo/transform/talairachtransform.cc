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

#include <aims/transform/talairachtransform.h>

using namespace aims ;
using namespace std ;
using namespace carto ;

TalairachTransformation::TranslationComputation( ) :
  _motion( 0 ), _motionComputed(false), _computable(false), _withMask(false),
  _toNormalizedTemplate(true)
{
  
}

TalairachTransformation::TranslationComputation( std::vector<Point3df> first, 
						 AimsData<short> firstMask,
						 std::vector<Point3df> second, 
						 AimsData<short> secondMask,
					     TranslationComputation translationComputation) :
  _firstPoints(first), _secondPoints(second), _firstMask(firstMask), _secondMask(secondMask),
  _translationComputation(translationComputation), _motion( 0 ), _motionComputed(false), 
  _computable(true), _withMask(true), _toNormalizedTemplate(true)
{
  if( (firstMask.dimX() > 1 || firstMask.dimY() > 1 ||  firstMask.dimZ() > 1 ) && 
      (secondMask.dimX() > 1 || secondMask.dimY() > 1 ||  secondMask.dimZ() > 1) )
    _withMask = true ;
  
  if( first.empty() )
    _computable = false ;
  
  if( !second.empty() )
    _toNormalizedTemplate = false ;
}

TalairachTransformation::~TalairachTransformation()
{
  if( _motion )
    delete _motion ;
}

void 
TalairachTransformation::setFirstTalairachPoints( std::vector<Point3df> first )
{
  _firstPoints = first ;
  if( firstPoints.empty() )
    _computable = false ;
  else
    _computable = true ;
  _motionComputed = false ;
}

void 
TalairachTransformation::setFirstMask( AimsData<short> firstMask )
{
  _firstMask = firstMask ;
  if( (firstMask.dimX() > 1 || firstMask.dimY() > 1 ||  firstMask.dimZ() > 1 ) && 
      (_secondMask.dimX() > 1 || _secondMask.dimY() > 1 ||  _secondMask.dimZ() > 1) )
    _withMask = true ;
  else 
    _withMask = false ;
  _motionComputed = false ;
}

void TalairachTransformation::setSecondTalairachPoints( std::vector<Point3df> second )
{
  _secondPoints = second ;
  if( secondPoints.empty() )
    _toNormalizedTemplate = false ;
  else
    _toNormalizedTemplate = true ;
  _motionComputed = false ;
}


void TalairachTransformation::setSecondMask( AimsData<short> secondMask )
{
  if( (_firstMask.dimX() > 1 || _firstMask.dimY() > 1 ||  _firstMask.dimZ() > 1 ) && 
      (secondMask.dimX() > 1 || secondMask.dimY() > 1 ||  secondMask.dimZ() > 1) )
    _withMask = true ;
  else 
    _withMask = false ;
  _motionComputed = false ;
}

const Motion& 
TalairachTransformation::transformation( )
{
  if( _motionComputed )
    return *_motion ;

  if( !_motion )
    _motion = new Motion ;
  if(!_computable){
    _motion->setToIdentity() ;
    return *_motion ;
  }
  
  _motionComputed = true ;
  if( _withMask )
    return withMaskComputation() ;
  else
    return withoutMaskComputation() ;
}

const Motion& 
TalairachTransformation::withoutMaskComputation()
{ 
  Point3df AC2 ;
  Point3df PC2 ;
  Point3df PIH2 ;
  
  if( _toNormalizedTemplate ){
    AC2 = Point3df( 0., 0., 0.) ;
    PC2 = Point3df( 0., 1., 0.) ;
    PIH2 = Point3df( 0., 0., 1.) ;        
  }else {
    AC2 = _secondPoints[0] ;
    PC2 = _secondPoints[1] ;
    PIH2 = _secondPoints[2] ;
  }
  
  Point3df AC1 = _firstPoints[0] ;
  Point3df PC1 = _firstPoints[1] ;
  Point3df PIH1 = _firstPoints[2] ;
    
  //definition du vecteur ACPC et valeur de scaling
  Point3df ACPC1(PC1 - AC1) ;
  float scale1 = ACPC1.norm() ;
  ACPC1.normalize() ;
  
  Point3df ACPC2(PC2 - AC2) ;
  float scale2 = ACPC2.norm() ;
  ACPC2.normalize() ;
  
  //definition du vecteur ACPIH
  Point3df ACPIH1(PIH1 - AC1) ;
  ACPIH1.normalize() ;

  Point3df ACPIH2(PIH2 - AC2) ;
  ACPIH2.normalize() ;

  //on orthogonalise les vecteurs 2 à 2 selon le principe de Schmidt

  Point3df ACPIHNormeOrtho1 = ACPIH1 - ACPC1 * ACPC1.dot(ACPIH1) ;
  Point3df cross1 = crossed(ACPC1,ACPIHNormeOrtho1) ;
  Point3df ACPIHNormeOrtho2 = ACPIH2 - ACPC2 * ACPC2.dot(ACPIH2) ;
  Point3df cross2 = crossed(ACPC2,ACPIHNormeOrtho2) ;

  //on genere la matrice de passage entre l'ancienne et la nouvelle base constituee des vecteurs
  //de la nouvelle base mis en colonne : cette matrice est la matrice de rotation
  //RQ:avec AimsData, les indices i et j sont inverses donc ici, on n'a pas a inverser ensuite la matrice
  //pour generer ma matrice de rotation

  AimsData<float> matriceRot1(3, 3) ;
  for (int i=0; i<3; i++){
    matriceRot1(0,i) = ACPC1[i] ;
    matriceRot1(1,i) = ACPIHNormeOrtho1[i] ;
    matriceRot1(2,i) = cross1[i] ;
  }

  AimsData<float> matriceRot2(3, 3) ;
  for (int i=0; i<3; i++){
    matriceRot2(i,0) = ACPC2[i] ;
    matriceRot2(i,1) = ACPIHNormeOrtho2[i] ;
    matriceRot2(i,2) = cross2[i] ;
  }
  
  _motion->rotation() = matriceRot2.cross(matriceRot1) ;
  
 //calcul de la matrice de scaling

  
  _motion->scale( Point3df(scale1, scale1, scale1), Point3df(scale2, scale2, scale2) ) ;
  
  //calcul du vecteur de translation
  _motion->setTranslation( AC2 - AC1 );
  
  return *_motion ;  
}



Motion
TalairachTransformationStrategy::transformation( const vector<Point3df>& base1, const vector<Point3df>& base2  )
{
  
 //on norme les vecteurs de indivBase

  Point3df AC1 = base1[0] ;
  Point3df PC1 = base1[1] ;
  Point3df PIH1 = base1[2] ;
  
  Point3df AC2 = base2[0] ;
  Point3df PC2 = base2[1] ;
  Point3df PIH2 = base2[2] ;
  
  
  //definition du vecteur ACPC et valeur de scaling
  Point3df ACPC1(PC1 - AC1) ;
  float scale1 = ACPC1.norm() ;
  ACPC1.normalize() ;
  
  Point3df ACPC2(PC2 - AC2) ;
  float scale2 = ACPC2.norm() ;
  ACPC2.normalize() ;
  
  //definition du vecteur ACPIH
  Point3df ACPIH1(PIH1 - AC1) ;
  ACPIH1.normalize() ;

  Point3df ACPIH2(PIH2 - AC2) ;
  ACPIH2.normalize() ;

  //on orthogonalise les vecteurs 2 à 2 selon le principe de Schmidt

  Point3df ACPIHNormeOrtho1 = ACPIH1 - ACPC1 * ACPC1.dot(ACPIH1) ;
  Point3df cross1 = crossed(ACPC1,ACPIHNormeOrtho1) ;
  Point3df ACPIHNormeOrtho2 = ACPIH2 - ACPC2 * ACPC2.dot(ACPIH2) ;
  Point3df cross2 = crossed(ACPC2,ACPIHNormeOrtho2) ;

  //on genere la matrice de passage entre l'ancienne et la nouvelle base constituee des vecteurs
  //de la nouvelle base mis en colonne : cette matrice est la matrice de rotation
  //RQ:avec AimsData, les indices i et j sont inverses donc ici, on n'a pas a inverser ensuite la matrice
  //pour generer ma matrice de rotation


  Motion motion ;

  AimsData<float> matriceRot1(3, 3) ;
  for (int i=0; i<3; i++){
    matriceRot1(0,i) = ACPC1[i] ;
    matriceRot1(1,i) = ACPIHNormeOrtho1[i] ;
    matriceRot1(2,i) = cross1[i] ;
  }


  AimsData<float> matriceRot2(3, 3) ;
  for (int i=0; i<3; i++){
    matriceRot2(i,0) = ACPC2[i] ;
    matriceRot2(i,1) = ACPIHNormeOrtho2[i] ;
    matriceRot2(i,2) = cross2[i] ;
  }
  
  motion.rotation() = matriceRot2.cross(matriceRot1) ;
  
 //calcul de la matrice de scaling

  
  motion.scale( Point3df(scale1, scale1, scale1), Point3df(scale2, scale2, scale2) ) ;
  
  //calcul du vecteur de translation
  motion.setTranslation( AC2 - AC1 );
  
  return motion ;

}

const Motion& 
TalairachTransformationStrategy::withMaskComputation()
{
  throw error("TalairachTransformationStrategy::withMaskComputation not available yet !") ;
}
