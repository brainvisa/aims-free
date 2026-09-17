#ifndef PYAIMS_DEBUG_DEBUGGINGVOL_H
#define PYAIMS_DEBUG_DEBUGGINGVOL_H

#include <cartodata/volume/volume.h>

namespace pyaims
{

  class DebuggingVolume : public carto::Volume<int16_t>
  {
  public:
    DebuggingVolume( int sizeX=1, int sizeY=1, int sizeZ=1, int sizeT=1 );
    virtual ~DebuggingVolume();

    static bool created;
    static bool deleted;
  };

}

#endif
