
#ifndef AIMS_MATH_BALBAK_H
#define AIMS_MATH_BALBAK_H

#include <aims/def/general.h>

namespace carto
{
  template <class T> class VolumeRef;
}


/** @name Back-balancing of a matrix */
template < class T >
class BackBalancing
{
public:

  /** Constructor and destructor */
  //@{
  /// constructor
  BackBalancing() { }
  /// destructor
  virtual ~BackBalancing() { }
  //@}

  /** Matrix back-balancing computation. \\
      This function is adapted from the Eispack routines. \\
      This routine returns the eigenvectors of a real general matrix
      by back transformaing those of the corresponding balanced
      matrix determined by Balancing and passed on input.\\
      Matrices of the real and imaginary parts of eigenvalues are given 
      on input to forms the balancing matrix of eigenvectors according 
      to the eigenvalue types, i.e. real or imaginary. \\
      @param sc contains information determining the permutations and
      scaling factors used by Balancing.
   */
  carto::VolumeRef< T > doit( carto::VolumeRef< T >,
                              const carto::VolumeRef< T > &,
                              const carto::VolumeRef< T > &,
                              const carto::VolumeRef< T > *sc = NULL );
};

#endif
