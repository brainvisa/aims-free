
%ModuleHeaderCode
#include <aims/roi/fill.h>
%End

namespace aims
{

  void floodFill( Volume_S16 & vol, const Point3d & pos, int16_t value,
                  aims::Connectivity::Type connectivity
                      = aims::Connectivity::CONNECTIVITY_18_XYZ,
                    int16_t tolerence = 0, bool verbose = false );
  void floodFill( Volume_U16 & vol, const Point3d & pos, uint16_t value,
                  aims::Connectivity::Type connectivity
                      = aims::Connectivity::CONNECTIVITY_18_XYZ,
                    uint16_t tolerence = 0, bool verbose = false );
  void floodFill( Volume_S32 & vol, const Point3d & pos, int32_t value,
                  aims::Connectivity::Type connectivity
                      = aims::Connectivity::CONNECTIVITY_18_XYZ,
                    int32_t tolerence = 0, bool verbose = false );
  void floodFill( Volume_U32 & vol, const Point3d & pos, uint32_t value,
                  aims::Connectivity::Type connectivity
                      = aims::Connectivity::CONNECTIVITY_18_XYZ,
                    uint32_t tolerence = 0, bool verbose = false );
  void floodFill( Volume_FLOAT & vol, const Point3d & pos, float value,
                  aims::Connectivity::Type connectivity
                      = aims::Connectivity::CONNECTIVITY_18_XYZ,
                    float tolerence = 0, bool verbose = false );
  void floodFill( Volume_DOUBLE & vol, const Point3d & pos, double value,
                  aims::Connectivity::Type connectivity
                      = aims::Connectivity::CONNECTIVITY_18_XYZ,
                    double tolerence = 0, bool verbose = false );

};

