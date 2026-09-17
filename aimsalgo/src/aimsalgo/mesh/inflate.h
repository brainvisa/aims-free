
#ifndef AIMS_SURFACE_INFLATE_H
#define AIMS_SURFACE_INFLATE_H

#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <set>
#include <map>
#include <vector>

float ponderation_force(float alpha,
												int it,
												float bound);

AimsSurfaceTriangle *AimsInflateMesh(	int t,
																			const AimsSurfaceTriangle &surf,
																			float Knorm,
																			float Kspring,
																			float Ksmooth,
																			float bound,
																			std::set<int> timeSet);

void AimsInflationStep(	const AimsSurfaceTriangle &surf1,
												AimsSurfaceTriangle &surf2,
												const std::vector<std::set<uint> > &neigh,
												const std::vector<std::map<uint, float> > &dist,
												int niter,
												float F_Norm,
												float F_Ress,
												float F_Smot,
												float bound);

#endif
