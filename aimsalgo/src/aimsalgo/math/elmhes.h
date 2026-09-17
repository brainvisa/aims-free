
#ifndef AIMS_MATH_ELMHES_H
#define AIMS_MATH_ELMHES_H

#include <aims/def/general.h>

namespace carto
{
  template <class T> class VolumeRef;
}


/** @name Hessenberg reduction of a matrix. */
template < class T >
class HessenbergReduction
{
public:

  /** Constructor and destructor */
  //@{
  /// constructor
  HessenbergReduction()  { }
  /// destructor
  virtual ~HessenbergReduction() { }
  //@}

  /** Hessenberg reduction computation. \\
      This function is adapted from the Numerical Recipes in C. \\
      This function returns the Hessenberg matrix form of the matrix
      given on input. \\
      @param isc keeps track of interchanges and is a vector used to
      backtrack eigenvectors.
  */
  carto::VolumeRef< T > doit( carto::VolumeRef< T >,
                              carto::VolumeRef< short > *isc = NULL );
};

#endif
