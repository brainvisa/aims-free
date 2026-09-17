
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/morphology/operatormorpho.h>
#include <cartodata/volume/volume.h>
#include <aims/distancemap/chamfer.h>
#include <aims/utility/threshold.h>
#include <aims/utility/bininverse.h>
#include <aims/math/mathelem.h>

using namespace carto;

template <>
VolumeRef<short> AimsMorphoConnectivityChamferClosing(
  const rc_ptr<Volume<short> > &vol,
  float size,
  Connectivity::Type type )
{
  ASSERT( vol->getSizeT() == 1 );
  ASSERT( size>0 && size<(float) square(vol->getSizeX()) &&
                    size<(float) square(vol->getSizeY()) );

  VolumeRef<short> dilated;
  dilated = AimsConnectivityChamferDistanceMap(vol,type);

  AimsThreshold<short,short> thresh1(AIMS_GREATER_THAN, (short)(size+0.01));
  dilated = thresh1.bin(dilated);

  VolumeRef<short> eroded;
  eroded = AimsConnectivityChamferDistanceMap(dilated, type);

  AimsThreshold<short,short> thresh2(AIMS_GREATER_THAN, (short)(size+0.01));
  return thresh2.bin(eroded);
}


template <>
VolumeRef<short> AimsMorphoChamferClosing( const rc_ptr<Volume<short> > &vol,
                                           float size,
                                           int xmask, int ymask, int zmask,
                                           float mult_fact )
{
  ASSERT( vol->getSizeT() == 1 );
  ASSERT( size>0 && size<(float) square(vol->getSizeX()) &&
                    size<(float) square(vol->getSizeY()) );

  VolumeRef<short> dilated;
  dilated = AimsChamferDistanceMap(vol,xmask,ymask,zmask,mult_fact);

  AimsThreshold<short,short> thresh1(AIMS_GREATER_THAN,
                                     (short)(size*mult_fact+0.5));
  dilated = thresh1.bin(dilated);

  VolumeRef<short> eroded;
  eroded = AimsChamferDistanceMap(dilated,xmask,ymask,zmask,mult_fact);

  AimsThreshold<short,short> thresh2(AIMS_GREATER_THAN,
                                     (short)(size*mult_fact+0.5));
  return thresh2.bin(eroded);
}


template <>
VolumeRef<short> AimsMorphoClosing( const rc_ptr<Volume<short> > &vol,
                                    float size, AimsMorphoMode mode )
{
  ASSERT( vol->getSizeT() == 1 );
  VolumeRef<short> close;

  switch (mode)
  {
    case AIMS_CHAMFER_BALL_3D :
      close = AimsMorphoChamferClosing(vol,size,3,3,3,50);
      break;
    case AIMS_CHAMFER_BALL_2D :
      close = AimsMorphoChamferClosing(vol,size,3,3,1,50);
      break;
  }
  return close;
}

