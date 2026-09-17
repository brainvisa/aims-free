/*
 *  Inversion and determinant of a matrix.
 */
#ifndef AIMS_MATH_GAUSSLU_H
#define AIMS_MATH_GAUSSLU_H

#include <aims/config/aimsdata_config.h>
#include <aims/def/general.h>

namespace carto
{
  template <class T> class VolumeRef;
}


/**@name Resolution of linear systems.*/
//@{

/**@name LU decomposition of matrix.
    The source matrix is a. As output, a is the LU decomposition. indx is
    an output vector that records the row permutation effected by the partial
    pivoting. d is output as +1 or -1 depending wether the number of row
    interchanges was even or odd.
*/
//@{
///
float
AimsDecompositionLU( carto::VolumeRef<float> &a,
                     carto::VolumeRef<int32_t>  &indx );
//@}

/**@name LU forward and backward substitution.
    Solves the set of N linear equations A.X=B. Here a is input but in LU
    form. indx is input as the permutation vector returned by LU
    decomposition. b is input as the right-hand side vector B, and returns
    with the solution vector X.
*/
//@{
///
void AimsBackSubstitutionLU( const carto::VolumeRef<float> &a,
                             const carto::VolumeRef<int32_t>  &indx,
                             carto::VolumeRef<float> &b );
//@}


/**@name Inverse of a matrix.
*/
//@{
/// Inverse of a float matrix
carto::VolumeRef<float>
AimsInversionLU( const carto::VolumeRef<float> &matrix );
/// Inverse of a complex float matrix
carto::VolumeRef< cfloat >
AimsInversionLU( const carto::VolumeRef< cfloat > &matrix );
//@}


/**@name Resolution of the linear equation A.X = B.
*/
//@{
/// Resolution in float 
carto::VolumeRef<float>
AimsLinearResolutionLU( const carto::VolumeRef<float> &matrix,
                        const carto::VolumeRef<float> &b );
/// Resolution in complex float
carto::VolumeRef< cfloat >
AimsLinearResolutionLU( const carto::VolumeRef< cfloat > &matrix,
                        const carto::VolumeRef< cfloat > &b );
//@}


/**@name Determinant of a matrix.
*/
//@{
///
float
AimsDeterminantLU( const carto::VolumeRef<float> &matrix );
//@}

/**@name Linear resolution of Toeplitz like system.
*/
//@{
/** Resolution of system R.x = y where R is a toeplitz matrix decomposed
    in r=[r(-(N-1)), ...., r(-1), r(0), r(1), ..., r(N-1)]. The result is 
    given in x
*/
void
AimsToeplitz( const carto::VolumeRef<float>& r,
              const carto::VolumeRef<float>& y,
              carto::VolumeRef<float>& x );
//@}

//@}

#endif







