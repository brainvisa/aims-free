/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <aims/mesh/texture.h>
#include <cartobase/object/object_d.h>

namespace carto {

INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< int16_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< uint32_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< int32_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< uint64_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< int64_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( Texture1d )
INSTANTIATE_GENERIC_OBJECT_TYPE( Texture2d )
#define _TMP_ TimeTexture< AimsVector< short, 2 > >
INSTANTIATE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_

INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< int16_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< uint32_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< int32_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< uint64_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< int64_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< Texture1d > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< Texture2d > )
#define _TMP_ rc_ptr< TimeTexture< AimsVector< short, 2 > > >
INSTANTIATE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_

} // namespace carto
