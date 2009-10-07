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


#ifndef KMEANSSTRATEGY_D_H
#define KMEANSSTRATEGY_D_H

#include <cstdlib>
#include <aims/classification/kmeansstrategy.h>
#include <aims/classification/classifstrategy.h>
#include <aims/classification/iterativeclassification.h>
#include <aims/classification/distance_d.h>
#include <aims/classification/individuals_d.h>
#include <vector>
#include <list>
#include <stdlib.h>


template <class T>
aims::KmeansStrategy<T>::KmeansStrategy( const KmeansStrategy<T>& kmeanStrat ) :
  aims::ClassifStrategy<T>( kmeanStrat )
{
  myMeanVector = kmeanStrat.myMeanVector ;
  myVarianceVector = kmeanStrat.myVarianceVector ;
  myDistance = kmeanStrat.myDistance ;
  myBeginIndex = kmeanStrat.myBeginIndex ;
  myEndIndex = kmeanStrat.myEndIndex ;
}


template <class T>
aims::KmeansStrategy<T>::KmeansStrategy( int nbIterations, DistanceType distanceType, 
					 int beginIndex , int endIndex ,
					 const std::vector< aims::Individuals<T> >& codeVector ) :
  aims::ClassifStrategy<T>( nbIterations )
{
  switch( distanceType ){
  case NORM1 :
    myDistance = &(aims::Distance<T>::norm1) ;
    break ;
  case NORM2 :
    myDistance = &(aims::Distance<T>::norm2) ;
    break ;
  case NORM2SQR :
    myDistance = &(aims::Distance<T>::norm2sqr) ;
    break ;
  case INFNORM :
    myDistance = &(aims::Distance<T>::infiniteNorm) ;
    break ;
  }
  
  ASSERT( beginIndex >= 0 && endIndex >= -1 ) ;
  myBeginIndex = beginIndex ;
  myEndIndex = endIndex ;      
  myMeanVector = codeVector ;
  if( !codeVector.empty() )    // si codeVector non vide
    this->myCodeVectorsGiven = true ;
}


template <class T>
aims::KmeansStrategy<T>::~KmeansStrategy()
{
}


template <class T>
aims::ClassifStrategy<T> * aims::KmeansStrategy<T>::clone() const
{
  return new aims::KmeansStrategy<T>( *this ) ;
}


