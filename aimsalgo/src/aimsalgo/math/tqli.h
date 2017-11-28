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


#ifndef AIMS_MATH_TQLI_H
#define AIMS_MATH_TQLI_H

#include <aims/def/general.h>

template <class T> class AimsData;


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
  void doit( AimsData< T >&, AimsData< T >&, AimsData< T >& );
};

#endif
