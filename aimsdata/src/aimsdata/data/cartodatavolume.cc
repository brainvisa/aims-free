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

// we don't want to issue a warning while compiling the obsolete classes
// themselves...
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/data/data.h>

// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>
#include <cartodata/volume/volumeview.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>
#include <aims/io/datatypecode.h>
#include <aims/vector/vector.h>
#include <aims/math/dtitensor.h>
#include <aims/data/volumeoperators_aims.h>
#include <cartodata/volume/volumeutil_d.h>
#include <cartobase/object/object_d.h>
#include <set>
#include <map>


namespace carto
{

template class carto::VolumeProxy< DtiTensor * >;
template class carto::Volume< DtiTensor * >;

template class carto::VolumeProxy< std::set<float> >;
template class carto::Volume< std::set<float> >;

template class carto::VolumeProxy< std::map<int, float> >;
template class carto::Volume< std::map<int, float> >;

// Mac LLVM compiler does not seem to allow indirect instanciation
template class carto::VolumeRef< DtiTensor * >;
template class carto::VolumeRef< std::set<float> >;
template class carto::VolumeRef< std::map<int, float> >;

  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< DtiTensor* > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< DtiTensor* > > )


template class Creator<Volume<DtiTensor*> >;
template class Creator<VolumeRef<DtiTensor*> >;

} // namespace carto
