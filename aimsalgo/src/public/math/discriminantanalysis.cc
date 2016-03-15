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


#include <aims/math/discriminantanalysis_d.h>

namespace aims {

template void DiscriminantAnalysisElement::doIt<int8_t>(const AimsData<int8_t>&) ;
template void DiscriminantAnalysisElement::doIt<uint8_t>(const AimsData<uint8_t>&) ;
template void DiscriminantAnalysisElement::doIt<int16_t>(const AimsData<int16_t>&) ;
template void DiscriminantAnalysisElement::doIt<uint16_t>(const AimsData<uint16_t>&) ;
template void DiscriminantAnalysisElement::doIt<int32_t>(const AimsData<int32_t>&) ;
template void DiscriminantAnalysisElement::doIt<uint32_t>(const AimsData<uint32_t>&) ;
template void DiscriminantAnalysisElement::doIt<float>(const AimsData<float>&) ;
template void DiscriminantAnalysisElement::doIt<double>(const AimsData<double>&) ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template void DiscriminantAnalysisElement::doIt<char>(const AimsData<char>&) ;
#endif
template void DiscriminantAnalysisElement::doIt<int64_t>(const AimsData<int64_t>&) ;
template void DiscriminantAnalysisElement::doIt<uint64_t>(const AimsData<uint64_t>&) ;


template void DiscriminantAnalysisElement::doIt<int8_t>(const std::list< Point3d>&, const AimsData<int8_t>&) ;
template void DiscriminantAnalysisElement::doIt<uint8_t>(const std::list< Point3d>&, const AimsData<uint8_t>&) ;
template void DiscriminantAnalysisElement::doIt<int16_t>(const std::list< Point3d>&, const AimsData<int16_t>&) ;
template void DiscriminantAnalysisElement::doIt<uint16_t>(const std::list< Point3d>&, const AimsData<uint16_t>&) ;
template void DiscriminantAnalysisElement::doIt<int32_t>(const std::list< Point3d>&, const AimsData<int32_t>&) ;
template void DiscriminantAnalysisElement::doIt<uint32_t>(const std::list< Point3d>&, const AimsData<uint32_t>&) ;
template void DiscriminantAnalysisElement::doIt<float>(const std::list< Point3d>&, const AimsData<float>&) ;
template void DiscriminantAnalysisElement::doIt<double>(const std::list< Point3d>&, const AimsData<double>&) ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template void DiscriminantAnalysisElement::doIt<char>(const std::list< Point3d>&, const AimsData<char>&) ;
#endif
template void DiscriminantAnalysisElement::doIt<int64_t>(const std::list< Point3d>&, const AimsData<int64_t>&) ;
template void DiscriminantAnalysisElement::doIt<uint64_t>(const std::list< Point3d>&, const AimsData<uint64_t>&) ;


template class DiscriminantAnalysis<int8_t> ;
template class DiscriminantAnalysis<uint8_t> ;
template class DiscriminantAnalysis<int16_t> ;
template class DiscriminantAnalysis<uint16_t> ;
template class DiscriminantAnalysis<int32_t> ;
template class DiscriminantAnalysis<uint32_t> ;
template class DiscriminantAnalysis<float> ;
template class DiscriminantAnalysis<double> ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class DiscriminantAnalysis<char> ;
#endif
template class DiscriminantAnalysis<int64_t> ;
template class DiscriminantAnalysis<uint64_t> ;

} // namespace aims