template <class T>
void aims::KmeansStrategy<T>::init( std::string initializationType, int nbOfClasses, 
				    std::vector< std::list< aims::Individuals<T> > >& classes )
{
  if( initializationType == "CodeVectorsGiven" ){ 
    std::cout << " vecteurs codes donnes" << std::endl << std::endl ;
    // vérifier : taille de codeVector = nb de classes, et compatible avec taille de Individus<T>
    if( int(myMeanVector.size()) != nbOfClasses )     // compatibilité avec nb de classes
      this->myValidStrategy = false ;
    for( int i = 0 ; i < nbOfClasses ; ++i ){
      if( myMeanVector[i].value().size() != myMeanVector[1].value().size() ) // &taille des individus
	this->myValidStrategy = false ;
    } 	   
  }
  else if( initializationType == "InitialClasses" ){
    std::cout << " classes initiales" << std::endl << std::endl ;
    std::vector< aims::Individuals<T> > myMeanVector( nbOfClasses + 1 ) ;   
    // nbOfClasses ne prend pas en compte la classe 0, contrairement à "analyse", 
    // d'où le +1 pour cohérence
    analyse( classes ) ;
  }
  else {
    // initialisation des vecteurs codes aléatoire (tirer au hasard 1 individu par classe)   
    std::cout << " individus donnes (tous ds la classe 0)" << std::endl ;
    int randInd ;
    bool change = false ;
    // création d'1 vecteur de dimension nbOfClasses:    
    std::vector< Individuals<T> > meanVector( nbOfClasses + 1 ) ;    
    int tabRandInd[nbOfClasses] ;
    std::cout << "Tirage des individus: " ;

    for( int c = 1 ; c <= nbOfClasses ; ++c ){
      //cout << "class " << c << " : nb of ind = " << classes[c].size() << endl ;
      // tirage aléatoire d'un individu ds la classe 0
      randInd = (int) ( ( (double)rand() / (double)(RAND_MAX+1.0) ) * (double)(classes[0].size()) ) ;   
      tabRandInd[c] = randInd ;
      std::cout << randInd << " " ;
      do{
	change = false ;
	for( int i = 1 ; i < c ; ++i ){
	  if( randInd == tabRandInd[i] ){
	    // nouveau tirage au sort si randInd a deja ete tire
	    randInd = (int) ( ((double)rand() / (double)(RAND_MAX+1.0)) * (double)(classes[0].size()) ) ;
	    change = true ;
	    std::cout << "(" << randInd << ")" << " " ;
      	  }
	}	  
      } while( change == true ) ;
 
      // copie des valeurs de l'individu choisi ds myMeanVector
      typename std::list< aims::Individuals<T> >::const_iterator iter( classes[0].begin() ), 
	last( classes[0].end() ) ;
      int count = 0 ;
      while ( iter != last && count < randInd ){
	++count ;
	++iter ;
      }
      meanVector[c] = *iter ;
    }   
    std::cout << std::endl << std::endl ;
    myMeanVector = meanVector ;
  }

  unsigned int maxIndex = myMeanVector[1].value().size() ;
  if( ( myEndIndex == -1 ) || ( (unsigned int)myEndIndex >= maxIndex ) )  
    myEndIndex = maxIndex - 1 ;
}


template <class T>
double aims::KmeansStrategy<T>::iterate( int& nbOfIterations, 
					 std::vector< std::list< Individuals<T> > >& classes )
{
  std::cout << "Kmeans clustering - Iteration n° " << nbOfIterations << std::endl ;
  int nbOfClasses = classes.size() ;
  std::vector< std::list< aims::Individuals<T> > > newClasses( nbOfClasses ) ;

/*   std::cout << "nb d'individus ds les classes en entrant ds iterate:" ; */
/*   for( int c = 0 ; c < nbOfClasses ; ++c ) */
/*     std::cout << " " << classes[c].size() ; */
/*   std::cout << std::endl ; */

//  std::cout << " agregation des individus" << std::endl ;
  for( int c = 0 ; c < nbOfClasses ; ++c ){
    typename std::list< aims::Individuals<T> >::const_iterator iter( classes[c].begin() ), 
      last( classes[c].end() ) ;
    while( iter != last ){
      int indMin = aggregate( *iter ) ;
      newClasses[indMin].push_back( *iter ) ;
      ++iter ;
    }
    classes[c].clear() ;                             
  }
//  delete classes ;
  classes = newClasses ;
  
  std::cout << "nb d'individus - iterate end:" ;
  for( int c = 0 ; c < nbOfClasses ; ++c )
    std::cout << " " << classes[c].size() ;
  std::cout << std::endl ;

  analyse( classes ) ;                   // faire l'analyse des classes
  double new_inertia = globInertia( classes ) ; // calcul de l'inertie globale

  ++nbOfIterations ;                                           // compteur nb d'iterations

  return new_inertia ;
}


