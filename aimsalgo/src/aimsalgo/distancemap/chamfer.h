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


#ifndef AIMS_DISTANCEMAP_CHAMFER_H
#define AIMS_DISTANCEMAP_CHAMFER_H

template <class T> class AimsData;

#include <aims/config/aimsalgo_config.h>
#include <aims/connectivity/connectivity_g.h>
using aims::Connectivity;

#define AIMS_CHAMFER_DISTANCE_MAP     0

#define AIMS_CHAMFER_OUTSIDE 0
#define AIMS_CHAMFER_INSIDE  1

#define AIMS_CHAMFER_DOMAIN         32500
#define AIMS_CHAMFER_OUTSIDE_DOMAIN 32501


class AIMSALGO_API AimsDistmapMaskPoint
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


class AIMSALGO_API AimsDistmapMask
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



AIMSALGO_API 
void AimsCreateAndSplitCubicMask (const AimsData<int16_t> &vol,
                                  int xsize,int ysize,int zsize,
                                  AimsDistmapMask &forward,
                                  AimsDistmapMask &backward,
                                  float mult_factor);

AIMSALGO_API 
void AimsCreateAndSplitConnectivityMask ( const AimsData<int16_t> &vol,
				      Connectivity::Type connectivity,
				      AimsDistmapMask &forward,
				      AimsDistmapMask &backward );

AIMSALGO_API 
void AimsForwardSweepingWithBorder(AimsData<int16_t> &vol,
                                   const AimsDistmapMask &mask,
                                   int borderlevel);

AIMSALGO_API 
void AimsBackwardSweepingWithBorder(AimsData<int16_t> &vol,
                                    const AimsDistmapMask &mask,
                                    int borderlevel);


/**@name Chamfer distance map.
    Two distance maps can be processed : the distance map of the outside
    of the object (give AIMS_CHAMFER_OUTSIDE argument), and the distance map
    of the inside of the object (give the AIMS_CHAMFER_INSIDE argument).
*/
//@{
/// Return customized short distance map 
AIMSALGO_API AimsData<int16_t> 
AimsChamferDistanceMap(const AimsData<int16_t> &vol,
				    int xmask=3,int ymask=3,int zmask=3,
				    float mult_factor=50);
/// Return Connectivity Chamfer Distance
AIMSALGO_API AimsData<int16_t> 
AimsConnectivityChamferDistanceMap(const AimsData<int16_t> &vol,
				   Connectivity::Type type);
/// Return float Chamfer distance map
AIMSALGO_API AimsData<float>
AimsFloatChamferDistanceMap(const AimsData<int16_t> &vol,
			    int side=AIMS_CHAMFER_OUTSIDE,
			    int xsize=3,int ysize=3,int zsize=3,
			    float mult_factor=50);
/// Return float distance map (minus outside object, plus inside object)
AIMSALGO_API AimsData<float>
AimsFloatSignedChamferDistanceMap(const AimsData<int16_t> &vol,
				  int xsize=3,int ysize=3,int zsize=3,
				  float mult_factor=50);
//@}


#endif
