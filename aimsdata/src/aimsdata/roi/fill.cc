// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/roi/fill_d.h>

namespace aims
{

  template
  void floodFill( carto::Volume<int8_t> & vol, const Point3d & pos,
                  int8_t value, Connectivity::Type connectivity,
                  int8_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<uint8_t> & vol, const Point3d & pos,
                  uint8_t value, Connectivity::Type connectivity,
                  uint8_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<uint16_t> & vol, const Point3d & pos,
                  uint16_t value, Connectivity::Type connectivity,
                  uint16_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<int16_t> & vol, const Point3d & pos,
                  int16_t value, Connectivity::Type connectivity,
                  int16_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<int32_t> & vol, const Point3d & pos,
                  int32_t value, Connectivity::Type connectivity,
                  int32_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<uint32_t> & vol, const Point3d & pos,
                  uint32_t value, Connectivity::Type connectivity,
                  uint32_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<float> & vol, const Point3d & pos,
                  float value, Connectivity::Type connectivity,
                  float tolerence, bool verbose );
  template
  void floodFill( carto::Volume<double> & vol, const Point3d & pos,
                  double value, Connectivity::Type connectivity,
                  double tolerence, bool verbose );

}

