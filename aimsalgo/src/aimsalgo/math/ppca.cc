
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/math/ppca_d.h>

using namespace carto ;
using namespace std ;

namespace aims {

double * ProbabilisticPcaElement::_exp = 0 ;

ProbabilisticPcaElement::ProbabilisticPcaElement( int significantNumberOfEV, 
						  double PIj ) : 
  _distanceRef(0.), _computed(false), _valid(true), _nbOfSignificantEV(significantNumberOfEV), 
  _PIj(PIj), _Sigma2(-1.), _detCi(0.), _normFactor(0.), _lnAddFactor(0.)
{
  if( !_exp )
    {
      _exp = new double[100010] ;
      for( int i = 0 ; i < 100010 ; ++i )
	_exp[i] = exp( -i/100. ) ;    
    }
}



template void  ProbabilisticPcaElement::doIt<int8_t>(
  const rc_ptr<Volume<int8_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<uint8_t>(const rc_ptr<Volume<uint8_t> >&, double) ;
template void  ProbabilisticPcaElement::doIt<int16_t>(const rc_ptr<Volume<int16_t> >&, double) ;
template void  ProbabilisticPcaElement::doIt<uint16_t>(const rc_ptr<Volume<uint16_t> >&, double) ;
template void  ProbabilisticPcaElement::doIt<int32_t>(const rc_ptr<Volume<int32_t> >&, double) ;
template void  ProbabilisticPcaElement::doIt<uint32_t>(const rc_ptr<Volume<uint32_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<int64_t>(const rc_ptr<Volume<int64_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<uint64_t>(const rc_ptr<Volume<uint64_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<float>(const rc_ptr<Volume<float> > &, double) ;
template void  ProbabilisticPcaElement::doIt<double>(const rc_ptr<Volume<double> > &, double) ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template void  ProbabilisticPcaElement::doIt<char>(const rc_ptr<Volume<char> > &, double) ;
#endif
#ifdef CARTO_LONG_IS_DISTINCT
template void  ProbabilisticPcaElement::doIt<long>(const rc_ptr<Volume<long> > &, double) ;
template void  ProbabilisticPcaElement::doIt<unsigned long>(const rc_ptr<Volume<unsigned long> > &, double) ;
#endif

template double  ProbabilisticPcaElement::noiseVariance<int8_t>(const rc_ptr<Volume<int8_t> > &, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<uint8_t>(const rc_ptr<Volume<uint8_t> > &, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<int16_t>(const rc_ptr<Volume<int16_t> > &, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<uint16_t>(const rc_ptr<Volume<uint16_t> > &, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<int32_t>(const rc_ptr<Volume<int32_t> > &, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<uint32_t>(const rc_ptr<Volume<uint32_t> > &, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<int64_t>(const rc_ptr<Volume<int64_t> > &, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<uint64_t>(const rc_ptr<Volume<uint64_t> > &, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<float>(const rc_ptr<Volume<float> > &, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<double>(const rc_ptr<Volume<double> > &, double &) const ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template double  ProbabilisticPcaElement::noiseVariance<char>(const rc_ptr<Volume<char> > &, double &) const ;
#endif
#ifdef CARTO_LONG_IS_DISTINCT
template double  ProbabilisticPcaElement::noiseVariance<long>(const rc_ptr<Volume<long> > &, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<unsigned long>(const rc_ptr<Volume<unsigned long> > &, double &) const ;
#endif

template void  ProbabilisticPcaElement::doIt<int8_t>(const std::list< Point3d>&, const rc_ptr<Volume<int8_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<uint8_t>(const std::list< Point3d>&, const rc_ptr<Volume<uint8_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<int16_t>(const std::list< Point3d>&, const rc_ptr<Volume<int16_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<uint16_t>(const std::list< Point3d>&, const rc_ptr<Volume<uint16_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<int32_t>(const std::list< Point3d>&, const rc_ptr<Volume<int32_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<uint32_t>(const std::list< Point3d>&, const rc_ptr<Volume<uint32_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<int64_t>(const std::list< Point3d>&, const rc_ptr<Volume<int64_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<uint64_t>(const std::list< Point3d>&, const rc_ptr<Volume<uint64_t> > &, double) ;
template void  ProbabilisticPcaElement::doIt<float>(const std::list< Point3d>&, const rc_ptr<Volume<float> > &, double) ;
template void  ProbabilisticPcaElement::doIt<double>(const std::list< Point3d>&, const rc_ptr<Volume<double> > &, double) ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template void  ProbabilisticPcaElement::doIt<char>(const std::list< Point3d>&, const rc_ptr<Volume<char> > &, double) ;
#endif
#ifdef CARTO_LONG_IS_DISTINCT
template void  ProbabilisticPcaElement::doIt<long>(const std::list< Point3d>&, const rc_ptr<Volume<long> > &, double) ;
template void  ProbabilisticPcaElement::doIt<unsigned long>(const std::list< Point3d>&, const rc_ptr<Volume<unsigned long> > &, double) ;
#endif


template class  ProbabilisticPca<int8_t> ;
template class  ProbabilisticPca<uint8_t> ;
template class  ProbabilisticPca<int16_t> ;
template class  ProbabilisticPca<uint16_t> ;
template class  ProbabilisticPca<int32_t> ;
template class  ProbabilisticPca<uint32_t> ;
template class  ProbabilisticPca<int64_t> ;
template class  ProbabilisticPca<uint64_t> ;
template class  ProbabilisticPca<float> ;
template class  ProbabilisticPca<double> ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class  ProbabilisticPca<char> ;
#endif
#ifdef CARTO_LONG_IS_DISTINCT
template class  ProbabilisticPca<long> ;
template class  ProbabilisticPca<unsigned long> ;
#endif

} // namespace aims
