

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
