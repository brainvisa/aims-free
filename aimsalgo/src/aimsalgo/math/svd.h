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


#ifndef AIMS_MATH_SVD_H
#define AIMS_MATH_SVD_H

#include <aims/def/general.h>

template <class T> class AimsData;

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
  AimsData< T > doit( AimsData< T >&, AimsData< T > *v = NULL );

  /// sort the U and V matrices and the W vector in decreasing order
  void sort( AimsData< T >&, AimsData< T >&, AimsData< T > *v = NULL );

  AimsData< T > backwardSubstitution( const AimsData< T >& U, 
                                      const AimsData< T >& W, 
                                      const AimsData< T >& V, 
                                      const AimsData< T >& S );

private:

  SVDReturnType retType;
};

#endif
