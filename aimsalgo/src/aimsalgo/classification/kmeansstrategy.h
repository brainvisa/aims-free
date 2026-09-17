

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
