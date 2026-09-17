

#ifndef PCASTRATEGY_H
#define PCASTRATEGY_H

#include <aims/classification/classifstrategy.h>
#include <aims/classification/individuals.h>
#include <aims/classification/distance.h>
#include <aims/vector/vector.h>
#include <vector>
#include <list>


namespace aims {
  template<class T>
    class PcaStrategy : public ClassifStrategy<T> {
    public:
    enum DistanceType {
      NORM1,
      NORM2,
      NORM2SQR,
      INFNORM
    } ;
    
    PcaStrategy( const PcaStrategy<T>& pcaStrat );
    PcaStrategy( int nbIterations = 50, 
		 DistanceType distanceType = NORM2SQR, 
		 const std::vector< Individuals<T> >& codeVector = std::vector< Individuals<T> >() ) ;
    virtual ~PcaStrategy() ;
    virtual ClassifStrategy<T> * clone() const ;
    
    virtual void analyse( const std::vector< std::list< Individuals<T> > >& classes ) ;              
    virtual int agregation( const Individuals<T>& individual ) ;   
    virtual bool classificationCompleted( int nbChanges ) ;
    
    private:
    virtual float distance( const Individuals<T>& individual, int classe ) ;
    float (Distance<T>:: * myDistance)( const std::vector<T>& ind1, const std::vector<T>& ind2, 
				   unsigned int beginIndex, int endIndex ) ;
  } ;
}

#endif
