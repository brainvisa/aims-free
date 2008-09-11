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


#ifndef ITERATIVECLASSIFICATION_D_H
#define ITERATIVECLASSIFICATION_D_H

#include <aims/classification/iterativeclassification.h>
#include <aims/vector/vector.h>
#include <iostream>
#include <vector>
#include <list>


template <class T>
aims::IterativeClassification<T>::IterativeClassification( std::vector< std::list< aims::Individuals<T> > >* classes, 
							   int nbOfClasses, int maxNbOfRuns, 
							   double threshold, bool classified, 
							   const ClassifStrategy<T>& strategy ): 
  myClasses(0), myNbOfClasses( nbOfClasses ), myMaxNbOfRuns( maxNbOfRuns ), myThreshold( threshold ), 
     myClassified( classified ), myClassifStrategy(0)
{
  myClasses = *classes ;
  myClassifStrategy = strategy.clone() ;
}


template <class T>
aims::IterativeClassification<T>::~IterativeClassification()
{
  delete myClassifStrategy ;
}


template <class T>
void aims::IterativeClassification<T>::setClassifStrategy(
  const ClassifStrategy<T>& strategy, bool /*keepPreviousResult*/ )
{
  delete myClassifStrategy ;
  myClassifStrategy = strategy.clone() ;
}


template <class T>
const std::vector< std::list< aims::Individuals<T> > >& 
aims::IterativeClassification<T>::getClasses()
{
  if( !myClassified )
    classification() ;
  return myClasses ;
}


template<class T>
void aims::IterativeClassification<T>::initialization( std::vector< std::list< Individuals<T> > >& classes,
						       int nbOfClasses ) 
{
  std::cout << "Initialisation..."  ;
  std::string init_type ;

  // CAS 1 : VECTEURS CODES DONNES
  if( myClassifStrategy->isCodeVectorsGiven() ){
    init_type = "CodeVectorsGiven" ;
    for( int c = 1 ; c <= nbOfClasses ; ++c ){
      if( classes[c].size() != 0 ){
	typename std::list< aims::Individuals<T> >::iterator iter( classes[c].begin() ), 
	  last( classes[c].end() ) ;
	while( iter != last ){
	  classes[0].push_back( *iter ) ;
	  ++iter ;
	}
	classes[c].clear() ;
      }
    }
  } 
  else{
    bool classesGiven = true, individualsGiven = true ;
    for( int i = 1 ; i <= myNbOfClasses ; ++i ){
      if( classes[i].size() == 0 )
	classesGiven = false ;
      else individualsGiven = false ;
    }
    // CAS 2 : CLASSES INITIALES (INDIVIDUS DANS LES CLASSES 1 à nbOfClasses)
    if( classesGiven == true )
      init_type = "InitialClasses" ;
    // CAS 3 : PAS DE CLASSES (TOUS LES INDIVIDUS DANS LA CLASSE 0)
    else if( classes[0].size() != 0 && individualsGiven == true )
      init_type = "NoInitialClasses" ;
    // CAS OU IL Y A DES CLASSES INITIALES MAIS 1 OU PLUSIEURS CLASSES SONT VIDES
    else{
      init_type = "bad" ;
      for( int c = 1 ; c <= nbOfClasses ; ++c ){
	if( classes[c].size() != 0 ){
	  typename std::list< aims::Individuals<T> >::iterator iter( classes[c].begin() ), 
	    last( classes[c].end() ) ;
	  while( iter != last ){
	    classes[0].push_back( *iter ) ;
	    ++iter ;
	  }
	  classes[c].clear() ;
	}
      }
    }
  }

  myClassifStrategy->init( init_type, nbOfClasses, classes ) ;
}


template <class T>
bool aims::IterativeClassification<T>::classification()
{
  bool valid = true, newRun = true ;
  int nbOfIterations ;
  double old_inertia = 0., new_inertia = 0., min_inertia = 0. ;
  int maxNbOfIterations = myClassifStrategy->getMaxNbOfIterations() ;
  int maxNbOfRuns = myMaxNbOfRuns ;
  double threshold = myThreshold ;

  // test classification valide
  valid = myClassifStrategy->isValidStrategy() ;
  if ( valid == false ){
    std::cout << "Non valide: impossible de faire la classification." ;
    // break ;
  }

  std::vector< std::list< aims::Individuals<T> > > initialClasses( myClasses ) ;
  std::vector< std::list< aims::Individuals<T> > > bestClasses( myNbOfClasses + 1 ) ;

  // test: si vecteurs codes donnés OU classes initiales données -> imposer 1 SEUL RUN 
  bool classesGiven = true ;
  for( int i = 1 ; i <= myNbOfClasses ; ++i )
    if( initialClasses[i].size() == 0 )
      classesGiven = false ;
  if( myClassifStrategy->isCodeVectorsGiven() || classesGiven == true )
    maxNbOfRuns = 1 ;

  // boucle sur les runs, éventuellement, initialisation des variables
  for( int r = 1 ; r <= maxNbOfRuns ; ++r ){
    std::cout << "Run numero " << r << std::endl ;
    newRun = true ;
    nbOfIterations = 0 ;
    new_inertia = 0. ;
    old_inertia = 0. ;
    initialization( initialClasses, myNbOfClasses ) ;
    
    // boucle sur les itérations
    do{
      if( new_inertia != 0. )
	old_inertia = new_inertia ;
      if( newRun ){                  // a chaque nouveau run, repartir des classes initiales 
	for( int c = 1 ; c <= myNbOfClasses ; ++c ){
	  if( myClasses[c].size() != 0 ){
	    typename std::list< aims::Individuals<T> >::iterator iter( myClasses[c].begin() ), 
	      last( myClasses[c].end() ) ;
	    while( iter != last ){
	      myClasses[0].push_back( *iter ) ;
	      ++iter ;
	    }
	    myClasses[c].clear() ;
	  }
	}
	myClassifStrategy->init( "Random", myNbOfClasses, myClasses ) ;


      }
      //initialization( myClasses, myNbOfClasses ) ;myNbOfClasses
	  //myClasses = initialClasses ;
      new_inertia = myClassifStrategy->iterate( nbOfIterations, myClasses ) ;
      newRun = false ;
      std::cout << "Inertie = " << new_inertia << " - Critere = " << (old_inertia - new_inertia ) / new_inertia << std::endl << std::endl ;
    } 
    while( ((( old_inertia - new_inertia ) / new_inertia ) > threshold || 
	    old_inertia == 0. || new_inertia > old_inertia )
	   && nbOfIterations < maxNbOfIterations ) ;
    
    if( nbOfIterations == maxNbOfIterations ){
      valid = false ;
      std::cout << "Nb max d'iterations atteint!" << std::endl << std::endl ;
    }
    if( ( new_inertia < min_inertia || min_inertia == 0. )
	&& nbOfIterations < maxNbOfIterations ){
      bestClasses = myClasses ;                        // conserver les classes du meilleur run
      min_inertia = new_inertia ;                        // conserver l'inertie minimale
      std::cout << "Meilleur run n° " << r << std::endl << std::endl ;
    }
  }
  std::cout << "done !" << std::endl ;
  if( min_inertia == 0. )
      std::cout << "Classification impossible" << std::endl << std::endl ;
  myClasses = bestClasses ;     // conserver la meilleure classification
  
  std::cout << "classes nb " << myClasses.size() << std::endl ;
  
  return valid ;
}


#endif
