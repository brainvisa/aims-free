#ifndef AIMS_ROI_FILL_H
#define AIMS_ROI_FILL_H

#include <cartodata/volume/volume.h>
#include <aims/vector/vector.h>
#include <aims/connectivity/connectivity.h>


namespace aims
{

  template <typename T>
  void floodFill( carto::Volume<T> & vol, const Point3d & pos, T value,
                  Connectivity::Type connectivity
                    = Connectivity::CONNECTIVITY_18_XYZ,
                  T tolerence = 0, bool verbose = false );
}

#endif


