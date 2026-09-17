
#ifndef AIMS_MATH_HOUSEHOLDER_H
#define AIMS_MATH_HOUSEHOLDER_H

#include <aims/config/aimsalgo_config.h>
#include <aims/def/general.h>

namespace carto
{
  template <class T> class VolumeRef;
}


/** @name Householder tridiagonalization */
template < class T >
class HouseholderTridiag
{
public:

  /** Constructor and destructor */
  //@{
  /// constructor
  HouseholderTridiag() { }
  /// destructor
  virtual ~HouseholderTridiag() { }
  //@}

  /** Real symmetric matrix Householder tridiagonalization. \\
      This function is adapted from the Numerical Recipes in C. \\
      The first parameter is the input real symmetric matrix. On output,
      it is replaced by the orthogonal matrix effecting the transformation. \\
      The second parameter is output as a vector of the diagonal elements
      of the tridiagonal matrix. \\
      The last parameter is output as a vector of the off-diagonal elements.
  */
  void doit( carto::VolumeRef< T >, carto::VolumeRef< T >,
             carto::VolumeRef< T > );
};

#endif
