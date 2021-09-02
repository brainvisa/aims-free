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


#ifndef AIMS_TRANSFORM_FFT1D_H
#define AIMS_TRANSFORM_FFT1D_H


#include <aims/config/aimsalgo_config.h>
#include <aims/def/general.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <typename T> class Volume;
  template <typename T> class VolumeRef;
}


/**@name Fourier transform.
*/
//@{
/**@name 1D Fast Fourier Transform.
         This Fourier transform is a Fast Fourier transform taken
         from the Numerical Recipee in C.
         @param vec 1D-input data of complex float items
         @param isign +1 for direct transform and -1 for inverse transform
*/
//@{
///
carto::VolumeRef< float > AimsXCor1d(
  const carto::rc_ptr<carto::Volume<float> > & data1,
  const carto::rc_ptr<carto::Volume<float> > & data2 );
//@}
/**@name Cross-correlation.
*/
//@{
/**@name 1D Cross-correlation.
         This cross-correlation is based upon Fast Fourier transform taken
         from the Numerical Recipee in C.
         @param vec 1D-input data of complex float items
         @param vec 1D-input data of complex float items
*/
//@{
///
carto::VolumeRef< cfloat >
AimsFFT1D(const carto::rc_ptr<carto::Volume< cfloat > > & vec,int isign=1 );
//@}

/// Linear to decibel scale transformation
carto::VolumeRef<float>
AimsLinearToDecibel( const carto::rc_ptr<carto::Volume<float> > & sqv );

//@}


#endif
