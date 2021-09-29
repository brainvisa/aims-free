/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


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
