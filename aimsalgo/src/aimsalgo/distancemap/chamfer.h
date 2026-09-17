
#ifndef AIMS_DISTANCEMAP_CHAMFER_H
#define AIMS_DISTANCEMAP_CHAMFER_H

namespace carto
{
  template <typename T> class Volume;
  template <typename T> class VolumeRef;
}

#include <aims/config/aimsalgo_config.h>
#include <aims/connectivity/connectivity_g.h>
using aims::Connectivity;

#define AIMS_CHAMFER_DISTANCE_MAP     0

#define AIMS_CHAMFER_OUTSIDE 0
#define AIMS_CHAMFER_INSIDE  1

#define AIMS_CHAMFER_DOMAIN         32500
#define AIMS_CHAMFER_OUTSIDE_DOMAIN 32501


class AimsDistmapMaskPoint
{
  public :
    int x;
    int y;
    int z;
    int Offset;
    int Dist;

    AimsDistmapMaskPoint();
    AimsDistmapMaskPoint(const AimsDistmapMaskPoint &mask);
    ~AimsDistmapMaskPoint();
};


class AimsDistmapMask
{
  public : 
    int xCubeSize;
    int yCubeSize;
    int zCubeSize;
    int Length;
    AimsDistmapMaskPoint *FirstPoint;

    AimsDistmapMask();
    AimsDistmapMask(const AimsDistmapMask &mask);
    AimsDistmapMask(int xsize,int ysize,int zsize,int length);
    ~AimsDistmapMask();

    AimsDistmapMask& operator = (const AimsDistmapMask &mask);

};



void AimsCreateAndSplitCubicMask(
  const carto::rc_ptr<carto::Volume<int16_t> > &vol,
  int xsize,int ysize,int zsize,
  AimsDistmapMask &forward,
  AimsDistmapMask &backward,
  float mult_factor);

void AimsCreateAndSplitConnectivityMask(
  const carto::rc_ptr<carto::Volume<int16_t> > &vol,
  Connectivity::Type connectivity,
  AimsDistmapMask &forward,
  AimsDistmapMask &backward );

void AimsForwardSweepingWithBorder(carto::rc_ptr<carto::Volume<int16_t> > &vol,
                                   const AimsDistmapMask &mask,
                                   int borderlevel);

void AimsBackwardSweepingWithBorder(
  carto::rc_ptr<carto::Volume<int16_t> > &vol,
  const AimsDistmapMask &mask,
  int borderlevel);


/**@name Chamfer distance map.
    Two distance maps can be processed : the distance map of the outside
    of the object (give AIMS_CHAMFER_OUTSIDE argument), and the distance map
    of the inside of the object (give the AIMS_CHAMFER_INSIDE argument).
*/
//@{
/// Return customized short distance map 
carto::VolumeRef<int16_t>
AimsChamferDistanceMap( const carto::rc_ptr<carto::Volume<int16_t> > &vol,
                        int xmask=3,int ymask=3,int zmask=3,
                        float mult_factor=50);
/// Return Connectivity Chamfer Distance
carto::VolumeRef<int16_t>
AimsConnectivityChamferDistanceMap(
  const carto::rc_ptr<carto::Volume<int16_t> > &vol,
  Connectivity::Type type);
/// Return float Chamfer distance map
carto::VolumeRef<float>
AimsFloatChamferDistanceMap( const carto::rc_ptr<carto::Volume<int16_t> > &vol,
                             int side=AIMS_CHAMFER_OUTSIDE,
                             int xsize=3,int ysize=3,int zsize=3,
                             float mult_factor=50);
/// Return float distance map (minus outside object, plus inside object)
carto::VolumeRef<float>
AimsFloatSignedChamferDistanceMap(
  const carto::rc_ptr<carto::Volume<int16_t> > &vol,
  int xsize=3,int ysize=3,int zsize=3,
  float mult_factor=50);
//@}


#endif
