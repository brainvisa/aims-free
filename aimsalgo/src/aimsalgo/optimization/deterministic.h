
#ifndef AIMS_OPTIMIZATION_DETERMINISTIC_H
#define AIMS_OPTIMIZATION_DETERMINISTIC_H

#include <cstdlib>
#include <aims/math/mathelem.h>
#include <aims/math/random.h>
#include <aims/vector/vector.h>
#include <aims/def/assert.h>
#include <aims/optimization/objective.h>
#include <aims/optimization/optimizer.h>


//
// class DetermOptimizer
//
template <class T, int D>
class DetermOptimizer : public Optimizer<T, D>
{
  public:
    DetermOptimizer( const ObjectiveFunc<T,D>& func, T error,
                     int maxIter = 100000, int stability = 1,
                     bool verbose = false )
                   : Optimizer< T, D >( func, error ),
                     _maxIter( maxIter ), _stability( stability ),
                     _verbose( verbose )
    { }
    virtual ~DetermOptimizer() { }

    AimsVector<T,D> doit( const AimsVector<T,D> & pinit,
			  const AimsVector<T,D> & deltaP );

  private:
    int _maxIter;
    int _stability;
    bool _verbose;
};


template <class T,int D> inline
AimsVector<T,D> 
DetermOptimizer<T,D>::doit( const AimsVector<T,D> & pinit,
			    const AimsVector<T,D> & deltaP )
{
  AimsVector<T,D> p( pinit ), new_p( pinit ), dP( deltaP );
  T eval, new_eval, old_eval, err;

  old_eval = eval = new_eval = this->_func.eval( p );
  int iter=0, cntStab = 0, k;
  do
  {
    for ( k = 0; k < D; k++ )
      new_p[ k ] = p[k] + (T)UniformRandom(-1.0,+1.0) * dP[ k ];  
    new_eval = this->_func.eval( new_p );
    if ( new_eval < eval )
    {
      old_eval = eval;
      p = new_p;
      eval = new_eval;
    }
    if ( ( err = fabs( eval - old_eval ) ) < this->_error )
    {
      cntStab++;
    }
    else if ( cntStab )
    {
      cntStab = 0;
    }
    if ( _verbose )
      std::cout
           << "it=" << iter
           << " param=" << p
           << " stab=" << cntStab
           << " objective=" << eval
           << " error=" << err
           << std::endl;
    iter++;
    dP *= (T)0.995f;
    ASSERT( iter != _maxIter );
  }
  while ( cntStab != _stability );

  return p;
}

#endif
