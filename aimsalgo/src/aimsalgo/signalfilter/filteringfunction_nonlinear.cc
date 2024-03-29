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

#include <aims/signalfilter/filteringfunction_nonlinear_d.h>

#define AIMS_TEMPLATE_NONLIN_FILTERFUNC( NAME )                              \
  template class NAME<int8_t>;                                               \
  template class NAME<uint8_t>;                                              \
  template class NAME<int16_t>;                                              \
  template class NAME<uint16_t>;                                             \
  template class NAME<int32_t>;                                              \
  template class NAME<uint32_t>;                                             \
  template class NAME<int64_t>;                                              \
  template class NAME<uint64_t>;                                             \
  template class NAME<float>;                                                \
  template class NAME<double>

namespace aims {
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(NonLinFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(NonLinFilterFuncFactory);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(MedianFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(NotNullMedianFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(MeanFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(NotNullMeanFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(MinFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(MaxFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(MajorityFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(NotNullMajorityFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(ExtremaDifferenceFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(SumFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(VarFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(StDevFilterFunc);
}
