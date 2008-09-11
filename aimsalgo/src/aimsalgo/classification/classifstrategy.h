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



#ifndef CLASSIFSTRATEGY_H
#define CLASSIFSTRATEGY_H

#include <aims/classification/individuals.h>
#include <aims/vector/vector.h>
#include <vector>
#include <list>

namespace aims{
  template<class T>
    class ClassifStrategy{
    public:
    ClassifStrategy( int maxNbOfIterations = 50 ) ;
    virtual ~ClassifStrategy() ;

    virtual ClassifStrategy<T> * clone() const = 0 ;
    bool isValidStrategy(){ return myValidStrategy ; }
    void reset() { myInit = true ; }
    bool isInit() { return myInit ; }
    bool isCodeVectorsGiven() { return myCodeVectorsGiven ; }
    int getMaxNbOfIterations() const { return myMaxNbOfIterations ; }

    virtual void init( std::string initializationType, int nbOfClasses, 
		       std::vector< std::list< Individuals<T> > >& classes ) = 0 ;   
    virtual double iterate ( int& nbOfIterations, 
			     std::vector< std::list< Individuals<T> > >& classes ) = 0 ;
    virtual void analyse( const std::vector< std::list< Individuals<T> > >& classes ) = 0 ; 
    virtual int aggregate( const Individuals<T>& individual ) = 0 ; 

    virtual Individuals<T> getMeanValue( int classe ) = 0 ;
    virtual std::vector< Individuals<T> > getMeanVector() = 0 ;

    virtual double globInertia( const std::vector< std::list< Individuals<T> > >& classes ) = 0 ;
    
    protected:
    virtual float distance( const Individuals<T>& individual, int classe ) = 0 ; 
    // type de données: vecteur de listes d'individus (pour chaque classe, on a la liste des individus)
    int myMaxNbOfIterations ;              // nombre maximum d'itérations
    bool myValidStrategy ;                  // strategie valide ?
    bool myInit ;                           // initialisation faite ou non ?
    bool myCodeVectorsGiven ;               // vecteurs codes donnes ou non ?
  } ;
}


#endif
