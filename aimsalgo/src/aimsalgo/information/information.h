
#ifndef AIMS_INFORMATION_INFORMATION_H
#define AIMS_INFORMATION_INFORMATION_H

#include <aims/mesh/surface.h>
#include <cartodata/volume/volume.h>

float AimsEntropy( const carto::rc_ptr<carto::Volume<float> >& p );

float AimsMutualInformation( const carto::rc_ptr<carto::Volume<float> >& p1,
                             const carto::rc_ptr<carto::Volume<float> >& p2,
                             const carto::rc_ptr<carto::Volume<float> >& p12 );

float AimsCorrelationRatio( const carto::rc_ptr<carto::Volume<float> >& p1,
                            const carto::rc_ptr<carto::Volume<float> >& p2,
                            const carto::rc_ptr<carto::Volume<float> >& p12 );

// what does this function do here ?? it should be in distancemap/

float AimsMeshDistance( const carto::rc_ptr<carto::Volume< float > > & refMap,
                        const AimsSurfaceTriangle & testMesh,
                        const Point3d  & dimImage,
                        const Point3df & sizeVoxel,
                        const float & maxDistanceMap,
                        const int32_t & numVertices,
                        const int16_t & dim          );
#endif

