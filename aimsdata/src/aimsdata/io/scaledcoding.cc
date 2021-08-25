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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cstdlib>
#include <limits>
#include <aims/io/scaledcoding.h>
#include <cartodata/volume/volume.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace aims
{

  template <>
  bool canEncodeAsScaledS16( const carto::Volume<float> & vol, float & slope,
                             float & offset, bool enableoffset,
                             double *maxerr )
  {
    vector<long> strides(4);
    strides[0] = &vol.at(1) - &vol.at(0);
    strides[1] = &vol.at(0, 1) - &vol.at(0);
    strides[2] = &vol.at(0, 0, 1) - &vol.at(0);
    strides[3] = &vol.at(0, 0, 0, 1) - &vol.at(0);
    return soma::canEncodeAsScaledS16( &vol.at(0), slope, offset, strides,
                                       vol.getSize(), enableoffset, maxerr );
  }


  template <>
  bool canEncodeAsScaledS16( const carto::Volume<double> & vol, float & slope,
                             float & offset, bool enableoffset,
                             double *maxerr )
  {
    vector<long> strides(4);
    strides[0] = &vol.at(1) - &vol.at(0);
    strides[1] = &vol.at(0, 1) - &vol.at(0);
    strides[2] = &vol.at(0, 0, 1) - &vol.at(0);
    strides[3] = &vol.at(0, 0, 0, 1) - &vol.at(0);
    return soma::canEncodeAsScaledS16( &vol.at(0), slope, offset, strides,
                                       vol.getSize(), enableoffset, maxerr );
  }


  template <typename INP, typename OUTP> 
  soma::ScaledEncodingInfo ScaledEncoding<INP, OUTP>::info(
    const carto::Volume<INP> & vol )
  {
    vector<long> strides(4);
    strides[0] = &vol(1) - &vol(0);
    strides[1] = &vol(0, 1) - &vol(0);
    strides[2] = &vol(0, 0, 1) - &vol(0);
    strides[3] = &vol(0, 0, 0, 1) - &vol(0);
    soma::ScaledEncoding<INP, OUTP> ssenc;
    return ssenc.info( &vol(0), strides, vol.getSize() );
  }


  template <typename INP, typename OUTP> 
  soma::ScaledEncodingInfo ScaledEncoding<INP, OUTP>::rescale(
    const carto::Volume<INP> & in, carto::Volume<OUTP> & out )
  {
    soma::ScaledEncoding<INP, OUTP> ssenc;
    vector<long> strides(4);
    strides[0] = &in(1) - &in(0);
    strides[1] = &in(0, 1) - &in(0);
    strides[2] = &in(0, 0, 1) - &in(0);
    strides[3] = &in(0, 0, 0, 1) - &in(0);
    return ssenc.rescale( &in(0), strides, in.getSize(), &out(0) );
  }


  // Instanciates ScaledEncoding class
  template class ScaledEncoding<float, int8_t>;
  template class ScaledEncoding<double, int8_t>;
  template class ScaledEncoding<float, uint8_t>;
  template class ScaledEncoding<double, uint8_t>;
  template class ScaledEncoding<float, int16_t>;
  template class ScaledEncoding<double, int16_t>;
  template class ScaledEncoding<float, uint16_t>;
  template class ScaledEncoding<double, uint16_t>;
  template class ScaledEncoding<float, int32_t>;
  template class ScaledEncoding<double, int32_t>;
  template class ScaledEncoding<float, uint32_t>;
  template class ScaledEncoding<double, uint32_t>;
}

