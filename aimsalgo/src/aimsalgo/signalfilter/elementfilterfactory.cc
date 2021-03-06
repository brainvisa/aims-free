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

#include <aims/signalfilter/elementfilterfactory_d.h>
#include <aims/rgb/rgb.h>

#define AIMS_INSTANTIATE_FILTERS( NAME )                                     \
  template class NAME<int8_t>;                                               \
  template class NAME<uint8_t>;                                              \
  template class NAME<int16_t>;                                              \
  template class NAME<uint16_t>;                                             \
  template class NAME<int32_t>;                                              \
  template class NAME<uint32_t>;                                             \
  template class NAME<int64_t>;                                              \
  template class NAME<uint64_t>;                                             \
  template class NAME<float>;                                                \
  template class NAME<double>;                                               \
  template class NAME<AimsRGB>;                                              \
  template class NAME<AimsRGBA>

namespace aims{
  AIMS_INSTANTIATE_FILTERS( ElementFilterFactory );
  AIMS_INSTANTIATE_FILTERS( MinFilter );
  AIMS_INSTANTIATE_FILTERS( MaxFilter );
  AIMS_INSTANTIATE_FILTERS( MedianFilter );
  AIMS_INSTANTIATE_FILTERS( NotNullMedianFilter );
  AIMS_INSTANTIATE_FILTERS( MeanFilter );
  AIMS_INSTANTIATE_FILTERS( NotNullMeanFilter );
  AIMS_INSTANTIATE_FILTERS( MajorityFilter );
  AIMS_INSTANTIATE_FILTERS( ExtremaDifferenceFilter );
  AIMS_INSTANTIATE_FILTERS( SumFilter );
  AIMS_INSTANTIATE_FILTERS( VarFilter );
  AIMS_INSTANTIATE_FILTERS( StDevFilter );
}
