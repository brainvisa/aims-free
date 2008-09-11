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



#ifndef KMEANSSTRATEGY_H
#define KMEANSSTRATEGY_H

#include <aims/classification/classifstrategy.h>
#include <aims/classification/individuals.h>
#include <aims/classification/distance_d.h>
#include <aims/vector/vector.h>
#include <vector>
#include <list>

namespace aims{
  enum DistanceType{
    NORM1,
    NORM2,
    NORM2SQR,
    INFNORM
  };

  template<class T>
    class KmeansStrategy : public ClassifStrategy<T>{
    public:

    KmeansStrategy( const KmeansStrategy<T>& kmeanStrat ) ;

    // L'initialisation de la classification par des vecteurs code 
    // est prioritaire sur le don de classes initiales.
    KmeansStrategy( int nbIterations = 50, 
		    DistanceType distanceType = NORM2SQR,
		    int beginIndex = 0, int endIndex = -1,
		    const std::vector< Individuals<T> >& codeVector = std::vector< Individuals<T> >() ) ;
    virtual ~KmeansStrategy() ;
    virtual ClassifStrategy<T> * clone() const ;
    
/*     virtual double iterate ( int & nbOfIterations,  */
/* 			     std::vector< std::list< Individuals<T> > >* classes ) ; */
    virtual double iterate ( int& nbOfIterations, 
			     std::vector< std::list< Individuals<T> > >& classes ) ;
    virtual void init( std::string initializationType, int nbOfClasses, 
		       std::vector< std::list< Individuals<T> > >& classes ) ;   
    virtual void analyse( const std::vector< std::list< Individuals<T> > >& classes ) ;   
    virtual int aggregate( const Individuals<T>& individual ) ; 

    virtual Individuals<T> getMeanValue( int classe ) { return myMeanVector[classe] ; }
    virtual std::vector< Individuals<T> > getMeanVector() { return myMeanVector ; }

    virtual double globInertia( const std::vector< std::list< Individuals<T> > >& classes ) ;
    
    protected:
    virtual float distance( const Individuals<T>& individual, int classe ) ;
/*     float (Distance<T>:: * myDistance)( const std::vector<T>& ind1, const std::vector<T>& ind2,  */
/* 					unsigned int beginIndex, unsigned int endIndex ) ;    */
    float ( * myDistance )( const std::vector<T>& ind1, const std::vector<T>& ind2, 
					unsigned int beginIndex, unsigned int endIndex ) ;   
    std::vector< Individuals<T> > myMeanVector ;
    std::vector< Individuals<T> > myVarianceVector ;
    int myBeginIndex, myEndIndex ;
  } ;
}

#endif
