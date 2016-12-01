%ModuleHeaderCode
#include <aims/mesh/texturetools.h>
%End

namespace aims
{
  size_t textureMax( const TimeTexture_S16 & intex );

/*
  map<short, size_t> * labelsHistogram(
    const TimeTexture_S16 & intex, int maxlabel, int minlabel, bool verbose );
*/

    rc_ptr_Volume_RGBA giftiColormap( const TimeTexture_S16 & texture );
    rc_ptr_Volume_RGBA giftiColormap( const TimeTexture_FLOAT & texture );
    rc_ptr_Volume_RGBA giftiColormap( const TimeTexture_DOUBLE & texture );
    rc_ptr_Volume_RGBA giftiColormap( const TimeTexture_S32 & texture );
    rc_ptr_Volume_RGBA giftiColormap( const TimeTexture_U32 & texture );

    rc_ptr_Volume_RGBA giftiColormap( const carto::Object header );
};

