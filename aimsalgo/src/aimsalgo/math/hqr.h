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
