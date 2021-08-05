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
