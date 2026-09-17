
#ifndef AIMS_MATH_HQR_H
#define AIMS_MATH_HQR_H

#include <aims/config/aimsalgo_config.h>
#include <aims/def/general.h>

namespace carto
{
  template <class T> class VolumeRef;
}


/** @name QR transformation for real Hessenberg matrices */
template < class T >
class HessenbergQR
{
public:

  /** Constructor and destructor */
  //@{
  /// constructor
  HessenbergQR() { }
  /// destructor
  virtual ~HessenbergQR() { }
  //@}

  /** Hessenberg matrices' QR transformation. \\
      This function is adapted from the Eispack routine 'hqr2.f'. \\
      This routine returns the real parts of the eigenvalues of a real upper 
      Hessenberg matrix passed on input by the QR method. \\
      The second parameter is output as the imaginary parts of the 
      eigenvalues. \\
      @param zz contains (on output) the real and imaginary parts of the
      eigenvectors. If the i-th eigenvalue is real, the i-th column of zz
      contains its eigenvector. If the i-th eigenvalue is complex with
      positive imaginary part, the i-th and (i+1)-th columns of zz contain
      the real and imaginary parts of its eigenvector, and an other 
      eigenvector is formed by its complex conjugate.
   */
  carto::VolumeRef< T > doit( carto::VolumeRef< T >, carto::VolumeRef< T > ,
                              carto::VolumeRef< T > *zz = NULL );
};

#endif
