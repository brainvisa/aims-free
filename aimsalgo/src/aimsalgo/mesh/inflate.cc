
#include <aims/mesh/inflate.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/mesh/texture.h>
#include <set>
#include <map>
#include <vector>

using namespace aims;
using namespace std;

float ponderation_force(float alpha, int it, float bound)
{
	return (alpha * (1.0 - exp((float)(-it/bound))));
}


AimsSurfaceTriangle
*AimsInflateMesh(int t, const AimsSurfaceTriangle &surf, float Knorm, float Kspring, float Ksmooth, float bound, set<int> timeSet)
{
	int                         					i, index=0, n_vert;
	AimsSurfaceTriangle		      	*surf1  = new AimsSurfaceTriangle(surf);
	AimsSurfaceTriangle		      	*surf2  = new AimsSurfaceTriangle(surf);
	AimsSurfaceTriangle		      	*permut = NULL;
	AimsSurfaceTriangle				*result = new AimsSurfaceTriangle();
        vector<set<uint> >         		neigh = SurfaceManip::surfaceNeighbours(surf);
        vector<map<uint, float> >  	distance = SurfaceManip::surfaceNeighbourDistance(surf);
	map<int, int>							equivalence;

	// equivalence = map that manages the removal of node during the inflation and is used
	// to reinsert those nodes afterwards
	n_vert=surf.vertex().size();
	for (i=0; i<n_vert; i++)
		equivalence[i]=i;

	for (i=0;i<t;i++)
	{
		cout << "\r" << (int)(100.0*i/t) << "%";cout.flush();
		AimsInflationStep(*surf1, *surf2, neigh, distance, i, Knorm, Kspring, Ksmooth, bound);
		if (timeSet.count(i) > 0)
		{
			( *result )[index] = ( *surf2 )[0];
			index++;
		}
		permut = surf1;
		surf1 = surf2;
		surf2 = permut;
	}

	delete surf1;
	delete surf2;
	return result;
}

void AimsInflationStep(const AimsSurfaceTriangle &surf1, AimsSurfaceTriangle &surf2, const vector<set<uint> > &neigh, const vector<map<uint, float> > &dist, int niter, float F_Norm, float F_Ress, float F_Smot, float bound)
{
	float LOC_F_Norm = ponderation_force(F_Norm, niter, bound);
	float LOC_F_Ress = -ponderation_force(F_Ress, niter, bound);

	const vector<Point3df>                      & vert = surf1.vertex();
	vector<Point3df>                            & vert2 = surf2.vertex();
	const vector<Point3df>                      & norm = surf1.normal();

	vector<Point3df>::const_iterator            it = vert.begin();
	vector<Point3df>::iterator                  it2 = vert2.begin();
	vector<Point3df>::const_iterator            itnorm = norm.begin();
	vector<set<uint> >::const_iterator         itneigh = neigh.begin();
	set<uint>::const_iterator                  itvois;
	vector<map<uint, float> >::const_iterator  itdist = dist.begin();
	map<uint, float>::const_iterator           itress;
	Point3df                                    force,
	                                            force2,
                                              diff;
	float                                       ecart,
	                                            ecart0,
	                                            ress;
	float                                       signe;

	for ( ; it != vert.end(); ++it, ++it2, ++itneigh, ++itdist, ++itnorm) 
	{
	  /* Force Normale */
	  *it2 = *it + LOC_F_Norm * (*itnorm);

	  /* Force Ressort  et Lissage */
	  itvois = itneigh->begin();
	  itress = itdist->begin();
	  force = Point3df(0.0);
	  force2 = Point3df(0.0);
	  for ( ; itvois != itneigh->end(); ++itvois, ++itress ) {
	    
	    force += vert[*itvois] - *it;
	    
	    diff=*it - vert[*itvois];                             // Courant - voisin
	    ecart = diff.norm();                                  // Distance avec voisin
	    ecart0=(*itress).second;                              // Distance originale
	    signe = ((ress=(ecart0 - ecart)) > 0.0 ? -1.0 : 1.0); // Signe de la force
            if( ecart0 == 0. ) // identical points at beginning
              ecart0 = 1.;
	    force2 += (signe * ress*ress/(ecart0))*diff;          // Force
	  }
	  force /= (double)itneigh->size();
	  *it2 += LOC_F_Ress * force2 + F_Smot * force;
	}

	surf2.updateNormals();

}
