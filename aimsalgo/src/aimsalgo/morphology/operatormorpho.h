/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#ifndef AIMS_MORPHOLOGY_OPERATORMORPHO_H
#define AIMS_MORPHOLOGY_OPERATORMORPHO_H

#include <aims/config/aimsalgo_config.h>
#include <aims/connectivity/connectivity_g.h>

using aims::Connectivity;

template <typename T> class AimsData;
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
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoErosion(
  const AimsData<T> &vol, float size, AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );

AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoConnectivityChamferHomotopicErosion(
  AimsData<T> &initvol, float size, Connectivity::Type connectivity );
///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoChamferErosion(
  const AimsData<T> &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );

///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoConnectivityChamferErosion(
  const AimsData<T> &vol, float size, Connectivity::Type type );

AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoConnectivityChamferHomotopicErosion(
  const AimsData<T> &initvol, float size, Connectivity::Type connectivity );

//@}


/**@name Dilation*/
//@{
///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoDilation(
  const AimsData<T> &vol,
  float size,
  AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );

///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoChamferDilation(
  const AimsData<T> &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );

///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoConnectivityChamferDilation(
  const AimsData<T> &vol, float size, Connectivity::Type type);
//@}


/**@name Closing*/
//@{
///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoClosing(
  const AimsData<T> &vol, float size, AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );
///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoChamferClosing(
  const AimsData<T> &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );
///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoConnectivityChamferClosing(
  const AimsData<T> &vol, float size, Connectivity::Type type );
//@}


/**@name Opening*/
//@{
///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoOpening(
  const AimsData<T> &vol, float size, AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );
///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoChamferOpening(
  const AimsData<T> &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );

///
AIMSALGO_API
template <typename T>
AimsData<T> AimsMorphoConnectivityChamferOpening(
  const AimsData<T> &vol, float size, Connectivity::Type type );
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
