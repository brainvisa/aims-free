
#ifndef AIMS_OPTIMIZATION_OPTIMIZER_H
#define AIMS_OPTIMIZATION_OPTIMIZER_H

#include <aims/math/mathelem.h>
#include <aims/vector/vector.h>
#include <aims/optimization/objective.h>
#include <cartobase/object/attributed.h>

//
// class Optimizer
//

template <class T, int D>
class OptimizerProbe {
public :
  OptimizerProbe() {}
  virtual ~OptimizerProbe() {}

  virtual OptimizerProbe * clone() { return new OptimizerProbe ; }
  virtual void iteration( const AimsVector<T,D> &, 
			  const carto::AttributedObject&,
			  const float * = 0) {}
  virtual void test( const AimsVector<T,D> &, 
		     const carto::AttributedObject &,
		     const float * = 0) {}
  virtual void end() {}
} ;


template <class T, int D>
class Optimizer
{
public:
  Optimizer( const ObjectiveFunc< T,D >& func, T error, 
	     OptimizerProbe<T, D> * probe = 0 ) :
    _func( func ), _error( error )
    { 
      if ( probe != 0 )
	_probe = probe->clone() ;
      else
	_probe = new OptimizerProbe<T,D>() ;
    }
  
  virtual ~Optimizer() { }
  
  virtual AimsVector<T,D> doit( const AimsVector<T,D> &,
				const AimsVector<T,D> & )
  { return AimsVector<T,D>( (T)0 ); }
  
  void setProbe( OptimizerProbe<T,D> * probe )
  { _probe = probe->clone() ; }
  
protected:
  const ObjectiveFunc< T , D >& _func;
  T _error;

  OptimizerProbe<T,D> * _probe ;
};

#endif
