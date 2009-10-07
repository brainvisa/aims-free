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


#include <aims/math/ppca_d.h>

using namespace aims;
using namespace std ;

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



template void  ProbabilisticPcaElement::doIt<int8_t>(const AimsData<int8_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<uint8_t>(const AimsData<uint8_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<int16_t>(const AimsData<int16_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<uint16_t>(const AimsData<uint16_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<int32_t>(const AimsData<int32_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<uint32_t>(const AimsData<uint32_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<float>(const AimsData<float>&, double) ;
template void  ProbabilisticPcaElement::doIt<double>(const AimsData<double>&, double) ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template void  ProbabilisticPcaElement::doIt<char>(const AimsData<char>&, double) ;
#endif
template void  ProbabilisticPcaElement::doIt<long>(const AimsData<long>&, double) ;
template void  ProbabilisticPcaElement::doIt<unsigned long>(const AimsData<unsigned long>&, double) ;

template double  ProbabilisticPcaElement::noiseVariance<int8_t>(const AimsData<int8_t>&, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<uint8_t>(const AimsData<uint8_t>&, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<int16_t>(const AimsData<int16_t>&, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<uint16_t>(const AimsData<uint16_t>&, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<int32_t>(const AimsData<int32_t>&, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<uint32_t>(const AimsData<uint32_t>&, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<float>(const AimsData<float>&, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<double>(const AimsData<double>&, double &) const ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template double  ProbabilisticPcaElement::noiseVariance<char>(const AimsData<char>&, double &) const ;
#endif
template double  ProbabilisticPcaElement::noiseVariance<long>(const AimsData<long>&, double &) const ;
template double  ProbabilisticPcaElement::noiseVariance<unsigned long>(const AimsData<unsigned long>&, double &) const ;

template void  ProbabilisticPcaElement::doIt<int8_t>(const std::list< Point3d>&, const AimsData<int8_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<uint8_t>(const std::list< Point3d>&, const AimsData<uint8_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<int16_t>(const std::list< Point3d>&, const AimsData<int16_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<uint16_t>(const std::list< Point3d>&, const AimsData<uint16_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<int32_t>(const std::list< Point3d>&, const AimsData<int32_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<uint32_t>(const std::list< Point3d>&, const AimsData<uint32_t>&, double) ;
template void  ProbabilisticPcaElement::doIt<float>(const std::list< Point3d>&, const AimsData<float>&, double) ;
template void  ProbabilisticPcaElement::doIt<double>(const std::list< Point3d>&, const AimsData<double>&, double) ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template void  ProbabilisticPcaElement::doIt<char>(const std::list< Point3d>&, const AimsData<char>&, double) ;
#endif
template void  ProbabilisticPcaElement::doIt<long>(const std::list< Point3d>&, const AimsData<long>&, double) ;
template void  ProbabilisticPcaElement::doIt<unsigned long>(const std::list< Point3d>&, const AimsData<unsigned long>&, double) ;


template class  ProbabilisticPca<int8_t> ;
template class  ProbabilisticPca<uint8_t> ;
template class  ProbabilisticPca<int16_t> ;
template class  ProbabilisticPca<uint16_t> ;
template class  ProbabilisticPca<int32_t> ;
template class  ProbabilisticPca<uint32_t> ;
template class  ProbabilisticPca<float> ;
template class  ProbabilisticPca<double> ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class  ProbabilisticPca<char> ;
#endif
template class  ProbabilisticPca<long> ;
template class  ProbabilisticPca<unsigned long> ;
