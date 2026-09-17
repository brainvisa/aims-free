
#ifndef AIMS_MORPHOLOGY_OPERATORMORPHO_H
#define AIMS_MORPHOLOGY_OPERATORMORPHO_H

#include <aims/config/aimsalgo_config.h>
#include <aims/connectivity/connectivity_g.h>

using aims::Connectivity;

namespace carto
{
  template <typename T> class Volume;
  template <typename T> class VolumeRef;
}

/** Morphological and chamfer defined modes*/
enum AimsMorphoMode
{
  ///
  AIMS_CHAMFER_BALL_2D,
  ///
  AIMS_CHAMFER_BALL_3D
};


/**@name Erosion*/
//@{
///
template <typename T>
carto::VolumeRef<T> AimsMorphoErosion(
  const carto::rc_ptr<carto::Volume<T> > &vol, float size, AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );

template <typename T>
carto::VolumeRef<T> AimsMorphoConnectivityChamferHomotopicErosion(
  carto::rc_ptr<carto::Volume<T> > &initvol, float size, Connectivity::Type connectivity );
///
template <typename T>
carto::VolumeRef<T> AimsMorphoChamferErosion(
  const carto::rc_ptr<carto::Volume<T> > &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );

///
template <typename T>
carto::VolumeRef<T> AimsMorphoConnectivityChamferErosion(
  const carto::rc_ptr<carto::Volume<T> > &vol, float size, Connectivity::Type type );

template <typename T>
carto::VolumeRef<T> AimsMorphoConnectivityChamferHomotopicErosion(
  const carto::rc_ptr<carto::Volume<T> > &initvol, float size, Connectivity::Type connectivity );

//@}


/**@name Dilation*/
//@{
///
template <typename T>
carto::VolumeRef<T> AimsMorphoDilation(
  const carto::rc_ptr<carto::Volume<T> > &vol,
  float size,
  AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );

///
template <typename T>
carto::VolumeRef<T> AimsMorphoChamferDilation(
  const carto::rc_ptr<carto::Volume<T> > &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );

///
template <typename T>
carto::VolumeRef<T> AimsMorphoConnectivityChamferDilation(
  const carto::rc_ptr<carto::Volume<T> > &vol, float size, Connectivity::Type type);
//@}


/**@name Closing*/
//@{
///
template <typename T>
carto::VolumeRef<T> AimsMorphoClosing(
  const carto::rc_ptr<carto::Volume<T> > &vol, float size, AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );
///
template <typename T>
carto::VolumeRef<T> AimsMorphoChamferClosing(
  const carto::rc_ptr<carto::Volume<T> > &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );
///
template <typename T>
carto::VolumeRef<T> AimsMorphoConnectivityChamferClosing(
  const carto::rc_ptr<carto::Volume<T> > &vol, float size, Connectivity::Type type );
//@}


/**@name Opening*/
//@{
///
template <typename T>
carto::VolumeRef<T> AimsMorphoOpening(
  const carto::rc_ptr<carto::Volume<T> > &vol, float size, AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );
///
template <typename T>
carto::VolumeRef<T> AimsMorphoChamferOpening(
  const carto::rc_ptr<carto::Volume<T> > &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );

///
template <typename T>
carto::VolumeRef<T> AimsMorphoConnectivityChamferOpening(
  const carto::rc_ptr<carto::Volume<T> > &vol, float size, Connectivity::Type type );
//@}

namespace aims
{

  /// Grey-level mathematical morphology
  template <typename T>
  class MorphoGreyLevel
  {
  public:
    MorphoGreyLevel();
    virtual ~MorphoGreyLevel();

    /** when enabled, on binary images, the chamfer-based morphomath is
        used instead of the grey-level one. This is the default as it is
        way faster.
    */
    bool isChamferBinaryMorphoEnabled() const { return _use_chamfer; }
    void setChamferBinaryMorphoEnabled( bool x ) { _use_chamfer = x; }

    carto::VolumeRef<T>
      doErosion( const carto::VolumeRef<T>& dataIn, float radius );
    carto::VolumeRef<T>
      doDilation( const carto::VolumeRef<T>& dataIn, float radius );
    carto::VolumeRef<T>
      doClosing( const carto::VolumeRef<T>& dataIn, float radius );
    carto::VolumeRef<T>
      doOpening( const carto::VolumeRef<T>& dataIn, float radius );

    float chamferFactor() const { return _chamfer_factor; }
    void setChamferFactor( float x ) { _chamfer_factor = x; }
    Point3d chamferMaskSize() const { return _chamfer_mask_size; }
    void setChamferMaskSize( const Point3d & p ) { _chamfer_mask_size = p; }
    int neededBorderWidth() const;
    static bool isBinary( const carto::VolumeRef<T>& dataIn );

  private:
    std::vector<Point3d> doStructElement( float radius,
                                          const Point4df & voxelsize );
    Point3d computeIntRadius( float radius, const Point4df & voxelsize);
    carto::VolumeRef<T> checkDistanceToBorder(
      const carto::VolumeRef<T>& dataIn, float radius ) const;
    carto::VolumeRef<T> checkBorderWidth(
      const carto::VolumeRef<T>& dataIn ) const;
    static float distanceToBorder( const carto::VolumeRef<T> &vol );
    static carto::VolumeRef<T> reallocateVolume(
      const carto::VolumeRef<T>& dataIn, int added_width, int border_width );
    carto::VolumeRef<T>
      tryChamferErosion( const carto::VolumeRef<T>& dataIn, float radius );
    carto::VolumeRef<T>
      tryChamferDilation( const carto::VolumeRef<T>& dataIn, float radius );
    carto::VolumeRef<T>
      tryChamferClosing( const carto::VolumeRef<T>& dataIn, float radius );
    carto::VolumeRef<T>
      tryChamferOpening( const carto::VolumeRef<T>& dataIn, float radius );

    float radius;
    Point3d int_radius;
    std::vector<Point3d> list;
    bool _use_chamfer;
    float _chamfer_factor;
    Point3d _chamfer_mask_size;
  };

}

#endif
