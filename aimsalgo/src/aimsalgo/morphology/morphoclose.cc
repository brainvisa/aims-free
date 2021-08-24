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

