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


#ifndef AIMS_OPTIMIZATION_OPTIMIZER_H
#define AIMS_OPTIMIZATION_OPTIMIZER_H

#include <aims/math/mathelem.h>
#include <aims/vector/vector.h>
#include <aims/def/assert.h>
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
