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

//--------------------------------------------------
// Compute the average distance beetween a mesh and
//  a binary image represented by its distance map
//--------------------------------------------------

// "merge" the mesh vertices in the distance map and average the distances
// corresponding to the positions of those vertices.
// NOTE: THE VERTICES OUTSIDE THE FIELD OF THE DISTANCE MAP ARE PENALIZED AS IF
// THEY WERE AT A POINT OF MAXIMUM DISTANCE VALUE. best practice is to use a
// distance map defined on a field wider than just the object bounding box

#include <aims/data/data.h>
#include <aims/mesh/surfaceOperation.h>

using namespace aims; 
using namespace std;
using namespace carto;

// SWITCH 2-D / 3-D
float AimsMeshDistance( const AimsData< float > & refMap,
												const AimsSurfaceTriangle & regMesh,
												const Point3d  & dimImage,
												const Point3df & sizeVoxel,
												const float & maxDistanceMap,
												const int32_t & numVertices,
												const int16_t & dim          )
{
	double score=0.;
	
	// variables for interpolation, given a voxel position in mm
	Point3d floorPixCoords; // contains the coordinates of the neighboring
													// voxels which has smallest coords
	Point3df Weights;       // the weights corresponding to those smallest
													// coords for linear interpolation
	float pixCoord;         // the equivalent coord. in pixels
	
	int16_t outside; // boolean to indicates points outside the distance map

	if( dim == 2 ) {
		for( int32_t v=0; v < numVertices; v++ ) {
			outside = 0;
			for( int16_t d = 0; d < 2; d++ ) {
				pixCoord =  regMesh.vertex()[v][d] / sizeVoxel[d];
				if( (pixCoord >= dimImage[d]-1) | (pixCoord < 0) ) {
					outside = 1;
					break;
				}
				else {
					floorPixCoords[d] = floor( pixCoord );
					Weights[d] = 1 - ( pixCoord - floorPixCoords[d] );
				}
			}
			if( outside ) {
				score += maxDistanceMap; // penalize out of range positions
				// or should we just skip them ? in that case, be careful to averaging
			}
			else {
				for(int16_t pX = 0; pX < 2; pX++)
					for(int16_t pY = 0; pY < 2; pY++) {
							score += (pX*(1-2*Weights[0])+Weights[0])*
												(pY*(1-2*Weights[1])+Weights[1])*
												refMap( floorPixCoords[0]+pX,
																floorPixCoords[1]+pY, 0 );
				}
			}
		}
	}
	else {
		for(int32_t v=0; v < numVertices; v++) {
			outside = 0;
			for(int16_t d = 0; d < 3; d++) {
				pixCoord =  regMesh.vertex()[v][d] / sizeVoxel[d];
				if( (pixCoord >= dimImage[d]-1) | (pixCoord < 0) ) {
					outside = 1;
					break;
				}
				else {
					floorPixCoords[d] = floor( pixCoord );
					Weights[d] = 1 - ( pixCoord - floorPixCoords[d] );
				}
			}
			if( outside ) {
				score += maxDistanceMap; // penalize out of range positions
			}
			else {
				for(int16_t pX = 0; pX < 2; pX++)
					for(int16_t pY = 0; pY < 2; pY++)
						for(int16_t pZ = 0; pZ < 2; pZ++) {
							score += (pX*(1-2*Weights[0])+Weights[0])*
											(pY*(1-2*Weights[1])+Weights[1])*
											(pZ*(1-2*Weights[2])+Weights[2])*
												refMap( floorPixCoords[0]+pX,
																floorPixCoords[1]+pY,
																floorPixCoords[2]+pZ );
				}
			}
		}
	}
	return score / numVertices;
}

