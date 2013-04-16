#ifndef AIMS_MESH_TEXTURETOOLS_H
#define AIMS_MESH_TEXTURETOOLS_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>


namespace aims
{
    size_t textureMax( const TimeTexture<short> & intex,
                       const AimsSurfaceTriangle & inAimsMesh );

    std::vector< size_t > * labelsHistogram(
      const TimeTexture<short> & intex, size_t labels_nb, bool verbose );

} // namespace aims

#endif // AIMS_MESH_TEXTURETOOLS_H
