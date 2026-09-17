
#ifndef AIMS_OPTIMIZATION_SINGLEOPTIMIZER_H
#define AIMS_OPTIMIZATION_SINGLEOPTIMIZER_H

#include <aims/math/mathelem.h>
#include <aims/vector/vector.h>
#include <aims/optimization/objective.h>
#include <aims/optimization/optimizer.h>


//
// class SingleOptimizer
//
template <class T, int D>
class SingleOptimizer : public Optimizer< T, D >
{
  public:
    SingleOptimizer( const ObjectiveFunc< T, D >& func )
                   : Optimizer< T, D >( func, (T)0 )
    { }
    virtual ~SingleOptimizer() { }

    AimsVector<T,D> doit( const AimsVector<T,D> & pinit,
			  const AimsVector<T,D> & deltaP );
};


template <class T,int D> inline
AimsVector<T,D> 
SingleOptimizer<T,D>::doit( const AimsVector<T,D> & pinit,
			    const AimsVector<T,D> & )
{
  std::cout << "param = " << pinit
       << "   func = " << this->_func.eval( pinit )
       << std::endl;
  return pinit;
}

#endif
