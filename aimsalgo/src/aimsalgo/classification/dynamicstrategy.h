

#ifndef DYNAMICSTRATEGY_H
#define DYNAMICSTRATEGY_H

#include <aims/classification/kmeansstrategy.h>
#include <aims/classification/individuals.h>
#include <aims/classification/distance_d.h>
#include <aims/vector/vector.h>
#include <vector>
#include <list>

namespace aims{
  template<class T>
    class DynamicStrategy : public KmeansStrategy<T>{
    public:
    
    DynamicStrategy( const DynamicStrategy<T>& dynamicStrat ) ;
    DynamicStrategy( int nbIterations = 50, 
		     DistanceType distanceType = NORM2SQR, 
		     int beginIndex = 0, int endIndex = -1,
		     const std::vector< aims::Individuals<T> >& codeVector = std::vector< Individuals<T> >() ) ;
    virtual ~DynamicStrategy() ;
    virtual ClassifStrategy<T> * clone() const ;
    
    virtual double iterate( int& nbOfIterations, 
			    std::vector< std::list< Individuals<T> > >& classes ) ;
/*     void centerComputation( int c, unsigned int card,  */
/* 			    bool add, const Individuals<T>& ind ) ; */
    void centerComputation( int cNew, int cOld, unsigned int cardNew, 
			    unsigned int cardOld, const Individuals<T>& ind ) ;
//    void classAnalyse( int c, const std::vector< std::list< Individuals<T> > >& classes ) ;

  } ;
}

#endif