template <class T>
void aims::KmeansStrategy<T>::analyse( const std::vector< std::list< aims::Individuals<T> > >& classes )
{
//  std::cout << " analyse des classes..." << std::endl ;
  unsigned int nbOfClasses = classes.size() ;

  // calculer myMeanVector (vecteur moyenne) et myVarVector (vecteur variance) pour chaque classe
  std::vector< aims::Individuals<T> > meanVector( nbOfClasses ) ;
  std::vector< aims::Individuals<T> > myVarianceVector( nbOfClasses ) ;
  Point3df meanPos( 0., 0., 0. ) ;
  Point3df varPos( 0., 0., 0. ) ;  
  unsigned int valIndSize = classes[1].front().value().size() ;
  std::vector<T> meanVal( valIndSize ) ;
  std::vector<T> varVal( valIndSize ) ;

  for( unsigned int cl = 1 ; cl < nbOfClasses ; ++cl ){
    unsigned int nbOfInd = classes[cl].size() ;
    typename std::list< aims::Individuals<T> >::const_iterator iter( classes[cl].begin() ), 
      last( classes[cl].end() ) ;
    while ( iter != last){
      Point3df indPos = iter->position() ;
      meanPos[0] += indPos[0];
      meanPos[1] += indPos[1];
      meanPos[2] += indPos[2];
      ++iter ;
    }   
    meanPos[0] = meanPos[0] / nbOfInd ;
    meanPos[1] = meanPos[1] / nbOfInd ;
    meanPos[2] = meanPos[2] / nbOfInd ;
    
    for( unsigned int k = 0 ; k < valIndSize ; ++k ){
      typename std::list< aims::Individuals<T> >::const_iterator iter( classes[cl].begin() ), 
	last( classes[cl].end() ) ;
      float sumVal = 0., prodVal = 0. ;
      while ( iter != last){
	std::vector<T> indVal = iter->value() ;	
	sumVal += indVal[k] ;
	prodVal += indVal[k] * indVal[k] ;
	++iter ;
      } 
      meanVal[k] = sumVal / nbOfInd ;
      varVal[k] = prodVal / nbOfInd - meanVal[k] * meanVal[k] ;
    }
    Individuals<T> meanIndiv( meanPos, meanVal ) ;
    Individuals<T> varIndiv( meanPos, varVal ) ;
    
    meanVector[cl] = meanIndiv ;
    myVarianceVector[cl] = varIndiv ;   
  }
  myMeanVector = meanVector ;
}


template <class T>
int aims::KmeansStrategy<T>::aggregate( const aims::Individuals<T>& individual )
{
  // parcourir les classes et mettre l'individu dans la classe la plus proche
  float distanceToClass ;
  int nbOfClasses = myMeanVector.size() ;
  int indMin = 1 ;

  float min = myDistance( individual.value(), myMeanVector[1].value(), 
			  myBeginIndex, myEndIndex ) ;
  for( int newC = 2 ; newC < nbOfClasses ; ++newC ){
    distanceToClass = myDistance( individual.value(), myMeanVector[newC].value(), 
				  myBeginIndex, myEndIndex ) ;
    if( distanceToClass < min ){
      indMin = newC ;
      min = distanceToClass ;
    }
  }
  return indMin ;
}


template <class T>
double aims::KmeansStrategy<T>::globInertia( const std::vector< std::list< aims::Individuals<T> > >& classes )
{
//  std::cout << " calcul de l'inertie globale..." << std::endl ;

  int nbOfClasses = classes.size() ;
  float dist = 0. ;
  double glob_dist = 0. ;

  for( int c = 1 ; c < nbOfClasses ; ++c ){
    typename std::list< aims::Individuals<T> >::const_iterator iter( classes[c].begin() ), 
      last( classes[c].end() ) ;
    while( iter != last ){
      dist = myDistance( iter->value(), myMeanVector[c].value(), myBeginIndex, myEndIndex ) ;
      glob_dist += (double) dist ;
      ++iter ;
    }
  }
  return glob_dist ;
}


template <class T>
float aims::KmeansStrategy<T>::distance( const aims::Individuals<T>& individual, int classe )
{
  // calculer la distance entre l'individu et le vecteur moyen de la classe
  float distanceToClass = 0. ;
  distanceToClass = myDistance( individual.value(), myMeanVector[classe].value(), 
				myBeginIndex, myEndIndex ) ;
  return distanceToClass ;
}
 
#endif
