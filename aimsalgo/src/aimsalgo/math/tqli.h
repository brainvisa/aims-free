
#ifndef AIMS_MATH_TQLI_H
#define AIMS_MATH_TQLI_H

#include <aims/def/general.h>

namespace carto
{
  template <class T> class VolumeRef;
}


/** @name TQLI decompposition of a tridiagonal matrix */
template < class T >
class DecompositionTQLI
{
public:

  /** Constructor and destructor */
  //@{
  /// constructor
  DecompositionTQLI() { }
  /// destructor
  virtual ~DecompositionTQLI() { }
  //@}

  /** TQLI decomposition of a tridiagonal matrix. \\
      This function is adapted from the Numerical Recipes in C. \\
      This routine determines the eigenvalues and eigenvectors of a
      real, symmetric, tridiagonal matrix, or of a real, symmetric matrix
      previously reduced by HouseholderTridiag. \\
      The first parameter contains (on input) the diagonal elements of the
      tridiagonal matrix. On output, it is replaced by the eigenvalues. \\
      The second parameter is the vector of the subdiagonal elements of the
      tridiagonal matrix. \\
      The last parameter is set (on input) as an identity matrix if the
      eigenvectors of a tridiagonal matrix are desired, or as the matrix
      output by HouseholderTridiag if the matrix was previously reduced. On
      output, it is replaced by the normalized eigenvectors. The k-th
      column of this matrix corresponds to the k-th eigenvector.
   */
  void doit( carto::VolumeRef< T >, carto::VolumeRef< T >,
             carto::VolumeRef< T > );
};

#endif
