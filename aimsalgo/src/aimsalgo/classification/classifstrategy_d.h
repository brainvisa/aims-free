

#ifndef CLASSIFSTRATEGY_D_H
#define CLASSIFSTRATEGY_D_H

#include <aims/classification/classifstrategy.h>
#include <vector>
#include <list>


template <class T>
aims::ClassifStrategy<T>::ClassifStrategy( int maxNbOfIterations ) :
  myMaxNbOfIterations( maxNbOfIterations ), myValidStrategy( true ), myInit( true ), myCodeVectorsGiven( false )
{
}


template <class T>
aims::ClassifStrategy<T>::~ClassifStrategy()
{
}


#endif
