
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/math/discriminantanalysis_d.h>

namespace aims {

template void DiscriminantAnalysisElement::doIt<int8_t>(const carto::rc_ptr<carto::Volume<int8_t> >&) ;
template void DiscriminantAnalysisElement::doIt<uint8_t>(const carto::rc_ptr<carto::Volume<uint8_t> >&) ;
template void DiscriminantAnalysisElement::doIt<int16_t>(const carto::rc_ptr<carto::Volume<int16_t> >&) ;
template void DiscriminantAnalysisElement::doIt<uint16_t>(const carto::rc_ptr<carto::Volume<uint16_t> >&) ;
template void DiscriminantAnalysisElement::doIt<int32_t>(const carto::rc_ptr<carto::Volume<int32_t> >&) ;
template void DiscriminantAnalysisElement::doIt<uint32_t>(const carto::rc_ptr<carto::Volume<uint32_t> >&) ;
template void DiscriminantAnalysisElement::doIt<float>(const carto::rc_ptr<carto::Volume<float> >&) ;
template void DiscriminantAnalysisElement::doIt<double>(const carto::rc_ptr<carto::Volume<double> >&) ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template void DiscriminantAnalysisElement::doIt<char>(const carto::rc_ptr<carto::Volume<char> >&) ;
#endif
template void DiscriminantAnalysisElement::doIt<int64_t>(const carto::rc_ptr<carto::Volume<int64_t> >&) ;
template void DiscriminantAnalysisElement::doIt<uint64_t>(const carto::rc_ptr<carto::Volume<uint64_t> >&) ;


template void DiscriminantAnalysisElement::doIt<int8_t>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<int8_t> >&) ;
template void DiscriminantAnalysisElement::doIt<uint8_t>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<uint8_t> >&) ;
template void DiscriminantAnalysisElement::doIt<int16_t>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<int16_t> >&) ;
template void DiscriminantAnalysisElement::doIt<uint16_t>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<uint16_t> >&) ;
template void DiscriminantAnalysisElement::doIt<int32_t>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<int32_t> >&) ;
template void DiscriminantAnalysisElement::doIt<uint32_t>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<uint32_t> >&) ;
template void DiscriminantAnalysisElement::doIt<float>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<float> >&) ;
template void DiscriminantAnalysisElement::doIt<double>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<double> >&) ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template void DiscriminantAnalysisElement::doIt<char>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<char> >&) ;
#endif
template void DiscriminantAnalysisElement::doIt<int64_t>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<int64_t> >&) ;
template void DiscriminantAnalysisElement::doIt<uint64_t>(const std::list< Point3d>&, const carto::rc_ptr<carto::Volume<uint64_t> >&) ;


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
