#ifndef AIMS_MESH_TEXTURETOOLS_H
#define AIMS_MESH_TEXTURETOOLS_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <cartodata/volume/volume.h>
#include <aims/rgb/rgb.h>


namespace aims {

  int textureMax(const TimeTexture<short> & intex);
  int textureMin(const TimeTexture<short> & intex);

  std::map<short, size_t> *labelsHistogram(
      const TimeTexture<short> &intex, int maxlabel = 32767,
      int minlabel = -32768, bool verbose = false);

  carto::VolumeRef<AimsRGBA> giftiColormap( const carto::Object header);

  template <typename T>
  carto::VolumeRef<AimsRGBA> giftiColormap(const TimeTexture<T> & texture) {
      return giftiColormap(carto::Object::reference(
        texture.header()));
  }

} // namespace aims

#endif // AIMS_MESH_TEXTURETOOLS_H
