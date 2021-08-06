/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

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







