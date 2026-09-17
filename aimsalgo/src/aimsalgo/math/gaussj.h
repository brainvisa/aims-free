
#ifndef AIMS_MATH_GAUSSJ_H
#define AIMS_MATH_GAUSSJ_H

#include <aims/def/general.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <typename T> class Volume;
}


/** @name Gauss-Jordan elimination */
template < class T >
class GaussJordan
{
public:

  /** Constructor and destructor */
  //@{
  /// constructor
  GaussJordan()  { }
  /// destructor
  virtual ~GaussJordan() { }
  //@}

  /** Gauss-Jordan elimination procedure. \\
      This function is adapted from the Numerical Recipes in C. \\
      It returns a matrix corresponding to the set of solution vectors of a 
      linear equation solution by Gauss-Jordan elimination of the input 
      matrix. \\
      The first parameter is the input matrix. On ouput, it is replaced by
      its matrix inverse. \\
      The second input is a matrix containing the right-hand side
      vectors. On output, this matrix is replaced by the 
      corresponding set of solution vectors (those that are also returned
      by the function). \\
      @param mUtil allows to perform the elimination only on a subset of
      the input matrix where mUtils represents the number of columns of the
      input matrix used for the computation.
  */
  bool doit( carto::rc_ptr<carto::Volume< T > > &,
             carto::rc_ptr<carto::Volume< T > > &, int mUtil=-1 );
};

#endif
