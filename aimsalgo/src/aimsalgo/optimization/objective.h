
#ifndef AIMS_OPTIMIZATION_OBJECTIVE_H
#define AIMS_OPTIMIZATION_OBJECTIVE_H

#include <cartobase/object/attributed.h>

template <class T, int D> class AimsVector;

//
// class ObjectiveFunc
// Base class for cost functions
//
template <class T, int P>
class ObjectiveFunc
{
public:
  typedef AimsVector<T, P> ParametersType;

  ObjectiveFunc( ) { }
  virtual ~ObjectiveFunc() { }
  
  virtual T eval( const ParametersType& ) const { return (T)0; }
  
  // This method adds int the attributed object ueseful parameters of the
  // objective function to be monitored
  virtual void fillMonitoredParameters( carto::AttributedObject& ) const {}
};


#endif
