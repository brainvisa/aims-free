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



#ifndef AIMS_DISTANCEMAP_MESHPARCELLATION_H
#define AIMS_DISTANCEMAP_MESHPARCELLATION_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <aims/data/data.h>
#include <aims/distancemap/stlsort.h>
#include <aims/distancemap/meshvoronoi.h>
#include <set>
#include <map>

class Graph;
namespace aims
{
  namespace meshdistance
  {


    /** Extract the boundarie of a voronoi diagram
	i.e. compute the skiz */
    template <class T>
    Texture<std::set<T> > 
    MeshBorderVoronoi( const AimsSurface<3,Void> & mesh,
                       const Texture<T> & inittex,
                       const std::set<T> & setBack,
                       const std::set<T> & setFor);

    /** Define the gyrus seed from the SKIZ */
    template <class T>
    Texture<std::set<T> > 
    gyrusSeedDefinition( const AimsSurface<3,Void> & mesh, 
                         const Texture<T> & inittex,
                         const std::set<T> & setBack,
                         const std::set<T> & setFor,
                         const std::set<std::set<T> > & labelAllowed);
    template <class T>
    Texture<T> 
    gyrusSeedDilationInSKIZ( const AimsSurface<3,Void> & mesh, 
                             const Texture<T> & seed,
                             const Texture<T> & skiz,
                             const T & Back,
                             const T & For );

    /// Convert a multidimensional boundary map to a texture of short
    template <class T>
    Texture<short> border2Texture( const Texture<std::set<T> > &tex,
                                   const AimsSurface<3,Void> & mesh,
                                   const std::set<T> & setBack,
                                   const std::set<T> & setFor);
  
  
    Texture<std::set<short> > 
    MeshVoronoi2Set( const AimsSurface<3,Void> & mesh, 
		     const Texture<std::set<short> > & inittex, 
		     const Texture<short> & sulcitex,
		     bool connexity);


    
    /* Convert a voronoi diagram whose label are multidimensional
       To a texture of short (for the visualisation) */
    template<class T, class U>
    Texture<short> Voronoi2toTexture(const Texture<std::set<T> > & vor, 
				     const AimsSurface<3,Void> & mesh,
				     const std::set<T> & setBack, 
				     const std::set<T> & setFor);


    AimsSurfaceTriangle MakePlaneMesh(unsigned n, Point3df Xi, Point3df Xf);

    /// Give the surface of labelled parcels defined from textures.
    template<class T>
    std::map<T,float>  SurfaceParcel(const Texture<T> & tex,
				     const AimsSurface<3,Void> & mesh);

    /// Give the surface of labelled parcels defined from volume.
    template<class T>
    std::map<T,float>  VolumeParcel(const AimsData<T> & vol);
 
    

    
    std::set<std::pair<short,short> > 
    ModelGraph2Boundaries( const Graph & g, 
			   const std::map<std::string,short> & traduction );

    std::set<std::set<short> > 
    gyrusHierarchy2setBoundaries(const std::map<std::string,short> &trans, 
				 const Graph & g);

    //std::set<std::set<short> > GyrusModel2SetOfSetOfSulci(const char *model,
    //const std::map<std::string,short> &trans );

    std::set<std::set<short> > 
    GyrusModel2SetOfSetOfSulci(const std::map<std::string, 
						   std::set<std::string> > 
						   &gyriAndSulci, 
						   const std::map<std::string,
						   short> &trans );
    std::set<std::string> 
    GyrusModel2SetOfSulci(const char *model, 
			  const std::set<std::string> &existingName );
    
    std::set<std::string> 
    GyrusModel2SetOfSulci(const std::map<std::string, std::set<std::string> > 
			  &gyriAndSulci, 
			  const std::set<std::string> &existingName );
    
    std::map<std::string, std::set<std::string> > 
    GyrusModel2GyriAndSulci(const char *model);

    /// global variable...
    extern  const short MESHDISTANCE_FORBIDDEN;
    extern  const short MESHDISTANCE_UNREACHED;

  }
}


#endif
