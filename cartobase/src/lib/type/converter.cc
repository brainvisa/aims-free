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

#include <cartobase/type/converter_d.h>

namespace carto
{

  RescalerInfo::RescalerInfo()
    : usevtypelimits(false),
      vmin( std::numeric_limits<double>::quiet_NaN() ),
      vmax( std::numeric_limits<double>::quiet_NaN() ),
      omin( std::numeric_limits<double>::quiet_NaN() ),
      omax( std::numeric_limits<double>::quiet_NaN() )
  {
  }

  bool RescalerInfo::explicitRescale() const
  {
    return usevtypelimits
      || !std::isnan( vmin )
      || !std::isnan( vmax )
      || !std::isnan( omin )
      || !std::isnan( omax );
  }

  template class DefaultedRescalerInfo<int8_t, int8_t>;
  template class DefaultedRescalerInfo<int8_t, uint8_t>;
  template class DefaultedRescalerInfo<int8_t, int16_t>;
  template class DefaultedRescalerInfo<int8_t, uint16_t>;
  template class DefaultedRescalerInfo<int8_t, int32_t>;
  template class DefaultedRescalerInfo<int8_t, uint32_t>;
  template class DefaultedRescalerInfo<int8_t, float>;
  template class DefaultedRescalerInfo<int8_t, double>;

  template class DefaultedRescalerInfo<uint8_t, int8_t>;
  template class DefaultedRescalerInfo<uint8_t, uint8_t>;
  template class DefaultedRescalerInfo<uint8_t, int16_t>;
  template class DefaultedRescalerInfo<uint8_t, uint16_t>;
  template class DefaultedRescalerInfo<uint8_t, int32_t>;
  template class DefaultedRescalerInfo<uint8_t, uint32_t>;
  template class DefaultedRescalerInfo<uint8_t, float>;
  template class DefaultedRescalerInfo<uint8_t, double>;

  template class DefaultedRescalerInfo<int16_t, int8_t>;
  template class DefaultedRescalerInfo<int16_t, uint8_t>;
  template class DefaultedRescalerInfo<int16_t, int16_t>;
  template class DefaultedRescalerInfo<int16_t, uint16_t>;
  template class DefaultedRescalerInfo<int16_t, int32_t>;
  template class DefaultedRescalerInfo<int16_t, uint32_t>;
  template class DefaultedRescalerInfo<int16_t, float>;
  template class DefaultedRescalerInfo<int16_t, double>;

  template class DefaultedRescalerInfo<uint16_t, int8_t>;
  template class DefaultedRescalerInfo<uint16_t, uint8_t>;
  template class DefaultedRescalerInfo<uint16_t, int16_t>;
  template class DefaultedRescalerInfo<uint16_t, uint16_t>;
  template class DefaultedRescalerInfo<uint16_t, int32_t>;
  template class DefaultedRescalerInfo<uint16_t, uint32_t>;
  template class DefaultedRescalerInfo<uint16_t, float>;
  template class DefaultedRescalerInfo<uint16_t, double>;

  template class DefaultedRescalerInfo<int32_t, int8_t>;
  template class DefaultedRescalerInfo<int32_t, uint8_t>;
  template class DefaultedRescalerInfo<int32_t, int16_t>;
  template class DefaultedRescalerInfo<int32_t, uint16_t>;
  template class DefaultedRescalerInfo<int32_t, int32_t>;
  template class DefaultedRescalerInfo<int32_t, uint32_t>;
  template class DefaultedRescalerInfo<int32_t, float>;
  template class DefaultedRescalerInfo<int32_t, double>;

  template class DefaultedRescalerInfo<uint32_t, int8_t>;
  template class DefaultedRescalerInfo<uint32_t, uint8_t>;
  template class DefaultedRescalerInfo<uint32_t, int16_t>;
  template class DefaultedRescalerInfo<uint32_t, uint16_t>;
  template class DefaultedRescalerInfo<uint32_t, int32_t>;
  template class DefaultedRescalerInfo<uint32_t, uint32_t>;
  template class DefaultedRescalerInfo<uint32_t, float>;
  template class DefaultedRescalerInfo<uint32_t, double>;

  template class DefaultedRescalerInfo<float, int8_t>;
  template class DefaultedRescalerInfo<float, uint8_t>;
  template class DefaultedRescalerInfo<float, int16_t>;
  template class DefaultedRescalerInfo<float, uint16_t>;
  template class DefaultedRescalerInfo<float, int32_t>;
  template class DefaultedRescalerInfo<float, uint32_t>;
  template class DefaultedRescalerInfo<float, float>;
  template class DefaultedRescalerInfo<float, double>;

  template class DefaultedRescalerInfo<double, int8_t>;
  template class DefaultedRescalerInfo<double, uint8_t>;
  template class DefaultedRescalerInfo<double, int16_t>;
  template class DefaultedRescalerInfo<double, uint16_t>;
  template class DefaultedRescalerInfo<double, int32_t>;
  template class DefaultedRescalerInfo<double, uint32_t>;
  template class DefaultedRescalerInfo<double, float>;
  template class DefaultedRescalerInfo<double, double>;

  // These three specializations seem to be needed in order to compile
  // the AimsFileConvert command.
  template class DefaultedRescalerInfo<char, short>;
  template class DefaultedRescalerInfo<unsigned long, short>;
  template class DefaultedRescalerInfo<long, short>;

}
