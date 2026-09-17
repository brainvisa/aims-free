
#ifndef AIMS_MATH_ELTRAN_H
#define AIMS_MATH_ELTRAN_H

#include <aims/def/general.h>

namespace carto
{
  template <class T> class VolumeRef;
}


/** @name Hessenberg accumulation */
template < class T >
class HessenbergAccumulation
{
public:

  /** Constructor and destructor */
  //@{
  /// constructor
  HessenbergAccumulation()  { }
  /// destructor
  virtual ~HessenbergAccumulation() { }
  //@}

  /** Matrix Hessenberg accumulation computation. \\
      This function is adapted from the Eispack routines. \\
      It returns a matrix of accumulations of the stabilized
      elementary similarity transformations used in the reduction
      of a real general matrix to upper Hessenberg form by
      HessenbergReduction. \\
      @param isc contains information on the rows and columns
      interchanged in the reduction by HessenbergReduction.
  */
  carto::VolumeRef< T > doit( const carto::VolumeRef< T > &,
                              const carto::VolumeRef< short > *isc = NULL );
};

#endif
