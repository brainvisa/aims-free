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


#include <aims/morphology/operatormorpho.h>
#include <aims/data/data.h>
#include <aims/distancemap/chamfer.h>
#include <aims/distancemap/stlsort.h>
#include <aims/utility/threshold.h>
#include <aims/utility/bininverse.h>
#include <aims/math/mathelem.h>
#include <aims/topology/topoClassif.h>

using namespace std;

template <>
AimsData<short> AimsMorphoConnectivityChamferErosion(
  const AimsData<short> &vol,
  float size,
  Connectivity::Type type )
{
  ASSERT(vol.dimT()==1);
  ASSERT( size>0 && size<(float)square(vol.dimX()) && 
                    size<(float)square(vol.dimY()) );

  AimsData<short> eroded;
  AimsBinaryInverse inversion;
  eroded = AimsConnectivityChamferDistanceMap(inversion(vol), type);

  AimsThreshold<short,short> thresh(AIMS_GREATER_THAN,(short)(size+0.01));


  return thresh.bin(eroded);
}


template <>
AimsData<short> AimsMorphoChamferErosion( const AimsData<short> &vol,
                                          float size,
                                          int xmask,int ymask,int zmask,
                                          float mult_fact )
{
  ASSERT(vol.dimT()==1);
  ASSERT( size>0 && size<(float)square(vol.dimX()) && 
                    size<(float)square(vol.dimY()) );

  AimsData<short> eroded;
  AimsBinaryInverse inversion;
  eroded = AimsChamferDistanceMap(inversion(vol),
                                  xmask,ymask,zmask,mult_fact);

  AimsThreshold<short,short> thresh(AIMS_GREATER_THAN,
                                    (short)(size*mult_fact+0.5));
  return thresh.bin(eroded);
}


template <>
AimsData<short> AimsMorphoConnectivityChamferHomotopicErosion(
  const AimsData<short> &initvol,float size,
  Connectivity::Type connectivity )
{
  
  int x,y,z;
  multimap<short,Point3d> front;
  multimap<short,Point3d>::reverse_iterator id,ed;
  Point3d		             pos,newpos;
  unsigned n;
  int dimX = initvol.dimX(), dimY = initvol.dimY(), dimZ = initvol.dimZ();
  AimsData<short>                     	distance(dimX,dimY,dimZ);
  AimsData<short>                     	eroded = initvol.clone();
  AimsData<short>                     	immortal(dimX,dimY,dimZ); //0 : still alive , 1: immortal
 Connectivity                       cd( eroded.oLine(), eroded.oSlice(), connectivity );
  //Def initial immortal point map
  ForEach3d(initvol,x,y,z)
    if (initvol(x,y,z) > 0)
      eroded(x,y,z) = 1;
    else
       eroded(x,y,z) = 0;
 
  TopologicalClassification<short> TC(eroded);

  //Distance map from the outside
  AimsBinaryInverse inversion;
  distance =  AimsConnectivityChamferDistanceMap(inversion(initvol), connectivity);


  // Front init
  ForEach3d(eroded,x,y,z)
    if (eroded(x,y,z) == 1)
      {
	pos = Point3d(x,y,z);
	for ( n = 0; n < (unsigned) cd.nbNeighbors(); n++ )
	  {
	    newpos = pos + cd.xyzOffset( n );
	    if ( newpos[0] >= 0   &&
		 newpos[0] < dimX &&
		 newpos[1] >= 0   &&
		 newpos[1] < dimY &&
		 newpos[2] >= 0   &&
		 newpos[2] < dimZ &&
		 (eroded(newpos) == 0 ) )
	      {
		front.insert(pair<short,Point3d>(distance(pos), pos));
		break;
	      }
	  }
      }

  immortal = 0;
  for (id=front.rbegin(), ed=front.rend();id != ed; ++id)
    if (!TC.isSimplePoint(id->second,1))
      immortal(id->second) = 1;

  unsigned inc = 0;

  while (inc != front.size()) 
  {
     
    inc = front.size();
    for (id=front.rbegin(), ed=front.rend();id != ed; ++id)
	if ( immortal(id->second) == 0 )
	  {
	    pos = id->second;
	    eroded(pos) = 0; 
	    TopologicalClassification<short> TCt(eroded);
	    if (!TCt.isSimplePoint(pos,1))
	      {
		//upgrade immortal map
		immortal(pos) = 1;
		for ( n = 0; n < (unsigned) cd.nbNeighbors(); n++ )
		  {
		    newpos = pos + cd.xyzOffset( n );
		    if ( newpos[0] >= 0   &&
			 newpos[0] < dimX &&
			 newpos[1] >= 0   &&
			 newpos[1] < dimY &&
			 newpos[2] >= 0   &&
			 newpos[2] < dimZ &&
			 eroded(newpos) == 1 && !TCt.isSimplePoint(newpos,1) )
		      {
			immortal(newpos) = 1;
			break;
		      }
		  }
	      } 
	  }
      
      //upgrade front 
      front.clear();
      ForEach3d(eroded,x,y,z)
	if (eroded(x,y,z) == 1  && immortal(x,y,z) == 0 && distance(x,y,z) <= size)
	  {
	    pos = Point3d(x,y,z);
	    for ( n = 0; n < (unsigned) cd.nbNeighbors(); n++ )
	      {
		newpos = pos + cd.xyzOffset( n );
		if ( newpos[0] >= 0   &&
		     newpos[0] < dimX &&
		     newpos[1] >= 0   &&
		     newpos[1] < dimY &&
		     newpos[2] >= 0   &&
		     newpos[2] < dimZ &&
		     (eroded(newpos) == 0) )
		  {
		    front.insert(pair<short,Point3d>(distance(pos), pos));
		    break;
		  }
	      }
	  }
    }
  
  return(eroded);
  
}


template <>
AimsData<short> AimsMorphoErosion( const AimsData<short> &vol,
                                   float size, AimsMorphoMode mode )
{
  ASSERT(vol.dimT()==1);
  AimsData<short> erosion;

  switch (mode)
  {
    case AIMS_CHAMFER_BALL_3D :
      erosion = AimsMorphoChamferErosion(vol,size,3,3,3,50);
      break;
    case AIMS_CHAMFER_BALL_2D :
      erosion = AimsMorphoChamferErosion(vol,size,3,3,1,50);
      break;
  }
  return erosion;
}

