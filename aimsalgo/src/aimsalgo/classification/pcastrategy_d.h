
#ifndef PCASTRATEGY_D_H
#define PCASTRATEGY_D_H

#include <aims/classification/pcastrategy.h>


template <class T>
aims::PcaStrategy<T>::PcaStrategy( const PcaStrategy<T>& pcaStrat ) :
  aims::ClassifStrategy<T>( pcaStrat )
{
  myDistance = pcaStrat.myDistance ;
}


template <class T>
aims::PcaStrategy<T>::PcaStrategy( int nbIterations,
			     DistanceType distanceType, 
			     const std::vector< aims::Individuals<T> >& codeVector ) :
  aims::ClassifStrategy<T>( nbIterations )
{
  switch( distanceType ) {
  case NORM1 :
    myDistance = aims::Distance<T>::norm1 ;
    break ;
  case NORM2 :
    myDistance = aims::Distance<T>::norm2 ;
    break ;
  case NORM2SQR :
    myDistance = aims::Distance<T>::norm2sqr ;
    break ;
  case INFNORM :
    myDistance = aims::Distance<T>::infiniteNorm ;
    break ;
  }
  
  // TODO
}


template <class T>
aims::PcaStrategy<T>::~PcaStrategy()
{
}


template <class T>
aims::ClassifStrategy<T> * aims::PcaStrategy<T>::clone() const
{
  return new aims::PcaStrategy<T>( *this ) ;
}


template <class T>
void aims::PcaStrategy<T>::analyse( const std::vector< std::list< aims::Individuals<T> > >& classes )
{
}


template <class T>
int aims::PcaStrategy<T>::agregation( const aims::Individuals<T>& individual )
{

  return this->indMin ;
}


template <class T>
bool aims::PcaStrategy<T>::classificationCompleted( int nbChanges )
{
  if ( nbChanges == 0 ) return false ;
  else return true ;
}


template <class T>
float aims::PcaStrategy<T>::distance( const aims::Individuals<T>& individual, int classe )
{
  T::not_implemented();
  return 0.0; // ### BEN ALORS ? IL MANQUE LE CODE DE CETTE FONCTION !
}

#endif
