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


#include <aims/resampling/resampler_d.h>
#include <aims/resampling/resamplerfactory_d.h>
#include <aims/resampling/splineresampler_d.h>
#include <aims/resampling/nearestneighborresampler_d.h>
#include <aims/resampling/linearresampler_d.h>
#include <aims/resampling/quadraticresampler_d.h>
#include <aims/resampling/cubicresampler_d.h>
#include <aims/resampling/quarticresampler_d.h>
#include <aims/resampling/quinticresampler_d.h>
#include <aims/resampling/sixthorderresampler_d.h>
#include <aims/resampling/seventhorderresampler_d.h>

namespace aims {

template class Resampler<int8_t>;
template class SplineResampler<int8_t>;
template class NearestNeighborResampler<int8_t>;
template class LinearResampler<int8_t>;
template class QuadraticResampler<int8_t>;
template class CubicResampler<int8_t>;
template class QuarticResampler<int8_t>;
template class QuinticResampler<int8_t>;
template class SixthOrderResampler<int8_t>;
template class SeventhOrderResampler<int8_t>;
template class ResamplerFactory<int8_t>;

template class Resampler<uint8_t>;
template class SplineResampler<uint8_t>;
template class NearestNeighborResampler<uint8_t>;
template class LinearResampler<uint8_t>;
template class QuadraticResampler<uint8_t>;
template class CubicResampler<uint8_t>;
template class QuarticResampler<uint8_t>;
template class QuinticResampler<uint8_t>;
template class SixthOrderResampler<uint8_t>;
template class SeventhOrderResampler<uint8_t>;
template class ResamplerFactory<uint8_t>;

template class Resampler<int16_t>;
template class SplineResampler<int16_t>;
template class NearestNeighborResampler<int16_t>;
template class LinearResampler<int16_t>;
template class QuadraticResampler<int16_t>;
template class CubicResampler<int16_t>;
template class QuarticResampler<int16_t>;
template class QuinticResampler<int16_t>;
template class SixthOrderResampler<int16_t>;
template class SeventhOrderResampler<int16_t>;
template class ResamplerFactory<int16_t>;

template class Resampler<uint16_t>;
template class SplineResampler<uint16_t>;
template class NearestNeighborResampler<uint16_t>;
template class LinearResampler<uint16_t>;
template class QuadraticResampler<uint16_t>;
template class CubicResampler<uint16_t>;
template class QuarticResampler<uint16_t>;
template class QuinticResampler<uint16_t>;
template class SixthOrderResampler<uint16_t>;
template class SeventhOrderResampler<uint16_t>;
template class ResamplerFactory<uint16_t>;

template class Resampler<int32_t>;
template class SplineResampler<int32_t>;
template class NearestNeighborResampler<int32_t>;
template class LinearResampler<int32_t>;
template class QuadraticResampler<int32_t>;
template class CubicResampler<int32_t>;
template class QuarticResampler<int32_t>;
template class QuinticResampler<int32_t>;
template class SixthOrderResampler<int32_t>;
template class SeventhOrderResampler<int32_t>;
template class ResamplerFactory<int32_t>;

template class Resampler<uint32_t>;
template class SplineResampler<uint32_t>;
template class NearestNeighborResampler<uint32_t>;
template class LinearResampler<uint32_t>;
template class QuadraticResampler<uint32_t>;
template class CubicResampler<uint32_t>;
template class QuarticResampler<uint32_t>;
template class QuinticResampler<uint32_t>;
template class SixthOrderResampler<uint32_t>;
template class SeventhOrderResampler<uint32_t>;
template class ResamplerFactory<uint32_t>;

template class Resampler<float>;
template class SplineResampler<float>;
template class NearestNeighborResampler<float>;
template class LinearResampler<float>;
template class QuadraticResampler<float>;
template class CubicResampler<float>;
template class QuarticResampler<float>;
template class QuinticResampler<float>;
template class SixthOrderResampler<float>;
template class SeventhOrderResampler<float>;
template class ResamplerFactory<float>;

template class Resampler<double>;
template class SplineResampler<double>;
template class NearestNeighborResampler<double>;
template class LinearResampler<double>;
template class QuadraticResampler<double>;
template class CubicResampler<double>;
template class QuarticResampler<double>;
template class QuinticResampler<double>;
template class SixthOrderResampler<double>;
template class SeventhOrderResampler<double>;
template class ResamplerFactory<double>;

template class Resampler<AimsRGB>;
template class SplineResampler<AimsRGB>;
template class NearestNeighborResampler<AimsRGB>;
template class LinearResampler<AimsRGB>;
template class QuadraticResampler<AimsRGB>;
template class CubicResampler<AimsRGB>;
template class QuarticResampler<AimsRGB>;
template class QuinticResampler<AimsRGB>;
template class SixthOrderResampler<AimsRGB>;
template class SeventhOrderResampler<AimsRGB>;
template class ResamplerFactory<AimsRGB>;


template class Resampler<AimsRGBA>;
template class SplineResampler<AimsRGBA>;
template class NearestNeighborResampler<AimsRGBA>;
template class LinearResampler<AimsRGBA>;
template class QuadraticResampler<AimsRGBA>;
template class CubicResampler<AimsRGBA>;
template class QuarticResampler<AimsRGBA>;
template class QuinticResampler<AimsRGBA>;
template class SixthOrderResampler<AimsRGBA>;
template class SeventhOrderResampler<AimsRGBA>;
template class ResamplerFactory<AimsRGBA>;

template class Resampler<Point3df>;
template class SplineResampler<Point3df>;
template class NearestNeighborResampler<Point3df>;
template class LinearResampler<Point3df>;
template class QuadraticResampler<Point3df>;
template class CubicResampler<Point3df>;
template class QuarticResampler<Point3df>;
template class QuinticResampler<Point3df>;
template class SixthOrderResampler<Point3df>;
template class SeventhOrderResampler<Point3df>;
template class ResamplerFactory<Point3df>;

} // namespace aims


#include <aims/resampling/masklinresampler_d.h>

namespace aims {

template class MaskLinearResampler<int16_t>;

} // namespace aims
