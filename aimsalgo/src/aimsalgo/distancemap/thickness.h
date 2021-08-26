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


#ifndef AIMS_DISTANCEMAP_THICKNESS_H
#define AIMS_DISTANCEMAP_THICKNESS_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <cartodata/volume/volume.h>
#include <aims/connectivity/connectivity.h>
#include <aims/distancemap/stlsort.h>
#include <set>
#include <list>
#include <map>

class Graph;
namespace aims
{
  namespace meshdistance
  {
    void  SulcusOperture( const AimsSurface<3,Void> & mesh, 
			  std::map<Point3df, std::pair<float,float>, 
                          Point3dfCompare > &sulci2mesh,
			  const carto::rc_ptr<carto::Volume<short> > & surface_vol,
			  const float demin, const float dpmin, 
                          const unsigned MINCC,
			  const std::map <short,std::string> & trans,
			  const std::set<std::string>	& labels);

    void ClosestMeshTrianglesFromSulcusVoxel(const Point3df &pt,
					     const std::pair<Point3df,
                                             Point3df> & bipoint,
					     Point3df & n_sulci, float & dmn, 
                                             float & dmp, 
					     float demin, float dpmin, 
					     const AimsSurface<3,Void> & mesh);
    
    void ClosestMeshNodesFromSulcusVoxel(const Point3df &pt,
                                         const std::pair<Point3df,Point3df> 
                                         & bipoint,
                                         Point3df & n0,
					 float demin, float dpmin,
					 float & dmn, float & dmp,
                                         const AimsSurface<3,Void> & mesh );
   
    
    carto::VolumeRef<float> OpertureParameters(
        const AimsSurface<3,Void> & mesh,
        const std::map<Point3df,
                        std::pair<float,float>,
                        Point3dfCompare > &sulci2mesh,
        const carto::rc_ptr<carto::Volume<short> > & vol);
    
    
    
    void CCOperture(const std::vector<Point3df>    &cc,
		    const AimsSurface<3,Void> & mesh,
		    const  Point3dfNeigh &neigh,
		    std::map<Point3df, std::pair<float,float>, Point3dfCompare >& sulci2mesh,
		    const float demin, const float dpmin);


    
    std::set<unsigned> SubMesh(const AimsSurface<3,Void> & mesh,
			       const Point3df &bbmin, const Point3df & bbmax);

    inline float max(float x,float y)
    {
      if (x > y)
	return(x);
      else
	return(y);
    }
    
    inline float min(float x,float y)
    {
      if (x < y)
	return(x);
      else
	return(y);
    }
    
    inline Point3df Point3dfMax( const Point3df & X, const Point3df &Y)
    {
      Point3df Z;
      Z[0] = max(X[0],Y[0]);
      Z[1] = max(X[1],Y[1]);
      Z[2] = max(X[2],Y[2]);
      return(Z);
    }
    
    inline Point3df Point3dfMin( const Point3df & X, const Point3df &Y)
    {
      Point3df Z;
      Z[0] = min(X[0],Y[0]);
      Z[1] = min(X[1],Y[1]);
      Z[2] = min(X[2],Y[2]);
      return(Z);
    }

  }
}


#endif
