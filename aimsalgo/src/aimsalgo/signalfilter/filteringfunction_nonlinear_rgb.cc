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

#include <aims/signalfilter/filteringfunction_nonlinear_rgb_d.h>
#include <aims/signalfilter/filteringfunction_nonlinear_d.h>


#define AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE( NAME, VOXELTYPE )    \
  NAME< VOXELTYPE > ::NAME( carto::Object options ): _m(options) {}          \
  NAME< VOXELTYPE >::~NAME() {}                                              \
  void NAME< VOXELTYPE >::setOptions( carto::Object options )                \
  {                                                                          \
    _m.setOptions( options );                                                \
  }                                                                          \
  VOXELTYPE NAME< VOXELTYPE >::execute(                                      \
    const carto::VolumeRef< VOXELTYPE > & volume                             \
  )                                                                          \
  {                                                                          \
    return multichannelfiltervalues_nonlin< VOXELTYPE >( _m, volume );       \
  }                                                                          \
  VOXELTYPE NAME< VOXELTYPE >::execute(                                      \
    const carto::VolumeRef< VOXELTYPE > & volume,                            \
    const StructuringElementRef & se                                         \
  )                                                                          \
  {                                                                          \
    return multichannelfiltervalues_nonlin< VOXELTYPE >( _m, volume, se );   \
  }                                                                          \
  NAME< VOXELTYPE > * NAME< VOXELTYPE >::clone() const                       \
  {                                                                          \
    return new NAME< VOXELTYPE > ();                                         \
  }

namespace aims {

  template class NonLinFilterFunc<AimsRGB>;
  template class NonLinFilterFunc<AimsRGBA>;
  template class NonLinFilterFuncFactory<AimsRGB>;
  template class NonLinFilterFuncFactory<AimsRGBA>;

  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MedianFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MedianFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMedianFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMedianFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MeanFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MeanFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMeanFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMeanFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MinFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MinFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MaxFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MaxFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MajorityFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MajorityFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(ExtremaDifferenceFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(ExtremaDifferenceFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(SumFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(SumFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(VarFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(VarFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(StDevFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(StDevFilterFunc, AimsRGBA)

} // namespace aims
