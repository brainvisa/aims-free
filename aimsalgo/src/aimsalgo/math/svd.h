
#ifndef AIMS_MATH_SVD_H
#define AIMS_MATH_SVD_H

#include <aims/def/general.h>

namespace carto
{
  template <class T> class VolumeRef;
}

/** @name Singular Value Decomposition. \\
\\
    SVD decomposition of a (m*n) matrix A such that \\
\\
        A = U.W.V' \\
\\
    where \\
\\
        U is a (m*n) matrix, \\
        V is a (n*n) matrix, \\
        W is a vector of size n. \\
*/
template< class T >
class AimsSVD
{
public:

  enum SVDReturnType
  {
    MatrixOfSingularValues,
    VectorOfSingularValues
  };

  /** Constructor and destructor */
  //@{
  /// constructor
  AimsSVD( SVDReturnType rt=MatrixOfSingularValues ) : retType( rt ) { }
  /// destructor
  virtual ~AimsSVD() { }
  //@}

  void setReturnType( SVDReturnType rt ) { retType = rt; }

  /** Singular Value Decomposition. \\
      This function is adapted from the Numerical Recipes in C. \\
      Returns the diagonal matrix of singular values W of the input
      matrix if the SVDReturnType is set to MatrixOfSingularValues or
      a vector of singular values if set to VectorOfSingularValues. \\
      The first parameter is the input matrix. On output, it is
      replaced by the (m*n) matrix U. \\
      @param v is output as the (n*n) matrix V (and not its transpose!).
   */
  carto::VolumeRef< T > doit( carto::VolumeRef< T >&,
                              carto::VolumeRef< T > *v = NULL );

  /// sort the U and V matrices and the W vector in decreasing order
  void sort( carto::VolumeRef< T >&, carto::VolumeRef< T >&,
             carto::VolumeRef< T > *v = NULL );

  carto::VolumeRef< T > backwardSubstitution( const carto::VolumeRef< T >& U,
                                              const carto::VolumeRef< T >& W,
                                              const carto::VolumeRef< T >& V,
                                              const carto::VolumeRef< T >& S );

private:

  SVDReturnType retType;
};

#endif
