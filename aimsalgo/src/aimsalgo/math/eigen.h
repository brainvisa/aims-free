
#ifndef AIMS_MATH_EIGEN_H
#define AIMS_MATH_EIGEN_H

#include <aims/def/general.h>

namespace carto
{
  template <class T> class VolumeRef;
}


/** @name Eigen system resolution. */
template < class T >
class AimsEigen
{
public:

  enum EigenReturnType
  {
    MatrixOfEigenValues,
    VectorOfEigenValues
  };

  /** @name Constructor and destructor */
  //@{
  /** constructor. \\
      Here, we suppose that we mainly want to solve general eigen
      system with a non-symmetric matrix on input */
  AimsEigen( EigenReturnType rt=MatrixOfEigenValues ) : retType( rt ) { }
  /// destructor
  virtual ~AimsEigen() { }
  //@}

  void setReturnType( EigenReturnType rt ) { retType = rt; }

  /** Eigen system resolution function. \\
      The various computation function used within this one are adapted
      from the Numerical Recipes in C.\\
      This function returns the diagonal matrix of eigenvalues, and the
      input matrix is transformed into a matrix which columns are the 
      eigenvectors. \\
      @param wi return the diagonal matrix of the imaginary parts of the
      eigenvalues if *wi exists.
  */
  carto::VolumeRef< T > doit( carto::VolumeRef< T >,
                              carto::VolumeRef< T > *wi = NULL );

  /// Sort the eigenvectors and eigenvalues in decreasing order.
  void sort( carto::VolumeRef< T > eigenvectors,
             carto::VolumeRef< T > eigenvalues,
             carto::VolumeRef< T > *wi = NULL );

private:

  EigenReturnType retType;
};

#endif
