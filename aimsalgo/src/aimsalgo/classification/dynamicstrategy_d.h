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


#ifndef DYNAMICSTRATEGY_D_H
#define DYNAMICSTRATEGY_D_H

#include <aims/classification/dynamicstrategy.h>
#include <aims/classification/classifstrategy.h>
#include <aims/classification/iterativeclassification.h>
#include <aims/classification/distance_d.h>
#include <aims/classification/individuals_d.h>
#include <vector>
#include <list>
#include <stdlib.h>


template <class T>
aims::DynamicStrategy<T>::DynamicStrategy( const DynamicStrategy<T>& dynamicStrat ) :
  aims::KmeansStrategy<T>( dynamicStrat )
{
  this->myMeanVector = dynamicStrat.myMeanVector ;
  this->myVarianceVector = dynamicStrat.myVarianceVector ;
  this->myDistance = dynamicStrat.myDistance ;
  this->myBeginIndex = dynamicStrat.myBeginIndex ;
  this->myEndIndex = dynamicStrat.myEndIndex ;
}


template <class T>
aims::DynamicStrategy<T>::DynamicStrategy( int nbIterations, DistanceType distanceType, 
					   int beginIndex , int endIndex ,
					   const std::vector< aims::Individuals<T> >& codeVector ) :
  aims::KmeansStrategy<T>( nbIterations, distanceType, beginIndex, endIndex, codeVector )
{
}


template <class T>
aims::DynamicStrategy<T>::~DynamicStrategy()
{
}


template <class T>
aims::ClassifStrategy<T> * aims::DynamicStrategy<T>::clone() const
{
  return new aims::DynamicStrategy<T>( *this ) ;
}


template <class T>
double aims::DynamicStrategy<T>::iterate( int& nbOfIterations, 
					   std::vector< std::list< Individuals<T> > >& classes )
{
  std::cout << "Dynamic clustering - Iteration n° " << nbOfIterations << std::endl ; 
  int nbOfClasses = classes.size() ;
  int indMin = 0 ;
  unsigned int classCardNew, classCardOld ;
  int nbChanges = 0 ;

/*   std::cout << "nb d'individus ds les classes en entrant ds iterate:" ; */
/*   for( int c = 0 ; c < nbOfClasses ; ++c ) */
/*     std::cout << " " << classes[c].size() ; */
/*   std::cout << std::endl ; */

  std::vector< std::list< aims::Individuals<T> > >* newClasses ;
  newClasses = &classes ;  

  // QUAND LES INDIVIDUS SONT DS LA CLASSE 0, FAIRE 1 ITERATION DE KMEANS
  if( !nbOfIterations && ( classes[0].size() != 0 ) ){
    // dispatcher les individus de la classe 0 dans les classes 1 à c
    typename std::list< aims::Individuals<T> >::iterator iter( classes[0].begin() ), 
      last( classes[0].end() ) ;
    while( iter != last ){
      int indMin = aggregate( *iter ) ;
      (*newClasses)[indMin].push_back( *iter ) ;
      ++iter ;
    }
    (*newClasses)[0].clear() ;
    // faire l'analyse des classes
    analyse( *newClasses ) ;
  }
  else{    
    for( int c = 0 ; c < nbOfClasses ; ++c ){
      typename std::list< aims::Individuals<T> >::iterator iter( classes[c].begin() ), 
	last( classes[c].end() ), iterToMove ;
      while( iter != last ){
	iterToMove = iter ;
	indMin = aggregate( *iter ) ;
	++iter ;
	// si l'individu change de classe, faire les modifs des classes et le calcul des centres de ces classes
	if( c != indMin ){
	  ++nbChanges ;
	  classCardNew = (*newClasses)[indMin].size() ;
	  classCardOld = (*newClasses)[c].size() ;

	  centerComputation( indMin, c, classCardNew, classCardOld, *iterToMove ) ;

	  (*newClasses)[indMin].push_back( *iterToMove ) ;
	  (*newClasses)[c].erase( iterToMove ) ;
	}
      }
    }
  }
  
  classes = *newClasses ;

  std::cout << "nb d'individus - iterate end :" ;
  for( int c = 0 ; c < nbOfClasses ; ++c )
    std::cout << " " << classes[c].size() ;
  std::cout << std::endl ;

  double new_inertia = globInertia( classes ) ; // calcul de l'inertie globale */
  
  ++nbOfIterations ;                                           // compteur nb d'iterations

  return new_inertia ;
}


template <class T>
void aims::DynamicStrategy<T>::centerComputation( int cNew, int cOld, unsigned int cardNew, 
						  unsigned int cardOld, const Individuals<T>& ind )
{
  // récupérer le vecteur code moyen de la classe
  Individuals<T> meanIndivNew = this->getMeanValue( cNew ) ;
  Individuals<T> meanIndivOld = this->getMeanValue( cOld ) ;

  Point3df meanIndivPosNew = meanIndivNew.position() ;
  Point3df meanIndivPosOld = meanIndivOld.position() ;
  Point3df indPos = ind.position() ;

  for ( int i = 0 ; i < 3 ; ++i ){ 
    meanIndivPosNew[i] = ( ( meanIndivPosNew[i] * cardNew ) + indPos[i] ) / ( cardNew + 1 ) ;
    meanIndivPosOld[i] = ( ( meanIndivPosOld[i] * cardOld ) - indPos[i] ) / ( cardOld - 1 ) ;
  }

  std::vector<T> indVal = ind.value() ;
  unsigned int valIndSize = indVal.size() ;

  std::vector<T> meanIndivValNew = meanIndivNew.value() ;
  std::vector<T> meanIndivValOld = meanIndivOld.value() ;
  std::vector<T> meanIndivValNewClass = meanIndivValNew ;
  std::vector<T> meanIndivValOldClass = meanIndivValOld ;

  for( unsigned int k = 0 ; k < valIndSize ; ++k ){
    meanIndivValNewClass[k] = ( ( meanIndivValNew[k] * cardNew ) + indVal[k] ) / ( cardNew + 1 ) ;
    meanIndivValOldClass[k] = ( ( meanIndivValOld[k] * cardOld ) - indVal[k] ) / ( cardOld - 1 ) ;
  }

  Individuals<T> newMeanIndiv( meanIndivPosNew, meanIndivValNewClass ) ;
  this->myMeanVector[cNew] = newMeanIndiv ;
  Individuals<T> oldMeanIndiv( meanIndivPosOld, meanIndivValOldClass ) ;
  this->myMeanVector[cOld] = oldMeanIndiv ;
}


#endif
