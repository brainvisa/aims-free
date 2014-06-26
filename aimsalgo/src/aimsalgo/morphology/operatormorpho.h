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

template <class T> class AimsData;

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
AimsData<short> AimsMorphoErosion(
  AimsData<short> &vol, float size, AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );

AIMSALGO_API
AimsData<short> AimsMorphoConnectivityChamferHomotopicErosion(
  AimsData<short> &initvol, float size, Connectivity::Type connectivity );
///
AIMSALGO_API
AimsData<short> AimsMorphoChamferErosion(
  AimsData<short> &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );

///
AIMSALGO_API
AimsData<short> AimsMorphoConnectivityChamferErosion(
  AimsData<short> &vol, float size, Connectivity::Type type );

AIMSALGO_API
AimsData<short> AimsMorphoConnectivityChamferHomotopicErosion(
  AimsData<short> &initvol, float size, Connectivity::Type connectivity );

//@}


/**@name Dilation*/
//@{
///
AIMSALGO_API
AimsData<short> AimsMorphoDilation(
  AimsData<short> &vol,
  float size,
  AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );

///
AIMSALGO_API
AimsData<short> AimsMorphoChamferDilation(
  AimsData<short> &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );

///
AIMSALGO_API
AimsData<short> AimsMorphoConnectivityChamferDilation(
  AimsData<short> &vol, float size, Connectivity::Type type);
//@}


/**@name Closing*/
//@{
///
AIMSALGO_API
AimsData<short> AimsMorphoClosing(
  AimsData<short> &vol, float size, AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );
///
AIMSALGO_API
AimsData<short> AimsMorphoChamferClosing(
  AimsData<short> &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );
///
AIMSALGO_API
AimsData<short> AimsMorphoConnectivityChamferClosing(
  AimsData<short> &vol, float size, Connectivity::Type type );
//@}


/**@name Opening*/
//@{
///
AIMSALGO_API
AimsData<short> AimsMorphoOpening(
  AimsData<short> &vol, float size, AimsMorphoMode mode=AIMS_CHAMFER_BALL_3D );
///
AIMSALGO_API
AimsData<short> AimsMorphoChamferOpening(
  AimsData<short> &vol,
  float size,
  int xmask=3,
  int ymask=3,
  int zmask=3,
  float mult_fact=50 );

///
AIMSALGO_API
AimsData<short> AimsMorphoConnectivityChamferOpening(
  AimsData<short> &vol, float size, Connectivity::Type type );
//@}

namespace aims
{

  template <typename T>
  class MorphoGreyLevel
  {
  public:
    MorphoGreyLevel();
    virtual ~MorphoGreyLevel();

    AimsData<T> doErosion( const AimsData<T>& dataIn, float radius );
    AimsData<T> doDilation( const AimsData<T>& dataIn, float radius );
    AimsData<T> doClosing( const AimsData<T>& dataIn, float radius );
    AimsData<T> doOpening( const AimsData<T>& dataIn, float radius );

  private:
    std::vector<Point3d> doStructElement( float radius,
                                          const Point4df & voxelsize );
    Point3d computeIntRadius( float radius, const Point4df & voxelsize);

    float radius;
    Point3d int_radius;
    std::vector<Point3d> list;
  };

}

#endif
