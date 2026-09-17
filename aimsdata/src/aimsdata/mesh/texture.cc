// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/mesh/texture.h>
#include <cartobase/object/object_d.h>

namespace carto
{

INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< int8_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< uint8_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< int16_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< uint16_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< int32_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< uint32_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< int64_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< uint64_t > )
INSTANTIATE_GENERIC_OBJECT_TYPE( TimeTexture< double > )
INSTANTIATE_GENERIC_OBJECT_TYPE( Texture1d )
INSTANTIATE_GENERIC_OBJECT_TYPE( Texture2d )
#define _TMP_ TimeTexture< AimsVector< short, 2 > >
INSTANTIATE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_

INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< int8_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< uint8_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< int16_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< uint16_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< int32_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< uint32_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< int64_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< uint64_t > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< double > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< Texture1d > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< Texture2d > )
#define _TMP_ rc_ptr< TimeTexture< AimsVector< short, 2 > > >
INSTANTIATE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_

} // namespace carto
