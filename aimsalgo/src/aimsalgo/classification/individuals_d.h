
#ifndef INDIVIDUALS_D_H
#define INDIVIDUALS_D_H


#include <aims/classification/individuals.h>



template<class T>
aims::Individuals<T>::Individuals( const Point3df& pos, const std::vector<T>& val )
{
  myPos = pos ;
  myVal = val ;
}

template<class T>
aims::Individuals<T>::~Individuals()
{
}

#endif
