

#ifndef ITERATIVECLASSIFICATION_H
#define ITERATIVECLASSIFICATION_H

#include <aims/classification/classifstrategy.h>
#include <aims/classification/individuals.h>
#include <aims/vector/vector.h>
#include <vector>
#include <list>

namespace aims{
  template <class T>
    class IterativeClassification{
    public:
//    enum ClassifMethod{
//      KMEANS,
//      DYNAMIC,
//      PCA
//    } ;

    IterativeClassification( std::vector< std::list< Individuals<T> > >* classes, 
			     int nbOfClasses, int maxNbOfRuns, double threshold, 
			     bool classified, const ClassifStrategy<T>& strategy ) ;
    ~IterativeClassification();
  
    void setClassifStrategy( const ClassifStrategy<T>& strategy, 
			     bool keepPreviousResult = false ) ;
    const std::vector< std::list< Individuals<T> > >& getClasses() ;    
    bool isCodeVectorsGiven() const 
      { return myClassifStrategy->isCodeVectorsGiven() ; }
    aims::Individuals<T> getMeanValue( int classe ) const 
      { return myClassifStrategy->getMeanValue( classe ) ; }
    std::vector< aims::Individuals<T> > getMeanVector() const
      { return myClassifStrategy->getMeanVector() ; }

    void initialization( std::vector< std::list< Individuals<T> > >& classes, int nbOfClasses ) ;
    bool classification() ;
  
    private:
    std::vector< std::list< Individuals<T> > > myClasses ;     // les classes d'individus
    int myNbOfClasses ;                                  // segmentation en N classes
    int myMaxNbOfRuns ;
    double myThreshold ;
    bool myClassified ;                                  // classification faite ou non?
    ClassifStrategy<T>* myClassifStrategy ;             // pointeur de classe ClassifStrategy
  } ;
}

#endif
