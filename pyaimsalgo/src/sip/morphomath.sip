%ModuleHeaderCode
#include <aims/morphology/operatormorpho.h>
#include <aims/distancemap/front.h>
#include <aims/distancemap/voronoi.h>
%End

enum AimsMorphoMode
{
  AIMS_CHAMFER_BALL_2D,
  AIMS_CHAMFER_BALL_3D
};


AimsData_S16 AimsMorphoErosion( const AimsData_S16 &, float,
  AimsMorphoMode = AIMS_CHAMFER_BALL_3D ) throw() /ReleaseGIL/;
AimsData_S16 AimsMorphoChamferErosion( const AimsData_S16 &, float, int=3,
  int=3, int=3, float=50) throw() /ReleaseGIL/;

AimsData_S16 AimsMorphoDilation( const AimsData_S16 &, float,
  AimsMorphoMode = AIMS_CHAMFER_BALL_3D ) throw() /ReleaseGIL/;
AimsData_S16 AimsMorphoChamferDilation( const AimsData_S16 &, float, int=3, int=3,
  int=3, float=50) throw() /ReleaseGIL/;

AimsData_S16 AimsMorphoClosing( const AimsData_S16 &, float,
  AimsMorphoMode=AIMS_CHAMFER_BALL_3D ) throw() /ReleaseGIL/;
AimsData_S16 AimsMorphoChamferClosing( const AimsData_S16 &vol, float size,
  int xmask=3, int ymask=3, int zmask=3, float mult_fact=50 ) throw()
  /ReleaseGIL/;

AimsData_S16 AimsMorphoOpening( const AimsData_S16 &vol, float size,
  AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D ) throw() /ReleaseGIL/;
AimsData_S16 AimsMorphoChamferOpening( const AimsData_S16 &vol, float size,
  int xmask=3, int ymask=3, int zmask=3, float mult_fact=50 ) throw()
  /ReleaseGIL/;

AimsData_S16
AimsVoronoiFrontPropagation( AimsData_S16 & vol,
                             short val_domain, short val_outside,
                             int xm, int ym, int zm, float mult_factor )
  throw() /ReleaseGIL/;

