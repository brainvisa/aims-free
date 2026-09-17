
#ifndef AIMS_MATH_BALANC_H
#define AIMS_MATH_BALANC_H

#include <aims/def/general.h>

namespace carto
{
  template <class T> class VolumeRef;
}


/** @name Balancing of a matrix */
template < class T >
class Balancing
{
public:

  /** Constructor and destructor */
  //@{
  /// cconstructor
  Balancing();
  /// destructor
  virtual ~Balancing() { }
  //@}

  /** Matrix balancing computation. \\
      This function is adapted from the Numerical Recipes in C. \\
      Given an input matrix, this routine returns a balanced
      matrix with identical eigenvalues. A symmetric matrix is
      already balanced and is unaffected by this procedure.
  */
  carto::VolumeRef< T > doit( carto::VolumeRef< T >, carto::VolumeRef< T > *sc = NULL );

  /** Floating-point precision. \\
      @param r should be the machine's floating-point radix.
  */
  void setRadix( T r ) { radix = r; }

private:

  T radix;
};

#endif
